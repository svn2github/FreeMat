/*
 * gcc -o builft builft.c -I/Users/basu/freetype-2.1.5/include /Users/basu/freetype-2.1.5/objs/.libs/libfreetype.a -lz
 * Helvetical is /usr/share/fonts/default/n019003l
 */
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdlib.h>
#include <stdio.h>

#define ERROR(x) {fprintf(stderr,x); exit(1);}

FT_Library library;
FT_Face face;
FT_Error error;
FT_UInt glyph_index;
FT_GlyphSlot slot;

typedef struct {
  int width;
  int height;
  int offset_top;
  int offset_left;
  int advance_x;
  int advance_y;
  unsigned char* glyphdat;
} FM_Glyph;

void writePPM(int width, int height, char*image) {
  FILE *fp;
  fp = fopen("dump.pgm","w");
  fprintf(fp,"P5\n");
  fprintf(fp,"%d %d\n",height,width);
  fprintf(fp,"255\n");
  fwrite(image,sizeof(char),width*height,fp);
  fclose(fp);
}

int main(int argc, char* argv[]) {
  int i, j, num_chars, n;
  int pen_x, pen_y;
  int s_penx, s_peny;
  unsigned char *image;
  FILE *fp;
  if (FT_Init_FreeType(&library)) 
    ERROR("Unable to initialize FreeType library!");
  if (FT_New_Face(library,argv[1],0,&face))
    ERROR("Unsupported file format");
  // 16 x 16 bitmap size
  if (FT_Set_Pixel_Sizes(face, 0, atoi(argv[2])))
    ERROR("Unable to set character size");
  fp = fopen(argv[3],"wb");
  if (!fp)
    ERROR("Unable to open output file");
  slot = face->glyph;
  for (n=0;n<256;n++) {
    FT_Load_Char(face, n, FT_LOAD_RENDER);
    fwrite(&slot->bitmap.rows,sizeof(int),1,fp);
    fwrite(&slot->bitmap.width,sizeof(int),1,fp);
    fwrite(&slot->bitmap_top,sizeof(int),1,fp);
    fwrite(&slot->bitmap_left,sizeof(int),1,fp);
    fwrite(&slot->advance.x,sizeof(int),1,fp);
    fwrite(&slot->advance.y,sizeof(int),1,fp);
    fwrite(slot->bitmap.buffer,sizeof(char),slot->bitmap.rows*slot->bitmap.width,fp);
  }
  fclose(fp);
}
