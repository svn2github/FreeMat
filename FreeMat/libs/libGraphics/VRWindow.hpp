/*
 * Copyright (c) 2009 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef VRWINDOW_H
#define VRWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>
#include "Array.hpp"
#ifdef HAVE_VOLPACK
#include "volpack.h"

class VRWidget : public QWidget
{
  Q_OBJECT
public:
  VRWidget(QWidget *parent, const Array &dp, 
	   const Array& scalar_ramp, 
	   const Array &material_ramp,
	   const Array &material_prop);
  ~VRWidget();
  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void resizeEvent(QResizeEvent *event);
  void setVolume(Array dp);
  void setColorTransferFunction(Array dp);
  void setOpacityRamp(Array dp);
  void rerender();
protected:
  void paintEvent(QPaintEvent *event);
private:
  QPoint lastPos;
  float curquat[4];
  float lastquat[4];
  int W, H;
  QImage backImg;
  Array data;
  Array mat_map;
  Array color_transfer_ramp;
  Array opacity_ramp;
  Array ones;
  vpContext *contxt;
  float *shade_table;
  float *weight_table;
};

#endif

#endif
