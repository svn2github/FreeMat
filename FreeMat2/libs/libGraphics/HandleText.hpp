#include "HandleObject.hpp"
#include "GLLabel.hpp"
#include "HandleAxis.hpp"

namespace FreeMat {
  class HandleText : public HandleObject {
    QFont fnt;
    std::string text;
    HandleAxis* GetParentAxis();
  public:
    HandleText();
    virtual ~HandleText();
    virtual void ConstructProperties();
    virtual void SetupDefaults();
    virtual void UpdateState();
    virtual void PaintMe(RenderEngine& gc);
    int GetTextHeightInPixels();
  };
}
