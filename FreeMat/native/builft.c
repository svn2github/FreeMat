/*
 * gcc -o builft builft.c -I/Users/basu/freetype-2.1.5/include /Users/basu/freetype-2.1.5/objs/.libs/libfreetype.a -lz
 * Helvetical is /usr/share/fonts/default/n019003l
 */
#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdlib.h>
#include <stdio.h>

#define ERROR(x) {fprintf(stderr,"%s  %x\n",x,error); exit(1);}

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
  if (FT_Init_FreeType(&library)) 
    ERROR("Unable to initialize FreeType library!");
  if (FT_New_Face(library,argv[1],0,&face))
    ERROR("Unsupported file format");
  // 16 x 16 bitmap size
  if (FT_Set_Pixel_Sizes(face, 0, atoi(argv[3])))
    ERROR("Unable to set character size");
  num_chars = strlen(argv[2]);
  image = (unsigned char *) malloc(512*512);
  memset(image,255,512*512);
  pen_x = 100; pen_y = 100;
  s_penx = pen_x << 6;
  s_peny = pen_y << 6;
  slot = face->glyph;
  for (n=0;n<num_chars;n++) {
    printf("Rendering character %c %d\n",argv[2][n],n);
    if (error = FT_Load_Char(face, argv[2][n] , FT_LOAD_RENDER))
      ERROR("Unable to render character");
    printf("bitmap size %d %d\n",slot->bitmap.rows,slot->bitmap.width);
    for (i=0;i<slot->bitmap.rows;i++)
      for (j=0;j<slot->bitmap.width;j++)
	image[512*(pen_y-slot->bitmap_top+i)+(pen_x+slot->bitmap_left+j)] = 
	  255 - slot->bitmap.buffer[i*slot->bitmap.width+j];
    s_penx += slot->advance.x;
    s_peny += slot->advance.y;
    pen_x = s_penx >> 6;
    pen_y = s_peny >> 6;
  }
  writePPM(512,512,image);
/*   writePPM(slot->bitmap.rows,slot->bitmap.width,slot->bitmap.buffer); */
}
