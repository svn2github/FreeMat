#include "RGBImage.hpp"

RGBImage::RGBImage(int awidth, int aheight, const byte *adata) {
  width = awidth;
  height = aheight;
  data = new byte[width*height*3];
  if (adata != NULL) 
    memcpy(data,adata,width*height*3);
}

RGBImage::RGBImage(const RGBImage &copy) {
  width = copy.width;
  height = copy.height;
  data = new byte[width*height*3];
  if (copy.data != NULL)
    memcpy(data,copy.data,width*height*3);
}

RGBImage::RGBImage() {
  width = height = 0;
  data = NULL;
}

RGBImage::~RGBImage() {
  delete[] data;
}

byte* RGBImage::GetPixelData() {
  return data;
}

int RGBImage::GetWidth() {
  return width;
}

int RGBImage::GetHeight() {
  return height;
}

void RGBImage::WritePPM(std::string filename) {
  FILE *fp;
  fp = fopen(filename.c_str(),"wb");
  fprintf(fp,"P6\n");
  fprintf(fp,"%d %d\n",width,height);
  fprintf(fp,"255\n");
  fwrite(data,sizeof(char),width*height*3,fp);
  fclose(fp);
}

void RGBImage::SetPixelDirect(int x, int y, byte valred, byte valgreen, byte valblue) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
    data[3*(y*width+x)] = valred;
    data[3*(y*width+x)+1] = valgreen;
    data[3*(y*width+x)+2] = valblue;
  }
}

void RGBImage::GetPixelDirect(int x, int y, byte &valred, byte &valgreen, byte &valblue) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
    valred = data[3*(y*width+x)];
    valgreen = data[3*(y*width+x)+1];
    valblue = data[3*(y*width+x)+2];
  }
}

void RGBImage::SetPixel(int x, int y, Color col) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
    data[3*(y*width+x)] = col.red;
    data[3*(y*width+x)+1] = col.green;
    data[3*(y*width+x)+2] = col.blue;
  }
}

Color RGBImage::GetPixel(int x, int y) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height)) 
    return Color(data[3*(y*width+x)],data[3*(y*width+x)+1],data[3*(y*width+x)+2]);
  else
    return Color(0,0,0);
}

void RGBImage::SetAllPixels(Color col) {
  int n;
  int i;

  n = width*height;
  for (i=0;i<n;i++) {
    data[3*i] = col.red;
    data[3*i+1] = col.green;
    data[3*i+2] = col.blue;
  }
}
