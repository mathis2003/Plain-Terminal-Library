#ifndef PTL_GRAPHICS_H
#define PTL_GRAPHICS_H


/* ----INCLUDES---- */
#include <stdlib.h>
#include <stdio.h>

/* DEFINES */
#if defined(unix) || defined(__unix) || defined(__unix__)

#define CLRSCREEN printf("\033[2J");

#elif defined(_WIN32)

#define CLRSCREEN CLS

#endif

/* DATA */
typedef struct ptlPixel{
    int x, y;
    char pixelChar;
} ptlPoint;

typedef struct ptlRaster{
    int width, height;
    char bgChar;
    struct ptlPixel* pixels;
} ptlRaster;

/* ----drawing---- */

void ptlInitRaster(ptlRaster* r, int width, int height, char bgChar) {
    r->width = width;
    r->height = height;
    r->bgChar = bgChar;
    r->pixels = malloc(width * height * sizeof(struct ptlPixel));
    for (int i = 0; i < width * height; i++){
        r->pixels[i].x = i % width;
        r->pixels[i].y = (i - r->pixels[i].x) / width;
        r->pixels[i].pixelChar = bgChar;
    }
}

void ptlDrawPixel(ptlRaster* raster, struct ptlPixel pixel) {
    raster->pixels[pixel.y * raster->width + pixel.x] = pixel;
}

void ptlRemovePixel(ptlRaster* raster, struct ptlPixel pixel) {
    raster->pixels[pixel.y * raster->width + pixel.x].pixelChar = raster->bgChar;
}

/*
void ptlDrawLine(ptlRaster raster, ptlPixel startPixel, ptlPixel endPixel) {
    
}
*/

void ptlRepaint(ptlRaster* raster) {
    printf("\033[2J");
    
    for (int i = 0; i < raster->height; i++){
        printf("\r\n");
        for (int j = 0; j < raster->width; j++){
            printf("%c", raster->pixels[i * raster->width + j].pixelChar);
        }
    }

    
}

#endif
