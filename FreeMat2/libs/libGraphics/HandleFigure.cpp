#include "HandleFigure.hpp"
#include "HandleList.hpp"
#include "HandleCommands.hpp"

#include <qgl.h>
#include <math.h>

namespace FreeMat {
  
  HandleFigure::HandleFigure() {
    ConstructProperties();
    SetupDefaults();
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
  }

  void HSVTORGB(double h, double s, double v,
		double &r, double &g, double &b) {
    int i;
    double f, p, q, t;
    if( s == 0 ) {
      // achromatic (grey)
      r = g = b = v;
      return;
    }
    h *= 6;                        // sector 0 to 5
    i = floor( h );
    f = h - i;                    // fractional part of h
    p = 0;
    q = 1 - f ;
    t = f ;
    switch( i ) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    default:                // case 5:
      r = v;
      g = p;
      b = q;
      break;
    }
  }

  void HandleFigure::LoadDefaultColorMap() {
    std::vector<double> cmap;
    for (int i=0;i<64;i++) {
      double h = i/(64.0);
      double s = 1;
      double v = 1;
      double r, g, b;
      HSVTORGB(h,s,v,r,g,b);
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
    SetStringDefault("type","figure");
    SetThreeVectorDefault("color",0.6,0.6,0.6);
    // Set a default colormap to hsv(64) - this matches
    LoadDefaultColorMap();
    resized = false;
  }

  void HandleFigure::PaintMe(RenderEngine &gc) {
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
    resized = true;
  }

  bool HandleFigure::Resized() {
    return resized;
  }


}

