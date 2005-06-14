#ifndef __GUITerminal_hpp__
#define __GUITerminal_hpp__

#include "TermWidget.hpp"
#include "BaseTerminal.hpp"

class GUITerminal : public TermWidget, public BaseTerminal {
public:
  GUITerminal(QWidget *parent);
  void OutputRawString(std::string txt);
  void MoveDown();
  void MoveUp();
  void MoveRight();
  void MoveLeft();
  void ClearEOL();
  void ClearEOD();
  void MoveBOL();
  int getTerminalWidth() {return getTextWidth();};
  void ProcessChar(int c) {OnChar(c);};
};

#endif
