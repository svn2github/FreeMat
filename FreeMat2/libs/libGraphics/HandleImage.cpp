#include "HandleImage.hpp"
#include "HandleAxis.hpp"
#include <QMatrix>

namespace FreeMat {
  HandleImage::HandleImage() {
    ConstructProperties();
    SetupDefaults();
  }

  HandleImage::~HandleImage() {
  }
  
  std::vector<double> HandleImage::GetLimits() {
    HPTwoVector *xp = (HPTwoVector *) LookupProperty("xdata");
    HPTwoVector *yp = (HPTwoVector *) LookupProperty("ydata");

    std::vector<double> limits;
    limits.push_back(xp->Data()[0]);
    limits.push_back(xp->Data()[1]);
    limits.push_back(yp->Data()[0]);
    limits.push_back(yp->Data()[1]);
    limits.push_back(0);
    limits.push_back(0);
    // The clim limit is just the min and max values of cdata
    Array cdata(ArrayPropertyLookup("cdata"));
    cdata.promoteType(FM_DOUBLE);
    limits.push_back(ArrayMin(cdata));
    limits.push_back(ArrayMax(cdata));
    std::vector<double> alphadata(VectorPropertyLookup("alphadata"));
    limits.push_back(VecMin(alphadata));
    limits.push_back(VecMax(alphadata));
    return limits;
  }


  void HandleImage::ConstructProperties() {
    AddProperty(new HPVector, "alphadata");
    AddProperty(new HPArray, "cdata");
    AddProperty(new HPMappingMode, "alphadatamapping");
    AddProperty(new HPDataMappingMode, "cdatamapping");
    AddProperty(new HPHandles,"children");
    AddProperty(new HPHandles,"parent");
    AddProperty(new HPString,"tag");
    AddProperty(new HPString,"type");
    AddProperty(new HPTwoVector,"xdata");
    AddProperty(new HPTwoVector,"ydata");
    AddProperty(new HPArray,"userdata");
    AddProperty(new HPOnOff,"visible");
  }

  void HandleImage::SetupDefaults() {
    HPVector *hp = (HPVector*) LookupProperty("alphadata");
    std::vector<double> gp;
    gp.push_back(1.0);
    hp->Data(gp);
    SetConstrainedStringDefault("alphadatamapping","none");
    SetConstrainedStringDefault("cdatamapping","scaled");
    SetStringDefault("type","image");
    SetTwoVectorDefault("xdata",0,1);
    SetTwoVectorDefault("ydata",0,1);
    SetConstrainedStringDefault("visible","on");
  }


  // Expand the current image using
  // colormap
  // cdatamapping
  // clim
  //
  //  If cdatamapping == direct, outputRGB = colormap[(int)(dp[i]-1)]
  //  If cdatamapping == scaled, outputRGB = colormap[rescale(dp[i])]
  //    where rescale(x) = (x-min(clim))/(max(clim)-min(clim))*colormap_count
  //
  double* HandleImage::RGBExpandImage(const double *dp, 
				      int rows, int cols) {
    // Allocate an output array of the right size
    double *ret = new double[rows*cols*3];
    // Retrieve the colormap
    std::vector<double> cmap(((HandleObject*)GetParentFigure())->VectorPropertyLookup("colormap"));
    HandleAxis* ap(GetParentAxis());
    std::vector<double> clim(((HandleObject*)ap)->VectorPropertyLookup("clim"));
    double clim_min(qMin(clim[0],clim[1]));
    double clim_max(qMax(clim[0],clim[1]));
     // Calculate the colormap length
    int cmaplen(cmap.size()/3);
    if (StringCheck("cdatamapping","direct")) {
      for (int i=0;i<rows*cols;i++) {
	int ndx;
	ndx = (int) dp[i] - 1;
	ndx = qMin(cmaplen-1,qMax(0,ndx));
	ret[i] = cmap[3*ndx];
	ret[i+rows*cols] = cmap[3*ndx+1];
	ret[i+2*rows*cols] = cmap[3*ndx+2];
      }
    } else {
      for (int i=0;i<rows*cols;i++) {
	int ndx;
	ndx = (int) ((dp[i]-clim_min)/(clim_max-clim_min)*(cmaplen-1));
	ndx = qMin(cmaplen-1,qMax(0,ndx));
	ret[i] = cmap[3*ndx];
	ret[i+rows*cols] = cmap[3*ndx+1];
	ret[i+2*rows*cols] = cmap[3*ndx+2];
      }      
    }
    return ret;
  }
				    

  void HandleImage::PrepImageRGBNoAlphaMap(const double *dp,
					   int rows, int cols,
					   std::vector<double> &alpha) {
    img = QImage(cols,rows,QImage::Format_ARGB32);
    for (int i=0;i<rows;i++) {
      QRgb *ibits = (QRgb*) img.scanLine(i);
      for (int j=0;j<cols;j++)
 	ibits[j] = qRgba(255*dp[(i+j*rows)],255*dp[(i+j*rows)+rows*cols],
 			 255*dp[(i+j*rows)+2*rows*cols],
			 255*alpha[i+j*rows]);
    }
  }

  std::vector<double> HandleImage::GetAlphaMap(int rows, int cols) {
    HPVector *hp = (HPVector*) LookupProperty("alphadata");
    std::vector<double> alphain(hp->Data());
    std::vector<double> alphaout;
    // Retrieve the alphamap
    std::vector<double> amap(((HandleObject*)GetParentFigure())->VectorPropertyLookup("alphamap"));
    int amaplen(amap.size());
    HandleAxis* ap(GetParentAxis());
    std::vector<double> alim(((HandleObject*)ap)->VectorPropertyLookup("alim"));
    double alim_min(qMin(alim[0],alim[1]));
    double alim_max(qMax(alim[0],alim[1]));
    int increment;
    if (alphain.size() == 0) {
      for (int i=0;i<rows*cols;i++)
	alphaout.push_back(1);
      return alphaout;
    } else if (alphain.size() != rows*cols) {
      increment = 0;
    } else
      increment = 1;
    if (StringCheck("alphadatamapping","none")) {
      for (int i=0;i<rows*cols;i++)
	alphaout.push_back(qMin(1.0,qMax(0.0,alphain[i*increment])));
    } else if (StringCheck("alphadatamapping","direct")) {
      for (int i=0;i<rows*cols;i++) {
	int ndx = (int) alphain[i*increment] -1;
	ndx = qMin(amaplen-1,qMax(0,ndx));
	alphaout.push_back(amap[ndx]);
      }
    } else {
      for (int i=0;i<rows*cols;i++) {
	int ndx = (int) alphain[i*increment] -1;
	ndx = (int) ((alphain[i*increment]-alim_min)/(alim_max-alim_min)*(amaplen-1));
	ndx = qMin(amaplen-1,qMax(0,ndx));
	alphaout.push_back(amap[ndx]);
      }
    }
    return alphaout;
  }

  void HandleImage::UpdateCAlphaData() {
    // Calculate the QImage
    Array cdata(ArrayPropertyLookup("cdata"));
    cdata.promoteType(FM_DOUBLE);
    // Retrieve alpha map
    std::vector<double> alphas(GetAlphaMap(cdata.getDimensionLength(0),
					   cdata.getDimensionLength(1)));
    // Check for the indexed or non-indexed case
    if ((cdata.getDimensions().getLength() == 3) &&
	(cdata.getDimensionLength(2) == 3)) {
	PrepImageRGBNoAlphaMap((const double*)cdata.getDataPointer(),
			       cdata.getDimensionLength(0),
			       cdata.getDimensionLength(1),
			       alphas);
    } else if (cdata.getDimensions().getLength() == 2) {
      double *dp = RGBExpandImage((const double*)cdata.getDataPointer(),
				  cdata.getDimensionLength(0),
				  cdata.getDimensionLength(1));
	PrepImageRGBNoAlphaMap(dp,
			       cdata.getDimensionLength(0),
			       cdata.getDimensionLength(1),
			       alphas);
      delete[] dp;
    }
  }

  void HandleImage::UpdateState() {
    UpdateCAlphaData();
    Array cdata(ArrayPropertyLookup("cdata"));
    HPTwoVector *xp = (HPTwoVector *) LookupProperty("xdata");
    if (xp->Data().empty())
      SetTwoVectorDefault("xdata",1,cdata.getDimensionLength(1));
    HPTwoVector *yp = (HPTwoVector *) LookupProperty("ydata");
    if (yp->Data().empty())
      SetTwoVectorDefault("ydata",1,cdata.getDimensionLength(0));
    // Need to check reverse flags for x and y axis... and flip the image appropriately
    HandleAxis *ax = GetParentAxis();
    bool xflip = false;
    bool yflip = false;
    xflip = (ax->StringCheck("xdir","reverse"));
    yflip = (ax->StringCheck("ydir","reverse"));
    if (xflip || yflip) {
      double m11, m22;
      if (xflip)
	m11 = -1;
      else
	m11 = 1;
      if (yflip)
	m22 = -1;
      else
	m22 = 1;
      QMatrix m(m11,0,0,m22,0,0);
      img = img.transformed(m);
    }
  }

  void HandleImage::PaintMe(RenderEngine& gc) {
    UpdateState();
    HPTwoVector *xp = (HPTwoVector *) LookupProperty("xdata");
    HPTwoVector *yp = (HPTwoVector *) LookupProperty("ydata");
    // Rescale the image
    int x1, y1, x2, y2;
    gc.toPixels(xp->Data()[0],yp->Data()[0],0,x1,y1);
    gc.toPixels(xp->Data()[1],yp->Data()[1],0,x2,y2);
    if ((abs(x2-x1)> 4096) || (abs(y2-y1) > 4096)) return;
    img = img.scaled(abs(x2-x1),abs(y2-y1));
    gc.drawImage(xp->Data()[0],yp->Data()[0],xp->Data()[1],yp->Data()[1],img);
  }
}
