

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

/* P6 reader. */
unsigned char* read_ppm_simple(const char *name, int *w, int *h) {
    FILE *f = fopen(name, "rb");
    if (!f) { perror("fopen"); return NULL; }
    char magic[3]; fscanf(f, "%2s", magic);        
    int width, height, maxv;
    fscanf(f, "%d %d %d", &width, &height, &maxv);
    fgetc(f); // skip single whitespace
    size_t n = (size_t)width * height * 3;
    unsigned char *buf = malloc(n);
    if (!buf) { fclose(f); return NULL; }
    fread(buf, 1, n, f);
    fclose(f);
    *w = width; *h = height;
    return buf;
}

/* P5 writer */
int write_pgm_simple(const char *name, unsigned char *gray, int w, int h) {
    FILE *f = fopen(name, "wb");
    if (!f) { perror("fopen"); return -1; }
    fprintf(f, "P5\n%d %d\n255\n", w, h);
    fwrite(gray, 1, (size_t)w * h, f);
    fclose(f);
    return 0;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (argc != 3) {
        if (rank == 0) fprintf(stderr, "Usage: %s in.ppm out.pgm\n", argv[0]);
        MPI_Finalize(); return 1;
    }

    int width = 0, height = 0;
    unsigned char *full_rgb = NULL;

    if (rank == 0) {
        full_rgb = read_ppm_simple(argv[1], &width, &height);
        if (!full_rgb) { MPI_Abort(MPI_COMM_WORLD, 1); }
    }

    /* Broadcast image size to all ranks */
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Decide how many rows per rank */
    int base = height / nprocs;
    int rem = height % nprocs;

    int *sendcounts = malloc(nprocs * sizeof(int));
    int *displs = malloc(nprocs * sizeof(int));
    int *gcount = malloc(nprocs * sizeof(int));
    int *gdisp = malloc(nprocs * sizeof(int));

    int off = 0, offg = 0;
    for (int r = 0; r < nprocs; ++r) {
        int rows = base + (r < rem ? 1 : 0);
        sendcounts[r] = rows * width * 3;   // bytes of RGB
        displs[r] = off;
        gcount[r] = rows * width;          // bytes of gray
        gdisp[r] = offg;
        off += sendcounts[r];
        offg += gcount[r];
    }

    int mybytes = sendcounts[rank];
    unsigned char *myrgb = malloc(mybytes);
    MPI_Scatterv(full_rgb, sendcounts, displs, MPI_UNSIGNED_CHAR,
                 myrgb, mybytes, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    int myrows = mybytes / (width * 3);
    unsigned char *mygray = malloc((size_t)myrows * width);

    /* compute grayscale locally */
    double t0 = MPI_Wtime();
    for (int r = 0, px = 0, gpos = 0; r < myrows; ++r) {
        for (int c = 0; c < width; ++c, px += 3, ++gpos) {
            unsigned char R = myrgb[px], G = myrgb[px+1], B = myrgb[px+2];
            int L = (int)(0.21f*R + 0.72f*G + 0.07f*B + 0.5f);
            if (L < 0) L = 0; if (L > 255) L = 255;
            mygray[gpos] = (unsigned char)L;
        }
    }
    double t1 = MPI_Wtime();
    double local = t1 - t0;

    /* gather timings and result */
    double *times = NULL;
    if (rank == 0) times = malloc(nprocs * sizeof(double));
    MPI_Gather(&local, 1, MPI_DOUBLE, times, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    unsigned char *full_gray = NULL;
    if (rank == 0) full_gray = malloc((size_t)width * height);
    MPI_Gatherv(mygray, gcount[rank], MPI_UNSIGNED_CHAR,
                full_gray, gcount, gdisp, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        /* print simple timing summary */
        double max = 0;
        for (int i = 0; i < nprocs; ++i) {
            printf("rank %d: %.6f s\n", i, times[i]);
            if (times[i] > max) max = times[i];
        }
        printf("max local compute = %.6f s\n", max);
        write_pgm_simple(argv[2], full_gray, width, height);
        printf("Wrote %s (%dx%d)\n", argv[2], width, height);
    }

    /* free and finish */
    free(myrgb); free(mygray);
    free(sendcounts); free(displs); free(gcount); free(gdisp);
    if (full_rgb) free(full_rgb);
    if (full_gray) free(full_gray);
    if (times) free(times);

    MPI_Finalize();
    return 0;
}



// Compile: mpicc -O3 grayscale_mpi.c -o grayscale_mpi
// Run:     mpirun -np 4 ./grayscale_mpi myphoto.ppm myphoto_gray_mpi.pgm
//Convert ppm to jpg : python3 pgm_to_jpg.py myphoto_gray_mpi.pgm 