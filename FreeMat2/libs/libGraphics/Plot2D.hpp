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

#ifndef __Plot2D_hpp__
#define __Plot2D_hpp__

#include "DataSet2D.hpp"
#include "NewAxis.hpp"
#include "XPWidget.hpp"

namespace FreeMat {

  typedef std::vector<DataSet2D> DataSet2DVector;
  
  /**
   * This class is a 2D plot class descended from
   * wxFrame.  It manages an x and y Axis, and a 
   * number of 2D data sets.  It is responsible 
   * for drawing the data sets and managing them.
   */
  class Plot2D : public XPWidget {
  public:
    /**
     * Construct a 2D plot frame with the given
     * figure number.
     */
    Plot2D(int width, int height);
    /**
     * Default destructor.
     */
    ~Plot2D();
    /**
     * Render the legend (if any) to the current plot.
     */
    void DrawLegend(GraphicsContext &gc);
    /**
     * Set the legend data for the current plot.  The
     * legendData array is a sequence of linestyle/label
     * pairs.
     */
    void SetLegend(double xc, double yc, Array style, ArrayVector legendData);
    /**
     * Add another data set to the current plot.
     */
    void AddPlot(DataSet2D dp);
    /**
     * Set the grid flag.
     */
    void SetGrid(bool);
    /**
     * Force the data range to an x-range of $$[x_1,x_2]$$
     * and a y-range of $$[y_1,y_2]$$.
     */
    void SetAxes(double x1, double x2, double y1, double y2);
    /**
     * Get the data ranges of the current axes.
     */
    void GetAxes(double &x1, double &x2, double &y1, double &y2);
    /**
     * Set the logarithmic flag for the x & y axis.
     */
    void SetLog(bool xLog, bool yLog);
    /**
     * Get the current value of the hold flag.
     */
    bool GetHoldFlag();
    /**
     * Set the hold flag - if the hold flag is true,
     * then the addition of a new plot is added on top
     * of the existing ones.  If the hold flag is false,
     * then the addition of a new plot replaces the 
     * existing one.
     */
    void SetHoldFlag(bool flag);
    /**
     * Set the text of the title.
     */
    void SetTitleText(std::string);
    /**
     * Set the title for the X axis.
     */ 
    void SetXLabel(std::string);
    /**
     * Set the title for the Y axis.
     */
    void SetYLabel(std::string);
    /**
     * Set us up for a sequence of plots.
     */
    void StartSequence();
    /**
     * A sequence of plots is complete.
     */
    void StopSequence();
    /**
     * Routine that draws the contents of the window.
     */
    void OnDraw(GraphicsContext &gc);
    /**
     * Force the axes to fit the data tightly - i.e., with no
     * additional buffering to make the labels come out nicely.
     */
    void SetAxesTight();
    /**
     * Reset the axes to fit the data automatically, with additional
     * buffering added so as to make the labels come out nicely.
     */
    void SetAxesAuto();
    void MapPoint(double, double, int&, int&);
    void DrawAxes(GraphicsContext& gc);
    void AddText(double, double, std::string);
    void DrawTextLabels(GraphicsContext &gc);
  private:
    /**
     * The vector of text data
     */
    std::vector<double> textx;
    std::vector<double> texty;
    std::vector<std::string> textlabel;
    /**
     * The vector of datasets.
     */
    DataSet2DVector data;
    /**
     * The x axis object.
     */
    NewAxis xAxis;
    std::string xlabel;
    /**
     * The y axis object.
     */
    NewAxis yAxis;
    std::string ylabel;
    bool gridflag;
    /**
     * The title of the plot.
     */
    std::string title;
    /**
     * The height of the title.
     */
    int titleHeight;
    /**
     * The width of the title.
     */
    int titleWidth;
    /**
     * A characteristic length in the construction of the plot.
     */
    int space;
    int ticlen;
    /**
     * The hold flag for this plot.
     */
    bool holdflag;
    /**
     * The state of the hold flag is saved when a sequence
     * is started, set to true for the duration of the sequence,
     * and then restored at the end.
     */
    bool holdSave;
    /**
     * Our figure number.
     */
    int myFigureNumber;
    /**
     * Set to true during plot updates.
     */ 
    bool updating;
    /**
     * The current set of legend data
     */
    bool legendActive;
    double legend_xc;
    double legend_yc;
    char legend_linestyle[4];
    ArrayVector legend_data;
    int sze_textheight;
    Rect2D viewport;
  };
  
  // These are some helper routines called by DataSet2D and
  // Plot2D itself
  LineStyleType UtilityMapLineStyleToType(char line);
  Color UtilityMapColorSpecToColor(char cspec);
  void PutSymbol(GraphicsContext &dc, int xp, int yp, char symbol, int len);
}

#endif  
