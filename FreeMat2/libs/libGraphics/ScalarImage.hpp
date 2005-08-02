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

#ifndef __ScalarImage_h__
#define __ScalarImage_h__

#include "Array.hpp"
#include "GraphicsContext.hpp"
#include "XPWidget.hpp"

namespace FreeMat {

  /**
   * This is a window for viewing a scalar image with
   * a color map.
   */
  class ScalarImage : public QWidget {
  public:
    /**
     * Create a new scalar image window with the given
     * figure number.
     */
    ScalarImage(QWidget* parent);
    /**
     * Destructor.
     */
    virtual ~ScalarImage();
    /**
     * Pick a point in the current image
     */
    Array GetPoint();
    void Zoom1D(double* src, double *dst,
		int s_stride, int d_stride,
		int s_count, int d_count);
    void Zoom(float z);
    /**
     * Set the colormap for the image
     */
    void SetColormap(Array &dp);
    /**
     * Set the contents of the figure to the given
     * array with dimensions.
     */
    void SetImageArray(Array &dp, double zoomf);    
    /**
     * Routine that draws the contents of the window.
     */
    void OnDraw(GraphicsContext &gc);
    /**
     * Routine to change the window level
     */
    void WindowLevel(double awindow, double alevel);
    double GetCurrentWindow();
    double GetCurrentLevel();
    int getZoomColumns();
    int getZoomRows();
    int ColorbarWidth(int zoomRows);
    void OnResize();
  private:
    /**
     * Our pointer to the raw data.  We own this data, and
     * are responsible for freeing it when destructed.
     */
    double *rawData;
    /**
     * The colormap to apply to the data when being displayed.
     */
    char colormap[3][256];
    /**
     * The display window.
     */
    double window;
    /**
     * The display level.
     */
    double level;
    /**
     * Number of rows in the image.
     */
    int rows;
    /**
     * Number of columns in the image.
     */
    int columns;
    /**
     * Our figure number.
     */
    int myFigureNumber;
    /**
     * The zoom factor.
     */
    double zoom;
    /**
     * The number of columns in the zoomed image
     */
    int zoomColumns;
    double *zoomImage;
    /**
     * The number of rows in the zoomed image
     */
    int zoomRows;
    // The data for the zoomed pic in RGB format
    uchar *picData;
    void UpdateZoom(bool forceUpdate);
    void UpdateImage();
    bool inClickState;
    int xposClick;
    int yposClick;
    bool drawColorBar;
    // The data for the colorbar
    uchar *barData;
    int barWidth;
    int numTicks;
    int tickWidth;
    int maxLabelWidth;
  };
}
#endif  
