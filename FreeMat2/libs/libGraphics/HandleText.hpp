#include "HandleObject.hpp"

namespace FreeMat {
  class HandleText : public HandleObject {
    HandleText();
    virtual ~HandleText();
    virtual void ConstructProperties();
    virtual void UpdateState();
    virtual void paintGL();
  };
}
