/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef TERMWIDGET_H
#define TERMWIDGET_H


class TermWidget {
 public:
  void scrollBack(int val);
  TermWidget();
  virtual ~TermWidget();
  void resizeTextSurface();
  //  virtual void ProcessChar(int c) = 0;
  void PutString(std::string txt);
  void blink();
  void SetCursor(int x, int y);
  int  getTextWidth() {return m_width;};
  void Initialize();
  //TK dependant functions
public:
  void setScrollbar(int val);
  void OnResize();
  void OnKeyPress(int key);
  void OnMouseDown(int x, int y);
  void OnMouseDrag(int x, int y);
  void OnMouseUp(int x, int y);
  void OnScroll(int val);
  void DrawContent();
  void EnableBlink();
  void DisableBlink();
  
  virtual int GetHeight() = 0;
  virtual int GetWidth() = 0;
  virtual void InstallEventTimers() = 0;
  virtual void ScrollLineUp() = 0;
  virtual void ScrollLineDown() = 0;
  virtual void SetScrollBarValue(int val) = 0;
  virtual void SetupScrollBar(int minval, int maxval, int step, int page, int val) = 0;
  virtual void BeginDraw() = 0;
  virtual void PutTagChar(int x, int y, tagChar g) = 0;
  virtual void EndDraw() = 0;
  virtual void setFont(int size) = 0;
  virtual void Erase() = 0;
};

#endif
