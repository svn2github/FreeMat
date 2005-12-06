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
    AddProperty(new HPMappingMode, "alphadatamapping");
    AddProperty(new HPArray, "cdata");
    AddProperty(new HPDataMappingMode, "cdatamapping");
    AddProperty(new HPHandles,"children");
    AddProperty(new HPString,"tag");
    AddProperty(new HPString,"type");
    AddProperty(new HPTwoVector,"xdata");
    AddProperty(new HPTwoVector,"ydata");
    AddProperty(new HPArray,"userdata");
    AddProperty(new HPOnOff,"visible");
  }

  void HandleImage::SetupDefaults() {
    SetScalarDefault("alphadata",1.0);
    SetConstrainedStringDefault("alphadatamapping","none");
    SetConstrainedStringDefault("cdatamapping","direct");
    SetStringDefault("type","image");
    SetConstrainedStringDefault("visible","on");
  }

  void HandleImage::UpdateState() {
    // Calculate the QImage
    Array cdata(ArrayPropertyLookup("cdata"));
    cdata.promoteType(FM_DOUBLE);
    // Check for the indexed or non-indexed case
    if (cdata.getDimensions().getLength() == 3) {
      
    }
  }

  void HandleImage::PaintMe(RenderEngine& gc) {
  }
}
