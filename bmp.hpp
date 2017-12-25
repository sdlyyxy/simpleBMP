#ifndef __BMP_HPP__
#define __BMP_HPP__

#include <ios>
#include <fstream>

typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned char BYTE;
struct BITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
};
struct BITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};
struct RGBQUAD
{
    BYTE rgbBLUE;
    BYTE rgbGreen;
    BYTE rgbRED;
    BYTE rgbReserved;
};
class bmp
{
  public:
    bmp();
    ~bmp();
    bool load(const char *filename);
    bool save(const char *filename);
    bool create(int width, int height, int colorBit);
    void circle();

  private:
    void setPixelColor(int i, int j);
    int getNumberOfColors();
    void setColor(RGBQUAD *rgb, BYTE r, BYTE g, BYTE b);
    BITMAPFILEHEADER bitMapFileHeader;
    BITMAPINFOHEADER *pBitMapInfoHeader;
    RGBQUAD *pRgbQuad;
    BYTE *pPixelData;
    BYTE *pDib;
};

bool bmp::load(const char *filename)
{
    std::ifstream ifs(filename, std::ios::binary);
    ifs.seekg(0, std::ios::end);
    int fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    ifs.read((char *)&bitMapFileHeader, sizeof BITMAPFILEHEADER);
    if (bitMapFileHeader.bfType != 0x4d42)
    {
        throw "File Type error! (looking for header)";
        return false;
    }
    if (fileSize != bitMapFileHeader.bfSize)
    {
        throw "File structure error!";
        return false;
    }
    pDib = new
}

#endif