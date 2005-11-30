#ifndef __HandleLineSeries_hpp__
#define __HandleLineSeries_hpp__

#include "HandleObject.hpp"

namespace FreeMat {
  class HandleLineSeries : public HandleObject {
  public:
    HandleLineSeries();
    virtual ~HandleLineSeries();
    virtual void ConstructProperties();
    virtual void SetupDefaults();
    virtual void UpdateState();
    virtual void PaintMe(RenderEngine& gc);    
  };
}

#endif
