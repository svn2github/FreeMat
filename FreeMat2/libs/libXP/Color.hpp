#ifndef __Color_hpp__
#define __Color_hpp__

class Color {
 public:
  int red;
  int green;
  int blue;
  Color();
  Color(int r, int g, int b);
  Color(const Color &copy);
  Color(const char* name);
  static Color Blend8(Color, Color, int);
};

#endif
