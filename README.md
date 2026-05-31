# CPU Matrix Benchmark

## Yêu cầu hệ thống
- Windows 10/11 (64-bit)
- CPU hỗ trợ tính toán đa luồng
- GPU hỗ trợ OpenCL 1.2
- Đã cài OpenCL runtime (thường đi kèm driver GPU)
- GCC (khuyến nghị MinGW-w64/MSYS2)

## Thư viện và phụ thuộc
- OpenCL headers: `OpenCL-Headers/`
- OpenCL loader/runtime: `C:\Windows\System32\OpenCL.dll`

## Build
CPU Single:
```bash
gcc -O3 gpu_bench.c -o gpu_bench_single.exe -I.\OpenCL-Headers -DCL_TARGET_OPENCL_VERSION=120 C:\Windows\System32\OpenCL.dll
```

CPU Multi:
```bash
gcc -O3 gpu_bench.c -o gpu_bench_multi.exe -I.\OpenCL-Headers -DCL_TARGET_OPENCL_VERSION=120 C:\Windows\System32\OpenCL.dll
```

GPU:
```bash
gcc -O3 gpu_bench.c -o gpu_bench_gpu.exe -I.\OpenCL-Headers -DCL_TARGET_OPENCL_VERSION=120 C:\Windows\System32\OpenCL.dll
```

## OpenCL-Headers
- Nguồn: https://github.com/KhronosGroup/OpenCL-Headers
- Repo sử dụng: `OpenCL-Headers/`
- License: Apache License 2.0 (xem `OpenCL-Headers/LICENSE`)

## Ghi chú
- Đảm bảo file nguồn `gpu_bench.c` tồn tại trong thư mục dự án trước khi biên dịch.
- Nếu tên file nguồn hiện tại khác, hãy thay `gpu_bench.c` bằng tên file thực tế.
