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

#ifndef __VolView_h__
#define __VolView_h__


#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

extern "C" {
#include "trackball.h"
#include <volpack.h>
}

#include "wx/socket.h"
#include "PrintableFrame.hpp"

#include "Array.hpp"

#define NUM_FIELDS          3
#define NUM_SHADE_FIELDS    2
#define NUM_CLASSIFY_FIELDS 2
#define NORM_FIELD  0
#define NORM_MAX    VP_NORM_MAX
#define SCALAR_FIELD  1
#define SCALAR_MAX    255
#define GRAD_FIELD    2
#define GRAD_MAX      VP_GRAD_MAX
#define DENSITY_PARAM		0		/* classification parameters */
#define GRADIENT_PARAM		1


class GraphicsServer;

namespace FreeMat {

  typedef unsigned char Scalar;
  typedef unsigned short Normal;
  typedef unsigned char Gradient;
  typedef struct {
    Normal normal;
    Scalar scalar;
    Gradient gradient;
  } Voxel;

  /**
   * This is a window for viewing a scalar image with
   * a color map.
   */
  class VolView: public PrintableFrame
  {
  public:
    /**
     * Create a new scalar image window with the given
     * figure number.
     */
    VolView(GraphicsServer* parent, int fignum);
    /**
     * Default constructor.
     */
    VolView() {}
    /**
     * Destructor.
     */
    ~VolView();
    /**
     * Update the rendering
     */
    void UpdateRendering();
    /**
     * Set the volume data...
     */
    void SetVolume(Array &dp);
    /**
     * Set the gradient ramp.
     */
    void SetGradientRamp(Array &dp);
    /**
     * Set the density ramp.
     */
    void SetDensityRamp(Array &dp);
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
    void DrawMe(wxDC &dc);
    /*
     * Moise motion
     */
    void OnMouse(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void Resize();
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
    unsigned char *rawData;
    /**
     * Number of rows in the image.
     */
    int rows;
    /**
     * Number of columns in the image.
     */
    int columns;
    /**
     * Number of slices in the image
     */
    int slice;
    /**
     * Our figure number.
     */
    int myFigureNumber;
    /**
     * Pointer to the main application (so that it can be notified
     * when we are closed).
     */
    GraphicsServer* m_server;
    /**
     * Update the internal wxImage and wxBitmap objects.
     */
    void UpdateImage();
    /**
     * Initialize the VolPack Render Context
     */
    void initializeVPC(char* densityData, int xlen, int ylen, int zlen);
    void destroyVPC();
    void classifyVolume(int* drampx, float *drampy, int dramp_cnt,
			int* grampx, float *drampy, int gramp_cnt);
    void setupShader();
    void setViewpoint(float matrix[4][4]);
    
    vpContext *vpc;
    Voxel* volume;
    float density_ramp[255+1]; 
    float gradient_ramp[VP_GRAD_MAX+1];
    float shade_table[VP_NORM_MAX+1];
    int beginx;
    int beginy;
    float defview[16];
    float quat[4];
    int client_width;
    int client_height;
    bool dirty;

    DECLARE_DYNAMIC_CLASS(VolView)
    DECLARE_EVENT_TABLE()
  };
}
#endif  
