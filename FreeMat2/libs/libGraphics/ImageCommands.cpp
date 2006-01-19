// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "ScalarImage.hpp"
#include "GraphicsCore.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include <math.h>
#include <stdio.h>
#include "Malloc.hpp"
#include "Figure.hpp"
#include "ColorBar.hpp"
#include "qlabel.h"
#include "Label.hpp"
#include "Util.hpp"

namespace FreeMat {
  ScalarImage* GetCurrentImage() {
    Figure *fig = GetCurrentFig();
    if (fig->getType() == figscimg) {
      QWidget *w = (QWidget *) fig->GetChildWidget();
      const QObjectList children = w->children();
      for (int i = 0; i < children.size(); ++i) {
	ScalarImage* p = dynamic_cast<ScalarImage*>(children.at(i));
	if (p)
	  return p;
      }
    }
    // Outer level container
    QWidget *w = new QWidget(fig);
    QGridLayout *l = new QGridLayout(w);
    ScalarImage* p = new ScalarImage(w);
    l->addWidget(p,1,1);
    l->setColumnStretch(1,1);
    l->setRowStretch(1,1);
    fig->SetFigureChild(w,figscimg);
    return p;
  }

  ArrayVector ColormapFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("colormap takes a single argument");
    Array t(arg[0]);
    t.promoteType(FM_DOUBLE);
    if (t.getLength() != 256*3)
      throw Exception("Colormap argument must have exactly 768 elements");
    ScalarImage *f;
    f = GetCurrentImage();
    f->SetColormap(t);
    f->repaint();
    return ArrayVector();
  }

  ArrayVector ColorbarFunction(int nargout, const ArrayVector& arg) {
    char orientation = 'e';
    if (arg.size() != 0) {
      const char *cp = ArrayToString(arg[0]);
      orientation = cp[0];
    }
    if ((orientation != 'e') && (orientation != 'w') &&
	(orientation != 's') && (orientation != 'n')) 
      throw Exception("Unable to determine orientation for colorbar");
    Figure *fig = GetCurrentFig();
    ScalarImage *g = GetCurrentImage();
    QWidget *f = fig->GetChildWidget();
    ColorBar *c;
    switch(orientation) {
    case 'n':
      ClearGridWidget(f,"colorbar_n");
      ClearGridWidget(f,"title");
      c = new ColorBar(f,"colorbar_n",orientation);
      SetGridWidget(f, c, 0, 1);
      break;
    case 's':
      ClearGridWidget(f,"colorbar_s");
      ClearGridWidget(f,"xlabel");
      c = new ColorBar(f,"colorbar_s",orientation);
      SetGridWidget(f, c, 2, 1);
      break;
    case 'e':
      ClearGridWidget(f,"colorbar_e");
      c = new ColorBar(f,"colorbar_e",orientation);
      SetGridWidget(f, c, 1, 2);
      break;
    case 'w':
      ClearGridWidget(f,"colorbar_w");
      ClearGridWidget(f,"ylabel");
      c = new ColorBar(f,"colorbar_w",orientation);
      SetGridWidget(f, c, 1, 0);
      break;
    }
    QObject::connect(g, SIGNAL(WinLevChanged(double,double)),
 		     c, SLOT(ChangeWinLev(double,double)));
    QObject::connect(g, SIGNAL(ColormapChanged(char*)),
 		     c, SLOT(ChangeColormap(char*)));
    c->ChangeColormap(g->GetCurrentColormap());
    c->ChangeWinLev(g->GetCurrentWindow(),g->GetCurrentLevel());
    return ArrayVector();
  }

  ArrayVector WinLevFunction(int nargout, const ArrayVector& arg) {
    ScalarImage *f;
    if (arg.size() == 0) {
      f = GetCurrentImage();
      double *dp = (double*) Malloc(sizeof(double)*2);
      dp[0] = f->GetCurrentWindow();
      dp[1] = f->GetCurrentLevel();
      Dimensions dim(2);
      dim[0] = 1;
      dim[1] = 2;
      Array ret(FM_DOUBLE,dim, dp);
      ArrayVector retvec;
      retvec.push_back(ret);
      return retvec;
    } else if (arg.size() != 2)
      throw Exception("Winlev function takes two, real arguments.");
    Array win(arg[0]);
    Array lev(arg[1]);
    double window, level;
    window = win.getContentsAsDoubleScalar();
    if (window == 0)
      throw Exception("zero is not a valid choice for the window");
    level = lev.getContentsAsDoubleScalar();
    f = GetCurrentImage();
    f->WindowLevel(window,level);
    Figure *fig = GetCurrentFig();
    fig->repaint();
    return ArrayVector();
  }

  ArrayVector ImageFunction(int nargout,const ArrayVector& arg) {
    if (arg.size() < 1)
      throw Exception("image function requires at least one argument");
    if (arg.size() > 2)
      throw Exception("image function takes at most two arguments (image and zoom factor)");
    Array img;
    img = arg[0];
    if (img.isEmpty())
      throw Exception("argument image is empty");
    if (!img.is2D())
      throw Exception("argument to image function must be 2D");
    if (img.isReferenceType())
      throw Exception("argument to image function must be a data type");
    if (img.isComplex())
      throw Exception("argument to image function must be real");
    img.promoteType(FM_DOUBLE);
    ScalarImage *f;
    f = GetCurrentImage();
    double zoomfact = 0.0f;
    if (arg.size() == 2) {
      Array z(arg[1]);
      zoomfact = z.getContentsAsDoubleScalar();
    }
    f->SetImageArray(img,zoomfact);
    Figure *t = GetCurrentFig();
    t->resize(f->getZoomColumns(),f->getZoomRows());
    t->adjustSize();
    f->updateGeometry();
    f->update();
    return ArrayVector();
  }

  ArrayVector ZoomFunction(int nargout, const ArrayVector& arg) {
    if (arg.size() != 1)
      throw Exception("Zoom function takes a single, real argument.");
    Array fact(arg[0]);
    fact.promoteType(FM_FLOAT);
    ScalarImage *f;
    f = GetCurrentImage();
    f->Zoom(fact.getContentsAsDoubleScalar());
    Figure *t = GetCurrentFig();
    t->adjustSize();
    t->repaint();
    return ArrayVector();
  }
  
  //@Module POINT Image Point Information Function
  //@@Section IMAGE
  //@@Usage
  //Returns information about the currently displayed image based on a use
  //supplied mouse-click.  The general syntax for its use is
  //@[
  //   y = point
  //@]
  //The returned vector @|y| has three elements: 
  //\[
  //  y= [r,c,v]
  //\]
  //where @|r,c| are the row and column coordinates of the scalar image selected
  //by the user, and @|v| is the value of the scalar image at that point.  Image
  //zoom is automatically compensated for, so that @|r,c| are the coordinates into
  //the original matrix.  They will generally be fractional to account for the
  //exact location of the mouse click.
  ArrayVector PointFunction(int nargout, const ArrayVector& arg) {
    ScalarImage *f;
    f = GetCurrentImage();
    ArrayVector retval;
    retval.push_back(f->GetPoint());
    return retval;
  }
}
