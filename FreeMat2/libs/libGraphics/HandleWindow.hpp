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
#ifndef __HandleWindow_hpp__
#define __HandleWindow_hpp__

#include <QWidget>
#include "HandleFigure.hpp"
#include <QStackedWidget>
#include <QTabWidget>
#include <QGLWidget>
#include <QEventLoop>

namespace FreeMat {

class HandleWindow : public QWidget {
 protected:
  unsigned handle;
  bool glActive;
  QGLWidget *glchild;
  QWidget *qtchild;
  HandleFigure *hfig;
  QStackedWidget *layout;
  //  QTabWidget *layout;
  QEventLoop m_loop;
  int click_x, click_y;
 public:
  HandleWindow(unsigned ahandle);
  ~HandleWindow() {delete hfig;}
  unsigned Handle();
  HandleFigure *HFig();
  void UpdateState();
  void closeEvent(QCloseEvent* e);
  void GetClick(int &x, int &y);
  void mousePressEvent(QMouseEvent* e);
};

}

#endif
