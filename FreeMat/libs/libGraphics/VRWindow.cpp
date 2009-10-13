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
#include "VRWindow.hpp"

#ifdef HAVE_VOLPACK
#include "trackball.h"
#include <QtGui>

typedef unsigned char Scalar;
typedef unsigned short Normal;
typedef unsigned char Gradient;
typedef struct {
  Normal normal;
  Scalar scalar;
  Gradient gradient;
} Voxel;

const int NUM_FIELDS = 3;
const int NUM_SHADE_FIELDS = 2;
const int NUM_CLASSIFY_FIELDS = 2;
const int NORM_FIELD = 0;
const int NORM_MAX = VP_NORM_MAX;
const int SCALAR_FIELD = 1;
const int SCALAR_MAX = 255;
const int GRAD_FIELD = 2;
const int GRAD_MAX = VP_GRAD_MAX;
const int COLOR_CHANNELS = 3;
#define GRADIENT_PARAM		1


#define GRADIENT_RAMP_POINTS 4
int GradientRampX[] =   {  0,   5,  20, 221};
float GradientRampY[] = {0.0, 0.0, 1.0, 1.0};


float gradient_ramp[GRAD_MAX+1];

VRWidget::VRWidget(QWidget *parent, const Array &dp,
		   const Array &scalar_ramp, 
		   const Array &material_map,
		   const Array &material_prop)
  : QWidget(parent) {
  Voxel *dummy_voxel;
  contxt = vpCreateContext();
  int rows = dp.rows();
  int cols = dp.cols();
  int slices = dp.dimensions()[2];
  int vcount = rows*cols*slices;
  data = dp.asDenseArray().toClass(UInt8);
  opacity_ramp = scalar_ramp.asDenseArray().toClass(Float);
  if (vpSetVolumeSize(contxt,rows,cols,slices) != VP_OK) 
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpSetVoxelSize(contxt,sizeof(Voxel),NUM_FIELDS,NUM_SHADE_FIELDS,
		     NUM_CLASSIFY_FIELDS) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    
  if (vpSetVoxelField(contxt,NORM_FIELD,sizeof(dummy_voxel->normal),
		      vpFieldOffset(dummy_voxel, normal), NORM_MAX) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    
  if (vpSetVoxelField(contxt,SCALAR_FIELD,sizeof(dummy_voxel->scalar),
		      vpFieldOffset(dummy_voxel, scalar), SCALAR_MAX) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    
  if (vpSetVoxelField(contxt,GRAD_FIELD,sizeof(dummy_voxel->gradient),
		      vpFieldOffset(dummy_voxel, gradient), GRAD_MAX) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  BasicArray<uint8> &cp(data.real<uint8>());
  int size = vcount * sizeof(Voxel);
  Voxel *vxl = new Voxel[vcount];
  for (int i=0;i<vcount;i++)
    vxl[i].scalar = cp[index_t(i+1)];
  if (vpSetRawVoxels(contxt,(char*) vxl,size,sizeof(Voxel),
		     rows*sizeof(Voxel), rows*cols*sizeof(Voxel)) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpVolumeNormals(contxt,cp.data(),rows*cols*slices,
		      SCALAR_FIELD,GRAD_FIELD,NORM_FIELD) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpSetClassifierTable(contxt,0,SCALAR_FIELD,
			   (float*)(opacity_ramp.getVoidPointer()),
			   opacity_ramp.length()*sizeof(float)) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  vpRamp(gradient_ramp, sizeof(float), GRADIENT_RAMP_POINTS,
	 GradientRampX, GradientRampY);
  vpSetClassifierTable(contxt, GRADIENT_PARAM, GRAD_FIELD,
		       gradient_ramp, sizeof(gradient_ramp));
  if (vpSetd(contxt, VP_MIN_VOXEL_OPACITY, 0.05) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpClassifyVolume(contxt) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  BasicArray<float> ones_a(NTuple(SCALAR_MAX,1));
  ones_a.fill(1);
  ones = Array(ones_a);
  int material_count = material_map.cols();
  if (material_map.rows() != scalar_ramp.length())
    throw Exception("Material map should have the same number of rows as the opacity transfer function has elements");
  weight_table = new float[(SCALAR_MAX+1)*material_count];
  shade_table = new float[COLOR_CHANNELS*(NORM_MAX+1)*material_count];
  if (vpSetLookupShader(contxt,COLOR_CHANNELS,material_count,NORM_FIELD,
			shade_table,
			COLOR_CHANNELS*material_count*(NORM_MAX+1)*sizeof(float),
			SCALAR_FIELD,weight_table,
			material_count*(SCALAR_MAX+1)*sizeof(float)) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  mat_map = material_prop;
  mat_map = mat_map.toClass(Float);
  const BasicArray<float> &mprop(mat_map.constReal<float>());
  for (index_t i=1;i<=mat_map.rows();i++) 
    for (index_t j=1;j<=mat_map.cols();j++) 
      weight_table[int((i-1)*material_count+(j-1))] = mprop[NTuple(i,j)];
  if ((material_prop.rows() != material_map.cols()) || 
      (material_prop.cols() != 10))
    throw Exception("Expect material property matrix to be N x 10, where N is the number of columns in the material map matrix");
  Array tmp(material_prop);
  tmp = tmp.toClass(Float);
  const BasicArray<float> &tmpa(tmp.constReal<float>());
  for (index_t i=1;i<=material_prop.rows();i++) {
    if (vpSetMaterial(contxt, VP_MATERIAL0+i-1,VP_AMBIENT,VP_BOTH_SIDES,
		      tmpa[NTuple(i,1)],tmpa[NTuple(i,2)],tmpa[NTuple(i,3)]) != VP_OK)
      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    
    if (vpSetMaterial(contxt, VP_MATERIAL0+i-1,VP_DIFFUSE,VP_BOTH_SIDES,
		      tmpa[NTuple(i,4)],tmpa[NTuple(i,5)],tmpa[NTuple(i,6)]) != VP_OK)
      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    
    if (vpSetMaterial(contxt, VP_MATERIAL0+i-1,VP_SPECULAR,VP_BOTH_SIDES,
		      tmpa[NTuple(i,7)],tmpa[NTuple(i,8)],tmpa[NTuple(i,9)]) != VP_OK)
      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    
    if (vpSetMaterial(contxt, VP_MATERIAL0+i-1,VP_SHINYNESS,VP_BOTH_SIDES,
		      tmpa[NTuple(i,10)],0,0) != VP_OK)
      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    
  }
  if (vpSetLight(contxt, VP_LIGHT0, VP_DIRECTION, 0.3, 0.3, 1.0) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpSetLight(contxt, VP_LIGHT0, VP_COLOR, 1.0, 1.0, 1.0) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpEnable(contxt, VP_LIGHT0, 1) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  trackball(curquat, 0, 0, 0, 0);
  W = width(); H = height();
}

VRWidget::~VRWidget() {
  vpDestroyContext(contxt);
}

QSize VRWidget::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize VRWidget::sizeHint() const {
  return QSize(400, 400);
}

void VRWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  backImg = QImage(qMax(8,width()),qMax(8,height()),QImage::Format_ARGB32);
  if (vpSetImage(contxt,backImg.bits(),backImg.width(),backImg.height(),
		 backImg.bytesPerLine(),VP_ABGR) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  W = width(); H = height();
  rerender();
}

void VRWidget::mousePressEvent(QMouseEvent *event) {
  lastPos = event->pos();
}

void VRWidget::paintEvent(QPaintEvent *event) {
  rerender();
  QPainter painter(this);
  painter.drawImage(rect(),backImg);
}

void VRWidget::mouseMoveEvent(QMouseEvent *event) {
  trackball(lastquat,
	    (2.0*lastPos.x() - W)/W,
	    (2.0*lastPos.y() - H)/H,
	    (2.0*event->x() - W)/W,
	    (2.0*event->y() - H)/H);
  
  add_quats(lastquat, curquat, curquat);
  update();
  lastPos = event->pos();
}

void VRWidget::rerender() {
  if (vpCurrentMatrix(contxt, VP_MODEL) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  vpMatrix4 m;
  build_rotmatrix(m,curquat);
  vpMatrix4 mtrans;
  for (int i=0;i<4;i++)
    for (int k=0;k<4;k++)
      mtrans[i][k] = m[k][i];
  vpSetMatrix(contxt, mtrans);
  
  if (vpCurrentMatrix(contxt, VP_PROJECT) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpIdentityMatrix(contxt) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpWindow(contxt, VP_PARALLEL, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpShadeTable(contxt) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpRenderClassifiedVolume(contxt) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
}

//@Module VOLVIEW Volume View Function
ArrayVector VolViewFunction(int nargout, const ArrayVector& arg) {
  VRWidget *wid = new VRWidget(NULL,arg[0],arg[1],arg[2],arg[3]);
  wid->setWindowTitle("Volume Viewer");
  wid->show();
  return ArrayVector();
}
#else

ArrayVector VolViewFunction(int nargout, const ArrayVector& arg) {
  throw Exception("FreeMat was built without volpack (and thus software volume rendering) support.");
  return ArrayVector();
}

#endif
