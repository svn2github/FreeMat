#include "Color.hpp"

Color::Color(int r, int g, int b) {
  red = r;
  green = g;
  blue = b;
}

Color::Color() {
  red = 0;
  green = 0;
  blue = 0;
}

Color::Color(const Color &copy) {
  red = copy.red;
  green = copy.green;
  blue = copy.blue;
}

Color Color::Blend8(Color fg, Color bg, int level) {
  Color ret;

  ret.red = (fg.red * level + bg.red * (256-level)) >> 8;
  ret.green = (fg.green * level + bg.green * (256-level)) >> 8;
  ret.blue = (fg.blue * level + bg.blue * (256-level)) >> 8;
  return ret;
}
