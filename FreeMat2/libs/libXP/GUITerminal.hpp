#ifndef __GUITerminal_hpp__
#define __GUITerminal_hpp__

#include "QTTerm.hpp"
#include "KeyManager.hpp"

class GUITerminal : public QTTerm, public KeyManager {
  Q_OBJECT

 protected:
  void resizeEvent( QResizeEvent *e );
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
