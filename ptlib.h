/*
 * MIT License
 *
 * Copyright (c) 2021 mathis2003
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */



#ifndef PTLIB_H
#define PTLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define HIDE_CURSOR printf("\x1b[?25l")
#define SHOW_CURSOR printf("\x1b[?25h")


enum keyCodes {

    KEYCODE_0 = '0', KEYCODE_1, KEYCODE_2, KEYCODE_3, KEYCODE_4, KEYCODE_5, KEYCODE_6, KEYCODE_7, KEYCODE_8, KEYCODE_9,

    KEYCODE_A = 'a', KEYCODE_B, KEYCODE_C, KEYCODE_D, KEYCODE_E, KEYCODE_F, KEYCODE_G, KEYCODE_H, KEYCODE_I, KEYCODE_J, KEYCODE_K, KEYCODE_L, KEYCODE_M,
    KEYCODE_N, KEYCODE_O, KEYCODE_P, KEYCODE_Q, KEYCODE_R, KEYCODE_S, KEYCODE_T, KEYCODE_U, KEYCODE_V, KEYCODE_W, KEYCODE_X, KEYCODE_Y, KEYCODE_Z,

    KEYCODE_TAB = '\t', KEYCODE_ENTER = '\n', KEYCODE_SPACE = ' ',

    KEYCODE_UP_ARROW = 1000, KEYCODE_DOWN_ARROW = 1001, KEYCODE_LEFT_ARROW = 1002, KEYCODE_RIGHT_ARROW, KEYCODE_SHIFT = 1003, KEYCODE_ESCAPE = 1004
};

typedef void* ptlRaster;

void ptlDie(const char* s);
void ptlClearScreen(ptlRaster raster);
ptlRaster ptlInitRaster(int width, int height, char bgChar);
void ptlDestroyRaster(ptlRaster raster);
void ptlDrawText(ptlRaster raster, int x, int y, char* text);
void ptlDrawPixel(ptlRaster raster, char pixelChar, int x, int y);
void ptlRemovePixel(ptlRaster raster, int x, int y);
void ptlDrawLine(ptlRaster raster, char pixelChar, int start_x, int start_y, int end_x, int end_y);
void ptlDrawRect(ptlRaster raster, char pixelChar, int width, int height, int x, int y);
void ptlRepaint(ptlRaster raster);
int ptlPressedKey(ptlRaster raster);
int ptlGetWidth(ptlRaster raster);
int ptlGetHeight(ptlRaster raster);

#ifdef PTLIB_IMPL

void ptlDie(const char* s) {
    printf("\n\r");
    perror(s);
    exit(1);
}



/* -----UNIX----- */


#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)

#define CSI "\x1b["

/* ----defines---- */
#define CLEAR_SCREEN system("clear")

/* ----includes---- */

#include <ctype.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

/* ----functions---- */

void ptlClearScreen() {
    printf("%s1;1f", CSI); //Set cursor position to 1,1 (0,0 doesn't exist </3 )
    printf("%s1000X", CSI); //For the next 1000 positions, erase it.
    printf("%s1;1f", CSI); //set the cursor BACK to position 1,1
}

typedef struct ptlRaster_UNIX {
    struct termios origTermios;
    int width, height;
    char bgChar;
    char* pixels;
} ptlRaster_UNIX;

void ptlEnableRawMode(struct termios* t) {
    if (tcgetattr(STDIN_FILENO, t) == -1) ptlDie("tcgetattr");

    struct termios raw = *t;

    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) ptlDie("tcsetattr");
}

ptlRaster ptlInitRaster(int width, int height, char bgChar) {
    ptlRaster_UNIX* r = malloc(sizeof(ptlRaster_UNIX));
    if (tcgetattr(STDIN_FILENO, &(r->origTermios)) == -1) ptlDie("tcgetattr");
    r->width = width;
    r->height = height;
    r->bgChar = bgChar;
    r->pixels = malloc(width * height * sizeof(char));

    ptlEnableRawMode(&(r->origTermios));

    CLEAR_SCREEN;
    HIDE_CURSOR;
    
    for (int i = 0; i < width * height; i++)
        r->pixels[i] = bgChar;

    return (ptlRaster)r;
}

void ptlDisableRawMode(struct termios* t) {
    SHOW_CURSOR;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, t) == -1) ptlDie("tcsetattr, close this terminal window and open a new one");
}

void ptlDestroyRaster(ptlRaster raster) {
    ptlRaster_UNIX* r = (ptlRaster_UNIX*)raster;

    ptlDisableRawMode(&(r->origTermios));
    
    free(r->pixels);
    free(r);
    CLEAR_SCREEN;
    SHOW_CURSOR;
}

int ptlPressedKey(ptlRaster raster) {
    ptlRaster_UNIX* r = (ptlRaster_UNIX*)raster;
    int keyCode = '\0';
    
    if (read(STDIN_FILENO, &keyCode, 1) == -1 && errno != EAGAIN) {
        ptlDisableRawMode(&(r->origTermios));
        ptlDie("read");
    }
    
    if (keyCode == '['){
        if (read(STDIN_FILENO, &keyCode, 1) == -1 && errno != EAGAIN) { ptlDie("read"); }
        if (keyCode == 'A') keyCode = KEYCODE_UP_ARROW;
        else if (keyCode == 'B') keyCode = KEYCODE_DOWN_ARROW;
        else if (keyCode == 'C') keyCode = KEYCODE_RIGHT_ARROW;
        else if (keyCode == 'D') keyCode = KEYCODE_LEFT_ARROW;
    }
    
    return keyCode;
}

void ptlDrawText(ptlRaster raster, int x, int y, char* text) {
    ptlRaster_UNIX* r = (ptlRaster_UNIX*)raster;
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (i >= r->width * r->height) {
            ptlDisableRawMode(&(r->origTermios));
            ptlDie("index out of bounds when calling ptlDrawText()");
        }
        r->pixels[y * r->width + x + i] = text[i];
    }
}

void ptlDrawPixel(ptlRaster raster, char pixelChar, int x, int y) {
    ptlRaster_UNIX* r = (ptlRaster_UNIX*)raster;

    if (x + y * r->width > r->width * r->height) {
        ptlDisableRawMode(&(r->origTermios));
        
        printf("y: %d\n", y);
        printf("x: %d", x);

        ptlDie("index out of bounds when calling ptlDrawPixel()");
    }

    r->pixels[y * r->width + x] = pixelChar;
}

void ptlRemovePixel(ptlRaster raster, int x, int y) {
    
    ptlRaster_UNIX* r = (ptlRaster_UNIX*)raster;
    if (x + y * r->width >= r->width * r->height){
        ptlDisableRawMode(&(r->origTermios));
        ptlDie("index out of bounds when calling ptlRemovePixel()");
    }

    r->pixels[y * r->width + x] = r->bgChar;
}

void ptlDrawLine(ptlRaster raster, char pixelChar, int start_x, int start_y, int end_x, int end_y) {
    ptlRaster_UNIX* r = (ptlRaster_UNIX*)raster;
    
    // make sure start coordinates are coordinates of the left point, if not, change the coordinates
    if (start_x > end_x) {
        int temp_x = start_x, temp_y = start_y;
        start_x = end_x;
        end_x = temp_x;
        start_y = end_y;
        end_y = temp_y;
    }
    
    int delta_x = end_x - start_x, delta_y = end_y - start_y;
    // check for edge cases such as a vertical line, a horizontal line or a single point
    
    if (delta_x == 0 && delta_y == 0){
        //draw a single point
        ptlDrawPixel(raster, pixelChar, start_x, start_y);
    } else if (delta_x == 0){
        //draw a vertical line
        int step = (start_y < end_y ? 1 : -1);
        for (int y = start_y; y != end_y; y += step) {
            ptlDrawPixel(raster, pixelChar, start_x, y);
        }
    } else if (delta_y == 0){
        //draw a horizontal line
        for (int x = start_x; x != end_x; x++) {
            ptlDrawPixel(raster, pixelChar, x, start_y);
        }
    } else {
        // draw a slope line
        float slope = ((float) delta_y) / ((float) delta_x);
        float step;
        int prev_y = 0;
        
        if (slope > 0) {
            step = (slope > 1 ? 1/slope : slope);
            for (float x = start_x; x < end_x; x += step) {
                int y = (int)(slope * x) > prev_y ? (prev_y += 1) : prev_y;
                ptlDrawPixel(raster, pixelChar, (int)x, start_y + y);
            }
        } else {
            step = (slope < -1 ? 1/slope : slope);
            for (float x = start_x; x < end_x; x += step * -1) {
                int y = (int)(slope * x) < prev_y ? (prev_y -= 1) : prev_y;
                ptlDrawPixel(raster, pixelChar, (int)x, start_y + y);
            }
        }
        
    }
    
}

void ptlDrawRect(ptlRaster raster, char pixelChar, int width, int height, int x, int y) {
    ptlDrawLine(raster, pixelChar, x, y, x + width - 1, y);
    ptlDrawLine(raster, pixelChar, x + 1, y, x + 1, y + height - 2);
    ptlDrawLine(raster, pixelChar, x + width, y, x + width, y + height - 2);
    ptlDrawLine(raster, pixelChar, x, y + height - 1, x + width - 1, y + height - 1);
}

void ptlRepaint(ptlRaster raster) {
    ptlRaster_UNIX* r = (ptlRaster_UNIX*)raster;
    ptlClearScreen(raster);

    for (int i = 0; i < r->height; i++) {
        printf("\r\n");
        for (int j = 0; j < r->width; j++)
            printf("%c", r->pixels[i * r->width + j]);
    }

    printf("\r\n");

}

char ptlGetPixelChar(ptlRaster raster, int x, int y) {
    ptlRaster_UNIX* r = (ptlRaster_UNIX*)raster;
    return r->pixels[(r->width * y + x)];
}

int ptlGetWidth(ptlRaster raster) {
    ptlRaster_UNIX* r = (ptlRaster_UNIX*)raster;
    return r->width;
}

int ptlGetHeight(ptlRaster raster) {
    ptlRaster_UNIX* r = (ptlRaster_UNIX*)raster;
    return r->height;
}

#endif



/* -----WINDOWS----- */


#if defined(_WIN32)
#include <Windows.h>


#if defined(_WIN32)
#define CLEAR_SCREEN system("CLS")
#include <Windows.h>

typedef struct ptlRaster_Win32 {
    DWORD original_mode;
    HANDLE in, out;
    int width, height;
    char bgChar;
    char* pixels;
} ptlRaster_Win32;


/* --raster windows-- */

ptlRaster ptlInitRaster(int width, int height, char bgChar) {
    ptlRaster_Win32* r = malloc(sizeof(ptlRaster_Win32));
    if (!r) ptlDie("Out of memory");

    r->in = GetStdHandle(STD_INPUT_HANDLE);
    r->out = GetStdHandle(STD_OUTPUT_HANDLE);
    r->width = width;
    r->height = height;
    r->bgChar = bgChar;
    r->pixels = malloc(width * height * sizeof(char));
    SetConsoleScreenBufferSize(r->out, (COORD) { width, height });

    if (!GetConsoleMode(r->out, &r->original_mode)) ptlDie("Could not initialize PTL");

    DWORD mode = r->original_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(r->out, mode)) ptlDie("Could not initialize PTL");

    system("CLS");
    HIDE_CURSOR;

    for (int i = 0; i < width * height; i++)
        r->pixels[i] = bgChar;

    return (ptlRaster)r;
}

void ptlClearScreen(ptlRaster raster) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;

    DWORD bytes_written;
    WriteFile(r->out, "\x1b[1;1f\x1b[1000X\x1b[1;1f", 19, &bytes_written, NULL);
}

void ptlDestroyRaster(ptlRaster raster) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;
    SetConsoleMode(r->out, r->original_mode);

    CLEAR_SCREEN;
    SHOW_CURSOR;

    free(r->pixels);
    free(r);
}


/* --input windows-- */

int ptlPressedKey(ptlRaster raster) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;
    int pressed = 0;

    INPUT_RECORD input;
    DWORD inputs_read;
    PeekConsoleInputA(r->in, &input, 1, &inputs_read);
    if (inputs_read > 0) {
        if (input.EventType == KEY_EVENT && input.Event.KeyEvent.bKeyDown) {
            int vk_key = input.Event.KeyEvent.wVirtualKeyCode;

            if (vk_key >= 'A' && vk_key <= 'Z') {
                pressed = tolower(vk_key);
            } else if (vk_key >= '0' && vk_key <= '9') {
                pressed = vk_key;
            } else if (vk_key == '\r') {
                pressed = KEYCODE_ENTER;
            } else if (vk_key == '\t') {
                pressed = KEYCODE_TAB;
            } else if (vk_key == ' ') {
                pressed = KEYCODE_SPACE;
            } else if (vk_key == VK_UP) {
                pressed = KEYCODE_UP_ARROW;
            } else if (vk_key == VK_DOWN) {
                pressed = KEYCODE_DOWN_ARROW;
            } else if (vk_key == VK_LEFT) {
                pressed = KEYCODE_LEFT_ARROW;
            } else if (vk_key == VK_RIGHT) {
                pressed = KEYCODE_RIGHT_ARROW;
            } else if (vk_key == VK_SHIFT) {
                pressed = KEYCODE_SHIFT;
            } else if (vk_key == VK_ESCAPE) {
                pressed = KEYCODE_ESCAPE;
            }
        }
        FlushConsoleInputBuffer(r->in);
    }
    return pressed;
}

void ptlRepaint(ptlRaster raster) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;

    ptlClearScreen(raster);

    DWORD bytes_written;
    for (int j = 0; j < r->height; j++) {
        WriteFile(r->out, &r->pixels[j * r->width], r->width, &bytes_written, NULL);
        WriteFile(r->out, "\r\n", 2, &bytes_written, NULL);
    }
}

#endif





/* GRAPHICS DRAWING FUNCTIONS */


void ptlDrawText(ptlRaster raster, int x, int y, char* text) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;

    for (int i = 0; text[i] != '\0'; i++) {
        if (i >= r->width * r->height) ptlDie("index out of bounds when calling ptlDrawText()");
        r->pixels[y * r->width + x + i] = text[i];
    }
}

void ptlDrawPixel(ptlRaster raster, char pixelChar, int x, int y) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;

    if (x + y * r->width > r->width * r->height) {
        printf("y: %d\n", y);
        printf("x: %d", x);

        ptlDie("index out of bounds when calling ptlDrawPixel()");
    }
    r->pixels[y * r->width + x] = pixelChar;
}

void ptlRemovePixel(ptlRaster raster, int x, int y) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;

    if (x + y * r->width >= r->width * r->height)
        ptlDie("index out of bounds when calling ptlRemovePixel()");

    r->pixels[y * r->width + x] = r->bgChar;
}

void ptlDrawLine(ptlRaster raster, char pixelChar, int start_x, int start_y, int end_x, int end_y) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;
    
    // make sure start coordinates are coordinates of the left point, if not, change the coordinates
    if (start_x > end_x) {
        int temp_x = start_x, temp_y = start_y;
        start_x = end_x;
        end_x = temp_x;
        start_y = end_y;
        end_y = temp_y;
    }
    
    int delta_x = end_x - start_x, delta_y = end_y - start_y;
    // check for edge cases such as a vertical line, a horizontal line or a single point
    
    if (delta_x == 0 && delta_y == 0){
        //draw a single point
        ptlDrawPixel(raster, pixelChar, start_x, start_y);
    } else if (delta_x == 0){
        //draw a vertical line
        int step = (start_y < end_y ? 1 : -1);
        for (int y = start_y; y != end_y; y += step) {
            ptlDrawPixel(raster, pixelChar, start_x, y);
        }
    } else if (delta_y == 0){
        //draw a horizontal line
        for (int x = start_x; x != end_x; x++) {
            ptlDrawPixel(raster, pixelChar, x, start_y);
        }
    } else {
        // draw a slope line
        float slope = ((float) delta_y) / ((float) delta_x);
        float step;
        int prev_y = 0;
        
        if (slope > 0) {
            step = (slope > 1 ? 1/slope : slope);
            for (float x = start_x; x < end_x; x += step) {
                int y = (int)(slope * x) > prev_y ? (prev_y += 1) : prev_y;
                ptlDrawPixel(raster, pixelChar, (int)x, start_y + y);
            }
        } else {
            step = (slope < -1 ? 1/slope : slope);
            for (float x = start_x; x < end_x; x += step * -1) {
                int y = (int)(slope * x) < prev_y ? (prev_y -= 1) : prev_y;
                ptlDrawPixel(raster, pixelChar, (int)x, start_y + y);
            }
        }
    }
    
}

void ptlDrawRect(ptlRaster raster, char pixelChar, int width, int height, int x, int y) {
    ptlDrawLine(raster, pixelChar, x, y, x + width - 1, y);
    ptlDrawLine(raster, pixelChar, x + 1, y, x + 1, y + height - 2);
    ptlDrawLine(raster, pixelChar, x + width, y, x + width, y + height - 2);
    ptlDrawLine(raster, pixelChar, x, y + height - 1, x + width - 1, y + height - 1);
}

char ptlGetPixelChar(ptlRaster raster, int x, int y) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;
    return r->pixels[(r->width * y + x)];
}

int ptlGetWidth(ptlRaster raster) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;
    return r->width;
}

int ptlGetHeight(ptlRaster raster) {
    ptlRaster_Win32* r = (ptlRaster_Win32*)raster;
    return r->height;
}

#endif

#endif

#endif
