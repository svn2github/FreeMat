#include "WinGC.hpp"

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

void WinGC::SetFont(std::string fontname, int fontsize) {
  if ((fontname != m_fontname) || (fontsize != m_fontsize)) {
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
    m_fontname = fontname;
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

