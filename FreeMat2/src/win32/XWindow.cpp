#include "XWindow.hpp"
#include "RGBImageGC.hpp"
#include "PostScriptGC.hpp"
#include "Exception.hpp"
#include <windows.h>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

enum {state_normal, state_click_waiting, state_box_start, state_box_anchored};

HINSTANCE AppInstance;

XWindow::XWindow() {
  m_window = CreateWindow("FreeMat Window",
			  "Figure Window",
			  WS_OVERLAPPEDWINDOW,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  500,
			  400,
			  NULL,
			  NULL,
			  AppInstance,
			  NULL);
  SetWindowLong(m_window,GWL_USERDATA,(LONG) this);
}

XWindow::~XWindow() {
}

void XWindow::Show() {
  ShowWindow(m_window, SW_SHOWNORMAL);
  UpdateWindow(m_window);
}

void XWindow::Hide() {
  ShowWindow(m_window, SW_MINIMIZE);
}

void XWindow::Raise() {
}

void XWindow::Close() {
}

void XWindow::OnExpose(int x, int y, int w, int h) {
  HDC hdc = GetDC(m_window);
  HDC hdcMem = CreateCompatibleDC(hdc);
  SelectObject (hdcMem, hBitmap);
  BitBlt(hdc, x, y, w, h, hdcMem, x, y, SRCCOPY);
  DeleteDC(hdcMem);
  ReleaseDC(m_window, hdc);
}

void XWindow::Refresh() {
  OnResize(m_width, m_height);
}

void XWindow::OnMouseUp(int x, int y) {
}

void XWindow::EraseRectangle(int cx, int cy, int cwidth, int cheight) {
}

void XWindow::SetSize(int w, int h) {
}

#define MIN(a,b) ((a) < (b) ? (a) : (b))
void XWindow::OnDrag(int x, int y) {
	HDC hdc;

  if (m_state == state_box_anchored) {
    int cx, cy, cwidth, cheight;
    // Re-sort the coordinates for drawing the rectangle...
    cx = MIN(m_box_x1,m_box_x2);
    cy = MIN(m_box_y1,m_box_y2);
    cwidth = abs(m_box_x2 - m_box_x1)+1;
    cheight = abs(m_box_y2 - m_box_y1)+1;
    EraseRectangle(cx,cy,cwidth,cheight);
    // Draw the new rectangle
    m_box_x2 = x;
    m_box_y2 = y;
    cx = MIN(m_box_x1,m_box_x2);
    cy = MIN(m_box_y1,m_box_y2);
    cwidth = abs(m_box_x2 - m_box_x1);
    cheight = abs(m_box_y2 - m_box_y1);
    hdc = GetDC(m_window);
    SelectObject(hdc,GetStockObject(WHITE_PEN));
    Rectangle(hdc,cx,cy,cx+cwidth,cy+cheight);
    ReleaseDC(m_window, hdc);
  }
}

void XWindow::OnMouseDown(int x, int y) {
  switch(m_state) {
  case state_normal:
    break;
  case state_click_waiting:
    m_state = state_normal; 
    m_clickx = x;
    m_clicky = y;
    break;
  case state_box_start:
    m_state = state_box_anchored;
    m_box_x1 = x;
    m_box_y1 = y;
    m_box_x2 = x;
    m_box_y2 = y;
    break;
  default:
    break;
  }
}

void XWindow::OnClose() {
}

void XWindow::OnResize(int w, int h) {
  if (w == 0 || h == 0) return;
  m_width = w;
  m_height = h;
  // Update the contents.
  unsigned char *data;
  data = (unsigned char *) malloc(3*sizeof(char)*w*h);
  UpdateContents(data,w,h);
  SetImage(data,w,h);
  free(data);
  OnSize();
}

void XWindow::SetTitle(std::string title) {
  SetWindowText(m_window, title.c_str());
}

void XWindow::SetImagePseudoColor(unsigned char *data, int width, int height) {
}

void XWindow::UpdateContents(unsigned char *data, int width, int height) {
  RGBImage img(width, height, data);
  RGBImageGC gc(img);
  img.SetAllPixels(Color("light grey"));
  OnDraw(gc);
}

void XWindow::Print(std::string filename) {
  // Logic to detect print mode..
  int np;
  np = filename.find_last_of(".");
  if (np > 0) {
    std::string extension(filename.substr(np));
	std::transform (extension.begin(), extension.end(), 
	       extension.begin(), tolower);
    if (extension == ".eps" || extension == ".ps") {
      PostScriptGC gc(filename, m_width, m_height);
      OnDraw(gc);
    } else {
      unsigned char *data;
      data = (unsigned char*) malloc(3*sizeof(char)*m_width*m_height);
      RGBImage img(m_width, m_height, data);
      RGBImageGC gc(img);
      img.SetAllPixels(Color("light grey"));
      OnDraw(gc);
      if (extension == ".jpeg" || extension == ".jpg") {
	img.WriteJPEG(filename);
	// JPEG
      } else if (extension == ".png") {
	img.WritePNG(filename);
	// PNG
      } else if (extension == ".tiff" || extension == ".tif") {
	img.WriteTIFF(filename);
	// TIFF
      } else if (extension == ".ppm" || extension == ".pnm") {
	img.WritePPM(filename);
	// PPM
      } else {
	free(data);
	throw FreeMat::Exception(std::string("Unrecognized extension ") + extension);
      }
      free(data);
    }
  } else
    throw FreeMat::Exception(std::string("Unable to determine format of output from filename"));
}

void XWindow::SetImage(unsigned char *data, int width, int height) {
  // Check for PseudoColor visual
  //  if ((m_visual->c_class != TrueColor) &&
  //      (m_visual->c_class != DirectColor)) {
  //    SetImagePseudoColor(data,width,height);
  //    return;
  //  }
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
  static char* pixelVals;
  int nwidth;
  nwidth = (3*width+3)&~3; // Width of the scanline in bytes
  pixelVals = (char*) malloc(height*nwidth*sizeof(char));
  int i, j;
  for (i=0;i<height;i++)
    for (j=0;j<width;j++) {
      pixelVals[nwidth*(height-1-i)+3*j] = (char) data[3*(i*width+j)];
      pixelVals[nwidth*(height-1-i)+3*j+1] = (char) data[3*(i*width+j)+1];
      pixelVals[nwidth*(height-1-i)+3*j+2] = (char) data[3*(i*width+j)+2];
    }
  HDC hdc;
  hdc = GetDC(m_window);
  hBitmap = CreateDIBitmap(hdc,&pBitmapInfo->bmiHeader,CBM_INIT,(BYTE*) pixelVals,pBitmapInfo,DIB_RGB_COLORS);
  ReleaseDC(m_window, hdc);
}

void XWindow::GetClick(int &x, int &y) {
}

int XWindow::GetState() {
  return m_state;
}

void XWindow::GetBox(int &x1, int &y1, int &x2, int &y2) {
}

LRESULT CALLBACK XWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  XWindow *xptr;

  xptr = (XWindow *) GetWindowLong(hwnd, GWL_USERDATA);
  switch(message) {
  case WM_CREATE:
    return 0;
  case WM_PAINT:
    HDC hdc;
    PAINTSTRUCT ps;
    hdc = BeginPaint(xptr->getWindow(),&ps);
    xptr->OnExpose(ps.rcPaint.left,ps.rcPaint.top,
		   ps.rcPaint.right-ps.rcPaint.left+1,
		   ps.rcPaint.bottom-ps.rcPaint.top+1);
    EndPaint(xptr->getWindow(), &ps);
    return 0;
  case WM_DESTROY:
    return 0;
  case WM_LBUTTONDOWN:
	  xptr->OnMouseDown(LOWORD(lParam),HIWORD(lParam));
	  return 0;
  case WM_MOUSEMOVE:
	  if (wParam & MK_LBUTTON)
		  xptr->OnDrag(LOWORD(lParam),HIWORD(lParam));
	  return 0;
  case WM_SIZE:
	  xptr->OnResize(LOWORD(lParam),HIWORD(lParam));
	  return 0;
  }
  return DefWindowProc(hwnd, message, wParam, lParam);
}


void InitializeXWindowSystem(HINSTANCE hInstance) {
  WNDCLASS wndclass;
  
  wndclass.style = CS_HREDRAW | CS_VREDRAW;
  wndclass.lpfnWndProc = XWndProc;
  wndclass.cbClsExtra = 0;
  wndclass.cbWndExtra = 4;
  wndclass.hInstance = hInstance;
  wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
  wndclass.lpszMenuName = NULL;
  wndclass.lpszClassName = "Freemat Window";
  if (!RegisterClass(&wndclass)) {
    MessageBox(NULL, TEXT("This program requires Windows NT!"),
	       "FreeMat",MB_ICONERROR);
    return;
  }
}

void Run() {
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
}
