#include "Color.hpp"
#include "Exception.hpp"
#include <string.h>

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

Color::Color(const char* name) {
  if (strcmp(name,"black")==0) {
    red = green = blue = 0;
  } else if (strcmp(name,"white")==0) {
    red = green = blue = 255;
  } else if (strcmp(name,"light grey")==0) {
    red = green = blue = 211;
  } else if (strcmp(name,"grey")==0) {
    red = green = blue = 100;
  } else if (strcmp(name,"yellow")==0) {
    red = green = 255; blue = 0;
  } else if (strcmp(name,"magenta")==0) {
    red = blue = 255; green = 0;
  } else if (strcmp(name,"cyan")==0) {
    red = 0; green = blue = 255;
  } else if (strcmp(name,"red")==0) {
    red = 255; green = blue = 0;
  } else if (strcmp(name,"green")==0) {
    green = 255; red = blue = 0;
  } else if (strcmp(name,"blue")==0) {
    blue = 255; red = green = 0;
  } else
    throw FreeMat::Exception("Unrecognized color!");
}

Color Color::Blend8(Color fg, Color bg, int level) {
  Color ret;

  ret.red = (fg.red * level + bg.red * (256-level)) >> 8;
  ret.green = (fg.green * level + bg.green * (256-level)) >> 8;
  ret.blue = (fg.blue * level + bg.blue * (256-level)) >> 8;
  return ret;
}
