#include <windows.h>
#include <math.h>
#include <string>
#include <iostream>
#include "WinGC.hpp"

#define M_PI 3.1415926535

class XPWindow {
public:
  HWND hwnd;
  HBITMAP hBitmp;
  HDC hdcMem;
  int m_win_width;
  int m_win_height;
  bool bitmap_active;
public:
  XPWindow(HINSTANCE hInstance, int iCmdShow);
  HWND window() {return hwnd;}
  virtual ~XPWindow() {}
  virtual void OnDraw() {}
  virtual void OnResize();
  virtual void Show();
  virtual void Hide();
  virtual void Raise();
  virtual void OnMouseDown(int x, int y) {}
  virtual void OnMouseDrag(int x, int y);
  virtual void OnMouseUp(int x, int y) {}
  virtual void OnKeyDown(int key) {}
  virtual int GetHeight();
  virtual int GetWidth();
  void OnExpose(int x, int y, int w, int h);
};

void XPWindow::OnExpose(int x, int y, int w, int h) {
  if (!bitmap_active) return;
  HDC hdc = GetDC(hwnd);
  BitBlt(hdc, x, y, w, h, hdcMem, x, y, SRCCOPY);
  ReleaseDC(hwnd, hdc);
}

void XPWindow::OnResize() {
  int w = GetWidth();
  int h = GetHeight();
  if (w == 0 || h == 0) return;
  m_win_width = w;
  m_win_height = h;
  HDC hdc = GetDC(hwnd);
  DeleteDC(hdcMem);
  hdcMem = CreateCompatibleDC(hdc);
  if (bitmap_active)
    DeleteObject(hBitmp);
  hBitmp = CreateCompatibleBitmap(hdc, m_win_width, m_win_height);
  RECT rct;
  rct.top = 0;
  rct.bottom = m_win_height;
  rct.left = 0;
  rct.right = m_win_width;
  SelectObject(hdcMem, hBitmp);

  WinGC gc(hdcMem,GetWidth(),GetHeight());

  gc.SetBackGroundColor(Color("grey"));
  gc.SetForeGroundColor(Color("grey"));
  gc.FillRectangle(Rect2D(0,0,GetWidth(),GetHeight()));
  gc.SetForeGroundColor(Color("black"));
  gc.DrawLine(Point2D(0,0),Point2D(GetWidth(),GetHeight()));
  gc.DrawLine(Point2D(0,GetHeight()),Point2D(GetWidth(),0));
  gc.DrawCircle(Point2D(GetWidth()/2,GetHeight()/2),20);
  gc.SetFont(12);
  gc.DrawTextString("Hello World", Point2D(GetWidth()/2,GetHeight()/2));
  unsigned char *data = new unsigned char[64*64*3];
  for (int i=0;i<64;i++)
    for (int j=0;j<64;j++) {
      double wx = i/64.0*M_PI/2;
      double wy = j/64.0*M_PI/2;
      data[i*64*3+j*3] = 255*cos(wx+wy);
      data[i*64*3+j*3+1] = 255*sin(wx+wy);
      data[i*64*3+j*3+2] = 255*sin(wx+wy)*cos(wx);
    }
  gc.BlitImage(data,64,64,20,30);

  InvalidateRect(hwnd,NULL,TRUE);
  UpdateWindow(hwnd);
  bitmap_active = true;
  ReleaseDC(hwnd, hdc);
}

void XPWindow::OnMouseDrag(int x, int y) {
}

void XPWindow::Raise() {
  // Don't know this one!
}

void XPWindow::Show() {
}

void XPWindow::Hide() {
}

int XPWindow::GetHeight() {
  return m_win_height;
}

int XPWindow::GetWidth() {
  return m_win_width;
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void SetupXPWindowClass(HINSTANCE hInstance) {
  WNDCLASS wndclass;
  wndclass.style = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc = WndProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 4;
  wndclass.hInstance = hInstance;
  //wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  //wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
  wndclass.lpszMenuName = NULL;
  wndclass.hIcon = 0;
  wndclass.lpszClassName = "XPWindow";
  if (!RegisterClass(&wndclass))
    {
      MessageBox(NULL, TEXT("This program requires Windows NT!"),
		 "WinTerm",MB_ICONERROR);
      return;
    }
}


XPWindow::XPWindow(HINSTANCE hInstance, int iCmdShow) {
  hwnd = CreateWindow("XPWindow",
		      TEXT("FreeMat"),
		      WS_OVERLAPPEDWINDOW,
		      CW_USEDEFAULT,
		      CW_USEDEFAULT,
		      500,
		      400,
		      NULL,
		      NULL,
		      hInstance,
		      NULL);
  m_win_width = 500;
  m_win_height = 400;
  SetWindowLong(hwnd,GWL_USERDATA,(LONG) this);
  bitmap_active = false;
  ShowWindow(hwnd, iCmdShow);
//   hWhiteBrush = (HBRUSH) GetStockObject(WHITE_BRUSH);
//   hBlackBrush = (HBRUSH) GetStockObject(BLACK_BRUSH);
//   hBlueBrush =  (HBRUSH) CreateSolidBrush(RGB(0,0,255));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  XPWindow* wptr;
  HDC hdc;
  PAINTSTRUCT ps;
  RECT rect;
  
  wptr = (XPWindow*) GetWindowLong(hwnd, GWL_USERDATA);
  
  switch (message)
    {
    case WM_SIZE:
		wptr->m_win_height = HIWORD(lParam);
		wptr->m_win_width = LOWORD(lParam);
      wptr->OnResize();
      return 0;
    case WM_CREATE:
      return 0;
    case WM_LBUTTONDOWN:
      wptr->OnMouseDown((short)LOWORD(lParam),(short)HIWORD(lParam));
      SetCapture(hwnd);
      return 0;
    case WM_MOUSEMOVE:
      if (wParam & MK_LBUTTON)
	wptr->OnMouseDrag((short)LOWORD(lParam),(short)HIWORD(lParam));
      return 0;
    case WM_LBUTTONUP:
      wptr->OnMouseUp((short)LOWORD(lParam),(short)HIWORD(lParam));
      ReleaseCapture();
      return 0;
	case WM_ERASEBKGND:
		return 1;
    case WM_PAINT:
      HDC hdc;
      PAINTSTRUCT ps;
      hdc = BeginPaint(wptr->window(),&ps);
      wptr->OnExpose(ps.rcPaint.left,ps.rcPaint.top,
		     ps.rcPaint.right-ps.rcPaint.left+1,
		     ps.rcPaint.bottom-ps.rcPaint.top+1);
      EndPaint(wptr->window(), &ps);
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_SETFOCUS:  
      break;
    case WM_KILLFOCUS:  
      break; 
    }
  return DefWindowProc(hwnd, message, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		     LPSTR lpCmdLine, int nCmdShow) {
  MSG        Msg;
  SetupXPWindowClass(hInstance);
  XPWindow term(hInstance, nCmdShow);
  
  while( GetMessage(&Msg, NULL, 0, 0) )
    {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
  return Msg.wParam;
}

