#include "RGBImageGC.hpp"
#include <math.h>

RGBImageGC::RGBImageGC(RGBImage& surface) : c_font("swiss",12) ,
					    c_bg(255,255,255), c_fg(0,0,0), 
					    c_linestyle(LINE_SOLID), 
					    canvas(surface)
{
}

RGBImageGC::~RGBImageGC() {
}

// Methods for generic GCs
Point2D RGBImageGC::GetCanvasSize() {
  return Point2D(canvas.GetWidth(),canvas.GetHeight());
}

Point2D RGBImageGC::GetTextExtent(std::string text) {
  FM_Glyph *currentFont;
  int penx;
  int stringheight, charheight;
  int len, i, g1, g2;
  
  currentFont = c_font.GetGlyphPointer();
  penx = 0;
  len = text.size();
  stringheight = 0;
  for (i=0;i<len-1;i++) {
    g1 = text[i];
    g2 = text[i+1];
    penx += (currentFont[g1].advance_x + currentFont[g1].kerning_x[g2]) >> 6;
    charheight = currentFont[g1].offset_top;
    stringheight = (stringheight > charheight) ? stringheight : charheight;
  }
  g1 = text[len-1];
  penx += currentFont[g1].offset_left + currentFont[g1].width;
  return Point2D(penx,stringheight);
}

bool RGBImageGC::InsideClippingRegion(int x, int y) {
  bool pixclipped = false;
  if (clipregions.size()>0)
    pixclipped = clipregions.back().TestOutside(Point2D(x,y));
  return !pixclipped;
}

void RGBImageGC::SetClippedPixel(int x, int y, Color col) {
  if (InsideClippingRegion(x,y))
    canvas.SetPixel(x,y,col);
}

void RGBImageGC::BlendPixel(int x, int y, int level) {
  if (InsideClippingRegion(x,y)) {
    Color bpix(canvas.GetPixel(x,y));
    Color toWrite(Color::Blend8(c_fg,bpix,level));
    canvas.SetPixel(x,y,toWrite);
  }
}

bool RGBImageGC::PenDraws() {
  int qstate;
  qstate = penstate++;
  switch(c_linestyle) {
  case LINE_SOLID:
    return true;
  case LINE_DASHED:
    return (qstate & 4);
  case LINE_DOTTED:
    return (qstate & 2);
  case LINE_DASH_DOT:
    qstate = qstate % 16;
    return ((qstate >=0 && qstate < 6) || (qstate >=10 && qstate < 12));
  }  
  return false;
}

// [c -s]
// [s c]
//
void RGBImageGC::DrawText(std::string text, Point2D pos, OrientationType orient) {
  FM_Glyph *currentFont;
  int penx, peny;
  int length, m, g1, g2;
  int delx, dely;
  int m11, m12, m21, m22;
  int i, j;
  
  switch (orient) {
  case ORIENT_0:
    m11 = 1;
    m12 = 0;
    m21 = 0;
    m22 = 1;
    break;
  case ORIENT_90:
    m11 = 0;
    m12 = 1;
    m21 = -1;
    m22 = 0;
    break;    
  case ORIENT_180:
    m11 = -1;
    m12 = 0;
    m21 = 0;
    m22 = -1;
    break;
  case ORIENT_270:
    m11 = 0;
    m12 = -1;
    m21 = 1;
    m22 = 0;
    break;
  }
  currentFont = c_font.GetGlyphPointer();
  penx = pos.x;
  peny = pos.y;
  length = text.size();
  for (m=0;m<length;m++) {
    g1 = text[m];
    FM_Glyph gptr(currentFont[g1]);
    for (i=0;i<gptr.height;i++)
      for (j=0;j<gptr.width;j++)
	if (gptr.glyphdat[i*gptr.width+j]) {
	  delx = gptr.offset_left+j;
	  dely = -gptr.offset_top+i;
	  BlendPixel(penx+(m11*delx+m12*dely),
		     peny+(m21*delx+m22*dely),
		     gptr.glyphdat[i*gptr.width+j]);
	}
    delx = currentFont[g1].advance_x >> 6;
    dely = currentFont[g1].advance_y >> 6;
    penx += (m11*delx+m12*dely);
    peny += (m21*delx+m22*dely);
    if (m<length-1) {
      g2 = text[m+1];
      delx = currentFont[g1].kerning_x[g2] >> 6;
      dely = currentFont[g1].kerning_y[g2] >> 6;
      penx += (m11*delx+m12*dely);
      peny += (m21*delx+m22*dely);
    }
  }  
}

void RGBImageGC::SetFont(std::string fontname, int fontsize) {
  c_font = BitmapFont(fontname,fontsize);
}

Color RGBImageGC::SetBackGroundColor(Color col) {
  Color tmp(c_bg);
  c_bg = col;
  return tmp;
}

Color RGBImageGC::SetForeGroundColor(Color col) {
  Color tmp(c_fg);
  c_fg = col;
  return tmp;
}

LineStyleType RGBImageGC::SetLineStyle(LineStyleType style) {
  LineStyleType tmp;
  
  tmp = c_linestyle;
  c_linestyle = style;
  penstate = 0;
  return tmp;
}

/*
 * AA-line drawing code - adapted from Michael Abrash's article in DDJ.
 */
void RGBImageGC::DrawAALineStyle(int X0, int Y0, int X1, int Y1, bool endPt)
{
  unsigned int IntensityShift, ErrorAdj, ErrorAcc;
  unsigned int ErrorAccTemp, Weighting, WeightingComplementMask;
  int DeltaX, DeltaY, Temp, XDir, YDir;
  if (PenDraws())
    BlendPixel(X0, Y0, 256);
  if ((X0 == X1) && (Y0 == Y1))
    return;
   if ((DeltaX = X1 - X0) >= 0) {
      XDir = 1;
   } else {
      XDir = -1;
      DeltaX = -DeltaX; /* make DeltaX positive */
   }
   if ((DeltaY = Y1 - Y0) >= 0) {
      YDir = 1;
   } else {
      YDir = -1; 
     DeltaY = -DeltaY; /* make DeltaY positive */
   }
  /* Special-case horizontal, vertical, and diagonal lines, which
     require no weighting because they go right through the center of
     every pixel */
  if (DeltaY == 0) {
    /* Horizontal line */
    while (DeltaX-- > 0) {
      X0 += XDir; 
      if (PenDraws())
	BlendPixel(X0, Y0, 256);
    }
    if (endPt && PenDraws())
      BlendPixel(X1, Y1, 256);
    return;
  }
  if (DeltaX == 0) {
    /* Vertical line */
    while (DeltaY-- > 0) {
      Y0 += YDir;
      if (PenDraws())
	BlendPixel(X0, Y0, 256);
    }  
    if (endPt && PenDraws())
      BlendPixel(X1, Y1, 256);   
    return;
  }
  if (DeltaX == DeltaY) {
    /* Diagonal line */
    do {
      X0 += XDir;
      Y0 += YDir;
      if (PenDraws())
	BlendPixel(X0, Y0, 256);
    } while (--DeltaY > 0);
    if (endPt && PenDraws())
      BlendPixel(X1, Y1, 256);       
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
      Y0 += YDir; /* Y-major, so always advance Y */
      /* The 8 most significant bits of ErrorAcc give us the
	 intensity weighting for this pixel, and the complement of the
	 weighting for the paired pixel */
      Weighting = ErrorAcc >> IntensityShift;
      if (PenDraws()) {
	BlendPixel(X0, Y0, 256 - Weighting);
	BlendPixel(X0 + XDir, Y0,
		   256 - (Weighting ^ WeightingComplementMask));
      }
    }
    /* Draw the final pixel, which is always exactly intersected by the line
       and so needs no weighting */
    if (endPt && PenDraws())
      BlendPixel(X1, Y1, 256);   
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
      Y0 += YDir;
    }
    X0 += XDir; /* X-major, so always advance X */
    /* The 8 most significant bits of ErrorAcc give us the
       intensity weighting for this pixel, and the complement of the
       weighting for the paired pixel */
    Weighting = ErrorAcc >> IntensityShift;
    if (PenDraws()) {
      BlendPixel(X0, Y0, 256 - Weighting);
      BlendPixel(X0, Y0 + YDir,
		 256 + (Weighting - WeightingComplementMask));
    }
   }
  /* Draw the final pixel, which is always exactly intersected by the line
     and so needs no weighting */
  if (endPt && PenDraws())
    BlendPixel(X1, Y1, 256);   
}

void RGBImageGC::DrawLine(Point2D pos1, Point2D pos2) {
  DrawAALineStyle(pos1.x,pos1.y,pos2.x,pos2.y,false);
}

void RGBImageGC::DrawPoint(Point2D pos) {
  BlendPixel(pos.x,pos.y,256);
}

void RGBImageGC::Plot8CirclePoints(int cx, int cy, int x, int y, int c) {
  BlendPixel(cx+x,cy+y,c);
  BlendPixel(cx-x,cy+y,c);
  BlendPixel(cx-x,cy-y,c);
  BlendPixel(cx+x,cy-y,c);
  BlendPixel(cx+y,cy+x,c);
  BlendPixel(cx-y,cy+x,c);
  BlendPixel(cx-y,cy-x,c);
  BlendPixel(cx+y,cy-x,c);  
}

void RGBImageGC::DrawCircle(Point2D pos, int radius) {
  int x = radius;
  int y = 0;
  float T = 0;
  float D;
  int Rsq;
  int ysq;
  float eps;

  Plot8CirclePoints(pos.x, pos.y, x, y, 256);
  Rsq = radius*radius;
  
  while (x>(y+1)) {
    y++;
    ysq = y*y;
    eps = sqrt(Rsq-ysq);
    D = ceil(eps) - eps;
    if (D < T)
      x--;
    Plot8CirclePoints(pos.x, pos.y, x, y, (int)(256*(1-D)));
    Plot8CirclePoints(pos.x, pos.y, x-1, y, (int)(256*D));
    T = D;
  }
}

void RGBImageGC::DrawRectangle(Rect2D rect) {
  std::vector<Point2D> rectlist;
  rectlist.push_back(Point2D(rect.x1,rect.y1));
  rectlist.push_back(Point2D(rect.x1+rect.width,rect.y1));
  rectlist.push_back(Point2D(rect.x1+rect.width,rect.y1+rect.height));
  rectlist.push_back(Point2D(rect.x1,rect.y1+rect.height));
  rectlist.push_back(Point2D(rect.x1,rect.y1));
  DrawLines(rectlist);
}

void RGBImageGC::FillRectangle(Rect2D rect) {
  int x, y;
  for (y=0;y<rect.height;y++) 
    for (x=0;x<rect.width;x++)
      BlendPixel(x+rect.x1,y+rect.y1,256);
}

void RGBImageGC::DrawLines(std::vector<Point2D> pts) {
  int i, n;
  n = pts.size();
  if (n < 2) return;
  for (i=0;i<n-1;i++)
    DrawAALineStyle(pts[i].x,pts[i].y,pts[i+1].x,pts[i+1].y,false);
  DrawAALineStyle(pts[n-2].x,pts[n-2].y,pts[n-1].x,pts[n-1].y,true);
}

void RGBImageGC::PushClippingRegion(Rect2D rect) {
  clipregions.push_back(rect);
}

Rect2D RGBImageGC::PopClippingRegion() {
  Rect2D tmp(clipregions.back());
  clipregions.pop_back();
  return tmp;
}

void RGBImageGC::BlitGrayscaleImage(Point2D pos, GrayscaleImage &img) {
  int x, y;
  int width, height;
  byte *data;
  width = img.GetWidth();
  height = img.GetHeight();
  data = img.GetPixelData();

  for (y=0;y<height;y++)
    for (x=0;x<width;x++)
      SetClippedPixel(x+pos.x,y+pos.y,Color(data[y*width+x],data[y*width+x],data[y*width+x]));
}

void RGBImageGC::BlitRGBImage(Point2D pos, RGBImage &img) {
  int x, y;
  int width, height;
  byte *data;
  width = img.GetWidth();
  height = img.GetHeight();

  data = img.GetPixelData();
  for (y=0;y<height;y++)
    for (x=0;x<width;x++)
      SetClippedPixel(x+pos.x,y+pos.y,
		      Color(data[3*(y*width+x)],data[3*(y*width+x)+1],data[3*(y*width+x)+2]));  
}
