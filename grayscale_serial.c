

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

static double now_seconds(void){
    struct timeval t; gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec*1e-6;
}

/* Read P6 PPM (binary RGB). */
unsigned char* read_ppm(const char* fname, int *w, int *h){
    FILE *f = fopen(fname,"rb");
    if(!f){ perror("fopen"); return NULL; }

    // Read magic (first token)
    char magic[3] = {0};
    if (fscanf(f, "%2s", magic) != 1) { fclose(f); return NULL; }
    if (magic[0] != 'P' || magic[1] != '6') {
        fprintf(stderr, "Error: not a P6 PPM file (magic=%s)\n", magic);
        fclose(f); return NULL;
    }

    // Skip whitespace and comments, read width, height, maxval
    int width=0, height=0, maxv=0;
    int c = fgetc(f); // consume single whitespace after magic token
    while (c == '\n' || c == '\r' || c == ' ' || c == '\t') c = fgetc(f);
    
    while (c == '#') {
        
        while (c != '\n' && c != EOF) c = fgetc(f);
        // skip whitespace to next token
        c = fgetc(f);
        while (c == '\n' || c == '\r' || c == ' ' || c == '\t') c = fgetc(f);
    }
    ungetc(c, f); // put back the first non-comment char

    if (fscanf(f, "%d %d %d", &width, &height, &maxv) != 3) {
        fprintf(stderr, "Error reading width/height/maxv\n");
        fclose(f); return NULL;
    }
    if (maxv != 255) {
        
        fprintf(stderr, "Warning: maxv != 255 (maxv=%d). Values will be scaled.\n", maxv);
    }
    // consume single whitespace (one byte) after header to reach binary data
    int sep = fgetc(f);
    if (sep == EOF) { fclose(f); return NULL; }

    size_t nbytes = (size_t)width * (size_t)height * 3;
    unsigned char *buf = (unsigned char*) malloc(nbytes);
    if (!buf) { fprintf(stderr,"malloc failed\n"); fclose(f); return NULL; }

    size_t read = fread(buf, 1, nbytes, f);
    if (read != nbytes) {
        fprintf(stderr, "Error: expected %zu bytes, read %zu bytes\n", nbytes, read);
        free(buf); fclose(f); return NULL;
    }
    fclose(f);
    *w = width; *h = height;
    return buf;
}

/* Write P5 PGM (binary grayscale). gray must be w*h bytes. */
int write_pgm(const char* fname, unsigned char *g, int w, int h){
    FILE *f = fopen(fname,"wb"); if(!f){ perror("fopen"); return -1; }
    if (fprintf(f,"P5\n%d %d\n255\n", w, h) < 0) { fclose(f); return -1; }
    size_t wrote = fwrite(g, 1, (size_t)w * h, f);
    fclose(f);
    if (wrote != (size_t)w * h) { fprintf(stderr,"fwrite error\n"); return -1; }
    return 0;
}

int main(int argc, char** argv){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s in.ppm out.pgm\n", argv[0]);
        return 1;
    }

    int w=0, h=0;
    unsigned char *rgb = read_ppm(argv[1], &w, &h);
    if (!rgb) return 1;

    size_t pixels = (size_t)w * h;
    unsigned char *gray = (unsigned char*) malloc(pixels);
    if (!gray) { perror("malloc"); free(rgb); return 1; }

    double t0 = now_seconds();
    for (size_t i = 0, px = 0; i < pixels; ++i, px += 3) {
        unsigned char r = rgb[px];
        unsigned char gch = rgb[px + 1];
        unsigned char b = rgb[px + 2];
        float L = 0.21f * r + 0.72f * gch + 0.07f * b;
        int v = (int)(L + 0.5f);
        if (v < 0) v = 0;
        if (v > 255) v = 255;
        gray[i] = (unsigned char)v;
    }
    double t1 = now_seconds();
    printf("Serial compute time: %.6f s\n", t1 - t0);

    if (write_pgm(argv[2], gray, w, h) != 0) {
        fprintf(stderr, "Failed to write output file\n");
    } else {
        printf("Wrote %s (%dx%d)\n", argv[2], w, h);
    }

    free(rgb); free(gray);
    return 0;
}



// Compile: gcc -O3 grayscale_serial.c -o grayscale_serial
// Run:     ./grayscale_serial myphoto.ppm myphoto_gray_serial.pgm
// Convert ppm to jpg : python3 pgm_to_jpg.py myphoto_gray_serial.pgm