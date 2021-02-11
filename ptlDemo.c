#include <stdio.h>
#include <stdlib.h>
#include "ptlKeyInput.h"
#include "ptlGraphics.h"

int main(){
    CLEAR_SCREEN;
    int inputUpdateTime = 2;
    ptlEnableRawMode(inputUpdateTime);
    
    ptlRaster* r_ptr = malloc(sizeof(struct ptlRaster));
    ptlInitRaster(r_ptr, 25, 20, '.');
    
    ptlDrawText(r_ptr, 2, 2, "hey!");
    
    int i = 0;
    while (1){
        
        char c = ptlPressedKey();
        //if (c != '\0') PRINT_PRESSED_KEY(c)
        if (c == 'q'){
            printf("\r\n");
            break;
        }
        
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





