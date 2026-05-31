# CPU Matrix Benchmark

## System Requirements
- Windows 10/11 (64-bit)
- Multi-thread capable CPU
- GPU with OpenCL 1.2 support
- OpenCL runtime installed (usually provided by GPU drivers)
- GCC (MinGW-w64/MSYS2 recommended)

## Libraries and Dependencies
- OpenCL headers: OpenCL-Headers/
- OpenCL loader/runtime: C:\Windows\System32\OpenCL.dll

## Build
Use the following command:

```bash
gcc -O3 gpu_bench.c -o gpu_bench.exe -I.\OpenCL-Headers -DCL_TARGET_OPENCL_VERSION=120 C:\Windows\System32\OpenCL.dll
```

## OpenCL-Headers
- Source: https://github.com/KhronosGroup/OpenCL-Headers
- Repository used: OpenCL-Headers/
- License: Apache License 2.0 (see OpenCL-Headers/LICENSE)

## Notes
- Ensure gpu_bench.c exists in the project directory before building.
- If your source filename is different, replace gpu_bench.c accordingly.
