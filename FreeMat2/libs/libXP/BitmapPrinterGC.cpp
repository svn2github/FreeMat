#include "BitmapPrinterGC.hpp"
#include "RGBImage.hpp"
#include <algorithm>
#include <map>
#include "Exception.hpp"
#include <ctype.h>

BitmapPrinterGC::BitmapPrinterGC(std::string fname) {
  filename = fname;
}

BitmapPrinterGC::~BitmapPrinterGC() {
}

void BitmapPrinterGC::BlitImage(unsigned char *data, int width, int height, int x0, int y0) {
  int np;
  np = filename.find_last_of(".");
  if (np <= 0) 
    throw FreeMat::Exception(std::string("Unable to determine format of output from filename"));
  std::string extension(filename.substr(np));
  std::transform (extension.begin(), extension.end(), 
		 extension.begin(), tolower);
  if (extension == ".jpeg" || extension == ".jpg") 
    WriteJPEGFile(filename, data, width, height);
  else if (extension == ".png")
    WritePNGFile(filename, data, width, height);
  else if (extension == ".tiff" || extension == ".tif")
    WriteTIFFFile(filename, data, width, height); 
  else if (extension == ".eps" || extension == ".ps")
    WriteEPSFile(filename, data, width, height);
  throw FreeMat::Exception(std::string("Unrecognized file extension ") + 
			   extension);
}
