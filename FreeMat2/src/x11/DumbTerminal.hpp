#ifndef __DumbTerminal_hpp__
#define __DumbTerminal_hpp__

#include "Terminal.hpp"

namespace FreeMat {
  class DumbTerminal : public Terminal {
  public:
    DumbTerminal();
    ~DumbTerminal();
    virtual void Initialize();
    virtual void RestoreOriginalMode();
    virtual void OutputRawString(std::string txt); 
    virtual char* getLine(const char* prompt);
    virtual void ResizeEvent();
    virtual void MoveDown();
    virtual void MoveUp();
    virtual void MoveRight();
    virtual void MoveLeft();
    virtual void ClearEOL();
    virtual void ClearEOD();
    virtual void MoveBOL();
  };
}

#endif
