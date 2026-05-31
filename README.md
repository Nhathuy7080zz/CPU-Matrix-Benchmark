# CPU Matrix Benchmark

README chính của dự án được viết bằng tiếng Việt. Bản tiếng Anh nằm tại `README.en.md`.

## Yêu cầu hệ thống
- Windows 10/11 (64-bit)
- CPU hỗ trợ tính toán đa luồng
- GPU hỗ trợ OpenCL 1.2
- Đã cài OpenCL runtime (thường đi kèm driver GPU)
- GCC (khuyến nghị MinGW-w64/MSYS2)

## Thư viện và phụ thuộc
- OpenCL headers: `OpenCL-Headers/`
- OpenCL loader/runtime: `C:\Windows\System32\OpenCL.dll`

## Biên dịch
Sử dụng lệnh sau để biên dịch:

```bash
gcc -O3 gpu_bench.c -o gpu_bench.exe -I.\OpenCL-Headers -DCL_TARGET_OPENCL_VERSION=120 C:\Windows\System32\OpenCL.dll
```

## Ghi công OpenCL-Headers
- Nguồn: https://github.com/KhronosGroup/OpenCL-Headers
- Thư mục sử dụng trong repo này: `OpenCL-Headers/`
- License: Apache License 2.0 (xem `OpenCL-Headers/LICENSE`)

## Ghi chú
- Đảm bảo file nguồn `gpu_bench.c` tồn tại trong thư mục dự án trước khi biên dịch.
- Nếu tên file nguồn hiện tại khác, hãy thay `gpu_bench.c` bằng tên file thực tế.
