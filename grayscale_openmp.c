// grayscale_openmp_simple.c
// Simple OpenMP grayscale converter (student-friendly)
// Compile: gcc-15 -O3 -fopenmp grayscale_openmp_simple.c -o grayscale_openmp_simple
// Run:   OMP_NUM_THREADS=4 ./grayscale_openmp_simple input.ppm output.pgm

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

/* Simple P6 PPM reader (assumes well-formed file). Returns malloc'd RGB buffer. */
unsigned char* read_ppm_simple(const char *name, int *w, int *h) {
    FILE *f = fopen(name, "rb");
    if (!f) { perror("fopen"); return NULL; }
    char magic[3]; fscanf(f, "%2s", magic);      // expect "P6"
    int width, height, maxv;
    // skip comments & read width height maxv in a simple way
    fscanf(f, "%d %d %d", &width, &height, &maxv);
    fgetc(f); // consume single whitespace after header
    size_t n = (size_t)width * height * 3;
    unsigned char *buf = malloc(n);
    if (!buf) { fclose(f); return NULL; }
    fread(buf, 1, n, f);
    fclose(f);
    *w = width; *h = height;
    return buf;
}

/* Simple P5 PGM writer */
int write_pgm_simple(const char *name, unsigned char *gray, int w, int h) {
    FILE *f = fopen(name, "wb");
    if (!f) { perror("fopen"); return -1; }
    fprintf(f, "P5\n%d %d\n255\n", w, h);
    fwrite(gray, 1, (size_t)w * h, f);
    fclose(f);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s in.ppm out.pgm\n", argv[0]);
        return 1;
    }

    int w=0, h=0;
    unsigned char *rgb = read_ppm_simple(argv[1], &w, &h);
    if (!rgb) return 1;

    size_t pixels = (size_t)w * h;
    unsigned char *gray = malloc(pixels);
    if (!gray) { free(rgb); return 1; }

    // --- Parallel region: each iteration handles one pixel (data-parallel)
    double t0 = omp_get_wtime();            // lecture: use omp_get_wtime for timings
    #pragma omp parallel for schedule(static)
    for (size_t i = 0; i < pixels; ++i) {
        size_t off = i * 3;
        unsigned char r = rgb[off];
        unsigned char g = rgb[off + 1];
        unsigned char b = rgb[off + 2];
        // weighted luminance (perceptual)
        float L = 0.21f * r + 0.72f * g + 0.07f * b;
        int v = (int)(L + 0.5f);
        if (v < 0) v = 0; if (v > 255) v = 255;
        gray[i] = (unsigned char)v;
    }
    double t1 = omp_get_wtime();

    printf("OpenMP time: %.6f s (threads=%d)\n", t1 - t0, omp_get_max_threads());

    write_pgm_simple(argv[2], gray, w, h);

    free(rgb);
    free(gray);
    return 0;
}
