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


#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/socket.h"
#include "PrintableFrame.hpp"

#include "Array.hpp"

class GraphicsServer;

namespace FreeMat {

  /**
   * This is a window for viewing a scalar image with
   * a color map.
   */
  class ScalarImage: public PrintableFrame
  {
  public:
    /**
     * Create a new scalar image window with the given
     * figure number.
     */
    ScalarImage(GraphicsServer* parent, int fignum);
    /**
     * Default constructor.
     */
    ScalarImage() {}
    /**
     * Destructor.
     */
    ~ScalarImage();
    /**
     * Pick a point in the current image
     */
    Array GetClick();
    void Zoom1D(double* src, double *dst,
	   int s_stride, int d_stride,
	   int s_count, int d_count);
    void Zoom(float z);
    void UpdateZoom();

    /**
     * Set the colormap for the image
     */
    void SetColormap(Array &dp);

    /**
     * Set the contents of the figure to the given
     * array with dimensions.
     */
    void SetImage(Array &dp);    
    /**
     * Painting event.
     */
    void OnPaint(wxPaintEvent & WXUNUSED(event));
    /**
     * Sends a close notification to the main application
     * when the window is closed.
     */
    void OnClose(wxCloseEvent &event);
    /**
     * If this window is resized, and zoom is set to -1, then
     * this method will update the zoom image.
     */
    void OnSize(wxSizeEvent &event);
    /**
     * If the user clicks in the window, and the state is 1
     * this method will record the click location.
     */
    void OnMouseLeftDown(wxMouseEvent &event);
    void DrawMe(wxDC &dc);
  private:
    /**
     * The wxImage object stores the device independent
     * representation of the image.
     */
    wxImage *img;
    /**
     * The wxBitmap object stores the image after it has
     * been mapped to a particular device context.
     */
    wxBitmap *gmp;
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
    /**
     * Int that tracks the current state
     */
    int state;
    /**
     * Track the result of a click when in "GetClick" mode
     */
    wxCoord xposClick;
    wxCoord yposClick;
    double valClick;
    /**
     * Pointer to the main application (so that it can be notified
     * when we are closed).
     */
    GraphicsServer* m_server;
    /**
     * Update the internal wxImage and wxBitmap objects.
     */
    void UpdateImage();
  
    DECLARE_DYNAMIC_CLASS(ScalarImage)
    DECLARE_EVENT_TABLE()
  };
}
#endif  
