/*
 * Program to render text using the font data dictionary created by 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>

typedef unsigned char uint8;

class FMGrayImage {
  int width;
  int height;
  uint8 *data;
 public:
  FMGrayImage(int awidth, int aheight, const uint8 *adata);
  FMGrayImage(const FMGrayImage &copy);
  FMGrayImage();
  ~FMGrayImage();
  uint8 * GetPixelData();
  int GetWidth();
  int GetHeight();
  void WritePPM(std::string filename);
  void SetPixel(int x, int y, uint8 val);
};

void FMGrayImage::SetPixel(int x, int y, uint8 val) {
  if (x>=0 && x < width && y>=0 && y < height)
    data[x+y*width] = val;
}

FMGrayImage::FMGrayImage(int awidth, int aheight, const uint8 *adata) {
  width = awidth;
  height = aheight;
  data = (uint8*) calloc(width*height,1);
  if (adata != NULL) 
    memcpy(data,adata,width*height);
}

FMGrayImage::FMGrayImage(const FMGrayImage &copy) {
  width = copy.width;
  height = copy.height;
  data = (uint8*) calloc(width*height,1);
  memcpy(data,copy.data,width*height);
}

FMGrayImage::FMGrayImage() {
  width = 0;
  height = 0;
  data = NULL;
}

FMGrayImage::~FMGrayImage() {
  if (data)
    free(data);
}

uint8* FMGrayImage::GetPixelData() {
  return data;
}

int FMGrayImage::GetWidth() {
  return width;
}

int FMGrayImage::GetHeight() {
  return height;
}

void FMGrayImage::WritePPM(std::string filename) {
  FILE *fp;
  fp = fopen("dump.pgm","w");
  fprintf(fp,"P5\n");
  fprintf(fp,"%d %d\n",width,height);
  fprintf(fp,"255\n");
  fwrite(data,sizeof(char),width*height,fp);
  fclose(fp);  
}

typedef struct {
  int width;
  int height;
  int offset_top;
  int offset_left;
  int advance_x;
  int advance_y;
  uint8* glyphdat;
  int kerning_x[256];
  int kerning_y[256];
} FM_Glyph;

FM_Glyph *currentFont;

FM_Glyph* LoadFontDictionary(const char *filename) {
  FILE *fp;
  FM_Glyph *fnt;
  int n, m, kerncnt;
  int glyphnum, xkern, ykern;

  fnt = (FM_Glyph*) malloc(sizeof(FM_Glyph)*256);
  fp = fopen(filename,"rb");
  if (!fp) {
    fprintf(stderr,"Unable to open font dictionary %s\n",filename);
    exit(1);
  }
  for (m=0;m<256;m++) {
    fscanf(fp,"glyph %d\n",&n);
    if (n != m) {
      fprintf(stderr,"Font dictionary corrupted! (found glyph %d instead of %d)\n",n,m);
      exit(1);
    }
    fscanf(fp,"rows %d\n",&fnt[m].height);
    fscanf(fp,"width %d\n",&fnt[m].width);
    fscanf(fp,"top %d\n",&fnt[m].offset_top);
    fscanf(fp,"left %d\n",&fnt[m].offset_left);
    fscanf(fp,"advancex %d\n",&fnt[m].advance_x);
    fscanf(fp,"advancey %d\n",&fnt[m].advance_y);
    memset(fnt[m].kerning_x,0,256*sizeof(int));
    memset(fnt[m].kerning_y,0,256*sizeof(int));
    fscanf(fp,"kerningCount %d\n",&kerncnt);
    for (n=0;n<kerncnt;n++) {
      fscanf(fp,"kern %d %d %d\n",&glyphnum,&xkern,&ykern);
      fnt[m].kerning_x[glyphnum] = xkern;
      fnt[m].kerning_y[glyphnum] = ykern;
    }
    fnt[m].glyphdat = (uint8*) malloc(fnt[m].height*fnt[m].width);
    for (n=0;n<fnt[m].height*fnt[m].width;n++) {
      int tmp;
      fscanf(fp,"%x ",&tmp);
      fnt[m].glyphdat[n] = (uint8) tmp;
    }
  }
  fclose(fp);
  return fnt;
}

void GetTextExtent(const char *text, int *width, int *height) {
  int penx, peny;
  int len, i, g1, g2;
  
  penx = 0;
  peny = 0;
  len = strlen(text);
  for (i=0;i<len-1;i++) {
    g1 = text[i];
    g2 = text[i+1];
    penx += (currentFont[g1].advance_x + currentFont[g1].kerning_x[g2]) >> 6;
    peny += (currentFont[g1].advance_y + currentFont[g1].kerning_y[g2]) >> 6;
  }
  g1 = text[len-1];
  penx += (currentFont[g1].advance_x) >> 6;
  peny += (currentFont[g1].advance_y) >> 6;
  *width = penx;
  *height = peny; 
}

void RenderText(FMGrayImage &out, std::string text, int xpos, int ypos) {
  int penx, peny;
  int length, m, g1, g2;
  int i, j;
  FM_Glyph gptr;

  penx = xpos;
  peny = ypos;
  length = text.size();
  for (m=0;m<length;m++) {
    g1 = text[m];
    gptr = currentFont[g1];
    for (i=0;i<gptr.height;i++)
      for (j=0;j<gptr.width;j++)
	if (gptr.glyphdat[i*gptr.width+j])
	  out.SetPixel(penx+gptr.offset_left+j,peny-gptr.offset_top+i,
		       255-gptr.glyphdat[i*gptr.width+j]);
    penx += currentFont[g1].advance_x >> 6;
    peny += currentFont[g1].advance_y >> 6;
    if (m<length-1) {
      g2 = text[m+1];
      penx += currentFont[g1].kerning_x[g2] >> 6;
      peny += currentFont[g1].kerning_y[g2] >> 6;
    }
  }
}

int main() {
  int width, height;
  FMGrayImage out(400,200,NULL);
  memset(out.GetPixelData(),255,400*200);
  currentFont = LoadFontDictionary("helv12_pack.dat");
  GetTextExtent("Hello Goodbye!\n",&width,&height);
  printf("Width = %d  Height = %d\n",width,height);
  RenderText(out,"Hello Goodbye!   AVA Hee Haw.  You're not going and that's final.\n",10,80);
  out.WritePPM("dump.ppm");
  return 0;
}
