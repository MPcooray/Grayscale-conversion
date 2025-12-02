# Grayscale Conversion

Simple C programs to convert a color `PPM` image to a grayscale `PGM` image. This repository contains serial, OpenMP and MPI implementations for educational and benchmarking purposes.

**Overview**
- `grayscale_cuda.cu` — CUDA/GPU converter (optional)

**Requirements**
- (Optional) NVIDIA CUDA Toolkit with `nvcc` and a CUDA-capable GPU (for CUDA build)

**Build**
Build everything using the provided `Makefile`:
```bash
make
```
Or build manually:
# If you have a CUDA implementation (file named e.g. `grayscale_cuda.cu`):
# compile with nvcc
nvcc -o grayscale_cuda grayscale_cuda.cu
```bash
gcc -o grayscale_serial grayscale_serial.c
gcc -fopenmp -o grayscale_openmp grayscale_openmp.c
mpicc -o grayscale_mpi grayscale_mpi.c
```

**Run examples**

CUDA (GPU) example
-------------------
If a CUDA implementation is present (commonly `grayscale_cuda.cu`), build with `nvcc` as shown above and run the binary directly. The program typically accepts the same arguments as the CPU versions:

```bash
./grayscale_cuda myphoto.ppm myphoto_gray_cuda.pgm
```

Notes for CUDA:
- **Device & drivers:** Ensure the NVIDIA driver and CUDA toolkit are installed and compatible with your GPU.
- **nvcc:** Use the `nvcc` compiler from the CUDA toolkit to build `.cu` sources.
- **Profiling:** Use `nvprof`, `nsys`, or Nsight tools to profile kernels and memory transfers.
- **I/O vs compute:** For small images the I/O and transfer costs can dominate; use sufficiently large images when benchmarking GPU performance.
- **Environment:** You can force a specific GPU with `CUDA_VISIBLE_DEVICES=0` or use device query code inside the program.
Input expected: a color `PPM` file (binary `P6`). Output: a grayscale `PGM` file (binary `P5`).

```

OpenMP:
```bash
./grayscale_openmp myphoto.ppm myphoto_gray_omp.pgm
```

MPI (4 processes):
```bash
mpirun -np 4 ./grayscale_mpi myphoto.ppm myphoto_gray_mpi_4.pgm
```

**Why `PGM/P5`?**
- `PGM (P5)` is a tiny, lossless binary format that is trivial to read/write from C/Python. It preserves exact pixel values and avoids the complexity and lossy compression of JPEG, which is important for image-processing experiments and benchmarking.

**Converting to JPG/PNG**
Use ImageMagick:
```bash
convert myphoto_gray_serial.pgm myphoto_gray_serial.jpg
convert myphoto_gray_serial.pgm myphoto_gray_serial.png
```
Or use the provided Python scripts:
```bash
python3 pgm_to_jpg.py myphoto_gray_serial.pgm
python3 pgm_to_png.py myphoto_gray_serial.pgm
```

