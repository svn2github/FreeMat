#include "XWindow.hpp"
#include "Reducer.hpp"
#include "WinGC.hpp"
#include "resource.h"
#include "PostScriptGC.hpp"
#include "BitmapPrinterGC.hpp"
#include "Exception.hpp"
#include <windows.h>
#include <winuser.h>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

enum {state_normal, state_click_waiting, state_box_start, state_box_anchored};

HINSTANCE AppInstance;

XWindow::XWindow(Rect2D sze) : XPWidget(NULL, sze) {
  m_window = CreateWindow("FreeMat Window",
			  "Figure Window",
			  WS_OVERLAPPEDWINDOW,
			  CW_USEDEFAULT,
			  CW_USEDEFAULT,
			  sze.width,
			  sze.height,
			  NULL,
			  NULL,
			  AppInstance,
			  NULL);
  SetWindowLong(m_window,GWL_USERDATA,(LONG) this);
  m_child = NULL;
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

void XWindow::OnChar(char key) {
  if (m_child)
    m_child->OnChar(key);
}

void XWindow::OnMouseDown(Point2D pt) {
  if (m_child)
    m_child->OnMouseDown(pt);
}

void XWindow::OnMouseUp(Point2D pt) {
  if (m_child)
    m_child->OnMouseUp(pt);
}

void XWindow::OnMouseDrag(Point2D pt) {
  if (m_child)
    m_child->OnMouseDrag(pt);
}

void XWindow::OnDraw(GraphicsContext& gc, Rect2D rect) {
  if (m_child)
    m_child->OnDraw(gc, rect);
}

void XWindow::OnResize(Point2D pt) {
  RECT sze;
  sze.left = 0;
  sze.top = 0;
  sze.right = pt.x;
  sze.bottom = pt.y;
  AdjustWindowRect(&sze,WS_OVERLAPPEDWINDOW,TRUE);
  SetWindowPos(m_window, 0, 0, 0, sze.right-sze.left, sze.bottom-sze.top, SWP_NOZORDER | SWP_NOMOVE);
  if (m_child)
    m_child->OnResize(pt);
}

void XWindow::SetTitle(std::string title) {
  SetWindowText(m_window, title.c_str());
}

void XWindow::Refresh(Rect2D rct) {
  RECT rect;
  rect.left = rct.x1;
  rect.top = rct.y1;
  rect.right = rct.x1+rct.width;
  rect.bottom = rct.y1+rct.height;
  InvalidateRect(m_window,&rect,TRUE);
  UpdateWindow(m_window);
}

void CloseXWindow(XWindow* ptr) {
  DestroyWindow(ptr->getWindow());
}

GraphicsContext XWindow::GetGC() {
  HDC hdc = GetDC(m_window);
  return WinGC(hdc, bounds.width, bounds.height);
}

LRESULT CALLBACK XWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  XWindow *xptr;

  xptr = (XWindow *) GetWindowLong(hwnd, GWL_USERDATA);
  switch(message) {
  case WM_CREATE:
    return 0;
  case WM_PAINT:
	  {
	  HDC hdc;
    PAINTSTRUCT ps;
    hdc = BeginPaint(xptr->getWindow(),&ps);
    Rect2D rect;
    rect.x1 = ps.rcPaint.left;
    rect.y1 = ps.rcPaint.top;
    rect.width = ps.rcPaint.right-ps.rcPaint.left+1;
    rect.height = ps.rcPaint.bottom-ps.rcPaint.top+1;
    WinGC wgc(hdc, rect.width, rect.height);
    xptr->OnDraw(wgc, rect);
    EndPaint(xptr->getWindow(), &ps);
	  }
    return 0;
  case WM_DESTROY:
    delete xptr;
    return 0;
  case WM_LBUTTONDOWN:
    xptr->OnMouseDown(Point2D(LOWORD(lParam),HIWORD(lParam)));
    return 0;
  case WM_LBUTTONUP:
    xptr->OnMouseUp(Point2D(LOWORD(lParam),HIWORD(lParam)));
    return 0;
  case WM_MOUSEMOVE:
    if (wParam & MK_LBUTTON)
      xptr->OnMouseDrag(Point2D(LOWORD(lParam),HIWORD(lParam)));
    return 0;
  case WM_SIZE:
    xptr->OnResize(Point2D(LOWORD(lParam),HIWORD(lParam)));
    return 0;
  case WM_CHAR:
    xptr->OnChar(wParam);
    break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case IDM2_EDIT_COPY:
      //      xptr->Copy();
      break;
    case IDM2_FILE_SAVE:
      //      xptr->Save();
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
  wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
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


