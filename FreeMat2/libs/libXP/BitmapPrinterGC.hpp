#ifndef __BitmapPrinterGC_hpp__
#define __BitmapPrinterGC_hpp__

#include "GraphicsContext.hpp"
#include <string>

class BitmapPrinterGC : public GraphicsContext {
  std::string filename;
public:
  BitmapPrinterGC(std::string fname);
  virtual ~BitmapPrinterGC();
  virtual void BlitImage(unsigned char *data, int width, int height, int x0, int y0);
};
#endif
