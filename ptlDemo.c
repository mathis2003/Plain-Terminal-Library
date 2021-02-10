#include "ptlKeyInput.h"
#include "ptlGraphics.h"

int main(){
    int inputUpdateTime = 2;
    ptlEnableRawMode(inputUpdateTime);
    
    ptlRaster* r_ptr = malloc(sizeof(struct ptlRaster));
    ptlInitRaster(r_ptr, 25, 20, '.');
    
    int i = 0;
    while (1){
        
        char c = ptlPressedKey();
        if (c != '\0') PRINT_PRESSED_KEY(c)
        if (c == 'q') break;
        
        struct ptlPixel p;
        p.x = i % 10;
        p.y = 5;
        p.pixelChar = '@';
        
        ptlDrawPixel(r_ptr, p);
        ptlRemovePixel(r_ptr, r_ptr->pixels[5 * r_ptr->width + (i%10)-1]);
        ptlRepaint(r_ptr);
        i++;
        
    }
    
    return 0;

}





