#define PTLIB_IMPL
#include "ptlib.h"

int main(){
    
    /* ----INITIALIZE FIRST SCREEN---- */
    ptlRaster screen1;
    screen1 = ptlInitRaster(25, 20, '.');
    
    ptlDrawText(screen1, 5, 7, "Hello from screen 1");
    
    while (1){
        // ALWAYS store the return value of ptlPressedKey() in an integer, and then never call ptlPressedKey() again in this scope
        int keyPressed = ptlPressedKey(screen1);
        
        if (keyPressed == KEYCODE_P) ptlDrawPixel(screen1, '#', 4, 5);
        else if (keyPressed == KEYCODE_Q) break;
        
        ptlRepaint(screen1);
    }
    
    /* ----DESTROY FIRST SCREEN---- */
    ptlDestroyRaster(screen1);
    
    
    
    
    /* ----INITIALIZE SECOND SCREEN---- */
    ptlRaster screen2;
    screen2 = ptlInitRaster(15, 10, '+');
    
    ptlDrawText(screen2, 5, 7, "Hello from screen 2");
    
    while (1){
        // ALWAYS store the return value of ptlPressedKey() in an integer, and then never call ptlPressedKey() again in this scope
        int keyPressed = ptlPressedKey(screen2);
        
        if (keyPressed == KEYCODE_L) ptlDrawLine(screen2, '#', 1, 2, 7, 6);
        else if (keyPressed == KEYCODE_Q) break;
        
        ptlRepaint(screen2);
    }
    
    /* ----DESTROY SECOND SCREEN---- */
    ptlDestroyRaster(screen2);
    
    
    return 0;
}
