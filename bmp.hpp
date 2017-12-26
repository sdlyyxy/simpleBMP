#ifndef __BMP_HPP__
#define __BMP_HPP__

// #pragma pack(1)
// #pragma pack(push,1)

#include <cstdio>
#include <cstring>
#include <fstream>
#include<vector>
#include <ios>

std::vector<std::pair<int, int>> list;

// typedef unsigned short WORD;
// typedef unsigned long DWORD;
// typedef long LONG;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef unsigned char BYTE;
struct BITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} __attribute__((packed));

struct BITMAPINFOHEADER {
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
} __attribute__((packed));

struct RGBQUAD {
    BYTE rgbBLUE;
    BYTE rgbGreen;
    BYTE rgbRED;
    BYTE rgbReserved;
} __attribute__((packed));

class BMP {
public:
    BMP();
    ~BMP();
    bool load(const char* filename);
    bool save(const char* filename);
    bool createEmptyBMP(int width, int height, int colorBit);
    void circle();

private:
    void setPixelColor(int i, int j);
    int getNumberOfColors();
    void setColor(RGBQUAD* rgb, BYTE r, BYTE g, BYTE b);
    int colorTableSize;
    int numberOfColors;
    BITMAPFILEHEADER bitMapFileHeader;
    BITMAPINFOHEADER* pBitMapInfoHeader;
    RGBQUAD* pRgbQuad;
    BYTE* pPixelData;
    BYTE* pDib;
};

bool BMP::load(const char* filename)
{
    std::ifstream ifs(filename, std::ios::binary);
    ifs.seekg(0, std::ios::end);
    int fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    ifs.read((char*)&bitMapFileHeader, sizeof(BITMAPFILEHEADER));
    if (bitMapFileHeader.bfType != 0x4d42) {
        throw "File Type error! (looking for header)";
        return false;
    }
    if (fileSize != bitMapFileHeader.bfSize) {
        throw "File structure error!";
        return false;
    }
    pDib = new BYTE[fileSize - sizeof(BITMAPFILEHEADER)];
    if (!pDib) {
        throw "Memory not enough/BMP file too large";
        return false;
    }
    ifs.read((char*)pDib, fileSize - sizeof(BITMAPFILEHEADER));
    pBitMapInfoHeader = (BITMAPINFOHEADER*)pDib;
    pRgbQuad = (RGBQUAD*)(pDib + sizeof(BITMAPINFOHEADER));
    colorTableSize = bitMapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER)
        - pBitMapInfoHeader->biSize;
    numberOfColors = getNumberOfColors();
    if (numberOfColors * sizeof(RGBQUAD) != colorTableSize) {
        delete[] pDib;
        pDib = nullptr;
        throw "File color table structure error!";
        return false;
    }
    // pPixelData = pDib + sizeof(BITMAPINFOHEADER) + colorTableSize;
    pPixelData = pDib + bitMapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
    
    return true;
}
int BMP::getNumberOfColors()
{
    if (pBitMapInfoHeader->biClrUsed) {
        return pBitMapInfoHeader->biClrUsed;
    } else {
        switch (pBitMapInfoHeader->biBitCount) {
        case 1:
            return 2;
        case 4:
            return 16;
        case 8:
            return 256;
        }
    }
    return 0;
}
bool BMP::createEmptyBMP(int nWidth, int nHeight, int nColor)
{
    if (pDib) {
        delete[] pDib;
        pDib = nullptr;
    }
    colorTableSize = 0;
    switch (nColor) {
    case 1:
        colorTableSize = 2 * sizeof(RGBQUAD);

        break;
    case 4:
    case 8:
        // nColor = 8;
        colorTableSize = 8 * sizeof(RGBQUAD);
        break;
    default:
        nColor = 24;
        break;
    }
    numberOfColors = colorTableSize / sizeof(RGBQUAD);
    int bytePerLine = (nWidth * nColor + 31) / 32 * 4;
    int dataSize = bytePerLine * nHeight;
    bitMapFileHeader.bfType = 0x4d42;
    bitMapFileHeader.bfReserved1 = 0;
    bitMapFileHeader.bfReserved2 = 0;
    bitMapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize;
    bitMapFileHeader.bfSize = bitMapFileHeader.bfOffBits + dataSize;
    pDib = new BYTE[bitMapFileHeader.bfSize - sizeof(BITMAPFILEHEADER)];
    if (!pDib) {
        throw "Memory not enough.";
        return false;
    }
    pBitMapInfoHeader = (BITMAPINFOHEADER*)pDib;
    pBitMapInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
    pBitMapInfoHeader->biBitCount = nColor;
    pBitMapInfoHeader->biClrImportant = 1;
    pBitMapInfoHeader->biClrUsed = numberOfColors;
    pBitMapInfoHeader->biCompression = 0;
    pBitMapInfoHeader->biPlanes = 1;
    pBitMapInfoHeader->biSizeImage = dataSize;
    pBitMapInfoHeader->biXPelsPerMeter = 1024;
    pBitMapInfoHeader->biYPelsPerMeter = 1024;
    pBitMapInfoHeader->biWidth = nWidth;
    pBitMapInfoHeader->biHeight = nHeight;
    // pPixelData = pDib + sizeof(BITMAPINFOHEADER) + colorTableSize;
    pPixelData = pDib + bitMapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
    pRgbQuad = (RGBQUAD*)(pDib + sizeof(BITMAPINFOHEADER));
    switch (nColor) {
    case 1:
        setColor(pRgbQuad , 0, 0, 0);
        setColor(pRgbQuad+1, 0xff, 0xff, 0xff);
        memset(pPixelData, 0, dataSize);
        break;
    case 4:
    case 8:
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    setColor(pRgbQuad +7- 4*i - j * 2 - k * 1, i * 0xff, j * 0xff, k * 0xff);
                }
            }
        }
        memset(pPixelData, 0xff, dataSize);
        break;
    default:
        memset(pPixelData, 0xff, dataSize);
        break;
    }
    return true;
}
void BMP::setColor(RGBQUAD* rgb, BYTE r, BYTE g, BYTE b)
{
    if (rgb) {
        rgb->rgbRED = r;
        rgb->rgbGreen = g;
        rgb->rgbBLUE = b;
        rgb->rgbReserved = 0;
    } else
        throw "error";
}
bool BMP::save(const char* filename)
{
    bitMapFileHeader;
    *pBitMapInfoHeader;
    if (!pDib) {
        return false;
    }
    FILE* pfile = fopen(filename, "wb");
    // char* s=(char*)&bitMapFileHeader;
    // for (int i = 0; i < 14; i++) {
    // printf("%x", (unsigned char)(((char*)&bitMapFileHeader)[i]));
    // printf
    // }
    // puts("");
    // printf("%x",s[0]);
    // unsigned char c = 255;
    // printf("%x", c);
    // printf("%d", sizeof(BITMAPFILEHEADER));
    // ofs.write((char*)&bitMapFileHeader, sizeof(BITMAPFILEHEADER));
    fwrite((char*)&bitMapFileHeader, 1, sizeof(BITMAPFILEHEADER), pfile);
    // ofs.write((char*)pDib, bitMapFileHeader.bfSize - sizeof(BITMAPFILEHEADER));
    fwrite((char*)pDib, 1, bitMapFileHeader.bfSize - sizeof(BITMAPFILEHEADER), pfile);
    fclose(pfile);
    // puts("eheh");
    // ofs.close();
    // puts("eheh");
    return true;
}
void BMP::circle()
{
    const int& biWidth = pBitMapInfoHeader->biWidth;
    const int& biHeight = pBitMapInfoHeader->biHeight;
    int x = biWidth / 2;
    int y = biHeight / 2;
    int radius = x > y ? y - 2 : x - 2;
    if (radius <= 2)
        return;
    // for (int i = 0; i < 20; i++) {
    //     for (int j = 0; j < 20; j++) {
            // int dist = (i - x) * (i - x) + (j - y) * (j - y);
            // if (dist > (radius - 1) * (radius - 1) && dist < (radius + 1) * (radius + 1)) {
                // setPixelColor(j, i);
                // list.push_back(std::pair<int, int>{ i, j });
            // }
        // }
    // }
    for (int i = 0; i < biWidth; i++) {
        for (int j = 0; j < biHeight; j++) {
            int dist = (i - x) * (i - x) + (j - y) * (j - y);
            if (dist > (radius - 1) * (radius - 1) && dist < (radius + 1) * (radius + 1)) {
                setPixelColor(j, i);
                list.push_back(std::pair<int, int>{ i, j });
            }
        }
    }
}
void BMP::setPixelColor(int i, int j)
{
    const int& biWidth = pBitMapInfoHeader->biWidth;
    const int& biHeight = pBitMapInfoHeader->biHeight;
    const int& biBitCount = pBitMapInfoHeader->biBitCount;
    int bytePerLine = (biWidth * biBitCount + 31) / 32 * 4;
    switch (biBitCount) {
    case 1:
        pPixelData[i * bytePerLine + j / 8] |= (1 << (7 - j % 8));
        break;
    case 4:
        pPixelData[i * bytePerLine + j / 2] &= (0x0f << (4 * (j % 2)));
        pPixelData[i * bytePerLine + j / 2] |= (0x07 << (4 * (1 - j % 2)));
        break;
    case 8:
        pPixelData[i * bytePerLine + j] = 7;
        break;
    default:
        BYTE* p = pPixelData + i * bytePerLine + j * 3;
        *p = *(p + 1) = *(p + 2) = 0;
    }
}
BMP::BMP()
{
    pDib = nullptr;
}
BMP::~BMP()
{
    if (pDib)
        delete[] pDib;
    pDib = nullptr;
}

#endif