# Plain-Terminal-Library

---------------GRAPHICS---------------


So for graphics there are basically three steps:

1) create a ptlRaster object
2) mess around with the pixels of the ptlRaster object. those pixels are just chars in an array
3) call ptlRepaint();

That is the general structure of rendering graphics, now here are the specifics:

#there are two structs the library provides:
```
typedef struct ptlPixel {
    int x, y;
    char pixelChar;
} ptlPixel;


typedef struct ptlRaster{
    int width, height;
    char bgChar;
    char* pixels;
} ptlRaster;
```

After you've created a raster struct, you immediately wanna initialize it with this function:
```
void ptlInitRaster(ptlRaster* r, int width, int height, char bgChar)
```





then there are multiple drawing functions which you use to modify the pixels in the raster:

````
void ptlDrawText(ptlRaster* raster, int x, int y, char* text)

void ptlDrawPixel(ptlRaster* raster, char pixelChar, int x, int y)

void ptlRemovePixel(ptlRaster* raster, int x, int y)

void ptlDrawLine(ptlRaster* raster, char pixelChar, int start_x, int start_y, int end_x, int end_y)

void ptlDrawRect(ptlRaster* raster, char pixelChar, int width, int height, int x, int y)

```




and finally there's repaint, which basically draws your updated raster to the screen:
```
void ptlRepaint(ptlRaster* raster)


