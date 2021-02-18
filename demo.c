
#define PTLIB_IMPL
#include "ptlib.h"

int main(){
    /* ----variables---- */
    ptlRaster screen;
    screen = ptlInitRaster(15, 10, '.');
    
    ptlDrawPixel(screen, '#', 5, 5);
    ptlRemovePixel(screen, 5, 5);
    
    /* ----game loop---- */
    while (1){
        // ALWAYS make sure you call ptlPressedKey() only ONCE each iteration, and store its return value in an integer
        int keyCode = ptlPressedKey(screen);
        
        
        if (keyCode == KEYCODE_Q) break;
        
            
            
        
        
        // paints the (un)modified version of the raster
        ptlRepaint(screen);
    }
    
    // ALWAYS destroy the raster before finishing the program
    ptlDestroyRaster(screen);
    
    return 0;
}
