# Makefile for grayscale project (macOS M1/M2/M4 friendly)
# Usage:
#   make                # build serial, openmp and mpi targets
#   make serial         # build serial
#   make openmp         # build OpenMP version (uses CC and OPENMP_FLAGS)
#   make mpi            # build MPI version (uses MPICC)
#   make run_serial     # run serial with input.ppm -> out_serial.pgm
#   OMP_NUM_THREADS=8 make run_openmp   # run OpenMP with 8 threads
#   make run_mpi NP=4   # run MPI with 4 processes
#   make clean

# default compilers (you can override: make CC=gcc-13 MPICC=mpicc)
CC ?= gcc-13
MPICC ?= mpicc

CFLAGS ?= -O3 -Wall
OPENMP_FLAGS ?= -fopenmp
LDFLAGS ?=

# source files (edit if filenames differ)
SRC_SERIAL = grayscale_serial.c
SRC_OPENMP = grayscale_openmp.c
SRC_MPI = grayscale_mpi.c

BIN_SERIAL = grayscale_serial
BIN_OPENMP = grayscale_openmp
BIN_MPI = grayscale_mpi

# Detect if running on macOS brew-installed paths (optional)
# You may override CC on command line if your gcc is named differently.
# Example: make CC=gcc-13 openmp

.PHONY: all serial openmp mpi clean run_serial run_openmp run_mpi check_deps

all: serial openmp mpi

serial: $(SRC_SERIAL)
	@echo "[make] compiling serial with $(CC)"
	$(CC) $(CFLAGS) $(SRC_SERIAL) -o $(BIN_SERIAL) $(LDFLAGS)

openmp: $(SRC_OPENMP)
	@echo "[make] compiling OpenMP with $(CC) ($(OPENMP_FLAGS))"
	$(CC) $(CFLAGS) $(OPENMP_FLAGS) $(SRC_OPENMP) -o $(BIN_OPENMP) $(LDFLAGS)

mpi: $(SRC_MPI)
	@echo "[make] compiling MPI with $(MPICC)"
	$(MPICC) $(CFLAGS) $(SRC_MPI) -o $(BIN_MPI) $(LDFLAGS)

# Run helpers ---------------------------------------------------------------
# These assume the input file is input.ppm in current folder.
run_serial:
	@if [ ! -f input.ppm ]; then echo "Error: input.ppm not found in current folder"; exit 1; fi
	./$(BIN_SERIAL) input.ppm out_serial.pgm

run_openmp:
	@if [ ! -f input.ppm ]; then echo "Error: input.ppm not found"; exit 1; fi
	@if [ -z "$(OMP_NUM_THREADS)" ]; then echo "Please set OMP_NUM_THREADS, e.g. export OMP_NUM_THREADS=8"; exit 1; fi
	@echo "[run] OMP_NUM_THREADS=$$OMP_NUM_THREADS"
	./$(BIN_OPENMP) input.ppm out_omp_$$OMP_NUM_THREADS.pgm

# Use: make run_mpi NP=4
run_mpi:
	@if [ ! -f input.ppm ]; then echo "Error: input.ppm not found"; exit 1; fi
	@if [ -z "$(NP)" ]; then echo "Please pass NP, e.g. make run_mpi NP=4"; exit 1; fi
	mpirun -np $(NP) ./$(BIN_MPI) input.ppm out_mpi_$(NP).pgm

# convenience target to print environment & check installed compilers
check_deps:
	@echo "Detected platform: $(shell uname -a)"
	@echo "CC = $(CC)"
	@echo "MPICC = $(MPICC)"
	@echo -n "which $(CC): " ; command -v $(CC) || echo "NOT FOUND"
	@echo -n "which mpicc: " ; command -v $(MPICC) || echo "NOT FOUND"
	@echo -n "which mpirun: " ; command -v mpirun || echo "NOT FOUND"
	@echo -n "OMP_NUM_THREADS (env): " ; echo $$OMP_NUM_THREADS
	@echo "If OpenMP build fails with clang, install gcc via Homebrew: brew install gcc"

clean:
	@rm -f $(BIN_SERIAL) $(BIN_OPENMP) $(BIN_MPI) *.pgm *.png *.jpg *.o
	@echo "Cleaned binaries and image outputs"
