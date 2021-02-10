// Basically some boilerplate code which can enter raw mode in POSIX terminals.
// Flags are set in enable_raw_mode()
// original canonical mode and all the original flags are set with disable_raw_mode.
// disable_raw_mode basically resets the terminal in a way
//
//
//





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

struct termios orig_termios;

enum keyCodes{
    
    KEYCODE_1 = '1', KEYCODE_2, KEYCODE_3, KEYCODE_4, KEYCODE_5, KEYCODE_6, KEYCODE_7, KEYCODE_8, KEYCODE_9,
    
    KEYCODE_A = 'a', KEYCODE_B, KEYCODE_C, KEYCODE_D, KEYCODE_E, KEYCODE_F, KEYCODE_G, KEYCODE_H, KEYCODE_I, KEYCODE_J, KEYCODE_K, KEYCODE_L, KEYCODE_M, KEYCODE_N,
    KEYCODE_O, KEYCODE_P, KEYCODE_Q, KEYCODE_R, KEYCODE_S, KEYCODE_T, KEYCODE_U, KEYCODE_V, KEYCODE_W, KEYCODE_X, KEYCODE_Y, KEYCODE_Z,
    
    KEYCODE_TAB = '\t', KEYCODE_ENTER = '\n', KEYCODE_SPACE = ' ',
    
    KEYCODE_UP_ARROW = 1000, KEYCODE_DOWN_ARROW = 1001, KEYCODE_LEFT_ARROW = 1002, KEYCODE_RIGHT_ARROW, KEYCODE_SHIFT = 1003, KEYCODE_ESCAPE = 1004
};

/* ----TERMINAL---- */

void die(const char* s){
        perror(s);
        exit(1);
}


void disable_raw_mode(){
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
                die("tcsetattr");
}

void enable_raw_mode(int update_time){
        if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
        atexit(disable_raw_mode);

        struct termios raw = orig_termios;

        raw.c_iflag &= ~(BRKINT| ICRNL| INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = update_time;

        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
        
}

int pressed_key(){
    int key_code = '\0';
    if (read(STDIN_FILENO, &key_code, 1) == -1 && errno != EAGAIN) die("read");
    //if (iscntrl(key_code));
    printf("\r");
    return key_code;
}

/* ----INIT---- */

int main(){
    enable_raw_mode(10);
    
    while (1){
        char c = pressed_key();
        if (c != '\0'){
            PRINT_PRESSED_KEY(c)
        }
        
        if (c == 'q') break;
    }
    
    return 0;

}





