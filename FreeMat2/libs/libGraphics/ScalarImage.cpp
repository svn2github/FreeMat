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
#include <math.h>
#include <iostream>
#include "Malloc.hpp"
#include "GraphicsCore.hpp"
#include "QTGC.hpp"

namespace FreeMat {

  ScalarImage::ScalarImage(QWidget* parent) 
    : QWidget(parent,"scalarimage",0) {
    rawData = NULL;
    for (int i=0;i<256;i++) {
      colormap[0][i] = i;
      colormap[1][i] = i;
      colormap[2][i] = i;
    }
    window = 0;
    level = 0;
    zoomImage = NULL;
    picData = NULL;
    inClickState = false;
    drawColorBar = true;
    barData = NULL;
    barWidth = 20;
    tickWidth = 10;
  }
  
  ScalarImage::~ScalarImage() {
    if (rawData) Free(rawData);
  }


  Array ScalarImage::GetPoint() {
    int xposClick;
    int yposClick;
    throw Exception("Need to implement!");
#if 0
    GetClick(xposClick, yposClick);
    double valClick;
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
    return retval;
#endif
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
    double d_delta;
    d_delta = s_count/((double) d_count);
    for (int i=0;i<d_count;i++) {
      int srcPoint = (int) (i*d_delta);
      dst[i*d_stride] = src[srcPoint*s_stride];
    }
  }

  void ScalarImage::Zoom(float z) {
    zoom = z;
    UpdateZoom(false);
    //    if (z>0) SetSize(zoomColumns,zoomRows);
  }

  void ScalarImage::UpdateZoom(bool forceUpdate) {
    int newZoomRows, newZoomColumns;
	int i;
    if (rawData == NULL) return;
    if (zoom>0) {
      newZoomColumns = (int) (zoom*columns);
      newZoomRows = (int) (zoom*rows);
    } else if (zoom == 0) {
      int client_width;
      int client_height;
      client_width = width();
      client_height = height();
      if (drawColorBar)
	client_width -= (barWidth - tickWidth - ColorbarWidth(client_height));
      double zoomColFactor;
      zoomColFactor = ((double) client_width)/columns;
      double zoomRowFactor;
      zoomRowFactor = ((double) client_height)/rows;
      double effectiveZoom;
      effectiveZoom = (zoomColFactor < zoomRowFactor) ? 
	zoomColFactor : zoomRowFactor;
      newZoomColumns = (int) (effectiveZoom*columns);
      newZoomRows = (int) (effectiveZoom*rows);
    } else {
      int client_width;
      int client_height;
      client_width = width();
      if (drawColorBar)
	client_width -= (barWidth - tickWidth - ColorbarWidth(client_height));
      client_height = height();
      newZoomColumns = (int) (client_width);
      newZoomRows = (int) (client_height);
    }
    if (newZoomColumns == zoomColumns &&
	newZoomRows == zoomRows &&
	zoomImage != NULL &&
	!forceUpdate) return;
    zoomColumns = newZoomColumns;
    zoomRows = newZoomRows;
    if (zoomImage != NULL)
      delete[] zoomImage;
    if (zoomColumns < 1) zoomColumns = 1;
    if (zoomRows < 1) zoomRows = 1;
    if (zoomColumns > 3000) zoomColumns = 3000;
    if (zoomRows > 3000) zoomRows = 3000;
    zoomImage = new double[zoomColumns*zoomRows];
    double *tmpImage = new double[zoomRows*columns];
    // First zoom the columns
    for (i=0;i<columns;i++)
      Zoom1D(rawData + i*rows,tmpImage + i*zoomRows,
	     1,1,rows,zoomRows);
    // Then zoom the rows 
    for (i=0;i<zoomRows;i++)
      Zoom1D(tmpImage + i, zoomImage + i,
	     zoomRows, zoomRows,
	     columns, zoomColumns);
    delete[] tmpImage;
    //    SetSize(zoomColumns,zoomRows);
    //    SetSize(zoomColumns,zoomRows);
    UpdateImage();
  }

  void ScalarImage::OnResize() {
    if (zoom <= 0) 
      UpdateZoom(false);
    repaint();
  }

  int ScalarImage::getZoomColumns() {
    if (!drawColorBar)
      return zoomColumns;
    else
      return (zoomColumns + barWidth + tickWidth + ColorbarWidth(zoomRows));
  }

  int ScalarImage::getZoomRows() {
    return zoomRows;
  }

  int ScalarImage::ColorbarWidth(int zoomRows) {
    QPainter pnt(this);
    QTGC gc(pnt,1,1);
    int numTicks = zoomRows/75;
    int delTick = zoomRows/numTicks;
    int maxWidth = 0;
    for (int i=0;i<=numTicks;i++) {
      char buffer[1000];
      sprintf(buffer,"%f",level-window/2.0+i/((float)numTicks)*window);
      Point2D pt(gc.GetTextExtent(buffer));
      maxWidth = (maxWidth > pt.x) ? maxWidth : pt.x;
    }
    return maxWidth;
  }

  void ScalarImage::OnDraw(GraphicsContext &gc) {
    if (rawData == NULL) return;
    gc.BlitImage(picData, zoomColumns, zoomRows, 0, 0);
    if (drawColorBar)
      gc.BlitImage(barData, barWidth, zoomRows, zoomColumns, 0);
    numTicks = zoomRows/75;
    int delTick = zoomRows/numTicks;
    for (int i=0;i<=numTicks;i++) {
      char buffer[1000];
      int ypos = (numTicks-i)*delTick;
      sprintf(buffer,"%f",level-window/2.0+i/((float)numTicks)*window);
      if (i == 0)
	gc.DrawTextStringAligned(buffer,
				 Point2D(zoomColumns+barWidth+tickWidth+3,ypos),
				 LR_LEFT,TB_BOTTOM);
      else if (i == numTicks) 
	gc.DrawTextStringAligned(buffer,
				 Point2D(zoomColumns+barWidth+tickWidth+3,ypos),
				 LR_LEFT,TB_TOP);
      else
	gc.DrawTextStringAligned(buffer,
				 Point2D(zoomColumns+barWidth+tickWidth+3,ypos),
				 LR_LEFT,TB_CENTER);
      
    }
  }

  void ScalarImage::SetImageArray(Array &dp, double zoomf) {
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
    zoom = zoomf;
    UpdateZoom(true);
    Zoom(zoom);
  }

  void ScalarImage::WindowLevel(double awindow, double alevel) {
    window = awindow;
    level = alevel;
    UpdateImage();
  }

  double ScalarImage::GetCurrentWindow() {
    return window;
  }

  double ScalarImage::GetCurrentLevel() {
    return level;
  }

  void ScalarImage::UpdateImage() {
    if (rawData == NULL) return;
    if (picData != NULL)
      delete picData;
    picData = new uchar[zoomColumns*zoomRows*3];
    uchar *op;
    op = picData;
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
    if (drawColorBar) {
      delete barData;
      barData = new uchar[barWidth*zoomRows*3];
      uchar *op = barData;
      for (int i=0;i<barWidth;i++) {
	for (int j=0;j<zoomRows;j++) {
	  int ndx;
	  int dv;
	  dv = ((float) j)/(zoomRows-1)*255;
	  ndx = 3*(i+barWidth*(zoomRows-1-j));
	  op[ndx] = colormap[0][dv];
	  op[ndx+1] = colormap[1][dv];
	  op[ndx+2] = colormap[2][dv];
	}
      }
    }
    setMinimumSize(getZoomColumns(),getZoomRows());
  }
}
