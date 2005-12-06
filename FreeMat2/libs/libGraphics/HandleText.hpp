#ifndef __HandleText_hpp__
#define __HandleText_hpp__

#include "HandleObject.hpp"
#include "GLLabel.hpp"

namespace FreeMat {
  class HandleText : public HandleObject {
    QFont fnt;
    std::string text;
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

#endif
