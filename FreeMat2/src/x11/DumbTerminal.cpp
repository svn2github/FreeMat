#include "DumbTerminal.hpp"

namespace FreeMat {
  DumbTerminal::DumbTerminal() {
  }

  void DumbTerminal::Initialize() {
    state = 0;
  }

  void DumbTerminal::RestoreOriginalMode() {
  }

  DumbTerminal::~DumbTerminal() {
  }
  
  void DumbTerminal::ResizeEvent() {
  }

  void DumbTerminal::MoveDown() {
    printf("\n");
    fflush(stdout);
  }

  void DumbTerminal::MoveUp() {
  }

  void DumbTerminal::MoveRight() {
  }

  void DumbTerminal::MoveLeft() {
  }

  void DumbTerminal::ClearEOL() {
  }

  void DumbTerminal::ClearEOD() {
  }

  void DumbTerminal::MoveBOL() {
    putchar('\r');
    fflush(stdout);
  }

  void DumbTerminal::OutputRawString(std::string txt) {
    printf("%s",txt.c_str());
    fflush(stdout);
  }
  
  char buffer[4096];
  char* DumbTerminal::getLine(const char* prompt) {
    fflush(stdout);
    printf("%s",prompt);
    fflush(stdout);
    if (feof(stdin)) return NULL;
    fgets(buffer,sizeof(buffer),stdin);
    printf("%s",buffer);
    fflush(stdout);
    return buffer;
  }
}
