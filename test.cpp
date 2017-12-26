#include "bmp.hpp"
int main(){
    int width = 200;
    int height = 100;
    int color = 4;
    BMP a;
    a.createEmptyBMP(width, height, color);
    a.circle();
    a.save("circle.bmp");
    return 0;
}