#include "bmp.hpp"
int main(){
    int width = 100;
    int height = 200;
    int color = 1;
    BMP a;
    a.createEmptyBMP(width, height, color);
    a.circle();
    a.save("circle.bmp");
    // for (int i = 0; i < list.size();i++){
    //     printf("i=%d,j=%d\n", list[i].first, list[i].second);
    // }
    // a.load("avatar.bmp");
    // a.circle();
    // a.save("hehe.bmp");
    // printf("%d", sizeof(short));
    return 0;
}