#ifndef PTLIB_H
#define PTLIB_H


/* ---------------------------COMMONS--------------------------- */


void ptlDie(const char* s){
    printf("\n\r");
    perror(s);
    exit(1);
}




/* ---------------------------GRAPHICS--------------------------- */



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

typedef struct ptlPixel {
    int x, y;
    char pixelChar;
} ptlPixel;

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
    
    for (int i = 0; i < width * height; i++)
        r->pixels[i] = bgChar;
}

void ptlDrawText(ptlRaster* raster, int x, int y, char* text){
    for (int i = 0; text[i] != '\0'; i++) {
        if (i >= raster->width * raster->height) ptlDie("index out of bounds when calling ptlDrawText()");
        raster->pixels[y * raster->width + x + i] = text[i];
    }
    
}

void ptlDrawPixel(ptlRaster* raster, char pixelChar, int x, int y) {
    if (x * y >= raster->width * raster->height) ptlDie("index out of bounds when calling ptlDrawPixel()");
    raster->pixels[y * raster->width + x] = pixelChar;
}

void ptlRemovePixel(ptlRaster* raster, int x, int y) {
    if (x * y >= raster->width * raster->height) ptlDie("index out of bounds when calling ptlRemovePixel()");
    raster->pixels[y * raster->width + x] = raster->bgChar;
}


void ptlDrawLine(ptlRaster* raster, char pixelChar, int start_x, int start_y, int end_x, int end_y) {
    if ((start_y * raster->width + start_x >= raster->width * raster->height) ||
        (end_y * raster->width + end_x >= raster->width * raster->height)) ptlDie("index out of bounds when calling ptlDrawLine()");
    
    
    
    
    int widthDif = start_x < end_x ? end_x - start_x : start_x - end_x;
    int heightDif = start_y < end_y ? end_y - start_y : start_y - end_y;
    
    if (!widthDif){
        for (int i = 0; i < heightDif; i++)
            ptlDrawPixel(raster, pixelChar, start_x, start_y + i);
        return;
    } else if (!heightDif){
        for (int i = 0; i < widthDif; i++)
            ptlDrawPixel(raster, pixelChar, start_x + i, start_y);
        return;
    }
    
    if (heightDif < widthDif) {
        
        int extra_pixel_amount = widthDif % heightDif;
        
        int x_offset = 0;
        for (int y_offset = 0; y_offset < heightDif; y_offset++){
            for (int i = 0; i < (int)(widthDif / heightDif); i++){
                x_offset++;
                ptlDrawPixel(raster, pixelChar, start_x + x_offset, start_y + y_offset);
            }
            if (y_offset < extra_pixel_amount) {
                x_offset++;
                ptlDrawPixel(raster, pixelChar, start_x + x_offset, start_y + y_offset);
            }
        }
    } else {
        int extra_pixel_amount = heightDif % widthDif;
        
        int y_offset = 0;
        for (int x_offset = 0; x_offset < widthDif; x_offset++){
            for (int i = 0; i < (int)(heightDif / widthDif); i++){
                y_offset++;
                ptlDrawPixel(raster, pixelChar, start_x + x_offset, start_y + y_offset);
            }
            if (x_offset < extra_pixel_amount) {
                y_offset++;
                ptlDrawPixel(raster, pixelChar, start_x + x_offset, start_y + y_offset);
            }
        }
    }
}



void ptlDrawRect(ptlRaster* raster, char pixelChar, int width, int height, int x, int y){
    ptlDrawLine(raster, pixelChar, x, y, x + width, y);
    ptlDrawLine(raster, pixelChar, x, y, x, y + height);
    ptlDrawLine(raster, pixelChar, x + width, y, x + width, y + height);
    ptlDrawLine(raster, pixelChar, x, y + height - 1, x + width, y + height - 1);
}


void ptlRepaint(ptlRaster* raster) {
    ptlClearScreen();
    
    for (int i = 0; i < raster->height; i++){
        printf("\r\n");
        for (int j = 0; j < raster->width; j++)
            printf("%c", raster->pixels[i * raster->width + j]);
        
    }
    
    printf("\r\n");
    
}



/* ---------------------------INPUT--------------------------- */




/* ----INCLUDES---- */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/* ----DEFINES---- */

#define PRINT_PRESSED_KEY(c) printf("%c\r\n", c);

#if defined(unix) || defined(__unix) || defined(__unix__)

#define HIDE_CURSOR printf("\x1b[?25l");

#define UP_ARROW_SEQ ""
#define DOWN_ARROW_SEQ ""
#define LEFT_ARROW_SEQ ""
#define RIGHT_ARROW_SEQ ""
#define SHIFT_SEQ ""
#define ESCAPE_SEQ ""

#elif defined(_WIN32)

#define HIDE_CURSOR printf("");

#define UP_ARROW_SEQ ""
#define DOWN_ARROW_SEQ ""
#define LEFT_ARROW_SEQ ""
#define RIGHT_ARROW_SEQ ""
#define SHIFT_SEQ ""
#define ESCAPE_SEQ ""


#endif

/* ----DATA---- */

struct termios origTermios;

enum keyCodes{
    
    KEYCODE_1 = '1', KEYCODE_2, KEYCODE_3, KEYCODE_4, KEYCODE_5, KEYCODE_6, KEYCODE_7, KEYCODE_8, KEYCODE_9,
    
    KEYCODE_A = 'a', KEYCODE_B, KEYCODE_C, KEYCODE_D, KEYCODE_E, KEYCODE_F, KEYCODE_G, KEYCODE_H, KEYCODE_I, KEYCODE_J, KEYCODE_K, KEYCODE_L, KEYCODE_M, KEYCODE_N,
    KEYCODE_O, KEYCODE_P, KEYCODE_Q, KEYCODE_R, KEYCODE_S, KEYCODE_T, KEYCODE_U, KEYCODE_V, KEYCODE_W, KEYCODE_X, KEYCODE_Y, KEYCODE_Z,
    
    KEYCODE_TAB = '\t', KEYCODE_ENTER = '\n', KEYCODE_SPACE = ' ',
    
    KEYCODE_UP_ARROW = 1000, KEYCODE_DOWN_ARROW = 1001, KEYCODE_LEFT_ARROW = 1002, KEYCODE_RIGHT_ARROW, KEYCODE_SHIFT = 1003, KEYCODE_ESCAPE = 1004
};

/* ----TERMINAL---- */


void ptlDisableRawMode(){
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios) == -1) ptlDie("tcsetattr");
}

void ptlEnableRawMode(int updateTime){
        if(tcgetattr(STDIN_FILENO, &origTermios) == -1) ptlDie("tcgetattr");
        atexit(ptlDisableRawMode);

        struct termios raw = origTermios;

        raw.c_iflag &= ~(BRKINT| ICRNL| INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = updateTime;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) ptlDie("tcsetattr");
        
}

int ptlPressedKey(){
    int keyCode = '\0';
    if (read(STDIN_FILENO, &keyCode, 1) == -1 && errno != EAGAIN) ptlDie("read");
    //if (iscntrl(key_code));
    return keyCode;
}








#endif
