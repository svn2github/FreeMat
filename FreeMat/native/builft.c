/*
 * gcc -o builft builft.c -I/Users/basu/freetype-2.1.5/include /Users/basu/freetype-2.1.5/objs/.libs/libfreetype.a -lz
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
  if (FT_Init_FreeType(&library)) 
    ERROR("Unable to initialize FreeType library!");
  if (FT_New_Face(library,argv[1],0,&face))
    ERROR("Unsupported file format");
  // 16 x 16 bitmap size
  if (FT_Set_Pixel_Sizes(face, 0, 160))
    ERROR("Unable to set character size");
  FT_Load_Char(face, argv[2][0] , FT_LOAD_RENDER);
  slot = face->glyph;
  printf("bitmap data  - num_grays = %d\n",slot->bitmap.num_grays);
  switch (slot->bitmap.pixel_mode) {
  case FT_PIXEL_MODE_NONE:
    printf("pixel mode none\n");
    break;
  case FT_PIXEL_MODE_MONO:
    printf("pixel mode mono\n");
    break;
  case FT_PIXEL_MODE_GRAY:
    printf("pixel mode gray\n");
    break;
  case FT_PIXEL_MODE_GRAY2:
    printf("pixel mode gray2\n");
    break;
  case FT_PIXEL_MODE_GRAY4:
     printf("pixel mode gray4\n");
    break;
 case FT_PIXEL_MODE_LCD:
    printf("pixel mode LCD\n");
    break;
  case FT_PIXEL_MODE_LCD_V:
    printf("pixel mode LCD_V\n");
    break;
  }
  printf("glyph size %d x %d\n",slot->bitmap.rows,slot->bitmap.width);
  printf("glyph offset %d x %d\n",slot->bitmap_left,slot->bitmap_top);
  printf("glyph increment %d x %d\n",slot->advance.x,slot->advance.y);
  writePPM(slot->bitmap.rows,slot->bitmap.width,slot->bitmap.buffer);
}
