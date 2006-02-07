#ifndef __FuncTerminal_hpp__
#define __FuncTerminal_hpp__

#include "KeyManager.hpp"

namespace FreeMat {
  class FuncTerminal : public KeyManager {
    char m_buffer[4096];
    bool line_sent;
  public:
    FuncTerminal(char *argv[], int argc, int funcMode);
    virtual ~FuncTerminal();
    virtual void Initialize();
    virtual void RestoreOriginalMode();
    virtual void OutputRawString(std::string txt); 
    virtual char* getLine(std::string prompt);
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
