#include "FuncTerminal.hpp"
#include <qapplication.h>
#include <qeventloop.h>

namespace FreeMat {
  FuncTerminal::FuncTerminal(char *myargv[], int myargc, int funcMode) {
    sprintf(m_buffer,"%s",myargv[funcMode+1]);
    for (int i=funcMode+2;i<myargc;i++) {
      strcat(m_buffer," ");
      strcat(m_buffer,myargv[i]);
    }
    strcat(m_buffer,"\n");
    line_sent = false;
  }

  void FuncTerminal::Initialize() {
  }

  void FuncTerminal::RestoreOriginalMode() {
  }

  FuncTerminal::~FuncTerminal() {
  }
  
  void FuncTerminal::ResizeEvent() {
  }

  void FuncTerminal::MoveDown() {
    printf("\n");
    fflush(stdout);
  }

  void FuncTerminal::MoveUp() {
  }

  void FuncTerminal::MoveRight() {
  }

  void FuncTerminal::MoveLeft() {
  }

  void FuncTerminal::ClearEOL() {
  }

  void FuncTerminal::ClearEOD() {
  }

  void FuncTerminal::MoveBOL() {
    putchar('\r');
    fflush(stdout);
  }

  void FuncTerminal::OutputRawString(std::string txt) {
    printf("%s",txt.c_str());
    fflush(stdout);
  }
  
  char* FuncTerminal::getLine(std::string prompt) {
    if (!line_sent) {
      line_sent = true;
      return strdup(m_buffer);
    } else
      return strdup("quit;\n");
  }

  int FuncTerminal::getTerminalWidth() {
    return 80;
  }
}
