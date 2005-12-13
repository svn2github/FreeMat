#include "HandleSurface.hpp"
#include <qgl.h>

namespace FreeMat {
  HandleSurface::HandleSurface() {
    ConstructProperties();
    SetupDefaults();
  }

  HandleSurface::~HandleSurface() {
  }

  void HandleSurface::ConstructProperties() {
    AddProperty(new HPVector, "alphadata");
    AddProperty(new HPMappingMode, "alphadatamapping");
    AddProperty(new HPScalar,"ambientstrength");
    AddProperty(new HPBackFaceLighting,"backfacelighting");
    AddProperty(new HPArray, "cdata");
    AddProperty(new HPDataMappingMode, "cdatamapping");
    AddProperty(new HPAutoManual, "cdatamode");
    AddProperty(new HPHandles,"children");
    AddProperty(new HPScalar,"diffusestrength");
    AddProperty(new HPEdgeAlpha,"edgealpha");
    AddProperty(new HPColorInterp,"edgecolor");
    AddProperty(new HPLightingMode,"edgelighting");
    AddProperty(new HPFaceAlpha,"facealpha");
    AddProperty(new HPColorInterp,"facecolor");
    AddProperty(new HPLightingMode,"facelighting");
    AddProperty(new HPLineStyle,"linestyle");
    AddProperty(new HPScalar,"linewidth");
    AddProperty(new HPSymbol,"marker");
    AddProperty(new HPAutoFlatColor,"markeredgecolor");
    AddProperty(new HPAutoFlatColor,"markerfacecolor");
    AddProperty(new HPScalar,"markersize");
    AddProperty(new HPRowColumns,"meshstyle");
    AddProperty(new HPAutoManual,"normalmode");
    AddProperty(new HPHandles,"parent");
    AddProperty(new HPScalar,"specularcolorreflectance");
    AddProperty(new HPScalar,"specularexponent");
    AddProperty(new HPScalar,"specularstrength");
    AddProperty(new HPString,"tag");
    AddProperty(new HPString,"type");
    AddProperty(new HPArray,"userdata");
    AddProperty(new HPArray,"vertexnormals");
    AddProperty(new HPArray,"xdata");
    AddProperty(new HPAutoManual,"xdatamode");
    AddProperty(new HPArray,"ydata");
    AddProperty(new HPAutoManual,"ydatamode");
    AddProperty(new HPArray,"zdata");
    AddProperty(new HPOnOff,"visible");
  }

  void HandleSurface::SetupDefaults() {
    HPVector *hp = (HPVector*) LookupProperty("alphadata");
    std::vector<double> gp;
    gp.push_back(1.0);
    hp->Data(gp);
    SetConstrainedStringDefault("alphadatamapping","none");
    SetScalarDefault("ambientstrength",0.55);
    SetConstrainedStringDefault("backfacelighting","unlit");
    SetConstrainedStringDefault("cdatamapping","scaled");
    SetConstrainedStringDefault("cdatamode","auto");
    SetScalarDefault("diffusestrength",0.6);
    SetScalarDefault("specularcolorreflectance",0.4);
    SetScalarDefault("specularexponent",0.1);
    SetScalarDefault("specularstrength",0.5);
    SetStringDefault("type","image");
    SetConstrainedStringDefault("visible","on");
    SetConstrainedStringScalarDefault("edgealpha","scalar",1);
    SetConstrainedStringColorDefault("edgecolor","colorspec",0,0,0);
    SetConstrainedStringDefault("edgelighting","none");
    SetConstrainedStringScalarDefault("facealpha","scalar",1);
    SetConstrainedStringDefault("facecolor","flat");
    SetConstrainedStringDefault("facelighting","none");
    SetConstrainedStringDefault("linestyle","-");
    SetScalarDefault("linewidth",0.5);
    SetConstrainedStringDefault("marker","none");
    SetConstrainedStringColorDefault("markeredgecolor","auto",0,0,0);
    SetConstrainedStringColorDefault("markerfacecolor","none",0,0,0);
    SetScalarDefault("markersize",6);
    SetConstrainedStringDefault("meshstyle","both");
    SetConstrainedStringDefault("normalmode","auto");
    SetStringDefault("type","surface");
    SetStringDefault("xdatamode","auto");
    SetStringDefault("ydatamode","auto");
  }

  void HandleSurface::DoAutoXMode() {
    Array zdata(ArrayPropertyLookup("zdata"));
    Array xdata(Array::doubleMatrixConstructor(zdata.rows(),zdata.columns()));
    double *dp = (double*) xdata.getReadWriteDataPointer();
    int cols(zdata.columns());
    int rows(zdata.rows());
    for (int j=0;j<cols;j++)
      for (int i=0;i<rows;i++)
	dp[i+j*rows] = j+1;
    HPArray *hp = (HPArray*) LookupProperty("xdata");
    hp->Data(xdata);
  }

  void HandleSurface::DoAutoYMode() {
    Array zdata(ArrayPropertyLookup("zdata"));
    Array ydata(Array::doubleMatrixConstructor(zdata.rows(),zdata.columns()));
    double *dp = (double*) ydata.getReadWriteDataPointer();
    int cols(zdata.columns());
    int rows(zdata.rows());
    for (int j=0;j<cols;j++)
      for (int i=0;i<rows;i++)
	dp[i+j*rows] = i+1;
    HPArray *hp = (HPArray*) LookupProperty("ydata");
    hp->Data(ydata);
  }

  void HandleSurface::DoAutoCMode() {
    Array zdata(ArrayPropertyLookup("zdata"));
    HPArray *hp = (HPArray*) LookupProperty("cdata");
    hp->Data(zdata);
  }

  void HandleSurface::UpdateState() {
    if (HasChanged("xdata"))
      ToManual("xdatamode");
    if (HasChanged("ydata"))
      ToManual("ydatamode");
    if (HasChanged("cdata"))
      ToManual("cdatamode");
    if (IsAuto("xdatamode")) 
      DoAutoXMode();
    if (IsAuto("ydatamode"))
      DoAutoYMode();
    if (IsAuto("cdatamode"))
      DoAutoCMode();
    HandleImage::UpdateState();
  }



  void HandleSurface::PaintMe(RenderEngine& gc) {
    UpdateState();
    // Get the x,y,z & color data points
    Array xdata(ArrayPropertyLookup("xdata"));
    Array ydata(ArrayPropertyLookup("ydata"));
    Array zdata(ArrayPropertyLookup("zdata"));
    if (zdata.isEmpty()) return;
    double *xdp = (double*) xdata.getDataPointer();
    double *ydp = (double*) ydata.getDataPointer();
    double *zdp = (double*) zdata.getDataPointer();
    int rows = zdata.rows();   int cols = zdata.columns();

    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0,-1.0);

    glColor3f(0,0,0);
    glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
    glShadeModel(GL_FLAT);
    for (int i=0;i<rows-1;i++) {
      QRgb *ibits = (QRgb*) img.scanLine(i);
      glBegin(GL_QUAD_STRIP);
      for (int j=0;j<cols;j++) {
	glVertex3f(xdp[i+j*rows],ydp[i+j*rows],zdp[i+j*rows]);
	glVertex3f(xdp[i+1+j*rows],ydp[i+1+j*rows],zdp[i+1+j*rows]);
      }
      glEnd();
    }

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    glShadeModel(GL_FLAT);
    for (int i=0;i<rows-1;i++) {
      QRgb *ibits = (QRgb*) img.scanLine(i);
      glBegin(GL_QUAD_STRIP);
      for (int j=0;j<cols;j++) {
	glColor4f(qRed(ibits[j])/255.0,qGreen(ibits[j])/255.0,
		  qBlue(ibits[j])/255.0,qAlpha(ibits[j])/255.0);
	glVertex3f(xdp[i+j*rows],ydp[i+j*rows],zdp[i+j*rows]);
	glVertex3f(xdp[i+1+j*rows],ydp[i+1+j*rows],zdp[i+1+j*rows]);
      }
      glEnd();
    }

//       glColor3f(0,0,0);
//       glBegin(GL_LINE_LOOP);
//       for (int j=0;j<cols-1;j++) {
// 	glVertex3f(xdp[i+j*rows],ydp[i+j*rows],zdp[i+j*rows]);
// 	glVertex3f(xdp[i+1+j*rows],ydp[i+1+j*rows],zdp[i+1+j*rows]);
// 	glVertex3f(xdp[i+1+(j+1)*rows],ydp[i+1+(j+1)*rows],
// 		   zdp[i+1+(j+1)*rows]);
// 	glVertex3f(xdp[i+(j+1)*rows],ydp[i+(j+1)*rows],zdp[i+(j+1)*rows]);
//       }
//       glEnd();
//    }
  }
}
