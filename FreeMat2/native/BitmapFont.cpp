#include "BitmapFont.hpp"

FM_Glyph::FM_Glyph() {
  width = 0;
  height = 0;
}

FM_Glyph::FM_Glyph(int awidth, int aheight) {
  width = awidth;
  height = aheight;
  glyphdat.resize(width*height);
}

FM_Glyph::~FM_Glyph() {
}

BitmapFont::BitmapFont(std::string fontname, int fontsize) {
  FILE *fp;
  int n, m, kerncnt;
  int glyphnum, xkern, ykern;
  char buffer[256];
  int width, height;
  
  m_fontname = fontname;
  m_fontsize = fontsize;
  sprintf(buffer,"%s.%d",fontname.c_str(),fontsize);
  fp = fopen(buffer,"rb");
  if (!fp) {
    fprintf(stderr,"Unable to open font dictionary %s\n",buffer);
    exit(1);
  }
  for (m=0;m<256;m++) {
    fscanf(fp,"glyph %d\n",&n);
    if (n != m) {
      fprintf(stderr,"Font dictionary corrupted! (found glyph %d instead of %d)\n",n,m);
      exit(1);
    }
    fscanf(fp,"rows %d\n",&height);
    fscanf(fp,"width %d\n",&width);
    data[m] = FM_Glyph(width,height);
    fscanf(fp,"top %d\n",&data[m].offset_top);
    fscanf(fp,"left %d\n",&data[m].offset_left);
    fscanf(fp,"advancex %d\n",&data[m].advance_x);
    fscanf(fp,"advancey %d\n",&data[m].advance_y);
    memset(data[m].kerning_x,0,256*sizeof(int));
    memset(data[m].kerning_y,0,256*sizeof(int));
    fscanf(fp,"kerningCount %d\n",&kerncnt);
    for (n=0;n<kerncnt;n++) {
      fscanf(fp,"kern %d %d %d\n",&glyphnum,&xkern,&ykern);
      data[m].kerning_x[glyphnum] = xkern;
      data[m].kerning_y[glyphnum] = ykern;
    }
    for (n=0;n<data[m].height*data[m].width;n++) {
      int tmp;
      fscanf(fp,"%x ",&tmp);
      data[m].glyphdat[n] = (byte) tmp;
    }
  }
  fclose(fp);
}

FM_Glyph* BitmapFont::GetGlyphPointer() {
  return data;
}

int BitmapFont::GetFontSize() {
  return m_fontsize;
}
