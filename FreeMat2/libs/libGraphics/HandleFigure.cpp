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
#include "HandleFigure.hpp"
#include "HandleList.hpp"
#include "HandleCommands.hpp"
#include "HandleWindow.hpp"
#include <math.h>
#include <qgl.h>
#include <math.h>

namespace FreeMat {

  HandleFigure::HandleFigure(HandleWindow *win) {
    m_width = 640;
    m_height = 480;
    m_win = win;
    ConstructProperties();
    SetupDefaults();
  }

  void HandleFigure::Repaint() {
    m_win->update();
  }
  
  void HandleFigure::ConstructProperties() {
    AddProperty(new HPVector,"alphamap");
    AddProperty(new HPColor,"color");
    AddProperty(new HPColorVector,"colormap");
    AddProperty(new HPHandles,"children");
    AddProperty(new HPHandles,"currentaxes");
    AddProperty(new HPHandles,"parent");
    AddProperty(new HPFourVector,"position");
    AddProperty(new HPString,"type");
    AddProperty(new HPArray,"userdata");
    AddProperty(new HPNextPlotMode,"nextplot");
    AddProperty(new HPTwoVector,"figsize");
    AddProperty(new HPRenderMode,"renderer");
  }

  void HSVRAMP(double h, double &r, double &g, double &b) {
    int i;
    double f, p, q, t;
    h *= 6;                        // sector 0 to 5
    i = floor( h );
    f = h - i;                    // fractional part of h
    p = 0;
    q = 1 - f ;
    t = f ;
    switch( i ) {
    case 0:
      r = 1;      g = t;      b = p;
      break;
    case 1:
      r = q;      g = 1;      b = p;
      break;
    case 2:
      r = p;      g = 1;      b = t;
      break;
    case 3:
      r = p;      g = q;      b = 1;
      break;
    case 4:
      r = t;      g = p;      b = 1;
      break;
    default:                // case 5:
      r = 1;      g = p;      b = q;
      break;
    }
  }

  void HandleFigure::UpdateState() {
    m_win->UpdateState();
  }

  void HandleFigure::LoadDefaultColorMap() {
    std::vector<double> cmap;
    for (int i=0;i<64;i++) {
      double h = i/(64.0);
      double r, g, b;
      HSVRAMP(h,r,g,b);
      cmap.push_back(r);
      cmap.push_back(g);
      cmap.push_back(b);
    }
    HPColorVector *hcv = (HPColorVector*) LookupProperty("colormap");
    hcv->Data(cmap);
    cmap.clear();
    cmap.push_back(1.0);
    HPVector *hv = (HPVector*) LookupProperty("alphamap");
    hv->Data(cmap);
  }

  void HandleFigure::SetupDefaults() {
    SetStringDefault("renderer","painters");
    SetStringDefault("type","figure");
    SetThreeVectorDefault("color",0.6,0.6,0.6);
    SetStringDefault("nextplot","replace");
    // Set a default colormap to hsv(64) - this matches
    LoadDefaultColorMap();
    resized = false;
  }

  void HandleFigure::PaintMe(RenderEngine &gc) {
    //    qDebug("size = %d %d",m_width,m_height);
    // draw the children...
    HPColor *color = (HPColor*) LookupProperty("color");
    gc.clear(color->Data());
    HPHandles *children = (HPHandles*) LookupProperty("children");
    std::vector<unsigned> handles(children->Data());
    for (int i=0;i<handles.size();i++) {
      HandleObject *fp = LookupHandleObject(handles[i]);
      fp->PaintMe(gc);
    }
    resized = false;
  }

  void HandleFigure::resizeGL(int width, int height) {
    m_width = width;
    m_height = height;
    SetTwoVectorDefault("figsize",width,height);
    resized = true;
  }

  bool HandleFigure::Resized() {
    return resized;
  }


}

