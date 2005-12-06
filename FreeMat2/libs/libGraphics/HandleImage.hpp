#ifndef __HandleImage_hpp__
#define __HandleImage_hpp__

#include "HandleObject.hpp"
#include <qimage.h>

namespace FreeMat {
  class HandleImage : public HandleObject {
    QImage img;
  public:
    HandleImage();
    virtual ~HandleImage();
    virtual void ConstructProperties();
    virtual void SetupDefaults();
    virtual void UpdateState();
    virtual void PaintMe(RenderEngine& gc);
  };
}

#endif
