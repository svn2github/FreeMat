#include "HandleFigure.hpp"
#include "HandleList.hpp"

#include <qgl.h>

namespace FreeMat {
  extern HandleList<HandleObject*> handleset;
  

  HandleFigure::HandleFigure() {
    ConstructProperties();
    SetupDefaults();
  }
  
  void HandleFigure::ConstructProperties() {
    AddProperty(new HPHandles,"children");
    AddProperty(new HPHandles,"parent");
    AddProperty(new HPFourVector,"position");
  }

  void HandleFigure::SetupDefaults() {
    resized = false;
  }

  void HandleFigure::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw the children...
    HPHandles *children = (HPHandles*) LookupProperty("children");
    std::vector<unsigned> handles(children->Data());
    for (int i=0;i<handles.size();i++) {
      HandleObject *fp = handleset.lookupHandle(handles[i]);
      fp->paintGL();
    }
    glFlush();
  }

  void HandleFigure::resizeGL(int width, int height) {
    m_width = width;
    m_height = height;
    resized = true;
  }

  bool HandleFigure::resized() {
    return resized;
  }

#if 0
  void HandleFigure::DrawMe(DrawEngine& gc) {
    // draw the children...
    HPHandles *children = LookupProperty("children");
    std::vector<unsigned> handles(children->Data());
    for (int i=0;i<handles.size();i++) {
      HandleObject *fp = handleset.lookupHandle(handles[i]);
      fp->DrawMe(gc);
    }
  }

  void HandleFigure::UpdateState() {
  }
#endif

}

