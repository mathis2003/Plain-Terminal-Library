#include <stdio.h>
#include <stdlib.h>
//#include "ptlKeyInput.h"
//#include "ptlGraphics.h"
#include "ptlib.h"

int main(){
    CLEAR_SCREEN;
    int inputUpdateTime = 2;
    ptlEnableRawMode(inputUpdateTime);
    
    ptlRaster* r_ptr = malloc(sizeof(struct ptlRaster));
    ptlInitRaster(r_ptr, 25, 20, '.');
    
    //ptlDrawText(r_ptr, 2, 2, "hey, Did you know that I am very smart? no? oh boy that's a shame!");
    ptlDrawLine(r_ptr,'o', 0, 0, 16, 16);
    
    int i = 0;
    while (1){
        
        if (ptlPressedKey() == KEYCODE_Q) break;
        
        struct ptlPixel p;
        p.x = i % 10;
        p.y = 5;
        p.pixelChar = '@';
        
        ptlDrawPixel(r_ptr, p.pixelChar, p.x, p.y);
        ptlRemovePixel(r_ptr, p.x - 1, p.y);
        ptlRepaint(r_ptr);
        i++;
        
    }
    
    return 0;

}
