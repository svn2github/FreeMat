#include "GrayscaleImage.hpp"

GrayscaleImage::GrayscaleImage(int awidth, int aheight, const byte *adata) {
  width = awidth;
  height = aheight;
  data = new byte[width*height];
  if (adata != NULL) 
    memcpy(data,adata,width*height);
}

GrayscaleImage::GrayscaleImage(const GrayscaleImage &copy) {
  width = copy.width;
  height = copy.height;
  data = new byte[width*height];
  if (copy.data != NULL)
    memcpy(data,copy.data,width*height);
}

GrayscaleImage::GrayscaleImage() {
  width = height = 0;
  data = NULL;
}

GrayscaleImage::~GrayscaleImage() {
  delete[] data;
}

byte* GrayscaleImage::GetPixelData() {
  return data;
}

int GrayscaleImage::GetWidth() {
  return width;
}

int GrayscaleImage::GetHeight() {
  return height;
}

void GrayscaleImage::WritePPM(std::string filename) {
  FILE *fp;
  fp = fopen(filename.c_str(),"wb");
  fprintf(fp,"P5\n");
  fprintf(fp,"%d %d\n",width,height);
  fprintf(fp,"255\n");
  fwrite(data,sizeof(char),width*height,fp);
  fclose(fp);
}

void GrayscaleImage::SetPixel(int x, int y, byte val) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height))
    data[y*width+x] = val;
}

byte GrayscaleImage::GetPixel(int x, int y) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height)) 
    return data[y*width+x];
  else
    return 0;
}

void GrayscaleImage::SetAllPixels(byte val) {
  memset(data,val,width*height);
}
