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
#ifndef __HandleFigure_hpp__
#define __HandleFigure_hpp__
#include "HandleObject.hpp"

namespace FreeMat {
  class HandleWindow;
  //Figure
  //   contains one or more axes
  //   to redraw the figure, we proxy our draws to the axes
  //   Axes
  //   contains one or more children
  //     to redraw the axes, we clear it with bkcolor
  //     then draw it
  //     set the transformation & clipping
  //     then draw the children
  class HandleFigure : public HandleObject {
    int m_width, m_height;
    bool resized;
    void LoadDefaultColorMap();
    HandleWindow *m_win;
  public:
    HandleFigure(HandleWindow *win);
    virtual ~HandleFigure() {}
    virtual void ConstructProperties();
    bool Resized();
    int GetWidth() {return m_width;}
    int GetHeight() {return m_height;}
    virtual void UpdateState();
    virtual void PaintMe(RenderEngine &gc);
    virtual void resizeGL(int width, int height);
    void SetupDefaults();
    void Repaint();
  };
}

#endif

