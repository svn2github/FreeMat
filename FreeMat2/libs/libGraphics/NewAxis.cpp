// Copyright (c) 2002-2005 Samit Basu
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

#include "NewAxis.hpp"
#include <math.h>
#include <iostream>

namespace FreeMat {

  NewAxis::NewAxis(double minVal, double maxVal, bool logarithmic) {
    tMin = minVal;
    tMax = maxVal;
    isLogarithmic = logarithmic;
    space = 10;
    tickLength = 5;
    AutoSetAxis();
  }

  void NewAxis::SetLabelText(std::string& text) {
    title = text;
  }

  void NewAxis::SetExtent(double minVal, double maxVal) {
    tMin = minVal;
    tMax = maxVal;
    AutoSetAxis();
  }

  void NewAxis::SetLogarithmic(bool logarithmic) {
    isLogarithmic = logarithmic;
    AutoSetAxis();
  }

  // To calculate the step size, we have a step size of
  // the order 10^n*2^p = (t2-t1)/m
  // Ultimately, we calculate a delta.  We know that
  // m*delta > (t2-t1).  Then we set
  // tstart = floor(t1/delta)*delta
  // tstop = ceil(t2/delta)*delta
  // tstart + n*edelt >= tstop
  //  n >= ceil((tstop-tstart)/edelt)
  //
  //function calcstep(t1,t2,m,autoset)
  //delt = (t2-t1)/m;
  //log(delt)/log(10);
  //n = floor(log(delt)/log(10));
  //rdelt = delt/(10^n);
  //p = ceil(log(rdelt)/log(2));
  //edelt = 10^n*2^p
  //if (autoset)
  //  tstart = floor(t1/edelt)*edelt;
  //  tstop = ceil(t2/edelt)*edelt;
  //  tbegin = tstart;
  //  tend = tstop;
  //else
  //  tstart = t1;
  //  tstop = t2;
  //  tbegin = ceil(t1/edelt)*edelt;
  //  tend = floor(t2/edelt)*edelt;
  //end
  //mprime = ceil((tend-tbegin)/edelt)
  //tbegin+(mprime-1)*edelt
  //if (autoset)
  //  if ((tbegin+(mprime-1)*edelt) >= t2)
  //    mprime = mprime-1
  //  end
  //else
  //  if ((tbegin+mprime*edelt) > t2)
  //    mprime = mprime-1
  //  end
  //end
  //tbegin + (0:mprime)*edelt
  void NewAxis::UpdateIntervals(double t1, double t2) {
    // Map the axes ranges to a start, stop and delta
    double range;
    if (!isLogarithmic)
      range = t2 - t1;
    else {
      t1 = (t1 > 0) ? log10(t1) : -10.0;
      t2 = (t2 > 0) ? log10(t2) : -10.0;
      range = t2 - t1;
    }
    int int_log_range = (int)floor( log10( range ) );
    tStep = 1.0;
    if (int_log_range > 0)
      for (int i = 0; i < int_log_range; i++)
	tStep *= 10; 
    if (int_log_range < 0)
      for (int i = 0; i < -int_log_range; i++)
	tStep /= 10; 
    tStart = floor(t1 / tStep) * tStep;
    tStop = ceil(t2 / tStep) * tStep;
    // if too few values, shrink size
    if ((range/tStep) < 6) {
      tStep /= 2;
      if (tStart-tStep > t1) tStart -= tStep;
      if (tStop+tStep < t2) tStop += tStep;
    }
    // if still too few, again
    if ((range/tStep) < 6) {
      tStep /= 2;
      if (tStart-tStep > t1) tStart -= tStep;
      if (tStop+tStep < t2) tStop += tStep;
    }
    // if still too few, again
    if ((range/tStep) < 6) {
      tStep /= 2;
      if (tStart-tStep > t1) tStart -= tStep;
      if (tStop+tStep < t2) tStop += tStep;
    }
    // Map the step to an integer (if we are a log scale)
    if (isLogarithmic)
      if (floor(tStep) != 0)
	tStep = floor(tStep);
    tStart = floor(t1 / tStep) * tStep;
    tStop = ceil(t2 / tStep) * tStep;
  }

  void NewAxis::GetAxisExtents(double &t1, double &t2) {
    t1 = tStart;
    t2 = tStop;
  }

  void NewAxis::ManualSetAxis(double t1, double t2) {
	  int i;
    UpdateIntervals(t1, t2);
    double acnt;
    acnt = (tStop - tStart)/tStep + 1.0;
    // Count the number of ticks inside the desired range
    tCount = 0;
    for (i=0;i<acnt;i++)
      if (((tStart + i * tStep) >= t1) && ((tStart + i * tStep) <= t2))
	tCount++;
    tlabels.clear();
    tickLocations = new double[tCount];
    int tNum = 0;
    for (i=0;i<acnt;i++)
      if (((tStart + i * tStep) >= t1) && ((tStart + i * tStep) <= t2))
	tickLocations[tNum++] = tStart + i * tStep;
    bool exponentialForm;
    exponentialForm = false;
    for (i=0;i<tCount;i++)
      if (tickLocations[i] != 0.0)
	exponentialForm |= (fabs(log10(fabs(tickLocations[i]))) >= 4.0);
    for (i=0;i<tCount;i++)
      tlabels.push_back( TrimPrint( tickLocations[i], exponentialForm) );  
    tStart = t1;
    tStop = t2;
  }

  void NewAxis::AutoSetAxis() {
	  int i;
    UpdateIntervals(tMin, tMax);
    tCount = 0;
    while ((tStart + tCount*tStep) <= tStop) 
      tCount++;
    tlabels.clear();
    tickLocations = new double[tCount];
    for (i=0;i<tCount;i++)
      if (!isLogarithmic)
	tickLocations[i] = tStart + i * tStep;
      else
	tickLocations[i] = pow(10.0, tStart + i * tStep);
    bool exponentialForm;
    exponentialForm = false;
    for (i=0;i<tCount;i++)
      if (tickLocations[i] != 0.0) {
	if (fabs(tickLocations[i]) > 1e-15)
	  exponentialForm |= (fabs(log10(fabs(tickLocations[i]))) >= 4.0);
      }
    for (i=0;i<tCount;i++) {
      if (!isLogarithmic)
	tlabels.push_back( TrimPrint( tStart + i * tStep, exponentialForm) );
      else
	tlabels.push_back( TrimPrint( tStart + i * tStep, false ));
    }
  }

  void NewAxis::GetIntervals(double &tmin, double &tdelta, int &tcount) {
    tmin = tMin;
    tdelta = tStep;
    tcount = tCount;
  }

  std::string NewAxis::TrimPrint(double val, bool scientificNotation) {
    char buffer[1000];
    char *p;
    if (!scientificNotation) {
      sprintf(buffer,"%f",val);
      p = buffer + strlen(buffer) - 1;
      while (*p == '0') {
	*p = 0;
	p--;
      }
      if ((*p == '.') || (*p == ',')) {
	*(p+1) = '0';
	*(p+2) = 0;
      }
      return std::string(buffer);
    } else {
      sprintf(buffer,"%e",val);
      std::string label(buffer);
      unsigned int ePtr;
      ePtr = label.size() - 1;
      while ((label[ePtr] != 'e') && (label[ePtr] != 'E'))
	ePtr--;
      ePtr--;
      while (label[ePtr] == '0') {
	label.erase(ePtr,1);
	ePtr--;
      }
      if ((label[ePtr] == '.') || (label[ePtr] == ','))
	label.insert(ePtr+1, 1,'0');
      ePtr = label.size() - 1;
      while ((label[ePtr] != 'e') && (label[ePtr] != 'E'))
	ePtr--;
      ePtr+=2;
      while ((label[ePtr] == '0') && ePtr < label.size()) {
	label.erase(ePtr,1);
      }
      if (ePtr == label.size())
	label.append("0");
      return label;
    }
  }

  void NewAxis::GetLabelExtent(GraphicsContext &dc, std::string &label, int &w, int &h) {
    Point2D tensize(dc.GetTextExtent("10"));
    int tenHeight(tensize.y);
    int tenWidth(tensize.x);
    Point2D labelsize(dc.GetTextExtent(label));
    int labelHeight(labelsize.y);
    int labelWidth(labelsize.x);
    if (!isLogarithmic) {
      w = labelWidth;
      h = labelHeight;
    } else {
      w = (int) (tenWidth + 1.05 * labelWidth);
      h = (int) (tenHeight + 0.7 * labelHeight);
    }      
  }

  void NewAxis::HCenterLabel(GraphicsContext &dc, std::string label, int x, int y) {
    Point2D tsize(dc.GetTextExtent(label));
    int tw(tsize.x), th(tsize.y);
    Point2D tensize(dc.GetTextExtent("10"));
    int tenHeight(tensize.y), tenWidth(tensize.x);
    int descender = (int) (0.7 * th);
    if (isLogarithmic) {
      dc.DrawTextString( "10", Point2D(x - (tw + tenWidth)/2, y + descender));
      dc.DrawTextString( label, Point2D((int)(x - (tw + tenWidth)/2 + tenWidth + 0.05 * tw), y ));
    } else 
      dc.DrawTextString( label, Point2D((int)(x - tw/2), y));
  }

  void NewAxis::VCenterLabel(GraphicsContext &dc, std::string label, int x, int y) {
    Point2D tsize(dc.GetTextExtent(label));
    int tw(tsize.x), th(tsize.y);
    Point2D tensize(dc.GetTextExtent("10"));
    int tenHeight(tensize.y), tenWidth(tensize.x);
    int descender = (int) (0.7 * th);
    if (isLogarithmic) {
      dc.DrawTextString( "10", Point2D((int)(x - tenWidth - 1.05 * tw),
				 (int)(y - ( tenHeight + 0.7 * th ) + descender)) );
      dc.DrawTextString( label, Point2D(x - tw, (int)(y - ( tenHeight + 0.7 * th ))) );
    } else 
      dc.DrawTextString( label, Point2D(x - tw, y + th / 2) );
  }  

  void NewAxis::ComputeTextBounds(GraphicsContext &dc) {
    int labelHeight, labelWidth;
    labelHeight = 0;
    labelWidth = 0;
    for (int i=0;i < tCount; i++) {
      int w, h;
      GetLabelExtent(dc, tlabels[i], w, h );
      labelHeight = (labelHeight > h) ? labelHeight : h;
      labelWidth = (labelWidth > w ) ? labelWidth : w;
    }
    maxLabelExtent = labelHeight;
    Point2D titleSize(dc.GetTextExtent(title));
    titleWidth = titleSize.x;
    titleHeight = titleSize.y;
  }

}
