# Grayscale Conversion (Full Instructions)

Small, educational programs that convert a color `PPM` image (P6) to a grayscale `PGM` image (P5).

**Overview**
- `grayscale_serial.c` — single-threaded converter
- `grayscale_openmp.c` — OpenMP-parallel converter
- `grayscale_mpi.c` — MPI-parallel converter
- `grayscale_cuda.cu` — CUDA/GPU converter (optional)
- helper scripts: `convert_to_ppm.py`, `pgm_to_jpg.py`, `pgm_to_png.py`

**Input / Output formats**
- Input: binary `PPM` (P6) color images.
- Output: binary `PGM` (P5) grayscale images.

**Requirements**
- `gcc` (or `clang`) for C builds
- OpenMP support in the compiler for the OpenMP build (`-fopenmp`)
- An MPI implementation (`mpicc`, `mpirun`) for the MPI build
- (Optional) NVIDIA CUDA Toolkit (`nvcc`) and a CUDA-capable GPU for the CUDA build
- (Optional) `ImageMagick` or Python + Pillow for converting PGM → JPG/PNG

**Build (quick)**
Build everything using the provided `Makefile`:

```bash
make
```

**Manual build commands**

```bash
# Serial
gcc -O2 -o grayscale_serial grayscale_serial.c

# OpenMP
gcc -O2 -fopenmp -o grayscale_openmp grayscale_openmp.c

# MPI
mpicc -O2 -o grayscale_mpi grayscale_mpi.c

# CUDA (if you have a `grayscale_cuda.cu`):
nvcc -O2 -arch=sm_60 -o grayscale_cuda grayscale_cuda.cu
```

Notes:
- Choose the appropriate `-arch`/compute capability for your GPU (e.g. `sm_60`, `sm_75`).
- Use `-O2` (or `-O3`) for benchmarking; add `-g` if you need debugging symbols.

**Makefile suggestion**
If you'd like a `make cuda` target, add something like:

```makefile
cuda:
	@nvcc -O2 -arch=sm_60 -o grayscale_cuda grayscale_cuda.cu
```

**Run examples**

Serial:

```bash
./grayscale_serial myphoto.ppm myphoto_gray_serial.pgm
```

OpenMP (set threads via `OMP_NUM_THREADS`):

```bash
export OMP_NUM_THREADS=8
./grayscale_openmp myphoto.ppm myphoto_gray_omp.pgm
```

MPI (example using 4 processes):

```bash
mpirun -np 4 ./grayscale_mpi myphoto.ppm myphoto_gray_mpi_4.pgm
```

CUDA:

```bash
./grayscale_cuda myphoto.ppm myphoto_gray_cuda.pgm
```

**Benchmarking & profiling tips**
- For timing: use `time ./grayscale_*` or add microsecond timers in the code (e.g., `gettimeofday()` or `clock_gettime`).
- OpenMP: try different `OMP_NUM_THREADS` values and pin threads if needed; measure scalability.
- MPI: run with different `-np` counts; ensure the input image is large enough to amortize MPI overhead.
- CUDA: profile with `nvprof`, `nsys`, or Nsight; watch host↔device transfer times and kernel occupancy.

**Why use PGM (P5) instead of JPG?**
- `PGM` is trivial to implement and is lossless; it keeps exact pixel values which is important for image-processing correctness and fair benchmarking. `JPEG` is lossy and requires linking heavier codec libraries.

**Converting to JPG/PNG**
With ImageMagick:

```bash
convert myphoto_gray_serial.pgm myphoto_gray_serial.jpg
convert myphoto_gray_serial.pgm myphoto_gray_serial.png
```

Or with the included Python helpers:

```bash
python3 pgm_to_jpg.py myphoto_gray_serial.pgm
python3 pgm_to_png.py myphoto_gray_serial.pgm
```

**Troubleshooting**
- If compilation fails for MPI, ensure `mpicc` is installed and on PATH.
- For CUDA builds, ensure the CUDA toolkit version matches your driver and GPU.
- Always open PPM/PGM files in binary mode in code (`"rb"`/`"wb"`).

**License**
No license specified. Add one if you plan to publish or share this code.
