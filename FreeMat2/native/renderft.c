/*
 * Program to render text using the font data dictionary created by 
 * 
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int width;
  int height;
  unsigned char *data;
} FMGrayscaleImage;

typedef struct {
  int width;
  int height;
  int offset_top;
  int offset_left;
  int advance_x;
  int advance_y;
  unsigned char* glyphdat;
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
    fnt[m].glyphdat = (unsigned char*) malloc(fnt[m].height*fnt[m].width);
    for (n=0;n<fnt[m].height*fnt[m].width;n++) {
      int tmp;
      fscanf(fp,"%x ",&tmp);
      fnt[m].glyphdat[n] = (unsigned char) tmp;
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
    printf("character %c %d  width = %d\n",g1,g1,currentFont[g1].advance_x);
    penx += (currentFont[g1].advance_x + currentFont[g1].kerning_x[g2]) >> 6;
    peny += (currentFont[g1].advance_y + currentFont[g1].kerning_y[g2]) >> 6;
  }
  g1 = text[len-1];
  penx += (currentFont[g1].advance_x) >> 6;
  peny += (currentFont[g1].advance_y) >> 6;
  *width = penx;
  *height = peny; 
}

int main() {
  int width, height;

  currentFont = LoadFontDictionary("helv12_pack.dat");
  GetTextExtent("Hello Goodbye!\n",&width,&height);
  printf("Width = %d  Height = %d\n",width,height);
  RenderText("Hello Goodbye!\n",
}
