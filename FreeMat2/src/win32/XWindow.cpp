#include "XWindow.hpp"
#include "Reducer.hpp"
#include "WinGC.hpp"
#include "resource.h"
#include "RGBImageGC.hpp"
#include "PostScriptGC.hpp"
#include "Exception.hpp"
#include <windows.h>
#include <winuser.h>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

enum {state_normal, state_click_waiting, state_box_start, state_box_anchored};

HINSTANCE AppInstance;

XWindow::XWindow(WindowType wtype) {
  m_type = wtype;
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
  m_width = 500;
  m_height = 400;
  SetWindowLong(m_window,GWL_USERDATA,(LONG) this);
  defcursor = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
  clickcursor = (HCURSOR) LoadImage(NULL, IDC_CROSS, IMAGE_CURSOR, 0, 0, LR_SHARED);
  m_bitmap_contents = NULL;
  bitmap_active = false;
  palette_active = false;
  static TCHAR szFilter[] = "JPEG Files (*.JPG)\0*.jpg\0TIFF Files (*.TIF)\0*.tif\0PNG Files (*.PNG)\0*.png\0EPS Files (*.EPS)\0*.eps\0\0";
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = m_window;
  ofn.hInstance = NULL;
  ofn.lpstrFilter = szFilter;
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = 0;
  ofn.lpstrFile = NULL;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = MAX_PATH;
  ofn.lpstrInitialDir = NULL;
  ofn.lpstrTitle = NULL;
  ofn.Flags = 0;
  ofn.nFileOffset = 0;
  ofn.nFileExtension = 0;
  ofn.lpstrDefExt = "jpg";
  ofn.lCustData = 0L;
  ofn.lpfnHook = NULL;
  ofn.lpTemplateName = NULL;
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
  ShowWindow(m_window, SW_SHOWNORMAL);
  UpdateWindow(m_window);  
}

void XWindow::Close() {
}

void XWindow::OnExpose(int x, int y, int w, int h) {
  if (!bitmap_active) return;
  OnResize(m_width, m_height);
  HDC hdc = GetDC(m_window);
  HDC hdcMem = CreateCompatibleDC(hdc);
  SelectObject (hdcMem, hBitmap);
  if (palette_active) {
    SelectPalette(hdcMem, hPalette, FALSE);
    RealizePalette(hdcMem);
    SelectPalette(hdc, hPalette, FALSE);
    RealizePalette(hdc);
  }
  BitBlt(hdc, x, y, w, h, hdcMem, x, y, SRCCOPY);
  ReleaseDC(m_window, hdc);
  DeleteDC(hdcMem);
}

void XWindow::Refresh() {
  OnResize(m_width, m_height);
}

void XWindow::OnMouseUp(int x, int y) {
}

void XWindow::EraseRectangle(int cx, int cy, int cwidth, int cheight) {
}

void XWindow::SetSize(int w, int h) {
  SetWindowPos(m_window, 0, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE);
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
  HDC hdc = GetDC(m_window);
  HDC hdcMem = CreateCompatibleDC(hdc);
  if (bitmap_active)
    DeleteObject(hBitmap);
  hBitmap = CreateCompatibleBitmap(hdc, m_width, m_height);
  SelectObject(hdcMem, hBitmap);
  HBRUSH redbrush;
  RECT rect;
  rect.bottom = 150;
  rect.top = 10;
  rect.left = 10;
  rect.right = 150;
  redbrush = (HBRUSH) GetStockObject(GRAY_BRUSH);
  FillRect(hdcMem,&rect,redbrush);
//  WinGC wgc(hdcMem, m_width, m_height);
//  OnDraw(wgc);
//  OnSize();
//  InvalidateRect(m_window,NULL,TRUE);
//  UpdateWindow(m_window);
  bitmap_active = true;
//  ReleaseDC(m_window, hdc);
}

void XWindow::SetTitle(std::string title) {
  SetWindowText(m_window, title.c_str());
}

void XWindow::SetImagePseudoColor(unsigned char *data, int width, int height) {
#if 0
  HDC hdc;
  hdc = GetDC(m_window);
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
  hPalette = CreatePalette(plp);
  palette_active = true;
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
  SelectPalette(hdc, hPalette, FALSE);
  RealizePalette(hdc);
  hBitmap = CreateDIBitmap(hdc,&pBitmapInfo->bmiHeader,CBM_INIT,
	  (unsigned char*) pixelVals,pBitmapInfo,DIB_RGB_COLORS);
  ReleaseDC(m_window, hdc);
  m_bitmap_contents = data;
#endif
}

void XWindow::SetImage(unsigned char *data, int width, int height) {
#if 0
  HDC hdc;
  hdc = GetDC(m_window);
  if (RC_PALETTE & GetDeviceCaps(hdc, RASTERCAPS)) {
    SetImagePseudoColor(data, width, height);
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
  hBitmap = CreateDIBitmap(hdc,&pBitmapInfo->bmiHeader,CBM_INIT,(BYTE*) pixelVals,pBitmapInfo,DIB_RGB_COLORS);
  ReleaseDC(m_window, hdc);
  m_bitmap_contents = data;
#endif
}

void XWindow::GetClick(int &x, int &y) {
  HCURSOR cursor;
  cursor = LoadCursor(AppInstance, IDC_CROSS);
  HCURSOR oldcursor;
  oldcursor = SetCursor(cursor);
  m_state = state_click_waiting;
  while (m_state != state_normal)
    DoEvents();
  x = m_clickx;
  y = m_clicky;
  SetCursor(oldcursor);
}

int XWindow::GetState() {
  return m_state;
}

void XWindow::GetBox(int &x1, int &y1, int &x2, int &y2) {
}

bool XWindow::SetTheCursor() {
  if (m_state == state_click_waiting) {
    SetCursor(clickcursor);
    return true;
  }
  return false;
}

void XWindow::UpdatePalette() {
  if (!palette_active) return;
  HDC hdc;
  hdc = GetDC(m_window);
  SelectPalette(hdc, hPalette, FALSE);
  RealizePalette(hdc);
  InvalidateRect(m_window, NULL, TRUE);
  ReleaseDC(m_window, hdc);
}

void XWindow::Copy() {
  // Obtain a handle to a reference device context. 
  HDC hdcRef = GetDC(m_window); 
 
  // Determine the picture frame dimensions. 
  // iWidthMM is the display width in millimeters. 
  // iHeightMM is the display height in millimeters. 
  // iWidthPels is the display width in pixels. 
  // iHeightPels is the display height in pixels 
  
  int iWidthMM = GetDeviceCaps(hdcRef, HORZSIZE); 
  int iHeightMM = GetDeviceCaps(hdcRef, VERTSIZE); 
  int iWidthPels = GetDeviceCaps(hdcRef, HORZRES); 
  int iHeightPels = GetDeviceCaps(hdcRef, VERTRES); 
  
  // Retrieve the coordinates of the client 
  // rectangle, in pixels. 

  RECT rect;
  GetClientRect(m_window, &rect); 
  
  // Convert client coordinates to .01-mm units. 
  // Use iWidthMM, iWidthPels, iHeightMM, and 
  // iHeightPels to determine the number of 
  // .01-millimeter units per pixel in the x- 
  //  and y-directions. 
  
  rect.left = (rect.left * iWidthMM * 100)/iWidthPels; 
  rect.top = (rect.top * iHeightMM * 100)/iHeightPels; 
  rect.right = (rect.right * iWidthMM * 100)/iWidthPels; 
  rect.bottom = (rect.bottom * iHeightMM * 100)/iHeightPels; 
  
  // Create the metafile device context. 
  HDC hdcMeta = CreateEnhMetaFile(hdcRef, NULL, &rect, NULL); 
  WinGC wgc(hdcMeta, m_width, m_height);
  OnDraw(wgc);
  HENHMETAFILE hMeta = CloseEnhMetaFile(hdcMeta);
  // Release the reference device context. 
  ReleaseDC(m_window, hdcRef); 
  OpenClipboard(m_window);
  EmptyClipboard();
  SetClipboardData(CF_ENHMETAFILE, hMeta);
  CloseClipboard();
}

void XWindow::Save() {
  char fname[MAX_PATH];
  char ftitle[MAX_PATH];
  ofn.Flags = OFN_OVERWRITEPROMPT;
  fname[0] = 0;
  ftitle[0] = 0;
  ofn.lpstrFile = fname;
  ofn.lpstrFileTitle = ftitle;
  if (m_type == BitmapWindow && m_bitmap_contents == NULL)
    throw FreeMat::Exception("Cannot save empty image window!\n");
  if (!GetSaveFileName(&ofn)) {
    DWORD tmp;
    tmp = CommDlgExtendedError();
    return;
  }
  PrintMe(fname);
}

void XWindow::PrintMe(std::string filename) {
#if 0
  if (m_type == BitmapWindow && m_bitmap_contents == NULL)
    throw FreeMat::Exception("Cannot print empty image window!\n");
  // Logic to detect print mode..
  int np;
  np = filename.find_last_of(".");
  if (np > 0) {
    std::string extension(filename.substr(np));
	std::transform (extension.begin(), extension.end(), 
	       extension.begin(), tolower);
    if (extension == ".eps" || extension == ".ps") {
      if (m_type == VectorWindow) {
	PostScriptGC gc(filename, m_width, m_height);
	OnDraw(gc);
      } else
	WriteEPSFile(filename, m_bitmap_contents, m_width, m_height);
    } else {
      unsigned char *pdata;
      if (m_type == VectorWindow) {
	static PBITMAPINFO		pBitmapInfo;
	pBitmapInfo = (PBITMAPINFO)malloc(sizeof(BITMAPINFOHEADER));
	pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBitmapInfo->bmiHeader.biWidth = m_width;
	pBitmapInfo->bmiHeader.biHeight = m_height;
	pBitmapInfo->bmiHeader.biPlanes = 1;
	pBitmapInfo->bmiHeader.biBitCount = 24;
	pBitmapInfo->bmiHeader.biCompression = BI_RGB;
	pBitmapInfo->bmiHeader.biSizeImage = 0;
	pBitmapInfo->bmiHeader.biXPelsPerMeter = 0;
	pBitmapInfo->bmiHeader.biYPelsPerMeter = 0;
	pBitmapInfo->bmiHeader.biClrUsed = 0;
	pBitmapInfo->bmiHeader.biClrImportant = 0;
	unsigned char* pixelVals;
	HDC hdc = GetDC(m_window);
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hBmp = CreateDIBSection(hdc,pBitmapInfo,DIB_RGB_COLORS,(void**)&pixelVals,NULL,0); 
	SelectObject(hdcMem, hBmp);
	WinGC wgc(hdcMem, m_width, m_height);
	OnDraw(wgc);
	unsigned char *rgbdata = (unsigned char *) malloc(m_height*m_width*3*sizeof(char));
	unsigned char *rgbdata2 = (unsigned char *) malloc(m_height*m_width*3*sizeof(char));
	GetDIBits(hdcMem, hBmp, 0, m_height, rgbdata, pBitmapInfo, DIB_RGB_COLORS);
	// "Fix" the image - remap it to a normal RGB image - this is a two step
	// process - we have to swap the image top to bottom and revert BGR --> RGB
	int i, j;
	for (i=0;i<m_height;i++)
	  for (j=0;j<m_width;j++) {
	    rgbdata2[3*((m_height-1-i)*m_width+j)] = rgbdata[3*(i*m_width+j)+2];
	    rgbdata2[3*((m_height-1-i)*m_width+j)+1] = rgbdata[3*(i*m_width+j)+1];
	    rgbdata2[3*((m_height-1-i)*m_width+j)+2] = rgbdata[3*(i*m_width+j)];
	  }
	free(rgbdata);
	pdata = rgbdata2;
	ReleaseDC(m_window, hdc);
	DeleteDC(hdcMem);
	DeleteObject(hBmp);
      } else {
	pdata = m_bitmap_contents;
      }
      if (extension == ".jpeg" || extension == ".jpg") 
	WriteJPEGFile(filename, pdata, m_width, m_height);
      else if (extension == ".png")
	WritePNGFile(filename, pdata, m_width, m_height);
      else if (extension == ".tiff" || extension == ".tif")
	WriteTIFFFile(filename, pdata, m_width, m_height);
      if (m_type == VectorWindow) {
	free(pdata);
      }
    }
  } else
    throw FreeMat::Exception(std::string("Unable to determine format of output from filename"));
#endif
}

void CloseXWindow(XWindow* ptr) {
  DestroyWindow(ptr->getWindow());
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
    delete xptr;
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
  case WM_SETCURSOR:
    if (xptr->SetTheCursor())
      return 0;
    break;
  case WM_QUERYNEWPALETTE:
    xptr->UpdatePalette();
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDM2_EDIT_COPY:
      xptr->Copy();
      break;
    case IDM2_FILE_SAVE:
      xptr->Save();
      break;
    case IDM2_FILE_CLOSE:
      DestroyWindow(xptr->getWindow());
      break;
    }
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
  wndclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU2);
  wndclass.lpszClassName = "Freemat Window";
  AppInstance = hInstance;
  if (!RegisterClass(&wndclass)) {
    MessageBox(NULL, TEXT("This program requires Windows NT!"),
	       "FreeMat",MB_ICONERROR);
    return;
  }
}

bool DoEvents() {
  MSG msg;
  if (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
	  return true;
  }
  return false; // Quit messsage posted.
}

void Run() {
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
}

void FlushWindowEvents() {
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
  }
}


