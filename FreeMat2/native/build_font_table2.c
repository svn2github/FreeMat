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
  fp = fopen(argv[3],"w");
  if (!fp)
    ERROR("Unable to open output file");
  slot = face->glyph;
  int kerncount=0;
  for (n=0;n<256;n++) {
    FT_Load_Char(face, n, FT_LOAD_RENDER);
    for (m=0;m<256;m++) {
      FT_Get_Kerning(face, FT_Get_Char_Index(face, n), 
		     FT_Get_Char_Index(face, m), 
		     ft_kerning_default, &kerning);
      if (kerning.x != 0) 
	kerncount++;
    }
  }
  fprintf(fp,"#define KERNCOUNT %d\n",kerncount);
  fprintf(fp,"int glyph_x[%d];\n",256);
  fprintf(fp,"int kernglyph1[%d];\n",kerncount);
  fprintf(fp,"int kernglyph2[%d];\n",kerncount);
  fprintf(fp,"int kerndelta[%d];\n",kerncount);
  fprintf(fp,"\n\nvoid InitializeFontTable() {\n");
  int p=0;
  for (n=0;n<256;n++) {
    FT_Load_Char(face, n, FT_LOAD_RENDER);
    fprintf(fp,"   glyph_x[%d] = %d;\n",n,slot->advance.x);
    for (m=0;m<256;m++) {
      FT_Get_Kerning(face, FT_Get_Char_Index(face, n), 
		     FT_Get_Char_Index(face, m), 
		     ft_kerning_default, &kerning);
      if (kerning.x != 0) {
	fprintf(fp,"   kernglyph1[%d] = %d; ",p,n);
	fprintf(fp,"   kernglyph2[%d] = %d; ",p,m);
	fprintf(fp,"   kerndelta[%d] = %d;\n",p,kerning.x);
	p++;
      }
    }
  }
  fprintf(fp,"}\n");
  fclose(fp);
}
