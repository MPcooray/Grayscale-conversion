# Grayscale Conversion

Simple C programs to convert a color `PPM` image to a grayscale `PGM` image. This repository contains serial, OpenMP and MPI implementations for educational and benchmarking purposes.

**Overview**
- `grayscale_serial.c` — single-threaded converter
- `grayscale_openmp.c` — OpenMP-parallel converter
- `grayscale_mpi.c` — MPI-parallel converter
- helper scripts: `convert_to_ppm.py`, `pgm_to_jpg.py`, `pgm_to_png.py`

**Requirements**
- `gcc` with OpenMP support (for OpenMP build)
- an MPI implementation with `mpicc` and `mpirun` (for MPI build)
- (Optional) `ImageMagick` or Python3 + Pillow for image conversions

**Build**
Build everything using the provided `Makefile`:
```bash
make
```
Or build manually:
```bash
gcc -o grayscale_serial grayscale_serial.c
gcc -fopenmp -o grayscale_openmp grayscale_openmp.c
mpicc -o grayscale_mpi grayscale_mpi.c
```

**Run examples**
Input expected: a color `PPM` file (binary `P6`). Output: a grayscale `PGM` file (binary `P5`).

Serial:
```bash
./grayscale_serial myphoto.ppm myphoto_gray_serial.pgm
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

