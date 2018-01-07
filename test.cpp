#include "bmp.hpp"
#include <cstdio>
#include <cmath>
#include <string>
using namespace std;
BMP a;
void printGray24()
{
    /**
     * @brief 输出24位灰度图像
     * 
     */
    a.load("avatar.bmp");
    a.toGray24();
    a.save("gray24.bmp");
}
void printGray8()
{
    /**
     * @brief 输出8位灰度图像
     * 
     */
    a.toGray8();
    a.save("gray8.bmp");
}
void printBw()
{
    /**
     * @brief 输出黑白图像
     * 
     */
    a.bw();
    a.save("bw.bmp");
}
void printHReverse()
{
    /**
     * @brief 输出水平翻转图像
     * 
     */
    a.load("avatar.bmp");
    a.toGray24();
    a.toGray8();
    a.hReverse();
    a.save("hreverse.bmp");
}
void printVReverse()
{
    /**
     * @brief 输出垂直反转图像
     * 
     */
    a.load("avatar.bmp");
    a.toGray24();
    a.toGray8();
    a.vReverse();
    a.save("vreverse.bmp");
}
void printSmooth4()
{
    /**
     * @brief 8位灰度4平滑
     * 
     */
    a.load("avatar.bmp");
    a.toGray24();
    a.toGray8();
    a.smooth4();
    a.save("smooth4.bmp");
}
void printSmooth8()
{
    /**
     * @brief 8位灰度8平滑
     * 
     */
    a.load("avatar.bmp");
    a.toGray24();
    a.toGray8();
    a.smooth8();
    a.save("smooth8.bmp");
}
void printLight(double alpha)
{
    /**
     * @brief 调整亮度并输出
     * 
     */
    a.load("avatar.bmp");
    a.setLight(alpha);
    char s[20];
    sprintf(s, "light%.1f.bmp", alpha);
    a.save(s);
}
void printContrast(double alpha)
{
    /**
     * @brief 调整对比度并输出
     * 
     */
    a.load("avatar.bmp");
    a.setContrast(alpha);
    char s[20];
    sprintf(s, "contrast%.1f.bmp", alpha);
    a.save(s);
}
int main()
{
    printGray24();       //输出24位灰度
    printGray8();        //输出8位灰度
    printBw();           //输出黑白
    printSmooth4();      //输出8位灰度4平滑
    printSmooth8();      //输出8位灰度8平滑
    printHReverse();     //8位灰度水平翻转
    printVReverse();     //8位灰度垂直翻转
    printLight(0.1);     //亮度参数为0.1
    printLight(0.5);     //亮度参数为0.5
    printLight(0.9);     //亮度参数为0.9
    printContrast(-0.9); //对比度参数为-0.9
    printContrast(-0.3); //对比度参数为-0.3
    printContrast(0.3);  //对比度参数为0.3
    printContrast(0.9);  //对比度参数为0.9
    return 0;
}
