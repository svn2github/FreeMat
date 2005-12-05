#include "HandleFigure.hpp"
#include "HandleList.hpp"
#include "HandleCommands.hpp"

#include <qgl.h>

namespace FreeMat {
  
  HandleFigure::HandleFigure() {
    ConstructProperties();
    SetupDefaults();
  }
  
  void HandleFigure::ConstructProperties() {
    AddProperty(new HPColor,"color");
    AddProperty(new HPHandles,"children");
    AddProperty(new HPHandles,"currentaxes");
    AddProperty(new HPHandles,"parent");
    AddProperty(new HPFourVector,"position");
    AddProperty(new HPString,"type");
  }

  void HandleFigure::SetupDefaults() {
    SetStringDefault("type","figure");
    SetThreeVectorDefault("color",0.6,0.6,0.6);
    resized = false;
  }

  void HandleFigure::PaintMe(RenderEngine &gc) {
    // draw the children...
    HPColor *color = (HPColor*) LookupProperty("color");
    gc.clear(color->Data());
    HPHandles *children = (HPHandles*) LookupProperty("children");
    std::vector<unsigned> handles(children->Data());
    for (int i=0;i<handles.size();i++) {
      HandleObject *fp = LookupHandleObject(handles[i]);
      fp->PaintMe(gc);
    }
    resized = false;
  }

  void HandleFigure::resizeGL(int width, int height) {
    m_width = width;
    m_height = height;
    resized = true;
  }

  bool HandleFigure::Resized() {
    return resized;
  }


}

