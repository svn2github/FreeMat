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

#ifndef __DataSet2D_hpp__
#define __DataSet2D_hpp__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Axis.hpp"
#include "Array.hpp"

namespace FreeMat {

  /**
   * This class encapsulates a single line in a 2D plot, including
   * the number of points in the plot, the x and y coordinates, the
   * color of the line, the symbol of the line, and the line-style.
   */
  class DataSet2D 
  {
    /**
     * The x-values for the data.
     */
    Array x;
    /**
     * The y-values for the data.
     */
    Array y;
    /**
     * The color for the data:
     *   - 'y' for yellow
     *   - 'm' for magenta
     *   - 'c' for cyan
     *   - 'r' for red
     *   - 'g' for green
     *   - 'b' for blue
     *   - 'w' for white
     *   - 'k' for black
     */
    char color;
    /**
     * The symbol for the data:
     *   - '.' for a point
     *   - 'o' for a circle
     *   - 'x' for an 'x' shape
     *   - '+' for a plus symbol
     *   - '*' for an 'x' and a '+' superimposed
     *   - 's' for a square
     *   - 'd' for a diamond
     *   - 'v' for a triangle that points down
     *   - '^' for a triangle that points up
     *   - '<' for a triangle that points left
     *   - '>' for a triangle that points right
     *   - 'h' for a six-sided star
     */
    char symbol;
    /**
     * The line style:
     *   - '-' for a solid line
     *   - ':' for a dotted line
     *   - ';' for a dash-dot line
     *   - '|' for a dashed line
     */
    char line;  
    /**
     * The characteristic length of the symbol when drawn.
     * For screen rendering, this is in pixels.
     */
    int symbolLength;
    /**
     * Set the pen color in the given device context to the line
     * color for the current dataset.  Optionally apply the line
     * style also.
     */
    void SetPenColor(wxDC&, bool);
    /**
     * Draw the indicated symbol at the given coordinates, with 
     * a characteristic length indicated by len.
     */
    void PutSymbol(wxDC& dc, wxCoord xp, wxCoord yp, char symbol, wxCoord len);
    /**
     * Converts the pen style into a wxWindows pen style.
     */
    int GetPenStyle();
  public:
    /**
     * Construct a dataset with the given attributes.
     */
    DataSet2D(Array xarg, Array yarg, char a_color, char a_symbol, char a_line);
    /**
     * Delete the data set - Calls Free on the various arrays (line
     * style, x and y values.
     */
    ~DataSet2D();
    /**
     * Plot the data set to the given device context, using the 
     * given x and y axis.
     */
    void DrawMe(wxDC& dc, Axis* xAxis, Axis* yAxis);
    /**
     * Calculate the range of the data.
     */
    void GetDataRange(double& xMin, double& xMax, double& yMin, double& yMax);
  };
}
#endif
