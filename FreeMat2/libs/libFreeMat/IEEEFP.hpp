#ifndef __IEEEFP_HPP__
#define __IEEEFP_HPP__

bool IsInfinite(float t);
bool IsInfinite(double t);
bool IsNaN(float t);
bool IsNaN(double t);

#ifdef WIN32
#define M_PI 3.141592653589793
#endif

#endif
