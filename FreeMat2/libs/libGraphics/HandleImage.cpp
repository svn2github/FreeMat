#include "HandleImage.hpp"

namespace FreeMat {
  HandleImage::HandleImage() {
    ConstructProperties();
    SetupDefaults();
  }

  HandleImage::~HandleImage() {
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
    SetConstrainedStringDefault("cdatamapping","direct");
    SetStringDefault("type","image");
    SetConstrainedStringDefault("visible","on");
  }

  void HandleImage::PrepImageRGBNoAlphaMap(const double *dp,
					   int rows, int cols,
					   std::vector<double> alpha) {
    int increment;
    if (alpha.size() != rows*cols)
      increment = 0;
    else
      increment = 1;
    img = QImage(cols,rows,QImage::Format_ARGB32);
    for (int i=0;i<rows;i++) {
      QRgb *ibits = (QRgb*) img.scanLine(i);
      for (int j=0;j<cols;j++)
// 	ibits[j] = qRgba(255*dp[3*(i+j*rows)],255*dp[3*(i+j*rows)+1],
// 			 255*dp[3*(i+j*rows)+2],255*alpha[(i+j*rows)*increment]);
// 	ibits[j] = qRgba(255*dp[3*(i+j*rows)],255*dp[3*(i+j*rows)+1],
// 			 255*dp[3*(i+j*rows)+2],128);
	ibits[j] = qRgba(i,j,(i+j)%256,128);
    }
  }

  void HandleImage::UpdateState() {
    // Calculate the QImage
    Array cdata(ArrayPropertyLookup("cdata"));
    cdata.promoteType(FM_DOUBLE);
    HPVector *hp = (HPVector*) LookupProperty("alphadata");
    // Check for the indexed or non-indexed case
    if ((cdata.getDimensions().getLength() == 3) &&
	(cdata.getDimensionLength(2) == 3)) {
      if (StringCheck("alphadatamapping","none"))
	PrepImageRGBNoAlphaMap((const double*)cdata.getDataPointer(),
			       cdata.getDimensionLength(0),
			       cdata.getDimensionLength(1),
			       hp->Data());
    }
  }

  void HandleImage::PaintMe(RenderEngine& gc) {
    UpdateState();
    HPTwoVector *xp = (HPTwoVector *) LookupProperty("xdata");
    HPTwoVector *yp = (HPTwoVector *) LookupProperty("ydata");
    gc.drawImage(xp->Data()[0],yp->Data()[0],xp->Data()[1],yp->Data()[1],img);
  }
}
