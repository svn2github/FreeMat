#include "WinGC.hpp"
#include "Reducer.hpp"

WinGC::WinGC(HDC dc, int width, int height) {
  hdc = dc;
  m_width = width;
  m_height = height;
}

WinGC::~WinGC() {
}

Point2D WinGC::GetCanvasSize() {
  return Point2D(m_width,m_height);
}

Point2D WinGC::GetTextExtent(std::string label) {
  Point2D a;
  SIZE t;
  GetTextExtentPoint32(hdc, label.c_str(), label.size(), &t);
  a.x = t.cx;
  a.y = t.cy;
  return a;
}

void WinGC::DrawTextString(std::string label, Point2D pos, OrientationType orient) {
  SIZE t;
  GetTextExtentPoint32(hdc, label.c_str(), label.size(), &t);
  int twiddlex, twiddley;
  if (orient == ORIENT_0) {
    SelectObject(hdc, m_hfont);
    twiddlex = 0;
    twiddley = -t.cy;
  }
  else {
    SelectObject(hdc, m_vfont);
    twiddlex = -t.cy;
    twiddley = 0;
  }
  SetBkColor(hdc, RGB(bgcol.red,bgcol.green,bgcol.blue));
  TextOut(hdc,pos.x+twiddlex, pos.y+twiddley, label.c_str(), label.size());
}

void WinGC::SetFont(int fontsize) {
  if (fontsize != m_fontsize) {
    int nHeight;
    nHeight = -MulDiv(fontsize-2, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    m_hfont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 
			 FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS,
			 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
			 FF_SWISS, "Arial");
    m_vfont = CreateFont(nHeight, 0, 900, 900, FW_NORMAL, FALSE, FALSE, 
			 FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS,
			 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
			 FF_SWISS, "Arial");
    m_fontsize = fontsize;
  }
}

Color WinGC::SetBackGroundColor(Color col) {
  Color oldbg;
  oldbg = bgcol;
  SetBkColor(hdc, RGB(col.red,col.green,col.blue));
  bgcol = col;
  return oldbg;
}

Color WinGC::SetForeGroundColor(Color col) {
  Color oldfg;
  oldfg = fgcol;
  fgcol = col;
  return oldfg;
}

LineStyleType WinGC::SetLineStyle(LineStyleType style) {
  LineStyleType old_style;
  old_style = m_style;
  m_style = style;
  return old_style;
}

HPEN GetWinPen(LineStyleType style, Color col) {
  HPEN hpen;
  int penStyle;
  switch (style) {
  case LINE_SOLID:
    penStyle = PS_SOLID;
    break;
  case LINE_DASHED:
    penStyle = PS_DASH;
    break;
  case LINE_DOTTED:
    penStyle = PS_DOT;
    break;
  case LINE_DASH_DOT:
    penStyle = PS_DASHDOT;
    break;
  case LINE_NONE:
    penStyle = PS_NULL;
    break;    
  }
  hpen = CreatePen(penStyle,1,RGB(col.red,col.green,col.blue));
  return hpen;
}

void WinGC::DrawLine(Point2D pos1, Point2D pos2) {
  HPEN hpen = GetWinPen(m_style, fgcol);
  SelectObject(hdc, hpen);
  MoveToEx(hdc, pos1.x, pos1.y, NULL);
  LineTo(hdc, pos2.x, pos2.y);
  DeleteObject(hpen);
}

void WinGC::DrawPoint(Point2D pos) {
  HPEN hpen = GetWinPen(m_style, fgcol);
  SelectObject(hdc, hpen);
  SelectObject(hdc, GetStockObject(NULL_BRUSH));
  Ellipse(hdc, pos.x - 1, pos.y - 1,
	  pos.x + 1, pos.y + 1);
  DeleteObject(hpen);
}

void WinGC::DrawCircle(Point2D pos, int radius) {
  HPEN hpen = GetWinPen(m_style, fgcol);
  SelectObject(hdc, hpen);
  SelectObject(hdc, GetStockObject(NULL_BRUSH));
  Ellipse(hdc, pos.x - radius, pos.y - radius,
	  pos.x + radius, pos.y + radius);
  DeleteObject(hpen);
}

void WinGC::DrawRectangle(Rect2D rect) {
  HPEN hpen = GetWinPen(m_style, fgcol);
  SelectObject(hdc, hpen);
  Rectangle(hdc, rect.x1, rect.y1, 
	    rect.x1+rect.width, rect.y1+rect.height);
  DeleteObject(hpen);
}

void WinGC::FillRectangle(Rect2D rect) {
  HBRUSH hbrush;
  hbrush = CreateSolidBrush(RGB(fgcol.red,fgcol.green,fgcol.blue));
  RECT rt;
  rt.left = rect.x1;
  rt.top = rect.y1;
  rt.right = rect.x1+rect.width;
  rt.bottom = rect.y1+rect.height;
  FillRect(hdc, &rt, hbrush);
  DeleteObject(hbrush);
}

void WinGC::DrawLines(std::vector<Point2D> pts) {
  POINT *pt;
  pt = (POINT *) malloc(sizeof(POINT)*pts.size());
  int i;
  for (i=0;i<pts.size();i++) {
    pt[i].x = pts[i].x;
    pt[i].y = pts[i].y;
  }
  HPEN hpen = GetWinPen(m_style, fgcol);
  SelectObject(hdc, hpen);
  Polyline(hdc, pt, pts.size());
  DeleteObject(hpen);
}

void WinGC::PushClippingRegion(Rect2D rect) {
  clipwin = CreateRectRgn(rect.x1, rect.y1, 
			  rect.x1+rect.width, rect.y1+rect.height);
  clipstack.push_back(rect);
  SelectClipRgn(hdc,clipwin);
}

Rect2D WinGC::PopClippingRegion() {
  clipstack.pop_back();
  Rect2D rect;
  if (clipstack.empty()) {
    rect.x1 = 0;
    rect.y1 = 0;
    rect.width = m_width;
    rect.height = m_height;
  } else {
    rect = clipstack.back();
  }
  clipwin = CreateRectRgn(rect.x1, rect.y1, 
			  rect.x1+rect.width, rect.y1+rect.height);
  SelectClipRgn(hdc,clipwin);
  return rect;
}

void WinGC::BlitImage(unsigned char *data, int width, int height, int x0, int y0) {
  if (RC_PALETTE & GetDeviceCaps(hdc, RASTERCAPS)) {
    BlitImagePseudoColor(data, width, height, x0, y0);
    return;
  }
  static PBITMAPINFO		pBitmapInfo;
  pBitmapInfo = (PBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER));
  pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pBitmapInfo->bmiHeader.biWidth = width;
  pBitmapInfo->bmiHeader.biHeight = height;
  pBitmapInfo->bmiHeader.biPlanes = 1;
  pBitmapInfo->bmiHeader.biBitCount = 24;
  pBitmapInfo->bmiHeader.biCompression = BI_RGB;
  pBitmapInfo->bmiHeader.biSizeImage = 0;
  pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
  pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
  pBitmapInfo->bmiHeader.biClrUsed = 0;
  pBitmapInfo->bmiHeader.biClrImportant = 0;
  static unsigned char* pixelVals;
  int nwidth;
  nwidth = (3*width+3)&~3; // Width of the scanline in bytes
  pixelVals = (unsigned char*) malloc(height*nwidth*sizeof(char));
  int i, j;
  for (i=0;i<height;i++)
    for (j=0;j<width;j++) {
      pixelVals[nwidth*(height-1-i)+3*j] = (unsigned char) data[3*(i*width+j)+2];
      pixelVals[nwidth*(height-1-i)+3*j+1] = (unsigned char) data[3*(i*width+j)+1];
      pixelVals[nwidth*(height-1-i)+3*j+2] = (unsigned char) data[3*(i*width+j)];
    }
  HBITMAP hBitmap = CreateDIBitmap(hdc,&pBitmapInfo->bmiHeader,CBM_INIT,(BYTE*) pixelVals,pBitmapInfo,DIB_RGB_COLORS);
  HDC hdcMem = CreateCompatibleDC(hdc);
  SelectObject (hdcMem, hBitmap);
  BitBlt(hdc, x0, y0, m_width, m_height, hdcMem, 0, 0, SRCCOPY);
  DeleteDC(hdcMem);
  DeleteObject(hBitmap);
}

bool WinGC::IsColormapActive() {
	return colormapActive;
}

HPALETTE WinGC::GetColormap() {
	return m_colormap;
}

void WinGC::BlitImagePseudoColor(unsigned char *data, int width, int height, int x0, int y0) {
  int pal_size;
  pal_size = GetDeviceCaps(hdc, SIZEPALETTE);
  int res_colors;
  res_colors = GetDeviceCaps(hdc, NUMRESERVED);
  char buffer[2000];
  int colorCount = pal_size - res_colors;
  colorCount = (colorCount > 32768) ? 32768 : colorCount;
  // OK, now we use the color reducer to get a colormapped image
  unsigned short *outimg = (unsigned short*) 
    malloc(width*height*sizeof(short));
  unsigned short *outcolors = (unsigned short*)
    malloc(colorCount*3*sizeof(short));
  int colorsUsed;
  colorsUsed = ColorReduce(data, width, height, colorCount, outcolors, outimg);
  LOGPALETTE *plp;
  plp = (LOGPALETTE *) malloc(sizeof(LOGPALETTE)+
			      colorsUsed*sizeof(PALETTEENTRY));
  plp->palVersion = 0x0300;
  plp->palNumEntries = colorsUsed;
  for (int k=0;k<colorsUsed;k++) {
    plp->palPalEntry[k].peRed = (BYTE) (outcolors[3*k]>>8);
    plp->palPalEntry[k].peGreen = (BYTE) (outcolors[3*k+1]>>8);
    plp->palPalEntry[k].peBlue = (BYTE) (outcolors[3*k+2]>>8);
    plp->palPalEntry[k].peFlags = 0;
  }
  m_colormap = CreatePalette(plp);
  colormapActive = true;
  free(plp);
  static PBITMAPINFO		pBitmapInfo;
  pBitmapInfo = (PBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER)+colorsUsed*sizeof(RGBQUAD));
  pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  pBitmapInfo->bmiHeader.biWidth = width;
  pBitmapInfo->bmiHeader.biHeight = height;
  pBitmapInfo->bmiHeader.biPlanes = 1;
  pBitmapInfo->bmiHeader.biBitCount = 8;
  pBitmapInfo->bmiHeader.biCompression = BI_RGB;
  pBitmapInfo->bmiHeader.biSizeImage = 0;
  pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
  pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
  pBitmapInfo->bmiHeader.biClrUsed = colorsUsed;
  pBitmapInfo->bmiHeader.biClrImportant = 0;
  RGBQUAD *ptr = (RGBQUAD *) &(pBitmapInfo->bmiColors[0]);
  for (int p=0;p<colorsUsed;p++) {
	  ptr[p].rgbBlue = outcolors[3*p+2]>>8;
	  ptr[p].rgbGreen = outcolors[3*p+1]>>8;
	  ptr[p].rgbRed = outcolors[3*p]>>8;
  }
  static unsigned char* pixelVals;
  int nwidth;
  nwidth = (width+3)&~3; // Width of the scanline in bytes
  pixelVals = (unsigned char*) malloc(height*nwidth*sizeof(char));
  int i, j;
  for (i=0;i<height;i++)
    for (j=0;j<width;j++)
      pixelVals[nwidth*(height-1-i)+j] = outimg[i*width+j];
  free(outcolors);
  free(outimg);
  SelectPalette(hdc, m_colormap, FALSE);
  RealizePalette(hdc);
  HBITMAP  hBitmap = CreateDIBitmap(hdc,&pBitmapInfo->bmiHeader,CBM_INIT,
	  (unsigned char*) pixelVals,pBitmapInfo,DIB_RGB_COLORS);
  HDC hdcMem = CreateCompatibleDC(hdc);
  SelectObject (hdcMem, hBitmap);
  BitBlt(hdc, x0, y0, m_width, m_height, hdcMem, 0, 0, SRCCOPY);
  DeleteDC(hdcMem);
  DeleteObject(hBitmap);
}

