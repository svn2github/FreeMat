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
FT_Vector kerning;

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
  int i, j, num_chars, n, m;
  int pen_x, pen_y;
  int s_penx, s_peny;
  unsigned char *image;
  int kerningCount;
  FILE *fp;
  if (FT_Init_FreeType(&library)) 
    ERROR("Unable to initialize FreeType library!");
  if (FT_New_Face(library,argv[1],0,&face))
    ERROR("Unsupported file format");
  // 16 x 16 bitmap size
  if (FT_Set_Pixel_Sizes(face, 0, atoi(argv[2])))
    ERROR("Unable to set character size");
  if (argc > 4) {
    if (error = FT_Attach_File(face, argv[4]))
      ERROR("Unable to load afm data");
    printf("Attached file %s\n",argv[4]);
    if (FT_HAS_KERNING(face)) 
      printf("Kerning found\n");
  }
  fp = fopen(argv[3],"wb");
  if (!fp)
    ERROR("Unable to open output file");
  slot = face->glyph;
  for (n=0;n<256;n++) {
    FT_Load_Char(face, n, FT_LOAD_RENDER);
    fprintf(fp,"glyph %d\n",n);
    fprintf(fp,"rows %d\n",slot->bitmap.rows);
    fprintf(fp,"width %d\n",slot->bitmap.width);
    fprintf(fp,"top %d\n",slot->bitmap_top);
    fprintf(fp,"left %d\n",slot->bitmap_left);
    fprintf(fp,"advancex %d\n",slot->advance.x);
    fprintf(fp,"advancey %d\n",slot->advance.y);
    /*
     * Count the kerning data
     */
    kerningCount = 0;
    for (m=0;m<256;m++) {
      FT_Get_Kerning(face, FT_Get_Char_Index(face, n), 
		     FT_Get_Char_Index(face, m), 
		     FT_KERNING_DEFAULT, &kerning);
      if (kerning.x != 0 || kerning.y != 0)
	kerningCount++;
    }
    fprintf(fp,"kerningCount %d\n",kerningCount);
    /*
     * Write the kerning data
     */
    for (m=0;m<256;m++) {
      FT_Get_Kerning(face, FT_Get_Char_Index(face, n), 
		     FT_Get_Char_Index(face, m), 
		     FT_KERNING_DEFAULT, &kerning);
      if (kerning.x != 0 || kerning.y != 0) {
	fprintf(fp,"kern %d %d %d\n",m,kerning.x,kerning.y);
      }
    }
    for (m=0;m<slot->bitmap.rows*slot->bitmap.width;m++) 
      fprintf(fp,"%x ",slot->bitmap.buffer[m]);
    if (slot->bitmap.rows*slot->bitmap.width != 0)
      fprintf(fp,"\n");
  }
  fclose(fp);
}
