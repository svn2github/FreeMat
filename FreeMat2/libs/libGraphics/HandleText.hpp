#include "HandleObject.hpp"
#include "GLLabel.hpp"
#include "HandleAxis.hpp"

namespace FreeMat {
  class HandleText : public HandleObject {
    GLLabel glab;
    HandleAxis* GetParentAxis();
  public:
    HandleText();
    virtual ~HandleText();
    virtual void ConstructProperties();
    virtual void SetupDefaults();
    virtual void UpdateState();
    virtual void paintGL();
  };
}
