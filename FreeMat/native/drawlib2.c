#include <stdio.h>
#include <stdlib.h>
#include <math.h>
unsigned char *image;
int fg_red, fg_green, fg_blue;
int bg_red, bg_green, bg_blue;
int pen_style;
int pen_state;

enum {
  PEN_SOLID,
  PEN_DASHED,
  PEN_DOTTED,
  PEN_DASH_DOT
} PenStyle;

int penDraws() {
  int qstate;
  qstate = pen_state++;
  switch(pen_style) {
  case PEN_SOLID:
    return 1;
  case PEN_DASHED:
    return (qstate & 4);
  case PEN_DOTTED:
    return (qstate & 2);
  case PEN_DASH_DOT:
    qstate = qstate & 7;
    return (qstate == 0 || qstate == 1 || qstate == 2 || qstate == 5);
  }
}

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
  c = c & 0x1ff;
  image[3*(x*512+y)] = (c*fg_red) >> 8;
  image[3*(x*512+y)+1] = (c*fg_green) >> 8;
  image[3*(x*512+y)+2] = (c*fg_blue) >> 8;
}

void blendPixel(int x, int y, int c) {
  c = c & 0x1ff;
  image[3*(x*512+y)] = (c*fg_red+(256-c)*image[3*(x*512+y)]) >> 8;
  image[3*(x*512+y)+1] = (c*fg_green+(256-c)*image[3*(x*512+y)+1]) >> 8;
  image[3*(x*512+y)+2] = (c*fg_blue+(256-c)*image[3*(x*512+y)+2]) >> 8;
}

void Plot8CirclePoints(int cx, int cy, int x, int y, int c) {
  blendPixel(cx+x,cy+y,c);
  blendPixel(cx-x,cy+y,c);
  blendPixel(cx-x,cy-y,c);
  blendPixel(cx+x,cy-y,c);
  blendPixel(cx+y,cy+x,c);
  blendPixel(cx-y,cy+x,c);
  blendPixel(cx-y,cy-x,c);
  blendPixel(cx+y,cy-x,c);  
}

void WuCircle(int cx, int cy, int R) {
  int x = R;
  int y = 0;
  float T = 0;
  float D;
  int Rsq;
  int ysq;
  float eps;
  Plot8CirclePoints(cx, cy, x, y, 256);
  Rsq = R*R;

  while (x>(y+1)) {
    y++;
    ysq = y*y;
    eps = sqrt(Rsq-ysq);
    D = ceil(eps) - eps;
    if (D < T)
      x--;
    Plot8CirclePoints(cx, cy, x, y, 256*(1-D));
    Plot8CirclePoints(cx, cy, x-1, y, 256*D);
    T = D;
  }
}

/* Wu antialiased line drawer.
 * (X0,Y0),(X1,Y1) = line to draw
 * BaseColor = color # of first color in block used for antialiasing, the
 *          100% intensity version of the drawing color
 * NumLevels = size of color block, with BaseColor+NumLevels-1 being the
 *          0% intensity version of the drawing color
 * IntensityBits = log base 2 of NumLevels; the # of bits used to describe
 *          the intensity of the drawing color. 2**IntensityBits==NumLevels
 */
void DrawWuLine(int X0, int Y0, int X1, int Y1)
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
   blendPixel(X0, Y0, 256);

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
         blendPixel(X0, Y0, 256);
      }
      return;
   }
   if (DeltaX == 0) {
      /* Vertical line */
      do {
         Y0++;
         blendPixel(X0, Y0, 256);
      } while (--DeltaY != 0);
      return;
   }
   if (DeltaX == DeltaY) {
      /* Diagonal line */
      do {
         X0 += XDir;
         Y0++;
         blendPixel(X0, Y0, 256);
      } while (--DeltaY != 0);
      return;
   }
   /* Line is not horizontal, diagonal, or vertical */
   ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
   /* # of bits by which to shift ErrorAcc to get intensity level */
   IntensityShift = 32 - 8;
   /* Mask used to flip all bits in an intensity weighting, producing the
      result (1 - intensity weighting) */
   WeightingComplementMask = 256 - 1;
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
         /* The 8 most significant bits of ErrorAcc give us the
            intensity weighting for this pixel, and the complement of the
            weighting for the paired pixel */
         Weighting = ErrorAcc >> IntensityShift;
         blendPixel(X0, Y0, 256 - Weighting);
         blendPixel(X0 + XDir, Y0,
               256 - (Weighting ^ WeightingComplementMask));
      }
      /* Draw the final pixel, which is always exactly intersected by the line
         and so needs no weighting */
      blendPixel(X1, Y1, 256);
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
      /* The 8 most significant bits of ErrorAcc give us the
         intensity weighting for this pixel, and the complement of the
         weighting for the paired pixel */
      Weighting = ErrorAcc >> IntensityShift;
      blendPixel(X0, Y0, 256 - Weighting);
      blendPixel(X0, Y0 + 1,
            256 + (Weighting - WeightingComplementMask));
   }
   /* Draw the final pixel, which is always exactly intersected by the line
      and so needs no weighting */
   blendPixel(X1, Y1, 256);
}

/* Wu antialiased line drawer.
 * (X0,Y0),(X1,Y1) = line to draw
 * BaseColor = color # of first color in block used for antialiasing, the
 *          100% intensity version of the drawing color
 * NumLevels = size of color block, with BaseColor+NumLevels-1 being the
 *          0% intensity version of the drawing color
 * IntensityBits = log base 2 of NumLevels; the # of bits used to describe
 *          the intensity of the drawing color. 2**IntensityBits==NumLevels
 */
void DrawWuLineStyle(int X0, int Y0, int X1, int Y1)
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
  if (penDraws())
    blendPixel(X0, Y0, 256);

  if ((X0 == X1) && (Y0 == Y1))
    return;
  
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
      if (penDraws())
	blendPixel(X0, Y0, 256);
    }
    return;
  }
  if (DeltaX == 0) {
    /* Vertical line */
    do {
      Y0++;
      if (penDraws())
	blendPixel(X0, Y0, 256);
    } while (--DeltaY != 0);
    return;
  }
  if (DeltaX == DeltaY) {
    /* Diagonal line */
    do {
      X0 += XDir;
      Y0++;
      if (penDraws())
	blendPixel(X0, Y0, 256);
    } while (--DeltaY != 0);
    return;
  }
  /* Line is not horizontal, diagonal, or vertical */
  ErrorAcc = 0;  /* initialize the line error accumulator to 0 */
  /* # of bits by which to shift ErrorAcc to get intensity level */
  IntensityShift = 32 - 8;
  /* Mask used to flip all bits in an intensity weighting, producing the
     result (1 - intensity weighting) */
  WeightingComplementMask = 256 - 1;
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
      /* The 8 most significant bits of ErrorAcc give us the
	 intensity weighting for this pixel, and the complement of the
	 weighting for the paired pixel */
      Weighting = ErrorAcc >> IntensityShift;
      if (penDraws()) {
	blendPixel(X0, Y0, 256 - Weighting);
	blendPixel(X0 + XDir, Y0,
		   256 - (Weighting ^ WeightingComplementMask));
      }
    }
    /* Draw the final pixel, which is always exactly intersected by the line
       and so needs no weighting */
    if (penDraws())
      blendPixel(X1, Y1, 256);
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
    /* The 8 most significant bits of ErrorAcc give us the
       intensity weighting for this pixel, and the complement of the
       weighting for the paired pixel */
    Weighting = ErrorAcc >> IntensityShift;
    if (penDraws()) {
      blendPixel(X0, Y0, 256 - Weighting);
      blendPixel(X0, Y0 + 1,
		 256 + (Weighting - WeightingComplementMask));
    }
   }
  /* Draw the final pixel, which is always exactly intersected by the line
     and so needs no weighting */
  if (penDraws())
    blendPixel(X1, Y1, 256);
}

/* Problem  with current design - if a line goes left to right, and then right to left, the pattern is not properly conserved.  Might be better to rewrite the Wu anti-aliasing renderer so that lines are traced from start to stop */
int main() {
  int t;
  int lastx, lasty;
  image = (unsigned char*) malloc(512*512*3);
  memset(image,255,512*512*3);
  setBGColor(255,0,0);
  setFGColor(0,0,0);
  WuCircle(256,256,150);
/*   DrawWuLine(52,37,129,307); */
  pen_state = 0;
  pen_style = PEN_DASHED;
  DrawWuLineStyle(52,37,129,307);
  pen_state = 0;
  pen_style = PEN_DOTTED;
  DrawWuLineStyle(72,37,149,307);
  pen_state = 0;
  pen_style = PEN_DASH_DOT;
  DrawWuLineStyle(92,37,169,307);
  pen_state = 0;
  pen_style = PEN_DOTTED;
  lastx = 100;
  lasty = 250;
  for (t=100;t < 400;t++) {
    float rad = (t-100.0)/400*2*M_PI*3;
    float y = sin(rad)*50 + 250;
    printf("line from %d %d to %d %d\n",lastx,lasty,t,(int)y);
    DrawWuLineStyle(lastx,lasty,t,(int)y);
    lastx = t; lasty = y;
  }
  writePPM();
}
