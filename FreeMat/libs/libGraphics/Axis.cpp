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

#include "Axis.hpp"
#include <math.h>
#include <iostream>

namespace FreeMat {

  Axis::Axis(double minVal, double maxVal, bool logarithmic, AxisType type) {
    tMin = minVal;
    tMax = maxVal;
    isLogarithmic = logarithmic;
    axisType = type;
    space = 10;
    tickLength = 5;
    AutoSetAxis();
    //  title = "a title";
    gridOn = false;
  }

  void Axis::SetLabelText(wxString& text) {
    title = text;
  }

  void Axis::SetExtent(double minVal, double maxVal) {
    tMin = minVal;
    tMax = maxVal;
    AutoSetAxis();
  }

  void Axis::SetLogarithmic(bool logarithmic) {
    isLogarithmic = logarithmic;
    AutoSetAxis();
  }

  void Axis::UpdateIntervals(double t1, double t2) {
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

  void Axis::GetAxisExtents(double &t1, double &t2) {
    t1 = tStart;
    t2 = tStop;
  }

  void Axis::ManualSetAxis(double t1, double t2) {
    UpdateIntervals(t1, t2);
    double acnt;
    acnt = (tStop - tStart)/tStep + 1.0;
    // Count the number of ticks inside the desired range
    tCount = 0;
    for (int i=0;i<acnt;i++)
      if (((tStart + i * tStep) >= t1) && ((tStart + i * tStep) <= t2))
	tCount++;
    tlabels.Clear();
    tickLocations = new double[tCount];
    int tNum = 0;
    for (int i=0;i<acnt;i++)
      if (((tStart + i * tStep) >= t1) && ((tStart + i * tStep) <= t2))
	tickLocations[tNum++] = tStart + i * tStep;
    bool exponentialForm;
    exponentialForm = false;
    for (int i=0;i<tCount;i++)
      if (tickLocations[i] != 0.0)
	exponentialForm |= fabs(log10(fabs(tickLocations[i]))) >= 4.0;
    for (int i=0;i<tCount;i++)
      tlabels.Add( TrimPrint( tickLocations[i], exponentialForm) );  
    tStart = t1;
    tStop = t2;
  }

  void Axis::AutoSetAxis() {
    UpdateIntervals(tMin, tMax);
    double acnt;
    acnt = (tStop - tStart)/tStep + 1.0;
    tCount = (int) ceil(acnt);
    tlabels.Clear();
    tickLocations = new double[tCount];
    for (int i=0;i<tCount;i++)
      if (!isLogarithmic)
	tickLocations[i] = tStart + i * tStep;
      else
	tickLocations[i] = pow(10.0, tStart + i * tStep);
    bool exponentialForm;
    exponentialForm = false;
    for (int i=0;i<tCount;i++)
      if (tickLocations[i] != 0.0)
	exponentialForm |= fabs(log10(fabs(tickLocations[i]))) >= 4.0;
    for (int i=0;i<tCount;i++)
      if (!isLogarithmic)
	tlabels.Add( TrimPrint( tStart + i * tStep, exponentialForm) );
      else
	tlabels.Add( TrimPrint( tStart + i * tStep, false ));
  }

  wxString Axis::TrimPrint(double val, bool scientificNotation) {
    wxString label;
  
    if (!scientificNotation) {
      label.Printf( wxT("%f"), val);
      while (label.Last() == wxT('0')) 
	label.RemoveLast();
      if ((label.Last() == wxT('.')) || (label.Last() == wxT(',')))
	label.Append( wxT('0') );
    } else {
      label.Printf( wxT("%e"), val);
      unsigned int ePtr;
      ePtr = label.Len() - 1;
      while ((label[ePtr] != wxT('e')) && (label[ePtr] != wxT('E')))
	ePtr--;
      ePtr--;
      while (label[ePtr] == wxT('0')) {
	label.Remove(ePtr,1);
	ePtr--;
      }
      if ((label[ePtr] == wxT('.')) || (label[ePtr] == wxT(',')))
	label.insert(ePtr+1, 1, wxT('0'));
      ePtr = label.Len() - 1;
      while ((label[ePtr] != wxT('e')) && (label[ePtr] != wxT('E')))
	ePtr--;
      ePtr+=2;
      while (label[ePtr] == wxT('0') && ePtr < label.Len()) {
	label.Remove(ePtr,1);
      }
      if (ePtr == label.Len())
	label.Append(wxT('0'));
    }
    return label;
  }

  void Axis::GetLabelExtent(wxDC &dc, wxString &label, wxCoord &w, wxCoord &h) {
    wxCoord tenHeight, tenWidth;
    dc.GetTextExtent( wxT("10"), &tenWidth, &tenHeight );
    wxCoord labelHeight, labelWidth;
    dc.GetTextExtent( label, &labelWidth, &labelHeight );
    if (!isLogarithmic) {
      w = labelWidth;
      h = labelHeight;
    } else {
      w = (int) (tenWidth + 1.05 * labelWidth);
      h = (int) (tenHeight + 0.7 * labelHeight);
    }      
  }

  void Axis::HCenterLabel(wxDC &dc, wxString label, wxCoord x, wxCoord y) {
    wxCoord tw, th;
    dc.GetTextExtent( label, &tw, &th );
    wxCoord tenHeight, tenWidth;
    dc.GetTextExtent( wxT("10"), &tenWidth, &tenHeight );
    wxCoord descender = (int) (0.7 * th);
    if (isLogarithmic) {
      dc.DrawText( wxT("10"), x - (tw + tenWidth)/2, y + descender);
      dc.DrawText( label, (int)(x - (tw + tenWidth)/2 + tenWidth + 0.05 * tw), y );
    } else 
      dc.DrawText( label, (int)(x - tw/2), y );
  }

  void Axis::VCenterLabel(wxDC &dc, wxString label, wxCoord x, wxCoord y) {
    wxCoord tw, th;
    dc.GetTextExtent( label, &tw, &th );
    wxCoord tenHeight, tenWidth;
    dc.GetTextExtent( wxT("10"), &tenWidth, &tenHeight );
    wxCoord descender = (wxCoord) (0.7 * th);
    if (isLogarithmic) {
      dc.DrawText( wxT("10"), (int)(x - tenWidth - 1.05 * tw),
		   (int)(y - ( tenHeight + 0.7 * th ) + descender) );
      dc.DrawText( label, x - tw, 
		   (int)(y - ( tenHeight + 0.7 * th )) );
    } else 
      dc.DrawText( label, x - tw, y - th / 2 );
  }  

  void Axis::ComputeTextBounds(wxDC &dc) {
    wxCoord labelHeight, labelWidth;
    labelHeight = 0;
    labelWidth = 0;
    for (int i=0;i < tCount; i++) {
      wxCoord w, h;
      GetLabelExtent(dc, tlabels[i], w, h );
      labelHeight = (labelHeight > h) ? labelHeight : h;
      labelWidth = (labelWidth > w ) ? labelWidth : w;
    }
    if (axisType == Axis_X)
      maxLabelExtent = labelHeight;
    else
      maxLabelExtent = labelWidth;
    dc.GetTextExtent( title, &titleWidth, &titleHeight );
  }

  void Axis::Place(wxCoord a_xOffset, wxCoord a_yOffset, 
		   wxCoord a_length, wxCoord a_grid_length) {
    xOffset = a_xOffset;
    yOffset = a_yOffset;
    length = a_length;
    grid_length = a_grid_length;
  }

  wxCoord Axis::MapPoint(double t) {
    if (!isLogarithmic) {
      if (axisType == Axis_X) 
	return ((wxCoord)(xOffset + (t - tStart)/(tStop - tStart) * length));
      if (axisType == Axis_Y)
	return ((wxCoord)(yOffset + (tStop - t)/(tStop - tStart) * length));
    } else {
      if (axisType == Axis_X) {
	double s;
	s = log10(t);
	return ((wxCoord)(xOffset + (s - tStart)/(tStop - tStart) * length));
      }
      if (axisType == Axis_Y) {
	double s;
	s = log10(t);
	return ((wxCoord)(yOffset + (tStop - s)/(tStop - tStart) * length));
      }      
    }
    return 0;
  }

  void Axis::DrawMe(wxDC &dc) {
    dc.SetPen( *wxBLACK_PEN);
    dc.SetBrush( *wxTRANSPARENT_BRUSH);
    if (axisType == Axis_X) {
      // The x title is centered on the axis, and is 2 spaces
      // below the axis line.
      dc.DrawText(title, xOffset + (length - titleWidth)/2,
		  yOffset + 2*space + maxLabelExtent);      
      dc.DrawLine(xOffset, yOffset, xOffset + length + 1, yOffset);
      for (int i=0;i<tCount;i++) {
	wxCoord tp;
	tp = MapPoint(tickLocations[i]);
	// Draw the grid line
	if (gridOn) {     
	  dc.SetPen( wxPen( wxT("light grey"), 1, wxDOT) );
	  dc.DrawLine(tp, yOffset, tp, yOffset - grid_length);
	  dc.SetPen ( *wxBLACK_PEN);
	}
	dc.DrawLine(tp, yOffset, tp, yOffset - tickLength);
	HCenterLabel(dc, tlabels[i], tp, yOffset + space);
      }
    } else {
      // The y title is centered on the axis, and is 2 spaces to 
      // the left of the axis line.
      if (titleWidth != 0)
	dc.DrawRotatedText(title, xOffset - 2*space - maxLabelExtent - titleHeight,
			   yOffset + (length + titleWidth)/2, 90);
      dc.DrawLine(xOffset, yOffset, xOffset, yOffset + length + 1);
      for (int i=0;i<tCount;i++) {
	wxCoord tp;
	tp = MapPoint(tickLocations[i]);
	// Draw the grid line
	if (gridOn) {     
	  dc.SetPen( wxPen( wxT("light grey"), 1, wxDOT) );
	  dc.DrawLine(xOffset, tp, xOffset + grid_length, tp);
	  dc.SetPen ( *wxBLACK_PEN);
	}
	dc.DrawLine(xOffset, tp, xOffset + tickLength, tp);
	//        wxCoord tw, th;
	//        dc.GetTextExtent( tlabels[i], &tw, &th );
	//        dc.DrawText( tlabels[i], xOffset - space - tw, tp - th / 2);
	VCenterLabel(dc, tlabels[i], xOffset - space, tp);
      }
    }
  }

  wxCoord Axis::getWidth() {
    if (axisType == Axis_X) {
      return length;
    } else {
      return (maxLabelExtent + 3*space + titleHeight);
    }
  }

  wxCoord Axis::getHeight() {
    if (axisType == Axis_X) {
      return (maxLabelExtent + 3*space + titleHeight);
    } else {
      return length;
    }
  }

  void Axis::SetGrid(bool gridArg) {
    gridOn = gridArg;
  }

}
