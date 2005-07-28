#include "IEEEFP.hpp"

static bool endianDetected = false;
static bool bigEndian = false;

union {
  long l;
  char c[sizeof (long)];
} u;

void CheckBigEndian() {
  u.l = 1;
  endianDetected = true;
  bigEndian = (u.c[sizeof(long) - 1] == 1);
  
}

bool IsInfinite(float t) {
  union {
    float f;
    unsigned int i;
  } u;
  u.f = t;
  if (((u.i & 0x7f800000) == 0x7f800000) && ((u.i & 0x007fffff) == 0))
    return true;
  return false;
}

bool IsInfinite(double t) {
  union
  {
    double d;
    unsigned int i[2];
  } u;
  u.d = t;
  if (!endianDetected) 
    CheckBigEndian();
  if (!bigEndian) {
    if( ((u.i[1] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[1] & 0x000fffff) == 0) && (u.i[0] == 0)))
      return true;
  } else {
    if( ((u.i[0] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[0] & 0x000fffff) == 0) && (u.i[1] == 0)))
      return true;
  }
  return false;

}

bool IsNaN(int t) {
  return true;
}

bool IsNaN(unsigned int t) {
  return true;
}

bool IsNaN(float t) {
  union {
    float f;
    unsigned int i;
  } u;
  u.f = t;
  if (((u.i & 0x7f800000) == 0x7f800000) && ((u.i & 0x007fffff) != 0))
    return true; 
  return false;
}

bool IsNaN(double t) {
  union
  {
    double d;
    unsigned int i[2];
  } u;
  u.d = t;

  if (!endianDetected) 
    CheckBigEndian();
  if (!bigEndian) {
    if( ((u.i[1] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[1] & 0x000fffff) != 0) || (u.i[0] != 0)))
      return true;
  } else {
    if( ((u.i[0] & 0x7ff00000) == 0x7ff00000)
	&& (((u.i[0] & 0x000fffff) != 0) || (u.i[1] != 0)))
      return true;
  }
  return false;
}

bool IsFinite(float t) {
  return (!(IsNaN(t) || IsInfinite(t)));
}

bool IsFinite(double t) {
  return (!(IsNaN(t) || IsInfinite(t)));
}
