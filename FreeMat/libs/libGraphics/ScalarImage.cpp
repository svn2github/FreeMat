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

#include "ScalarImage.hpp"
#include "GraphicsServer.hpp"
#include <math.h>
#include <iostream>
#include "Malloc.hpp"
#include <wx/image.h>
#include "Command.hpp"

namespace FreeMat {

IMPLEMENT_DYNAMIC_CLASS(ScalarImage, wxFrame)

BEGIN_EVENT_TABLE(ScalarImage, wxFrame)
  EVT_PAINT(ScalarImage::OnPaint)
  EVT_CLOSE(ScalarImage::OnClose)
  EVT_SIZE(ScalarImage::OnSize)
  EVT_LEFT_DOWN(ScalarImage::OnMouseLeftDown)
END_EVENT_TABLE()

  ScalarImage::ScalarImage(GraphicsServer *server, int fignum) : 
    PrintableFrame(_T("Image Window"), wxDefaultSize) {
  SetBackgroundColour( *wxBLACK);
  m_server = server;
  rawData = NULL;
  for (int i=0;i<256;i++) {
    colormap[0][i] = i;
    colormap[1][i] = i;
    colormap[2][i] = i;
  }
  window = 0;
  level = 0;
  state = 0;
  myFigureNumber = fignum;
  wxString title;
  title.Printf("Image Window %d",fignum+1);
  SetTitle(title);  
  img = NULL;
  gmp = NULL;
  zoomImage = NULL;
}

  ScalarImage::~ScalarImage() {
    if (rawData) Free(rawData);
    if (gmp) delete gmp;
    if (img) delete img;
  }

  void ScalarImage::GetClick() {
    // First, change our cursor to 
    SetCursor(*wxCROSS_CURSOR);
    // Set the click bit.
    // state = 1;
    //while (state == 1) {
    //  m_server->Yield(true);
    // }
    // First, change our cursor to 
    // Set the click bit.
    state = 1;
//     Array retval(Array::doubleVectorConstructor(3));
//     double *d_ip;
//     d_ip = (double*) retval.getReadWriteDataPointer();
//     d_ip[0] = (double) (yposClick/((double)zoomRows)*rows)+1;
//     d_ip[1] = (double) (xposClick/((double)zoomColumns)*columns)+1;
//     d_ip[2] = (double) valClick;
//     return retval;
  }

  void ScalarImage::SetColormap(Array &dp) {
    dp.promoteType(FM_DOUBLE);
    double *dpdat;
    dpdat = (double*) dp.getDataPointer();
    for (int j=0;j<3;j++)
      for (int i=0;i<256;i++)
	colormap[j][i] = (char)(255.0*(*dpdat++));
    UpdateImage();
  }

  void ScalarImage::Zoom1D(double* src, double *dst,
		       int s_stride, int d_stride,
		       int s_count, int d_count) {
    double s_time, d_time;
    double d_delta;
    d_delta = s_count/((double) d_count);
    for (int i=0;i<d_count;i++) {
      int srcPoint = (int) (i*d_delta);
      dst[i*d_stride] = src[srcPoint*s_stride];
    }
  }

  void ScalarImage::Zoom(float z) {
    zoom = z;
    UpdateZoom();
  }

  void ScalarImage::UpdateZoom() {
    if (rawData == NULL) return;
    if (zoom>0) {
      zoomColumns = (int) (zoom*columns);
      zoomRows = (int) (zoom*rows);
    } else if (zoom == 0) {
      int client_width;
      int client_height;
      GetClientSize(&client_width,&client_height);
      double zoomColFactor;
      zoomColFactor = ((double) client_width)/columns;
      double zoomRowFactor;
      zoomRowFactor = ((double) client_height)/rows;
      double effectiveZoom;
      effectiveZoom = (zoomColFactor < zoomRowFactor) ? 
	zoomColFactor : zoomRowFactor;
      int newZoomColumns = (int) (effectiveZoom*columns);
      int newZoomRows = (int) (effectiveZoom*rows);
      zoomColumns = newZoomColumns;
      zoomRows = newZoomRows;
    } else {
      int client_width;
      int client_height;
      GetClientSize(&client_width,&client_height);
      int newZoomColumns = (int) (client_width);
      int newZoomRows = (int) (client_height);
      zoomColumns = newZoomColumns;
      zoomRows = newZoomRows;
    }
    if (zoomImage != NULL)
      delete[] zoomImage;
    if (zoomColumns < 1) zoomColumns = 1;
    if (zoomRows < 1) zoomRows = 1;
    zoomImage = new double[zoomColumns*zoomRows];
    double *tmpImage = new double[zoomRows*columns];
    // First zoom the columns
    for (int i=0;i<columns;i++)
      Zoom1D(rawData + i*rows,tmpImage + i*zoomRows,
	     1,1,rows,zoomRows);
    // Then zoom the rows 
    for (int i=0;i<zoomRows;i++)
      Zoom1D(tmpImage + i, zoomImage + i,
	     zoomRows, zoomRows,
	     columns, zoomColumns);
    delete[] tmpImage;
    SetSize(zoomColumns,zoomRows);
    UpdateImage();
  }

  void ScalarImage::OnClose(wxCloseEvent &event) {
    m_server->NotifyImageClose(myFigureNumber);
    Destroy();
  }

  void ScalarImage::OnSize(wxSizeEvent &event) {
    if (zoom <= 0) 
      UpdateZoom();
  }

  void ScalarImage::OnMouseLeftDown(wxMouseEvent &event) {
    if (state == 1) {
      event.GetPosition(&xposClick,&yposClick);
      if (zoomImage == NULL) 
	valClick = atof("nan");
      else 
	valClick = zoomImage[xposClick*zoomRows+yposClick];
      Array retval(Array::doubleVectorConstructor(3));
      double *d_ip;
      d_ip = (double*) retval.getReadWriteDataPointer();
      d_ip[0] = (double) (yposClick/((double)zoomRows)*rows)+1;
      d_ip[1] = (double) (xposClick/((double)zoomColumns)*columns)+1;
      d_ip[2] = (double) valClick;
      PostGUIReply(new FreeMat::Command(CMD_PointAcq,retval));
      SetCursor(*wxSTANDARD_CURSOR);
      state = 0;
    }
  }

  void ScalarImage::DrawMe(wxDC &dc) {
    if (rawData == NULL) return;
    wxMemoryDC temp_dc;
    temp_dc.SelectObject(*gmp); 
    dc.Blit(0,0,zoomColumns,zoomRows,&temp_dc,0,0);
  }

  void ScalarImage::SetImage(Array &dp) {
    if (rawData != NULL)
      Free(rawData);
    rows = dp.getDimensionLength(0);
    columns = dp.getDimensionLength(1);
    rawData = (double*) Malloc(rows*columns*sizeof(double));
    memcpy(rawData,dp.getDataPointer(),rows*columns*sizeof(double));
    double minval;
    double maxval;
    minval = rawData[0];
    maxval = rawData[0];
    int length;
    length = rows*columns;
    for (int i=0;i<length;i++) {
      minval = (minval < rawData[i]) ? minval : rawData[i];
      maxval = (maxval > rawData[i]) ? maxval : rawData[i];
    }
    window = maxval - minval;
    level = (maxval + minval)/2.0;
    zoom = 0.0;
    UpdateZoom();
  }

  void ScalarImage::UpdateImage() {
    if (rawData == NULL) return;
    if (img != NULL)
      delete img;
    if (gmp != NULL)
      delete gmp;
    img = new wxImage(zoomColumns,zoomRows);
    unsigned char* op = img->GetData();
    int length = zoomColumns * zoomRows;
    double minval = level - window/2.0;
    double delta = 255.0/window;
    for (int i=0;i<zoomColumns;i++) {
      for (int j=0;j<zoomRows;j++) {
	int dv, ndx;
	dv = (int)((zoomImage[i*zoomRows+j] - minval)*delta);
	dv = (dv < 0) ? 0 : dv;
	dv = (dv > 255) ? 255 : dv;
	ndx = 3*(i+zoomColumns*j);
	op[ndx] = colormap[0][dv];
	op[ndx+1] = colormap[1][dv];
	op[ndx+2] = colormap[2][dv];
      }
    }
    gmp = new wxBitmap(*img);  
    Refresh(false);
    Update();
  }

  void ScalarImage::OnPaint(wxPaintEvent &WXUNUSED(event)) {
    if (gmp == NULL) return;

    wxPaintDC dc( this );
    PrepareDC( dc );
    DrawMe(dc);
  }
}
