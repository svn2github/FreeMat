#ifndef __HandleSurface_hpp__
#define __HandleSurface_hpp__

#include "HandleImage.hpp"
#include <qimage.h>

namespace FreeMat {
  class HandleSurface : public HandleImage {
    void DoAutoXMode();
    void DoAutoYMode();
    void DoAutoCMode();
  public:
    HandleSurface();
    virtual ~HandleSurface();
    virtual void ConstructProperties();
    virtual void SetupDefaults();
    virtual void UpdateState();
    virtual void PaintMe(RenderEngine& gc);
    std::vector<double> GetLimits();
  };
}

#endif
