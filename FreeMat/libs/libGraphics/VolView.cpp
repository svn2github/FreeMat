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

#include "VolView.hpp"
#include "GraphicsServer.hpp"
#include <math.h>
#include <iostream>
#include "Malloc.hpp"
#include <wx/image.h>

int rampx[] =    {  0,  100, 255};
float rampy[] =  {0.0, .2, 1.0};
int grampx[] =   {  0,   5,  20, 221};
float grampy[] = {0.0, 0.0, 1.0, 1.0};

namespace FreeMat {

IMPLEMENT_DYNAMIC_CLASS(VolView, wxFrame)

BEGIN_EVENT_TABLE(VolView, wxFrame)
  EVT_PAINT(VolView::OnPaint)
  EVT_CLOSE(VolView::OnClose)
  EVT_SIZE(VolView::OnSize)
  EVT_MOUSE_EVENTS(VolView::OnMouse)
END_EVENT_TABLE()

  VolView::VolView(GraphicsServer *server, int fignum) : 
    PrintableFrame(_T("Volume Render Window"), wxDefaultSize) {
  SetBackgroundColour( *wxBLACK);
  m_server = server;
  rawData = NULL;
  volume = NULL;
  myFigureNumber = fignum;
  wxString title;
  title.Printf("Volume Render Window %d",fignum+1);
  SetTitle(title);  
  img = NULL;
}
  
  VolView::~VolView() {
    if (rawData) free(rawData);
    if (img) delete img;
    if (volume) free(volume);
  }
  
  void VolView::initializeVPC(char* densityData, int xlen, int ylen, int zlen) {
    Voxel *dummy_voxel;
    int size;
    
    vpc = vpCreateContext();
    vpSetVolumeSize(vpc, xlen, ylen, zlen);
    vpSetVoxelSize(vpc, sizeof(Voxel), NUM_FIELDS, NUM_SHADE_FIELDS,
		   NUM_CLASSIFY_FIELDS);
    vpSetVoxelField(vpc, NORM_FIELD, sizeof(dummy_voxel->normal),
		    vpFieldOffset(dummy_voxel, normal), NORM_MAX);
    vpSetVoxelField(vpc, SCALAR_FIELD, sizeof(dummy_voxel->scalar),
		    vpFieldOffset(dummy_voxel, scalar), SCALAR_MAX);
    vpSetVoxelField(vpc, GRAD_FIELD, sizeof(dummy_voxel->gradient),
		    vpFieldOffset(dummy_voxel, gradient), GRAD_MAX);
    size = xlen*ylen*zlen*sizeof(Voxel);
    volume = (Voxel*) malloc(size);
    vpSetRawVoxels(vpc,volume, size, sizeof(Voxel),
		   xlen * sizeof(Voxel), ylen * xlen * sizeof(Voxel));
    vpVolumeNormals(vpc, (unsigned char*) densityData, xlen*ylen*zlen,
		    SCALAR_FIELD, GRAD_FIELD, NORM_FIELD);
  }
  
  void VolView::SetVolume(Array &dp) {
    initializeVPC((char*) dp.getDataPointer(),dp.getDimensionLength(0),
		  dp.getDimensionLength(1),dp.getDimensionLength(2));
    classifyVolume(rampx, rampy, 3, grampx, grampy, 4);
    setupShader();
    trackball(quat, 0.0, 0.0, 0.0, 0.0);
    Resize();
    Refresh(FALSE);
  }

  void VolView::destroyVPC() {
    free(volume);
    vpDestroyContext(vpc);
  }
  
  void VolView::classifyVolume(int* drampx, float *drampy, int dramp_cnt,
			       int* grampx, float *grampy, int gramp_cnt) {
    vpRamp(density_ramp,sizeof(float),dramp_cnt,drampx,drampy);
    vpSetClassifierTable(vpc,DENSITY_PARAM,SCALAR_FIELD,density_ramp,
			 sizeof(density_ramp));
    vpRamp(gradient_ramp,sizeof(float),gramp_cnt,grampx,grampy);
    vpSetClassifierTable(vpc,GRADIENT_PARAM,GRAD_FIELD,gradient_ramp,
			 sizeof(gradient_ramp));
    vpSetd(vpc, VP_MIN_VOXEL_OPACITY, 0.05);
    vpSetd(vpc, VP_MAX_RAY_OPACITY, 0.95);
    vpClassifyVolume(vpc);
  }

  void VolView::setupShader() {
    vpSetLookupShader(vpc, 1, 1, NORM_FIELD, shade_table,
		      sizeof(shade_table), 0, NULL, 0);
    vpSetMaterial(vpc, VP_MATERIAL0, VP_AMBIENT, VP_BOTH_SIDES,
		  0.18, 0.18, 0.18);
    vpSetMaterial(vpc, VP_MATERIAL0, VP_DIFFUSE, VP_BOTH_SIDES,
		  0.35, 0.35, 0.35);
    vpSetMaterial(vpc, VP_MATERIAL0, VP_SPECULAR, VP_BOTH_SIDES,
		  0.39, 0.39, 0.39);
    vpSetMaterial(vpc, VP_MATERIAL0, VP_SHINYNESS, VP_BOTH_SIDES,10.0,0.0,0.0);
    vpSetLight(vpc, VP_LIGHT0, VP_DIRECTION, 0.3, 0.3, 1.0);
    vpSetLight(vpc, VP_LIGHT0, VP_COLOR, 1.0, 1.0, 1.0);
    vpEnable(vpc, VP_LIGHT0, 1);
    vpSetDepthCueing(vpc, 1.4, 1.5);
    vpEnable(vpc, VP_DEPTH_CUE, 1);
  }

  void VolView::setViewpoint(float matrix[4][4]) {
    vpMatrix4 viewmat;
    int i, j;
    
    vpCurrentMatrix(vpc, VP_PROJECT);
    vpIdentityMatrix(vpc);
    vpWindow(vpc, VP_PARALLEL, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5);
    vpCurrentMatrix(vpc, VP_MODEL);
    for (i=0;i<4;i++)
      for (j=0;j<4;j++)
	viewmat[i][j] = matrix[j][i];
    vpSetMatrix(vpc,viewmat);
  }

  void VolView::UpdateRendering() {
    float m[4][4];
  
    build_rotmatrix(m, quat);
    setViewpoint(m);
    vpShadeTable(vpc);
    vpRenderClassifiedVolume(vpc);
    unsigned char *imgData;
    imgData = img->GetData();
    for (int i=0;i<client_width*client_height;i++) {
      imgData[3*i] = rawData[i];
      imgData[3*i+1] = rawData[i];
      imgData[3*i+2] = rawData[i];
    }
  }

  void VolView::OnClose(wxCloseEvent &event) {
    m_server->NotifyVolumeClose(myFigureNumber);
    Destroy();
  }

  void VolView::Resize() {
    if (volume == NULL) return;
    GetClientSize(&client_width,&client_height);
    if (img != NULL)
      delete img;
    if (rawData != NULL)
      free(rawData);
    img = new wxImage(client_width,client_height);
    rawData = (unsigned char* )malloc(client_width*client_height);
    vpSetImage(vpc, (unsigned char*) rawData, client_width, client_height,
	       client_width, VP_LUMINANCE);
    UpdateRendering();
  }

  void VolView::OnSize(wxSizeEvent &event) {
    Resize();
  }

  void VolView::OnMouse(wxMouseEvent &event) {
    wxSize sz(GetClientSize());
    if (event.Dragging())
      {
	float spin_quat[4];
	trackball(spin_quat,
		  -(2.0*beginx -       sz.x) / sz.x,
		(     sz.y - 2.0*beginy) / sz.y,
		  -(     2.0*event.GetX() - sz.x) / sz.x,
		  (    sz.y - 2.0*event.GetY()) / sz.y);
	
	add_quats( spin_quat, quat, quat );
	dirty = true;
	Refresh(FALSE);
      }
    beginx = event.GetX();
    beginy = event.GetY();
  }

  void VolView::DrawMe(wxDC &dc) {
    if (rawData == NULL || volume == NULL) return;
    if (dirty) {
      UpdateRendering();
      dirty = false;
    }
    wxMemoryDC temp_dc;
    wxBitmap gmp(*img);
    temp_dc.SelectObject(gmp); 
    dc.Blit(0,0,client_width,client_height,&temp_dc,0,0);
  }


  void VolView::OnPaint(wxPaintEvent &WXUNUSED(event)) {
    if (img == NULL) return;
    wxPaintDC dc( this );
    PrepareDC( dc );
    DrawMe(dc);
  }
}
