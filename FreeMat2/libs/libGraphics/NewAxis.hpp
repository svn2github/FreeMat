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
  class NewAxis {
  private:
    /**
     * The min and max values for the data represented with this axis.
     */
    double tMin, tMax;
    /**
     * The start, stop and step size for the tick marks
     */
    double tStart, tStop, tStep;
    /**
     * True if we are a log-axis
     */
    bool isLogarithmic;
    /**
     * The number of ticks to draw.
     */
    int tCount;
    /**
     * The contents of the labels along the axis.
     */
    std::vector<std::string> tlabels;
    /**
     * The locations of the ticks along the axis (in t-coordinates)
     */
    double *tickLocations;
    /**
     * The maximum label extent (height for X axis, width for Y axis)
     */
    int maxLabelExtent;
    /**
     * The title associated with the axis.
     */
    std::string title;
    /**
     * The width of the title in pixels
     */
    int titleWidth;
    /**
     * The height of the title in pixels
     */
    int titleHeight;
    /**
     * The x and y coordinates (offset) for the axis
     * from its upper left corner.
     */
    int xOffset, yOffset;
    /**
     * The length of the axis box.
     */
    int length;
    /**
     * A parameter that controls the length of various spaces
     * when the axis is rendered.
     */
    int space;
    /**
     * The length of the tick marks (in pixels).
     */
    int tickLength;
    /**
     * Print the given floating point argument, truncating superfluous
     * zeros, and optionally, print it in exponential format.
     */
    std::string TrimPrint(double, bool);
    /**
     * Calculate the size of the given string given a device context - 
     * for logarithmic axes, the text size includes the size of the "10"
     * and the superscript spacing.
     */
    void GetLabelExtent(GraphicsContext &dc, std::string &, int &, int &);
    /**
     * Draw the given label horizontally centered at the given 
     * coordinates.  For logarithmic axes, the text is drawn 
     * in the superscript of a "10".  The vertical position of
     * the top of the label matches the given coordinate.
     */
    void HCenterLabel(GraphicsContext &dc, std::string, int, int);
    /**
     * Draw the given label vertically centered at the given 
     * coordinates.  For logarithmic axes, the text is drawn 
     * in the superscript of a "10".  The horizontal position of
     * the right of the label matches the given coordinate.
     */
    void VCenterLabel(GraphicsContext &dc, std::string, int, int);
  public:
    /**
     * Empty constructor.
     */
    NewAxis() {}
    /**
     * Construct an axis object that covers the given range of values,
     * with the specified logarithmic flag and axis type.
     */
    NewAxis(double minVal, double maxVal, bool logarithmic);
    /**
     * Update our estimate of maxLabelExtent and the title dimensions
     * using the given device context.
     */
    void ComputeTextBounds(GraphicsContext &dc);
    /**
     * Calls UpdateIntervals for the current tMin and tMax.  And then
     * updates the set of labels.
     */
    void AutoSetAxis();
    /**
     * Set the range of the axis to [t1,t2].
     */
    void ManualSetAxis(double t1, double t2);
    /**
     * Get the range of the axis.
     */
    void GetAxisExtents(double &t1, double &t2);
    /**
     * Set the logarithmic flag to the specified value.
     */
    void SetLogarithmic(bool);
    /**
     * Set the title for the current axis to the given text.
     */
    void SetLabelText(std::string&);
    /**
     * Set tMin and tMax to the given range, and then
     * call AutoSetAxis.
     */
    void SetExtent(double minVal, double maxVal);
    /**
     * Get the axis info
     */
    void GetIntervals(double &tmin, double &tdelta, int &tcount);
    /**
     * Calculate tStart, tStop and tStep for the given range assignment.
     * This is based on a heuristic algorithm, that I took from Robert
     * Roebling's wxPlotWindow code.  It essentially starts out by estimating
     * the step size as the power of ten that is closest to the range:
     * $$t_{\mathrm{step}} = 10^{\lfloor\log_{10}(\Delta t)\rfloor}$$
     * Next, the number of steps that fit in the range is calculated.
     * The step size is adjusted until at least 6 steps fit inside the
     * range.  For a logarithmic scale, the step is mapped to the closest
     * integer (only integer power steps are allowed for logarithmic
     * axes.  Finally, tStart and tStop are calculated via
     * $$t_{\mathrm{Start}} = t_{\mathrm{Step}} \lfloor \frac{t1}{t_{\mathrm{Step}}} \rfloor$$
     * $$t_{\mathrm{Stop}} = t_{\mathrm{Step}} \lceil \frac{t2}{t_{\mathrm{Step}}} \rceil$$
     */
    void UpdateIntervals(double t1, double t2);
  };
}

#endif
