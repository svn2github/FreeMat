#include <stdio.h>
#include <stdlib.h>
#include <math.h>
char *image;
int fg_red, fg_green, fg_blue;
int bg_red, bg_green, bg_blue;

void writePPM() {
  FILE *fp;
  fp = fopen("dump.ppm","w");
  fprintf(fp,"P6\n");
  fprintf(fp,"%d %d\n",512,512);
  fprintf(fp,"255\n");
  fwrite(image,sizeof(char),512*512*3,fp);
  fclose(fp);
}

void setBGColor(int r, int g, int b) {
  bg_red = r;
  bg_green = g;
  bg_blue = b;
}

void setFGColor(int r, int g, int b) {
  fg_red = r;
  fg_green = g;
  fg_blue = b;
}

void setPixel(int x, int y, int c) {
  c = c & 0xff;
  image[3*(x*512+y)] = (c*fg_red) >> 8;
  image[3*(x*512+y)+1] = (c*fg_green) >> 8;
  image[3*(x*512+y)+2] = (c*fg_blue) >> 8;
}

void blendPixel(int x, int y, int c) {
  c = c & 0xff;
  image[3*(x*512+y)] = (c*fg_red+(256-c)*image[3*(x*512+y)]) >> 8;
  image[3*(x*512+y)+1] = (c*fg_red+(256-c)*image[3*(x*512+y)+1]) >> 8;
  image[3*(x*512+y)+2] = (c*fg_red+(256-c)*image[3*(x*512+y)+2]) >> 8;
}

void Plot8CirclePoints(int cx, int cy, int x, int y, int c) {
  putPixel(cx+x,cy+y,c);
  putPixel(cx-x,cy+y,c);
  putPixel(cx-x,cy-y,c);
  putPixel(cx+x,cy-y,c);
  putPixel(cx+y,cy+x,c);
  putPixel(cx-y,cy+x,c);
  putPixel(cx-y,cy-x,c);
  putPixel(cx+y,cy-x,c);  
}

void PlotCircle(int cx, int cy, int radius) {
  int x, y, g;
  int diagonalInc, rightInc;

  x = 0;
  y = radius;
  g = 3 - (2 * radius);
  diagonalInc = 10 - 4*radius;
  rightInc = 6;
  while (x <= y) {
    Plot8CirclePoints(cx, cy, x, y, 100);
    if (g >= 0) {
      g += diagonalInc;
      diagonalInc += 8;
      y -= 1;
    } else {
      g += rightInc;
      diagonalInc += 4;
    }
    rightInc += 4;
    x++;
  }
}
 
void WuCircle(int cx, int cy, int R) {
  int x = R;
  int y = 0;
  float T = 0;
  float D;
  int Rsq;
  int ysq;
  float eps;
  Plot8CirclePoints(cx, cy, x, y, 100);
  Rsq = R*R;

  while (x>(y+1)) {
    y++;
    ysq = y*y;
    eps = sqrt(Rsq-ysq);
    D = ceil(eps) - eps;
    if (D < T)
      x--;
    Plot8CirclePoints(cx, cy, x, y, 100*(1-D));
    Plot8CirclePoints(cx, cy, x-1, y, 100*D);
    T = D;
  }
}

float coverage(float d, float r) {
  if (d>=r)
    return 0.0;
  else
    return 0.5-d*sqrt(r*r-d*d)/(M_PI*r*r) - 1/M_PI*asin(d/r);
}

int IntensifyPixel(int x, int y, float thickness, float distance)
{
  float cov;
  if ((cov = coverage(thickness,distance))>0)
    putPixel(x,y,100*cov);
  return cov;
}

int ThickAntiAliasedLine(int x1, int y1, int x2, int y2, float thickness)
{
  // initial values used in Bresenham's algorithm
  int dx = x2-x1;
  int dy = y2-y1;
  int incrE = 2*dy;
  int incrNE = 2*(dy-dx);
  int d = 2*dy-dx;
  
  int two_v_dx = 0;   //numerator, v=0 for start pixel
  float invDenom = 1/(2*sqrt(dx*dx+dy*dy)); //precomputed inverse denominator
  float two_dx_invDenom = 2*dx*invDenom;   //precomputed constant
  
  int x = x1;
  int y = y1;
  
  int i;
  IntensifyPixel(x,y,thickness,0);
  for (i=1; IntensifyPixel(x,y+i,thickness,i*two_dx_invDenom); i++);
  for (i=1; IntensifyPixel(x,y-i,thickness,i*two_dx_invDenom); i++);    
  while (x<x2)
    {
      if (d<0)   //Choose E
	{
	  two_v_dx = d+dx;
	  d += incrE;
	  x++;
	}
      else      //Choose NE
	{
	  two_v_dx = d-dx;
	  d += incrNE;
	  x++;
	  y++;
	}
      
      //Now set chosen pixel and its neighbours
      
      IntensifyPixel(x,y,thickness,two_v_dx*invDenom);
      for (i=1; IntensifyPixel(x, y+i, thickness, 
			       i*two_dx_invDenom-two_v_dx*invDenom);i++);
      for (i=1; IntensifyPixel(x, y-i, thickness, 
			       i*two_dx_invDenom+ two_v_dx*invDenom);i++);       
    }
}


/* Function to draw an antialiased line from (X0,Y0) to (X1,Y1), using an
 * antialiasing approach published by Xiaolin Wu in the July 1991 issue of
 * Computer Graphics. Requires that the palette be set up so that there
 * are NumLevels intensity levels of the desired drawing color, starting at
 * color BaseColor (100% intensity) and followed by (NumLevels-1) levels of
 * evenly decreasing intensity, with color (BaseColor+NumLevels-1) being 0%
 * intensity of the desired drawing color (black). This code is suitable for
 * use at screen resolutions, with lines typically no more than 1K long; for
 * longer lines, 32-bit error arithmetic must be used to avoid problems with
 * fixed-point inaccuracy. No clipping is performed in DrawWuLine; it must be
 * performed either at a higher level or in the putPixel function.
 * Tested with Borland C++ 3.0 in C compilation mode and the small model.
 */

/* Wu antialiased line drawer.
 * (X0,Y0),(X1,Y1) = line to draw
 * BaseColor = color # of first color in block used for antialiasing, the
 *          100% intensity version of the drawing color
 * NumLevels = size of color block, with BaseColor+NumLevels-1 being the
 *          0% intensity version of the drawing color
 * IntensityBits = log base 2 of NumLevels; the # of bits used to describe
 *          the intensity of the drawing color. 2**IntensityBits==NumLevels
 */
void DrawWuLine(int X0, int Y0, int X1, int Y1, int BaseColor, int NumLevels,
   unsigned int IntensityBits)
{
   unsigned int IntensityShift, ErrorAdj, ErrorAcc;
   unsigned int ErrorAccTemp, Weighting, WeightingComplementMask;
   int DeltaX, DeltaY, Temp, XDir;

   /* Make sure the line runs top to bottom */
   if (Y0 > Y1) {
      Temp = Y0; Y0 = Y1; Y1 = Temp;
      Temp = X0; X0 = X1; X1 = Temp;
   }
   /* Draw the initial pixel, which is always exactly intersected by
      the line and so needs no weighting */
   putPixel(X0, Y0, BaseColor);

   if ((DeltaX = X1 - X0) >= 0) {
      XDir = 1;
   } else {
      XDir = -1;
      DeltaX = -DeltaX; /* make DeltaX positive */
   }
   /* Special-case horizontal, vertical, and diagonal lines, which
      require no weighting because they go right through the center of
      every pixel */
   if ((DeltaY = Y1 - Y0) == 0) {
      /* Horizontal line */
      while (DeltaX-- != 0) {
         X0 += XDir;
         putPixel(X0, Y0, BaseColor);
      }
      return;
   }
   if (DeltaX == 0) {
      /* Vertical line */
      do {
         Y0++;
         putPixel(X0, Y0, BaseColor);
      } while (--DeltaY != 0);
      return;
   }
   if (DeltaX == DeltaY) {
      /* Diagonal line */
      do {
         X0 += XDir;
         Y0++;
         putPixel(X0, Y0, BaseColor);
      } while (--DeltaY != 0);
      return;
   }
   /* Line is not horizontal, diagonal, or vertical */
   ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
   /* # of bits by which to shift ErrorAcc to get intensity level */
   IntensityShift = 32 - IntensityBits;
   /* Mask used to flip all bits in an intensity weighting, producing the
      result (1 - intensity weighting) */
   WeightingComplementMask = NumLevels - 1;
   /* Is this an X-major or Y-major line? */
   if (DeltaY > DeltaX) {
      /* Y-major line; calculate 16-bit fixed-point fractional part of a
         pixel that X advances each time Y advances 1 pixel, truncating the
         result so that we won't overrun the endpoint along the X axis */
      ErrorAdj = ((unsigned long long) DeltaX << 32) / (unsigned long long) DeltaY;
      /* Draw all pixels other than the first and last */
      while (--DeltaY) {
         ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
         ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
         if (ErrorAcc <= ErrorAccTemp) {
            /* The error accumulator turned over, so advance the X coord */
            X0 += XDir;
         }
         Y0++; /* Y-major, so always advance Y */
         /* The IntensityBits most significant bits of ErrorAcc give us the
            intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
         Weighting = ErrorAcc >> IntensityShift;
         putPixel(X0, Y0, BaseColor - Weighting);
         putPixel(X0 + XDir, Y0,
               BaseColor - (Weighting ^ WeightingComplementMask));
      }
      /* Draw the final pixel, which is always exactly intersected by the line
         and so needs no weighting */
      putPixel(X1, Y1, BaseColor);
      return;
   }
   /* It's an X-major line; calculate 16-bit fixed-point fractional part of a
      pixel that Y advances each time X advances 1 pixel, truncating the
      result to avoid overrunning the endpoint along the X axis */
   ErrorAdj = ((unsigned long long) DeltaY << 32) / (unsigned long long) DeltaX;
   /* Draw all pixels other than the first and last */
   while (--DeltaX) {
      ErrorAccTemp = ErrorAcc;   /* remember currrent accumulated error */
      ErrorAcc += ErrorAdj;      /* calculate error for next pixel */
      if (ErrorAcc <= ErrorAccTemp) {
         /* The error accumulator turned over, so advance the Y coord */
         Y0++;
      }
      X0 += XDir; /* X-major, so always advance X */
      /* The IntensityBits most significant bits of ErrorAcc give us the
         intensity weighting for this pixel, and the complement of the
         weighting for the paired pixel */
      Weighting = ErrorAcc >> IntensityShift;
      putPixel(X0, Y0, BaseColor - Weighting);
      putPixel(X0, Y0 + 1,
            BaseColor + (Weighting - WeightingComplementMask));
   }
   /* Draw the final pixel, which is always exactly intersected by the line
      and so needs no weighting */
   putPixel(X1, Y1, BaseColor);
}

int main() {
  image = (char*) malloc(512*512*3);
  memset(image,0,512*512*3);
  PlotCircle(256,256,100);
  WuCircle(256,256,150);
  //  ThickAntiAliasedLine(52,37,129,307,2.3);
  DrawWuLine(52,37,129,307,255,256,8);
  writePPM();
}
