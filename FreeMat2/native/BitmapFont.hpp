#ifndef __BitmapFont_hpp__
#define __BitmapFont_hpp__

#include <string>
#include <vector>

typedef unsigned char byte;
class FM_Glyph {
public:
  int width;
  int height;
  int offset_top;
  int offset_left;
  int advance_x;
  int advance_y;
  std::vector<byte> glyphdat;
  int kerning_x[256];
  int kerning_y[256];
  FM_Glyph(int awidth, int aheight);
  FM_Glyph();
  ~FM_Glyph();
};

class BitmapFont {
  FM_Glyph data[256];
  std::string m_fontname;
  int m_fontsize;
public:
  BitmapFont(std::string fontname, int fontsize);
  FM_Glyph* GetGlyphPointer();
  int GetFontSize();
};

#endif
