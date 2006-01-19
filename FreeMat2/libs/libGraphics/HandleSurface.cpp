#include "HandleSurface.hpp"
#include "HandleAxis.hpp"
#include <qgl.h>

namespace FreeMat {
  HandleSurface::HandleSurface() {
    ConstructProperties();
    SetupDefaults();
  }

  HandleSurface::~HandleSurface() {
  }

  std::vector<double> HandleSurface::GetLimits() {
    std::vector<double> limits;
    Array xdata(ArrayPropertyLookup("xdata"));
    Array ydata(ArrayPropertyLookup("ydata"));
    Array zdata(ArrayPropertyLookup("zdata"));
    Array cdata(ArrayPropertyLookup("cdata"));
    limits.push_back(ArrayMin(xdata));
    limits.push_back(ArrayMax(xdata));
    limits.push_back(ArrayMin(ydata));
    limits.push_back(ArrayMax(ydata));
    limits.push_back(ArrayMin(zdata));
    limits.push_back(ArrayMax(zdata));
    limits.push_back(ArrayMin(cdata));
    limits.push_back(ArrayMax(cdata));
    std::vector<double> alphadata(VectorPropertyLookup("alphadata"));
    limits.push_back(VecMin(alphadata));
    limits.push_back(VecMax(alphadata));
    return limits;
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
    HandleImage::UpdateCAlphaData();
    HandleAxis* parent = GetParentAxis();
    parent->UpdateState();
  }

  /*
    0
    q1
    1
    q2
    2
  */

  std::vector<std::vector<cpoint> > HandleSurface::BuildQuadsNoTexMap(HPConstrainedStringColor* cp,
								      HPConstrainedStringScalar* ap) {
    // Get the x,y,z & color data points
    std::vector<std::vector<cpoint> > retval;
    Array xdata(ArrayPropertyLookup("xdata"));
    xdata.promoteType(FM_DOUBLE);
    Array ydata(ArrayPropertyLookup("ydata"));
    ydata.promoteType(FM_DOUBLE);
    Array zdata(ArrayPropertyLookup("zdata"));
    zdata.promoteType(FM_DOUBLE);
    if ((xdata.getLength() != zdata.getLength()) ||
	(xdata.getLength() != ydata.getLength())) return retval;
    if (zdata.isEmpty()) return retval;
    double *xdp = (double*) xdata.getDataPointer();
    double *ydp = (double*) ydata.getDataPointer();
    double *zdp = (double*) zdata.getDataPointer();
    int rows = zdata.rows();   int cols = zdata.columns();
    if (cp->Is("interp") && ((img.height() < rows) || (img.width() < cols))) return retval;
    if (ap->Is("interp") && ((img.height() < rows) || (img.width() < cols))) return retval;
    if (cp->Is("flat") && ((img.height() < rows-1) || (img.width() < cols-1))) return retval;
    if (ap->Is("flat") && ((img.height() < rows-1) || (img.width() < cols-1))) return retval;
    if (cp->Is("none")) return retval;
    QRgb *dummyline;
    if (cp->Is("colorspec") || ap->Is("scalar")) {
      dummyline = new QRgb[cols];
      double r = 0;
      double g = 0;
      double b = 0;
      double alphaval = 1.0;
      if (ap->Is("scalar"))
	alphaval = ap->Scalar();
      if (cp->Is("colorspec")) {
	std::vector<double> p(cp->ColorSpec());
	if (p[0] == -1) return retval;
	r = p[0]; g = p[1]; b = p[2];
      }
      for (int i=0;i<cols;i++)
	dummyline[i] = qRgba(255*r,255*g,255*b,255*alphaval);
    }
    for (int i=0;i<rows-1;i++) {
      QRgb *cbits1, *cbits2, *abits1, *abits2;
      int col_lim, alp_lim;
      if (cp->Is("interp")) {
	cbits1 = (QRgb*) img.scanLine(i);
	cbits2 = (QRgb*) img.scanLine(i+1);
	col_lim = cols-1;
     } else if (cp->Is("flat")) {
	cbits1 = (QRgb*) img.scanLine(i);
	cbits2 = (QRgb*) img.scanLine(i);
	col_lim = cols-2;
      } else if (cp->Is("colorspec")) {
	cbits1 = (QRgb*) dummyline;
	cbits2 = (QRgb*) dummyline;
	col_lim = cols-1;
      }
      if (ap->Is("interp")) {
	abits1 = (QRgb*) img.scanLine(i);
	abits2 = (QRgb*) img.scanLine(i+1);
	alp_lim = cols-1;
      } else if (ap->Is("flat")) {
	abits1 = (QRgb*) img.scanLine(i);
	abits2 = (QRgb*) img.scanLine(i);
	alp_lim = cols-2;
      } else if (ap->Is("scalar")) {
	abits1 = (QRgb*) dummyline;
	abits2 = (QRgb*) dummyline;
	alp_lim = cols-1;
      }
      std::vector<cpoint> linequads;
      for (int j=0;j<cols;j++) {
	int ccol = qMin(j,col_lim);
	int acol = qMin(j,alp_lim);
	linequads.push_back(cpoint(xdp[i+j*rows],ydp[i+j*rows],zdp[i+j*rows],
				   qRed(cbits1[ccol])/255.0,qGreen(cbits1[ccol])/255.0,
				   qBlue(cbits1[ccol])/255.0,qAlpha(abits1[acol])/255.0));
	linequads.push_back(cpoint(xdp[i+1+j*rows],ydp[i+1+j*rows],
				   zdp[i+1+j*rows],qRed(cbits2[ccol])/255.0,
				   qGreen(cbits2[ccol])/255.0,qBlue(cbits2[ccol])/255.0,
				   qAlpha(abits2[acol])/255.0));
      }
      retval.push_back(linequads);
    }
    qDebug("retval size is %d",retval.size());
    if (cp->Is("colorspec") || ap->Is("scalar")) 
      delete[] dummyline;
    return retval;
  }
 
  void HandleSurface::PaintMe(RenderEngine& gc) {
    UpdateState();
    // Get the x,y,z & color data points
    Array xdata(ArrayPropertyLookup("xdata"));
    xdata.promoteType(FM_DOUBLE);
    Array ydata(ArrayPropertyLookup("ydata"));
    ydata.promoteType(FM_DOUBLE);
    Array zdata(ArrayPropertyLookup("zdata"));
    zdata.promoteType(FM_DOUBLE);
    if (zdata.isEmpty()) return;
    double *xdp = (double*) xdata.getDataPointer();
    double *ydp = (double*) ydata.getDataPointer();
    double *zdp = (double*) zdata.getDataPointer();
    int rows = zdata.rows();   int cols = zdata.columns();
    // There are many render styles...
    // edgealpha, edgecolor, facealpha, facecolor
    // facecolor
    // Texture mapping not supported yet
    if (StringCheck("facecolor","texturemap")) return;
    if (StringCheck("facealpha","texturemap")) return;
    // A quadstrip is defined by its 
    std::vector<std::vector<cpoint> > surfquads(BuildQuadsNoTexMap((HPConstrainedStringColor*) 
								   LookupProperty("facecolor"),
								   (HPConstrainedStringScalar*)
								   LookupProperty("facealpha")));
    std::vector<std::vector<cpoint> > edgequads(BuildQuadsNoTexMap((HPConstrainedStringColor*) 
								   LookupProperty("edgecolor"),
								   (HPConstrainedStringScalar*)
								   LookupProperty("edgealpha")));
    gc.quadStrips(surfquads,StringCheck("facecolor","flat"),
		  edgequads,StringCheck("edgecolor","flat"));
#if 0
    HPAutoFlatColor *ec = (HPAutoFlatColor*) LookupProperty("markeredgecolor");
    HPAutoFlatColor *fc = (HPAutoFlatColor*) LookupProperty("markerfacecolor");
    std::vector<double> edgecolor;
    std::vector<double> facecolor;
    if (ec->Is("colorspec")) 
      edgecolor = ec->ColorSpec();
    else
      edgecolor.push_back(-1);
    if (fc->Is("colorspec")) 
      facecolor = fc->ColorSpec();
    else
      facecolor.push_back(-1);

    RenderEngine::SymbolType typ = StringToSymbol(StringPropertyLookup("marker"));
    double sze = ScalarPropertyLookup("markersize")/2.0;
    // Make sure there's something to draw...
    if ((typ != RenderEngine::None) || (edgecolor[0] != -1) || (facecolor[0] != -1)) {
      // Calculate the u/v coordinates (pixels)
      std::vector<double> uc;
      std::vector<double> vc;
      std::vector<double> zc;
      for (int i=0;i<rows*cols;i++) {
	double u, v;
	bool clipped;
	gc.toPixels(xdp[i],ydp[i],zdp[i],u,v,clipped);
	if (!clipped) {
	  uc.push_back(u); vc.push_back(v); zc.push_back(zdp[i]);
	}
      }
      //       gc.setupDirectDraw();
      //       gc.depth(true);
      //       for (int i=0;i<uc.size();i++) 
      // 	DrawSymbol(gc,typ,uc[i],vc[i],zc[i],sze,edgecolor,facecolor,width);
      //       for (int i=0;i<uc.size();i++) 
      // 	DrawSymbol(gc,typ,xdp[i],ydp[i],zdp[i],sze,edgecolor,facecolor,width);
      //       gc.releaseDirectDraw();
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
#endif
  }
}
