/*
 * Copyright (c) 2002-2007 Samit Basu
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
#include "HandleContour.hpp"
#include "HandleList.hpp"
#include "HandleObject.hpp"
#include "HandleAxis.hpp"
#include "IEEEFP.hpp"

HandleContour::HandleContour() {
  ConstructProperties();
  SetupDefaults();
}

HandleContour::~HandleContour() {
}

// Calculate the limits - should return a vector of the
// form...
// [xmin, xmax, ymin, ymax, zmin, zmax, cmin, cmax, amin, amax]
std::vector<double> HandleContour::GetLimits() {
//   std::vector<double> xs(VectorPropertyLookup("xdata"));
//   std::vector<double> ys(VectorPropertyLookup("ydata"));
//  std::vector<double> zs(VectorPropertyLookup("zdata"));
  std::vector<double> limits;
  bool initialized = false;
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double zmin;
  double zmax;
  for (int i=0;i<pset.size();i++) {
    // For each level.
    lineset cset(pset[i]);
    for (int j=0;j<cset.size();j++) {
      cline aline(cset[j]);
      for (int k=0;k<aline.size();k++) {
	if (!initialized) {
	  xmin = xmax = aline[k].x;
	  ymin = ymax = aline[k].y;
	  zmin = zmax = 0;
	  initialized = true;
	} else {
	  xmin = qMin(xmin,aline[k].x);
	  ymin = qMin(ymin,aline[k].y);
	  xmax = qMax(xmax,aline[k].x);
	  ymax = qMax(ymax,aline[k].y);
	}
      }
    }
  }
  if (!initialized) {
    xmin = -1;
    xmax = 1;
    ymin = -1;
    ymax = 1;
    zmin = 0;
    zmax = 0;
  }
  limits.push_back(xmin);
  limits.push_back(xmax);
  limits.push_back(ymin);
  limits.push_back(ymax);
  limits.push_back(zmin);
  limits.push_back(zmax);
  limits.push_back(0);
  limits.push_back(0);
  limits.push_back(0);
  limits.push_back(0);    
  return limits;
}

inline bool operator==(const contour_point& p1, const contour_point& p2) {
  return ((p1.x == p2.x) && (p1.y == p2.y));
}

inline cline Reverse(const cline& src) {
  cline ret;
  for (int i=src.size()-1;i>=0;i--)
    ret << src.at(i);
  return ret;
}

inline bool Connected(const cline& current, const cline& test) {
  return ((current.front() == test.front()) || 
	  (current.front() == test.back()) ||
	  (current.back() == test.front()) ||
	  (current.back() == test.back()));
}

inline void Join(cline& current, const cline& toadd) {
  if (current.front() == toadd.front())
    current = Reverse(toadd) + current;
  else if (current.front() == toadd.back())
    current = toadd + current;
  else if (current.back() == toadd.front())
    current += toadd;
  else if (current.back() == toadd.back())
    current += Reverse(toadd);
}

#define FOLD(x) MAP((x),row-1)
#define FNEW(x) MAP((x),row)
#define MAP(x,y) func[(y)+(x)*numy]
#define AINTER(a,b) ((val-(a))/((b)-(a)))
#define ALEFT(i,j) (((j)-1)+AINTER(FOLD((i)-1),FNEW((i)-1)))
#define TOP(i) (((i)-1)+AINTER(FNEW((i)-1),FNEW((i))))
#define BOT(i) (((i)-1)+AINTER(FOLD((i)-1),FOLD((i))))
#define RIGHT(i,j) (((j)-1)+AINTER(FOLD((i)),FNEW((i))))
#define DRAW(a,b,c,d) {allLines << (cline() << contour_point((double)a,(double)b) << contour_point((double)c,(double)d));}

lineset ContourCDriver(Array m, double val) {
  lineset allLines;
  lineset bundledLines;
  m.promoteType(FM_DOUBLE);
  const double *func = (const double *) m.getDataPointer();
  int outcnt = 0;
  int numy = m.rows();
  int numx = m.columns();
  for (int row=1;row<numy;row++)
    for (int col=1;col<numx;col++) {
      int l = 0;
      if (FOLD(col) >= val) l  = l + 1;
      if (FOLD(col-1) >= val) l = l + 2;
      if (FNEW(col) >= val) l = l + 4;
      if (FNEW(col-1) >= val) l = l + 8;
      switch (l) {
      case 1:
      case 14:
	DRAW(BOT(col),row-1,col,RIGHT(col,row));
	break;
      case 2:
      case 13:
	DRAW(col-1,ALEFT(col,row),BOT(col),row-1);
	break;
      case 3:
      case 12:
	DRAW(col-1,ALEFT(col,row),col,RIGHT(col,row));
	break;
      case 4:
      case 11:
	DRAW(TOP(col),row,col,RIGHT(col,row));
	break;
      case 5:
      case 10:
	DRAW(BOT(col),row-1,TOP(col),row);
	break;
      case 6:
      case 9:
	{
	  double x0 = AINTER(FOLD(col-1),FOLD(col));
	  double x1 = AINTER(FNEW(col-1),FNEW(col));
	  double y0 = AINTER(FOLD(col-1),FNEW(col-1));
	  double y1 = AINTER(FOLD(col),FNEW(col));
	  double y = (x0*(y1-y0)+y0)/(1.0-(x1-x0)*(y1-y0));
	  double x = y*(x1-x0) + x0;
	  double fx1 = MAP(col-1,row-1)+x*(MAP(col,row-1)-MAP(col-1,row-1));
	  double fx2 = MAP(col-1,row)+x*(MAP(col,row)-MAP(col-1,row));
	  double f = fx1 + y*(fx2-fx1);
	  if (f==val) {
	    DRAW(BOT(col),row-1,TOP(col),row);
	    DRAW(col-1,ALEFT(col,row),col,RIGHT(col,row));
	  } else if (((f > val) && (FNEW(col) > val)) || 
		     ((f < val) && (FNEW(col) < val))) {
	    DRAW(col-1,ALEFT(col,row),TOP(col),row);
	    DRAW(BOT(col),row-1,col,RIGHT(col,row));
	  } else {
	    DRAW(col-1,ALEFT(col,row),BOT(col),row-1);
	    DRAW(TOP(col),row,col,RIGHT(col,row));
	  }
	}
	break;
      case 7:
      case 8:
	DRAW(col-1,ALEFT(col,row),TOP(col),row);
	break;
      }
    }
  // Now we link the line segments into longer lines.
  int allcount = allLines.size();
  while (!allLines.empty()) {
    // Start a new line segment
    cline current(allLines.takeAt(0));
    bool lineGrown = true;
    while (lineGrown) {
      lineGrown = false;
      int i = 0;
      while (i<allLines.size()) {
	if (Connected(current,allLines.at(i))) {
	  Join(current,allLines.takeAt(i));
	  lineGrown = true;
	} else
	  i++;
      }
    }
    bundledLines << current;
  }
  return bundledLines;
}

void HandleContour::UpdateState() {
  if (HasChanged("levellist")) ToManual("levellistmode");
  Array zdata(ArrayPropertyLookup("zdata"));
  double zmin = ArrayMin(zdata);
  double zmax = ArrayMax(zdata);
  QList<double> levels;
  if (StringCheck("levellistmode","auto")) {
    levels = GetTicksInner(zmin,zmax,false);
    if (levels.front() == zmin) levels.pop_front();
    if (levels.back() == zmax) levels.pop_back();
    std::vector<double> ulevels;
    for (int i=0;i<levels.size();i++)
      ulevels.push_back(levels[i]);
    HPVector *hp = (HPVector*) LookupProperty("levellist");
    hp->Data(ulevels);
    hp->ClearModified();
  } else {
    std::vector<double> ulevels(VectorPropertyLookup("levellist"));
    for (int i=0;i<ulevels.size();i++)
      levels.push_back(ulevels[i]);
  }
  pset.clear();
  zvals.clear();
  for (int i=0;i<levels.size();i++) { 
    pset << ContourCDriver(zdata,levels[i]);
    zvals << levels[i];
  }
  MarkDirty();
}

void HandleContour::PaintMe(RenderEngine& gc) {
  if (StringCheck("visible","off"))
    return;
  // Draw the line...
  double width(ScalarPropertyLookup("linewidth"));
  HPColor *lc = (HPColor*) LookupProperty("linecolor");
  if (!lc->IsNone()) {
    gc.color(lc->Data());
    gc.setLineStyle(StringPropertyLookup("linestyle"));
    gc.lineWidth(width);
    HandleAxis *parent = (HandleAxis*) GetParentAxis();
    for (int i=0;i<pset.size();i++) {
      // For each level.
      lineset cset(pset[i]);
      for (int j=0;j<cset.size();j++) {
	cline aline(cset[j]);
	std::vector<double> xs;
	std::vector<double> ys;
	std::vector<double> zs;
	for (int k=0;k<aline.size();k++) {
	  xs.push_back(aline[k].x);
	  ys.push_back(aline[k].y);
	  zs.push_back(0);
	}
	std::vector<double> mxs, mys, mzs;
	parent->ReMap(xs,ys,zs,mxs,mys,mzs);
	gc.lineSeries(mxs,mys,mzs);
      }
    }
  }
}

void HandleContour::SetupDefaults() {
  SetThreeVectorDefault("linecolor",0,0,0);
  SetConstrainedStringDefault("levellistmode","auto");
  SetConstrainedStringDefault("linestyle","-");
  SetScalarDefault("linewidth",1.0);
  SetConstrainedStringDefault("fill","off");
  SetConstrainedStringDefault("floating","off");
//   SetConstrainedStringDefault("marker","none");
//   SetThreeVectorDefault("markeredgecolor",0,0,0);
//   SetThreeVectorDefault("markerfacecolor",0,0,0);
//   SetScalarDefault("markersize",6);
  SetStringDefault("type","contour");
  SetConstrainedStringDefault("visible","on");
//   SetConstrainedStringDefault("xdatamode","manual");
}
  
void HandleContour::ConstructProperties() {
  //!
  //@Module COUNTOUR Contour Object Properties
  //@@Section HANDLE
  //@@Usage
  //Below is a summary of the properties for a line series.
  //\begin{itemize}
  //  \item @|displayname| - @|string| - The name of this line series as it
  //    appears in a legend.
  //  \item @|fill| - @|{'on','off'}| - set to on to fill the contours.
  //  \item @|floating| - @|{'on','off'}| - set to on to have floating (3D) contours
  //  \item @|labelspacing| - @|scalar| - distance in points between labels.
  //  \item @|levellist| - @|vector| - a vector of Z-values for the contours
  //  \item @|levellistmode| - @|{'auto','manual'}| - set to auto for 
  //    automatic selection  of Z-values of the contours.
  //  \item @|linecolor| - color of the contour lines.
  //  \item @|linestyle| - @|{'-','--',':','-.','none'}| - the line style to draw the contour in.
  //  \item @|linewidth| - @|scalar| - the width of the lines
  //  \item @|parent| - @|handle| - The axis that contains this object
  //  \item @|showtext| - @|{'on','off'}| - set to on to show labels on the contours.
  //  \item @|tag| - @|string| - A string that can be used to tag the object.
  //  \item @|textlist| - @|vector| - contour values to label.
  //  \item @|textlistmode| - @|{'auto','manual'}| - controls the setting of
  //  the @|textlist| property.
  //  \item @|textstep| - @|scalar| - Determines which contour lines have 
  //  numeric labels.
  //  \item @|textstepmode| - @|{'auto','manual'}| - Set to manual for 
  //  \item @|type| - @|string| - Returns the string @|'contour'|.
  //  \item @|userdata| - @|array| - Available to store any variable you
  // want in the handle object.
  //  \item @|visible| - @|{'on','off'}| - Controls visibility of the the line.
  //  \item @|xdata| - @|vector| - Vector of x coordinates of points on the line.  Must be
  // the same size as the @|ydata| and @|zdata| vectors.
  //  \item @|xdatamode| - @|{'auto','manual'}| - When set to @|'auto'| FreeMat will autogenerate
  // the x coordinates for the points on the line.  These values will be @|1,..,N| where
  // @|N| is the number of points in the line.
  //  \item @|ydata| - @|vector| - Vector of y coordinates of points on the line.  Must be
  // the same size as the @|xdata| and @|zdata| vectors.
  //  \item @|ydatamode| - @|{'auto','manual'}| - When set to @|'auto'| FreeMat will autogenerate
  // the x coordinates for the points on the line.  These values will be @|1,..,N| where
  // @|N| is the number of points in the line.
  //  \item @|zdata| - @|vector| - Vector of z coordinates of points on the line.  Must be
  // the same size as the @|xdata| and @|ydata| vectors.
  //\end{itemize}
  //!
  AddProperty(new HPHandles,"children");
  AddProperty(new HPString,"displayname");
  AddProperty(new HPOnOff,"fill");
  AddProperty(new HPOnOff,"floating");
  AddProperty(new HPScalar,"labelspacing");
  AddProperty(new HPVector,"levellist");
  AddProperty(new HPAutoManual,"levellistmode");
  AddProperty(new HPColor,"linecolor");
  AddProperty(new HPLineStyle,"linestyle");
  AddProperty(new HPScalar,"linewidth");
  AddProperty(new HPHandles,"parent");
  AddProperty(new HPOnOff,"showtext");
  AddProperty(new HPString,"tag");
  AddProperty(new HPVector,"textlist");
  AddProperty(new HPAutoManual,"textlistmode");
  AddProperty(new HPScalar,"textstep");
  AddProperty(new HPAutoManual,"textstepmode");
  AddProperty(new HPString,"type");
  AddProperty(new HPArray,"userdata");
  AddProperty(new HPOnOff,"visible");
  AddProperty(new HPVector,"xdata");
  AddProperty(new HPAutoManual,"xdatamode");
  AddProperty(new HPVector,"ydata");
  AddProperty(new HPAutoManual,"ydatamode");
  AddProperty(new HPArray,"zdata");
}
