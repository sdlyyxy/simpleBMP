#ifndef __BMP_HPP__
#define __BMP_HPP__

// #pragma pack(1)
// #pragma pack(push,1)

#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>
#include <ios>
#include <iostream>
#include <bitset>
#include <cmath>
using namespace std;
std::vector<std::pair<int, int>> list;

// typedef unsigned short WORD;
// typedef unsigned long DWORD;
// typedef long LONG;
typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef unsigned char BYTE;
struct BITMAPFILEHEADER
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} __attribute__((packed));

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
} __attribute__((packed));

struct RGBQUAD
{
    BYTE rgbBLUE;
    BYTE rgbGreen;
    BYTE rgbRED;
    BYTE rgbReserved;
} __attribute__((packed));

class BMP
{
  public:
    /**
 * @brief 构造函数
 * 
 */
    BMP();
    /**
     * @brief 析构函数
     * 
     */
    ~BMP();
    /**
     * @brief 载入图形
     * 
     * @param filename 文件名
     * @return true 载入成功
     * @return false 载入失败
     */
    bool load(const char *filename);
    /**
     * @brief 保存图形到文件
     * 
     * @param filename 文件名
     * @return true 保存成功
     * @return false 保存失败
     */
    bool save(const char *filename);
    /**
     * @brief 创建空BMP
     * 
     * @param width 
     * @param height 
     * @param colorBit 
     * @return true 
     * @return false 
     */
    bool createEmptyBMP(int width, int height, int colorBit);
    /**
     * @brief 在BMP中心画圆
     * 
     */
    void circle();
    /**
     * @brief 转24位灰度
     * 
     * @return true 成功
     * @return false 失败
     */
    bool toGray24();
    /**
     * @brief 水平翻转
     * 
     */
    void hReverse();
    /**
     * @brief 竖直翻转
     * 
     */
    void vReverse();
    /**
     * @brief 转8位灰度
     * 
     */
    void toGray8();
    /**
     * @brief 8位灰度4平滑
     * 
     */
    void smooth4();
    /**
     * @brief 8位灰度8平滑
     * 
     */
    void smooth8();
    /**
     * @brief 转黑白图形
     * 
     */
    void bw();
    /**
     * @brief 调整亮度
     * 
     * @param alpha 
     */
    void setLight(double alpha);
    /**
     * @brief 调整对比度
     * 
     * @param alpha 
     */
    void setContrast(double alpha);
    void to256();

  private:
    void setPixelColor(int i, int j);
    int getNumberOfColors();
    /**
     * @brief 设置调色板
     * 
     * @param rgb 
     * @param r 
     * @param g 
     * @param b 
     */
    void setColor(RGBQUAD *rgb, BYTE r, BYTE g, BYTE b);
    /**
     * @brief 以下各个update函数是对文件结构参数进行正确计算
     * 
     */
    void updateBytePerLine();
    void updateBiSizeImage();
    void updateColorTableSize();
    void updateBfOffbits();
    void updateBfSize();
    int colorTableSize;
    int numberOfColors;
    BITMAPFILEHEADER bitMapFileHeader;
    BITMAPINFOHEADER *pBitMapInfoHeader; //todo:remove this pointer
    BITMAPINFOHEADER bitMapInfoHeader;
    RGBQUAD *pRgbQuad;
    BYTE *pPixelData;
    BYTE *pDib;
    int bytePerLine;
};

void BMP::updateBfOffbits()
{
    bitMapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize;
}
void BMP::updateBfSize()
{
    bitMapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize + bitMapInfoHeader.biSizeImage;
}

void BMP::updateBytePerLine()
{
    bytePerLine = (bitMapInfoHeader.biWidth * bitMapInfoHeader.biBitCount + 31) / 32 * 4;
}
void BMP::updateBiSizeImage()
{
    bitMapInfoHeader.biSizeImage = bitMapInfoHeader.biHeight * bytePerLine;
}
void BMP::updateColorTableSize()
{
    colorTableSize = sizeof(RGBQUAD) * bitMapInfoHeader.biClrUsed;
}
bool BMP::load(const char *filename)
{
    if (pDib)
    {
        delete[] pDib;
    }
    std::ifstream ifs(filename, std::ios::binary);
    ifs.seekg(0, std::ios::end);
    int fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    ifs.read((char *)&bitMapFileHeader, sizeof(BITMAPFILEHEADER));
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
    pDib = new BYTE[fileSize - sizeof(BITMAPFILEHEADER)];
    if (!pDib)
    {
        throw "Memory not enough/BMP file too large";
        return false;
    }
    ifs.read((char *)pDib, fileSize - sizeof(BITMAPFILEHEADER));
    pBitMapInfoHeader = (BITMAPINFOHEADER *)pDib;
    memcpy(&bitMapInfoHeader, pBitMapInfoHeader, sizeof(BITMAPINFOHEADER));
    pRgbQuad = (RGBQUAD *)(pDib + sizeof(BITMAPINFOHEADER));
    colorTableSize = bitMapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER) - bitMapInfoHeader.biSize;
    numberOfColors = getNumberOfColors();
    if (numberOfColors * sizeof(RGBQUAD) != colorTableSize)
    {
        delete[] pDib;
        pDib = nullptr;
        throw "File color table structure error!";
        return false;
    }
    // std::cout << fileSize << std ::endl;
    // std::cout << sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
    // bitMapInfoHeader.biSizeImage<< std:: endl;
    // pPixelData = pDib + sizeof(BITMAPINFOHEADER) + colorTableSize;
    pPixelData = pDib + bitMapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
    int bytePerLine = (bitMapInfoHeader.biWidth * bitMapInfoHeader.biBitCount + 31) / 32 * 4;
    this->bytePerLine = bytePerLine;
    // std::cout << this->bytePerLine << std::endl;
    return true;
}
int BMP::getNumberOfColors()
{
    if (bitMapInfoHeader.biClrUsed)
    {
        return bitMapInfoHeader.biClrUsed;
    }
    else
    {
        switch (bitMapInfoHeader.biBitCount)
        {
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
    if (pDib)
    {
        delete[] pDib;
        pDib = nullptr;
    }
    colorTableSize = 0;
    switch (nColor)
    {
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
    this->bytePerLine = bytePerLine;
    int dataSize = bytePerLine * nHeight;
    bitMapFileHeader.bfType = 0x4d42;
    bitMapFileHeader.bfReserved1 = 0;
    bitMapFileHeader.bfReserved2 = 0;
    bitMapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTableSize;
    bitMapFileHeader.bfSize = bitMapFileHeader.bfOffBits + dataSize;
    pDib = new BYTE[bitMapFileHeader.bfSize - sizeof(BITMAPFILEHEADER)];
    if (!pDib)
    {
        throw "Memory not enough.";
        return false;
    }
    pBitMapInfoHeader = (BITMAPINFOHEADER *)pDib;
    bitMapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitMapInfoHeader.biBitCount = nColor;
    bitMapInfoHeader.biClrImportant = 1;
    bitMapInfoHeader.biClrUsed = numberOfColors;
    bitMapInfoHeader.biCompression = 0;
    bitMapInfoHeader.biPlanes = 1;
    bitMapInfoHeader.biSizeImage = dataSize;
    bitMapInfoHeader.biXPelsPerMeter = 1024;
    bitMapInfoHeader.biYPelsPerMeter = 1024;
    bitMapInfoHeader.biWidth = nWidth;
    bitMapInfoHeader.biHeight = nHeight;
    // pPixelData = pDib + sizeof(BITMAPINFOHEADER) + colorTableSize;
    pPixelData = pDib + bitMapFileHeader.bfOffBits - sizeof(BITMAPFILEHEADER);
    pRgbQuad = (RGBQUAD *)(pDib + sizeof(BITMAPINFOHEADER));
    switch (nColor)
    {
    case 1:
        setColor(pRgbQuad, 0, 0, 0);
        setColor(pRgbQuad + 1, 0xff, 0xff, 0xff);
        memset(pPixelData, 0, dataSize);
        break;
    case 4:
    case 8:
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    setColor(pRgbQuad + 7 - 4 * i - j * 2 - k * 1, i * 0xff, j * 0xff, k * 0xff);
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
void BMP::setColor(RGBQUAD *rgb, BYTE r, BYTE g, BYTE b)
{
    if (rgb)
    {
        rgb->rgbRED = r;
        rgb->rgbGreen = g;
        rgb->rgbBLUE = b;
        rgb->rgbReserved = 0;
    }
    else
        throw "error";
}
bool BMP::save(const char *filename)
{
    // bitMapFileHeader;
    // *pBitMapInfoHeader;
    if (!pDib)
    {
        return false;
    }
    FILE *pfile = fopen(filename, "wb");
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
    fwrite((char *)&bitMapFileHeader, 1, sizeof(BITMAPFILEHEADER), pfile);
    // ofs.write((char*)pDib, bitMapFileHeader.bfSize - sizeof(BITMAPFILEHEADER));
    // fwrite((char*)pDib, 1, bitMapFileHeader.bfSize - sizeof(BITMAPFILEHEADER), pfile);
    // std::cout << colorTableSize << std::endl;
    // std::cout << this->bitMapInfoHeader.biSizeImage << std :: endl;
    fwrite((char *)&bitMapInfoHeader, 1, sizeof(BITMAPINFOHEADER), pfile);
    fwrite((char *)pRgbQuad, 1, colorTableSize, pfile);
    // fwrite((char*)(pDib+sizeof(BITMAPINFOHEADER)), 1,this->bitMapInfoHeader.biSizeImage, pfile);
    fwrite((char *)pPixelData, 1, this->bitMapInfoHeader.biSizeImage, pfile);
    fclose(pfile);
    // puts("eheh");
    // ofs.close();
    // puts("eheh");
    return true;
}
void BMP::circle()
{
    const int &biWidth = bitMapInfoHeader.biWidth;
    const int &biHeight = bitMapInfoHeader.biHeight;
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
    for (int i = 0; i < biWidth; i++)
    {
        for (int j = 0; j < biHeight; j++)
        {
            int dist = (i - x) * (i - x) + (j - y) * (j - y);
            if (dist > (radius - 1) * (radius - 1) && dist < (radius + 1) * (radius + 1))
            {
                setPixelColor(j, i);
                // list.push_back(std::pair<int, int>{ i, j });
            }
        }
    }
}
void BMP::setPixelColor(int i, int j)
{
    const int &biWidth = bitMapInfoHeader.biWidth;
    const int &biHeight = bitMapInfoHeader.biHeight;
    const int &biBitCount = bitMapInfoHeader.biBitCount;
    int bytePerLine = (biWidth * biBitCount + 31) / 32 * 4;
    switch (biBitCount)
    {
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
        BYTE *p = pPixelData + i * bytePerLine + j * 3;
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
bool BMP::toGray24()
{
    // std::cout << pBitMapInfoHeader->biBitCount << std::endl;
    if (bitMapInfoHeader.biBitCount != 24)
    {
        return false;
    }
    // std::cout << this->bytePerLine << std::endl;
    BYTE(*p)
    [this->bytePerLine] = (BYTE(*)[this->bytePerLine])pPixelData;
    for (int i = 0; i < bitMapInfoHeader.biHeight; i++)
    {
        for (int j = 0; j < this->bytePerLine; j += 3)
        {
            // int tmp = 0 * p[i][j] + 0* p[i][j + 1] + 1 * p[i][j + 2];
            int tmp = 0.59 * p[i][j] + 0.11 * p[i][j + 1] + 0.3 * p[i][j + 2];
            p[i][j] = p[i][j + 1] = p[i][j + 2] = tmp;
            // p[i][j] = 200;
            // p[i][j + 1] = p[i][j + 2] = 0;
        }
    }
    return true;
}
void BMP::toGray8()
{
    // toGray24();
    bitMapInfoHeader.biBitCount = 8;
    int oldBytePerLine = bytePerLine;
    updateBytePerLine();
    updateBiSizeImage();
    bitMapInfoHeader.biClrUsed = 256;
    updateColorTableSize();
    updateBfOffbits();
    updateBfSize();
    pRgbQuad = new RGBQUAD[256];
    for (int i = 0; i < 256; i++)
    {
        setColor(pRgbQuad + i, i, i, i);
    }
    BYTE *newdata = new BYTE[bitMapInfoHeader.biSizeImage];
    // cout << bitMapInfoHeader.biSizeImage << endl;
    BYTE(*work_new)
    [bytePerLine] = (BYTE(*)[bytePerLine])newdata;
    BYTE(*work_old)
    [oldBytePerLine] = (BYTE(*)[oldBytePerLine])pPixelData;
    for (int i = 0; i < bitMapInfoHeader.biHeight; i++)
    {
        for (int j = 0; j < bitMapInfoHeader.biWidth; j++)
        {
            work_new[i][j] = work_old[i][j * 3];
            // work_new[i][j] = 100;
        }
    }
    pPixelData = newdata;
}
void BMP::bw()
{
    bitMapInfoHeader.biBitCount = 1;
    int oldBytePerLine = bytePerLine;
    updateBytePerLine();
    updateBiSizeImage();
    bitMapInfoHeader.biClrUsed = 2;
    updateColorTableSize();
    updateBfOffbits();
    updateBfSize();
    pRgbQuad = new RGBQUAD[2];
    for (int i = 0; i < 2; i++)
    {
        setColor(pRgbQuad + i, i * 255, i * 255, i * 255);
    }
    int threshold = 180;
    BYTE *newdata = new BYTE[bitMapInfoHeader.biSizeImage];
    // cout << bitMapInfoHeader.biSizeImage << endl;
    BYTE(*work_new)
    [bytePerLine] = (BYTE(*)[bytePerLine])newdata;
    BYTE(*work_old)
    [oldBytePerLine] = (BYTE(*)[oldBytePerLine])pPixelData;
    for (int i = 0; i < bitMapInfoHeader.biHeight; i++)
    {
        for (int j = 0; j < bytePerLine; j++)
        {
            std::bitset<8> bs;
            for (int k = 0; k < 8; k++)
            {
                if (k + j * 8 < bitMapInfoHeader.biWidth)
                {
                    if (work_old[i][k + j * 8] > threshold)
                    {
                        bs[k] = 1;
                    }
                }
            }
            work_new[i][j] = bs.to_ulong();
            // work_new[i][j] = 100;
        }
    }
    pPixelData = newdata;
}
void BMP::hReverse()
{
    BYTE(*work)
    [bytePerLine] = (BYTE(*)[bytePerLine])pPixelData;
    int width = bitMapInfoHeader.biWidth;
    for (int i = 0; i < bitMapInfoHeader.biHeight; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            swap(work[i][j], work[i][width - 1 - j]);
        }
    }
}
void BMP::vReverse()
{
    BYTE(*work)
    [bytePerLine] = (BYTE(*)[bytePerLine])pPixelData;
    int width = bitMapInfoHeader.biWidth;
    int height = bitMapInfoHeader.biHeight;
    for (int i = 0; i < height / 2; i++)
    {
        for (int j = 0; j < width; j++)
        {
            swap(work[i][j], work[height - i - 1][j]);
        }
    }
}
void BMP::smooth4()
{
    BYTE *newdata = new BYTE[bitMapInfoHeader.biSizeImage];
    // cout << bitMapInfoHeader.biSizeImage << endl;
    BYTE(*work_old)
    [bytePerLine] = (BYTE(*)[bytePerLine])pPixelData;
    BYTE(*work)
    [bytePerLine] = (BYTE(*)[bytePerLine])newdata;
    int width = bitMapInfoHeader.biWidth;
    int height = bitMapInfoHeader.biHeight;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int n = 0, res = 0;
            const int dx[] = {0, 0, 0, -1, 1};
            const int dy[] = {0, 1, -1, 0, 0};
            for (int k = 0; k < 5; k++)
            {
                int x = i + dx[k];
                int y = j + dy[k];
                if ((x >= 0) && (y >= 0) && (x < height) && (y < width))
                {
                    n++;
                    res += work_old[x][y];
                }
            }
            work[i][j] = res / n;
        }
    }
    delete[] pPixelData;
    pPixelData = newdata;
}
void BMP::smooth8()
{
    BYTE *newdata = new BYTE[bitMapInfoHeader.biSizeImage];
    // cout << bitMapInfoHeader.biSizeImage << endl;
    BYTE(*work_old)
    [bytePerLine] = (BYTE(*)[bytePerLine])pPixelData;
    BYTE(*work)
    [bytePerLine] = (BYTE(*)[bytePerLine])newdata;
    int width = bitMapInfoHeader.biWidth;
    int height = bitMapInfoHeader.biHeight;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int n = 0, res = 0;
            const int dx[] = {0, 0, 0, -1, 1, 1, 1, -1, -1};
            const int dy[] = {0, 1, -1, 0, 0, 1, -1, 1, -1};
            for (int k = 0; k < 9; k++)
            {
                int x = i + dx[k];
                int y = j + dy[k];
                if ((x >= 0) && (y >= 0) && (x < height) && (y < width))
                {
                    n++;
                    res += work_old[x][y];
                }
            }
            work[i][j] = res / n;
        }
    }
    delete[] pPixelData;
    pPixelData = newdata;
}
void BMP::setLight(double alpha)
{
    if (alpha < 0 || alpha > 1)
        alpha = 0;
    // BYTE* newdata = new BYTE[bitMapInfoHeader.biSizeImage];
    // cout << bitMapInfoHeader.biSizeImage << endl;
    BYTE(*work)
    [bytePerLine] = (BYTE(*)[bytePerLine])pPixelData;
    for (int i = 0; i < bitMapInfoHeader.biHeight; i++)
    {
        for (int j = 0; j < bytePerLine / 2; j += 3)
        {
            BYTE &R = work[i][j + 2];
            R = pow(R, alpha) * pow(255, 1 - alpha);
            BYTE &G = work[i][j + 1];
            G = pow(G, alpha) * pow(255, 1 - alpha);
            BYTE &B = work[i][j];
            B = pow(B, alpha) * pow(255, 1 - alpha);
        }
    }
}
void BMP::setContrast(double alpha)
{
    if (alpha < -1 || alpha > 1)
        alpha = 0;
    // BYTE* newdata = new BYTE[bitMapInfoHeader.biSizeImage];
    // cout << bitMapInfoHeader.biSizeImage << endl;
    int mid = 128;
    BYTE(*work)
    [bytePerLine] = (BYTE(*)[bytePerLine])pPixelData;
    for (int i = 0; i < bitMapInfoHeader.biHeight; i++)
    {
        for (int j = 0; j < bytePerLine / 2; j += 3)
        {
            int R = work[i][j + 2];
            R = mid + (R - mid) * (1 + alpha);
            if (R < 0)
                R = 0;
            if (R > 255)
                R = 255;
            work[i][j + 2] = R;
            int G = work[i][j + 1];
            G = mid + (G - mid) * (1 + alpha);
            if (G < 0)
                G = 0;
            if (G > 255)
                G = 255;
            work[i][j + 1] = G;
            int B = work[i][j];
            B = mid + (B - mid) * (1 + alpha);
            if (B < 0)
                B = 0;
            if (B > 255)
                B = 255;
            work[i][j] = B;
        }
    }
}
void BMP::to256()
{
    bitMapInfoHeader.biBitCount = 8;
    int oldBytePerLine = bytePerLine;
    updateBytePerLine();
    updateBiSizeImage();
    bitMapInfoHeader.biClrUsed = 256;
    updateColorTableSize();
    updateBfOffbits();
    updateBfSize();
    pRgbQuad = new RGBQUAD[256];
    for (int i = 0; i < 256; i++)
    {
        setColor(pRgbQuad + i, ((i & 0b11) << 6) & 0b11111111, (((i >> 2) & 0b111) << 5) & 0b11111111,
                 (i >> 5 << 5) & 0b11111111);
        //setColor is r.g.b....
    }
    BYTE *newdata = new BYTE[bitMapInfoHeader.biSizeImage];
    // cout << bitMapInfoHeader.biSizeImage << endl;
    BYTE(*work_new)
    [bytePerLine] = (BYTE(*)[bytePerLine])newdata;
    BYTE(*work_old)
    [oldBytePerLine] = (BYTE(*)[oldBytePerLine])pPixelData;
    for (int i = 0; i < bitMapInfoHeader.biHeight; i++)
    {
        for (int j = 0; j < bitMapInfoHeader.biWidth; j++)
        {
            work_new[i][j] = (work_old[i][j * 3] >> 5 << 5) & 0b11111111;
            work_new[i][j] += (((work_old[i][j * 3 + 1] >> 2) & 0b111) << 2) & 0b11111111;
            work_new[i][j] += work_old[i][j * 3 + 2] & 0b11;
            // work_new[i][j] = (work_old[i][j * 3] >> 5 << 5) & 0b11111111;
            // work_new[i][j] = (((work_old[i][j*3+1]>>2)&0b111)<<2)&0b11111111;
            // work_new[i][j] = work_old[i][j*3+2]&0b11;
            printf("%d ", work_new[i][j]);

            // work_new[i][j] = 100;
            // work_new[i][j] = i ;
        }
    }
    pPixelData = newdata;
}
#endif