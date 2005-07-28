#ifndef __DumbTerminal_hpp__
#define __DumbTerminal_hpp__

#include "KeyManager.hpp"

namespace FreeMat {
  class DumbTerminal : public KeyManager {
  public:
    DumbTerminal();
    virtual ~DumbTerminal();
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
    virtual int getTerminalWidth();
  };
}

#endif
