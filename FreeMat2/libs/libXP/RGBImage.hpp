#ifndef __RGBImage_hpp__
#define __RGBImage_hpp__

#include "Color.hpp"
#include <string>
typedef unsigned char byte;

class RGBImage {
  int width;
  int height;
  byte *data;
 public:
  RGBImage(int awidth, int aheight, byte *adata);
  RGBImage(const RGBImage &copy);
  RGBImage();
  ~RGBImage();
  byte* GetPixelData();
  int GetWidth();
  int GetHeight();
  void WritePPM(std::string filename);
  void WriteJPEG(std::string filename);
  void WriteTIFF(std::string filename);
  void WritePNG(std::string filename);
  void SetPixelDirect(int x, int y, byte valred, byte valgreen, byte valblue);
  void GetPixelDirect(int x, int y, byte &valred, byte &valgreen, byte &valblue);
  void SetPixel(int x, int y, Color col);
  Color GetPixel(int x, int y);
  void SetAllPixels(Color col);
};

void WritePNGFile(std::string filename, byte *data, int width, int height);
void WriteJPEGFile(std::string filename, byte *data, int width, int height);
void WriteTIFFFile(std::string filename, byte *data, int width, int height);
void WriteEPSFile(std::string filename, byte *data, int width, int height);
#endif
