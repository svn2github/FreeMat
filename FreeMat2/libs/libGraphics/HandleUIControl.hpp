#ifndef __HandleUIControl_hpp__
#define __HandleUIControl_hpp__

#include "HandleObject.hpp"

namespace FreeMat {
  class HandleUIControl : public HandleObject {
  public:
    HandleUIControl();
    virtual ~HandleUIControl();
    virtual void ConstructProperties();
    virtual void SetupDefaults();
  };
}

#endif
