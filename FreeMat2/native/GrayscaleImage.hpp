#ifndef __GrayscaleImage_hpp__
#define __GrayscaleImage_hpp__

#include <string>
typedef unsigned char byte;
class GrayscaleImage {
  int width;
  int height;
  byte *data;
 public:
  GrayscaleImage(int awidth, int aheight, const byte *adata);
  GrayscaleImage(const GrayscaleImage &copy);
  GrayscaleImage();
  ~GrayscaleImage();
  byte* GetPixelData();
  int GetWidth();
  int GetHeight();
  void WritePPM(std::string filename);
  void SetPixel(int x, int y, byte val);
  byte GetPixel(int x, int y);
  void SetAllPixels(byte val);
};

#endif
