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

#ifndef __NewAxis_hpp__
#define __NewAxis_hpp__

#include "GraphicsContext.hpp"
#include <vector>
#include <string>

namespace FreeMat {

  // A new axis class - reimplements what the Axis class
  // used to do, but more generically.
  // An axis cannot draw itself, but is the logical entity
  class NewAxis {
  private:
    /**
     * The min and max values for the data represented with this axis.
     */
    double tMin, tMax;
    /**
     * the axis range
     */
    double tBegin, tEnd;
    double tStart, tStop;
    double tDelt;
    int tCount;
    /**
     * True if we are a log-axis
     */
    bool isLogarithmic;
    /**
     * The contents of the labels along the axis.
     */
    std::vector<std::string> tlabels;
    /**
     * The locations of the ticks along the axis (in t-coordinates)
     */
    std::vector<double> tickLocations;
    bool manualmode;
    void SetTicks();
    int axisLength;
  public:
    /**
     * Default constructor.
     */
    NewAxis();
    void SetAxisLength(int npix);
    int GetNominalTickCount();
    void GetIntervals(double &t1, double &t2, int &tn);
    /**
     * Set the data range for the axis to [t1,t2]
     */
    void SetDataRange(double t1, double t2);
    /**
     * Set the range of the axis to [t1,t2].
     */
    void ManualSetAxis(double t1, double t2);
    /**
     * Set the range of the axis to [t1,t2] and auto-adjust
     */
    void AutoSetAxis();
    /**
     * Set the logarithmic flag to the specified value.
     */
    void SetLogarithmic(bool);
    void GetAxisExtents(double& t1, double & t2);
    double Normalize(double);
    std::vector<double> GetTickLocations();
    std::vector<std::string> GetTickLabels();
  };
}

#endif
