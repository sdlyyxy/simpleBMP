#include "bmp.hpp"
#include<cstdio>
#include<cmath>
#include<string>
using namespace std;
BMP a;
void printGray24(){
    /**
     * @brief 输出24位灰度图像
     * 
     */
    a.load("avatar.bmp");
    a.toGray24();
    a.save("gray24.bmp");
}
void printGray8(){
    /**
     * @brief 输出8位灰度图像
     * 
     */
    a.toGray8();
    a.save("gray8.bmp");
}
void printBw(){
    a.bw();
    a.save("bw.bmp");
}
void printHReverse(){
    a.load("avatar.bmp");
    a.toGray24();
    a.toGray8();
    a.hReverse();
    a.save("hreverse.bmp");
}
void printVReverse(){
    a.load("avatar.bmp");
    a.toGray24();
    a.toGray8();
    a.vReverse();
    a.save("vreverse.bmp");
}
void printSmooth4(){
    a.load("avatar.bmp");
    a.toGray24();
    a.toGray8();
    a.smooth4();
    a.save("smooth4.bmp");  
}
void printSmooth8(){
    a.load("avatar.bmp");
    a.toGray24();
    a.toGray8();
    a.smooth8();
    a.save("smooth8.bmp");  
}
void printLight(double alpha){
    a.load("avatar.bmp");
    a.setLight(alpha);
    char s[20];
    sprintf(s, "light%.1f.bmp", alpha);
    a.save(s);
}
void printContrast(double alpha){
    a.load("avatar.bmp");
    a.setContrast(alpha);
    char s[20];
    sprintf(s, "contrast%.1f.bmp", alpha);
    a.save(s);
}
int main()
{
    printGray24();
    printGray8();
    printBw();
    printSmooth4();
    printSmooth8();
    printHReverse();
    printVReverse();
    printLight(0.1);
    printLight(0.5);
    printLight(0.9);
    printContrast(-0.9);
    printContrast(-0.3);
    printContrast(0.3);
    printContrast(0.9);
    return 0;
}
