#include <iostream>
#include "CImg.h"

#define cimg_use_jpeg
#define THRESHOLD 10
using namespace cimg_library;

enum ColorCode { CBlack = 0, CWhite, CGrey, CRed, COrange, CYellow, CGreen, CAqua, CBlue, CPurple, COLOR_CODES_COUNT };
std::string ColorNamesArray[COLOR_CODES_COUNT] = { "Black", "White", "Grey", "Red", "Orange", "Yellow", "Green", "Aqua", "Blue", "Purple" };


int COLOR_RGB[COLOR_CODES_COUNT][3] {//R, G, B
    {0,     0,      0},
    {255,   255,    255},
    {128,   128,    128},
    {255,   0,      255},
    {255,   165,    0},
    {255,   255,    0},
    {0,     128,    0},
    {0,     255,    255},
    {0,     0,      255},
    {128,   0,      128}
};
int COLOR_HSV[COLOR_CODES_COUNT][3]{//H, S, V
    {0,     0,      0},
    {0,     0,      200},
    {0,     0,      100},
    {0,     100,    200},
    {18,    100,    200},
    {30,    100,    200},
    {60,    100,    100},
    {90,    100,    200},
    {120,   100,    200},
    {150,   100,    100},
};
void GetHSVByColorCode(int& H, int& S, int& V, int ColorCode) 
{   //[0-180][0-100][0-200]
    if (ColorCode < 0 || ColorCode > COLOR_CODES_COUNT)
    {
        throw 2;
    }
    else
    {
        H = COLOR_HSV[ColorCode][0];
        S = COLOR_HSV[ColorCode][1];
        V = COLOR_HSV[ColorCode][2];
    }
    
}
void GetRGBByColorCode(int& R, int& G, int& B, int ColorCode) 
{
    if (ColorCode < 0 || ColorCode > COLOR_CODES_COUNT)
    {
        throw 2;
    }
    else
    {
        R = COLOR_RGB[ColorCode][0];
        G = COLOR_RGB[ColorCode][1];
        B = COLOR_RGB[ColorCode][2];
    }
}

int GetColorCodeByHSV(int H, int S, int V)
{
    int color = CBlack;

    if      (V < 75) { color = CBlack; }
    else if (V > 190 && S < 27) { color = CWhite; }
    else if (S < 53 && V < 185) { color = CGrey; }
    else
    {
        if (H < 7) { color = CRed; }
        else if (H < 25) { color = COrange; } // TODO
        else if (H < 34) { color = CYellow; }
        else if (H < 73) { color = CGreen; }
        else if (H < 102) { color = CAqua; }
        else if (H < 140) { color = CBlue; }
        else if (H < 170) { color = CPurple; }
        else { color = CRed; }// if full circle: back to Red
    }
    return color;
}

void RGBtoHSV(float fR, float fG, float fB, float& fH, float& fS, float& fV) {
    // Based on https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72 

    float fCMax = std::max(std::max(fR, fG), fB);
    float fCMin = std::min(std::min(fR, fG), fB);
    float fDelta = fCMax - fCMin;

    if (fDelta > 0) 
    {
        if (fCMax == fR) 
        {
            fH = 60 * (fmod(((fG - fB) / fDelta), 6));
        }
        else if (fCMax == fG) 
        {
            fH = 60 * (((fB - fR) / fDelta) + 2);
        }
        else if (fCMax == fB) 
        {
            fH = 60 * (((fR - fG) / fDelta) + 4);
        }

        if (fCMax > 0) 
        {
            fS = fDelta / fCMax;
        }
        else 
        {
            fS = 0;
        }
        fV = fCMax;
    }
    else 
    {
        fH = 0;
        fS = 0;
        fV = fCMax;
    }

    if (fH < 0) 
    {
        fH = 360 + fH;
    }
    //edit HSV [0-360][0-1][0-255] in [0-180][0-100][0-200]
    fH /= 2;
    fS *= 100;
    fV /= 1.275;
}

class Histogram
{
private:
    int ColorCodePixelCounts[COLOR_CODES_COUNT];
    CImg<float> img;
public:
    Histogram(CImg<float> &img)
    {
        this->img = img;
        for (int ccode = 0; ccode < COLOR_CODES_COUNT; ccode++)
        {
            ColorCodePixelCounts[ccode] = 0;
        }
    }
    void calculate() 
    {
        float  h, s, v;
        float  r, g, b;
        cimg_forXY(img, x, y)
        {
            r = img(x, y, 0, 0);
            g = img(x, y, 0, 1);
            b = img(x, y, 0, 2);
            RGBtoHSV(r, g, b, h, s, v);// convert rgb to hsv [0-180][0-100][0-200]
            ColorCodePixelCounts[GetColorCodeByHSV(h, s, v)] += 1;
        }
    }
    void print() 
    {
        float len = img.width() * img.height();
        float percent;
        for (int i = 0; i < COLOR_CODES_COUNT; i++)
        {
            if (ColorCodePixelCounts[i] > 0)
            {
                percent = (ColorCodePixelCounts[i] / len) * 100;//%
                if (percent > THRESHOLD)
                {
                    std::cout << ColorNamesArray[i] << " = " << percent << " %" << '\n';
                }
            }
        }
    }
};



int main(int argc, char* argv[])
{   
    // ColorDetect [img file name] {filter name}
    // R G B
    try
    {
        char* name;
        if (argc != 2) 
        {
            throw 1;
            
        }
        else 
        {
            name = argv[1];
        }
    
        CImg<float> img(name);
        Histogram a(img);
        a.calculate();
        a.print();
    }
    catch (int error_code)
    {
        switch (error_code)
        {
        case 1:
            std::cout << "Incorrect input" << std::endl;
            break;
        case 2:
            std::cout << "Error ColorCode" << std::endl;
            break;
        default:
            std::cout << "Unknown error" << std::endl;
            break;
        }
    }
   
    std::cin.get();
	return 0;
}