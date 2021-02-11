#ifndef PTL_GRAPHICS_H
#define PTL_GRAPHICS_H


/* ----INCLUDES---- */
#include <stdlib.h>
#include <stdio.h>

/* DEFINES */
#define CSI "\x1b["


#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)

#define CLEAR_SCREEN system("clear")

#elif defined(_WIN32)

#define CLEAR_SCREEN system("CLS")

#endif

/* DATA */
typedef struct ptlPixel{
    int x, y;
    char pixelChar;
} ptlPoint;

typedef struct ptlRaster{
    int width, height;
    char bgChar;
    char* pixels;
} ptlRaster;

/* ----drawing---- */


void ptlClearScreen(){
    printf("%s1;1f", CSI); //Set cursor position to 1,1 (0,0 doesn't exist </3 )
    printf("%s1000X", CSI); //For the next 1000 positions, erase it.
    printf("%s1;1f", CSI); //set the cursor BACK to position 1,1
}

void ptlInitRaster(ptlRaster* r, int width, int height, char bgChar) {
    r->width = width;
    r->height = height;
    r->bgChar = bgChar;
    r->pixels = malloc(width * height * sizeof(char));
    
    for (int i = 0; i < width * height; i++){
        r->pixels[i] = bgChar;
    }
}

void ptlDrawText(ptlRaster* raster, int x, int y, char* text){
    for (int i = 0; text[i] != '\0'; i++){
        raster->pixels[y * raster->width + x + i] = text[i];
    }
}

void ptlDrawPixel(ptlRaster* raster, char pixelChar, int x, int y) {
    raster->pixels[y * raster->width + x] = pixelChar;
}

void ptlRemovePixel(ptlRaster* raster, int x, int y) {
    raster->pixels[y * raster->width + x] = raster->bgChar;
}

/*
void ptlDrawLine(ptlRaster raster, ptlPixel startPixel, ptlPixel endPixel) {
    
}
*/

void ptlRepaint(ptlRaster* raster) {
    ptlClearScreen();
    
    for (int i = 0; i < raster->height; i++){
        printf("\r\n");
        for (int j = 0; j < raster->width; j++){
            printf("%c", raster->pixels[i * raster->width + j]);
        }
    }

    
}

#endif
