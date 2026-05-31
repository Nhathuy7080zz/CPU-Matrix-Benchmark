#pragma GCC optimize("O3,unroll-loops")
#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

typedef struct {
    double *A;
    double *B;
    double *C;
    int N;
} ThreadArgs;

// Biến đếm hàng dùng chung giữa các luồng (Dynamic Load Balancing)
volatile int current_row = 0;
const int CHUNK_SIZE = 16;

double* allocate_matrix(int N) {
    // ps_malloc cấp phát vào PSRAM (bắt buộc cho ma trận lớn trên ESP32)
    return (double*)ps_malloc((size_t)N * N * sizeof(double));
}

void initialize_matrix(double *matrix, int N) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            matrix[i * N + j] = (double)(rand() % 100) / 10.0;
}

void zero_matrix(double *matrix, int N) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            matrix[i * N + j] = 0.0;
}

// Hàm luồng: Dynamic Load Balancing + Cache Blocking (tiling 32x32, vừa L1 Cache ESP32)
void* multiply_matrix_ikj_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    double *A = args->A, *B = args->B, *C = args->C;
    int N = args->N;
    int BLOCK = 32;

    while (1) {
        // Lấy khối hàng tiếp theo bằng atomic, không cần mutex
        int rs = __atomic_fetch_add(&current_row, CHUNK_SIZE, __ATOMIC_RELAXED);
        if (rs >= N) break;
        int re = (rs + CHUNK_SIZE > N) ? N : rs + CHUNK_SIZE;

        for (int k_blk = 0; k_blk < N; k_blk += BLOCK) {
            int k_end = (k_blk + BLOCK > N) ? N : k_blk + BLOCK;
            for (int j_blk = 0; j_blk < N; j_blk += BLOCK) {
                int j_end = (j_blk + BLOCK > N) ? N : j_blk + BLOCK;
                for (int i = rs; i < re; i++) {
                    for (int k = k_blk; k < k_end; k++) {
                        double a_ik = A[i * N + k];
                        for (int j = j_blk; j < j_end; j++)
                            C[i * N + j] += a_ik * B[k * N + j];
                    }
                }
            }
        }

        yield(); // Nhường CPU tránh Watchdog reset
    }

    pthread_exit(NULL);
    return NULL;
}

void print_sysinfo() {
    Serial.println("=========================================");
    Serial.println("         SYSTEM INFORMATION");
    Serial.println("=========================================");
    Serial.printf(" - Chip Model           : %s (Rev %d)\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf(" - So luong nhan (Cores): %d\n",           ESP.getChipCores());
    Serial.printf(" - Xung nhip CPU        : %d MHz\n",       ESP.getCpuFreqMHz());

    Serial.printf(" - Internal RAM (SRAM)  : %u KB Tong / %u KB Trong\n",
                  ESP.getHeapSize() / 1024, ESP.getFreeHeap() / 1024);

    if (psramFound()) {
        Serial.printf(" - External PSRAM       : %u MB Tong / %u MB Trong\n",
                      ESP.getPsramSize() / (1024 * 1024), ESP.getFreePsram() / (1024 * 1024));
    } else {
        Serial.println(" - External PSRAM       : KHONG CO (Xem canh bao!)");
    }
    Serial.println("=========================================\n");
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    print_sysinfo();

    if (!psramFound()) {
        Serial.println("[CANH BAO] PSRAM khong tim thay! Vao Tools -> PSRAM -> OPI PSRAM de bat len.");
    }

    int N           = 400;
    int NUM_THREADS = (ESP.getChipCores() > 0) ? ESP.getChipCores() : 1;
    
    printf("RUNNING", N);
    Serial.println("=========================================");

    srand(analogRead(0));

    double ram_mb = (3.0 * (size_t)N * N * sizeof(double)) / (1024.0 * 1024.0);
    Serial.printf(">> Kich thuoc ma tran   : %d x %d\n", N, N);
    Serial.printf(">> Dung luong RAM can   : %.0f MB\n",  ram_mb);
    Serial.print (">> Khoi tao du lieu     : ");

    double *A = allocate_matrix(N);
    double *B = allocate_matrix(N);
    double *C = allocate_matrix(N);

    if (A == NULL || B == NULL || C == NULL) {
        Serial.println("LOI (Out of memory)!");
        if (A) free(A);
        if (B) free(B);
        if (C) free(C);
        return;
    }

    initialize_matrix(A, N);
    initialize_matrix(B, N);
    zero_matrix(C, N);

    Serial.println("Xong.");
    Serial.printf(">> Dang tinh toan...\n\n");

    pthread_t  threads[NUM_THREADS];
    ThreadArgs thread_args[NUM_THREADS];

    current_row = 0;
    uint32_t start_time = millis();

    for (int t = 0; t < NUM_THREADS; t++) {
        thread_args[t] = (ThreadArgs){ A, B, C, N };
        if (pthread_create(&threads[t], NULL, multiply_matrix_ikj_thread, &thread_args[t]) != 0) {
            Serial.printf("[LOI] Khong the tao luong thu %d\n", t);
            return;
        }
    }

    for (int t = 0; t < NUM_THREADS; t++)
        pthread_join(threads[t], NULL);

    uint32_t end_time = millis();

    double time_spent = (double)(end_time - start_time) / 1000.0;
    double total_ops  = (double)N * N * N;
    double gflops     = (total_ops / time_spent) / 1e9;

    Serial.println("                 KET QUA");
    Serial.println("=========================================");
    Serial.printf(" - Kich thuoc ma tran   : %d x %d\n",   N, N);
    Serial.printf(" - So luong luong       : %d (Da luong)\n", NUM_THREADS);
    Serial.printf(" - Thoi gian chay       : %.4f giay (%.2f phut)\n", time_spent, time_spent / 60.0);
    Serial.printf(" - Tong so phep tinh    : %.2f ty (N^3 = %.2e)\n",  total_ops / 1e12, total_ops);
    Serial.printf(" - Hieu nang            : %.2f GFLOPS\n", gflops);
    Serial.println("=========================================");

    free(A);
    free(B);
    free(C);

    Serial.println("Hoan thanh!");
}

void loop() {
    delay(1000);
}
