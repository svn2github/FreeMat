#include "GUITerminal.hpp"
#include <qapplication.h>

void GUITerminal::resizeEvent( QResizeEvent *e ) {
  QTTerm::resizeEvent(e);
  setTerminalWidth(getTextWidth());
}

GUITerminal::GUITerminal(QWidget *parent) :
  QTTerm(parent,NULL) {
}

void GUITerminal::MoveDown() {
  SetCursor(m_cursor_x,m_cursor_y+1);
}

void GUITerminal::MoveUp() {
  SetCursor(m_cursor_x,m_cursor_y-1);
}

void GUITerminal::MoveLeft() {
  SetCursor(m_cursor_x-1,m_cursor_y);
}

void GUITerminal::MoveRight() {
  SetCursor(m_cursor_x+1,m_cursor_y);
}

void GUITerminal::MoveBOL() {
  SetCursor(0,m_cursor_y);
}

void GUITerminal::ClearEOL() {
  for (int i=m_cursor_x;i<m_width;i++)
    m_surface[i + m_cursor_y*m_width] = tagChar(' ');
}

void GUITerminal::ClearEOD() {
  for (int i=m_cursor_x + m_cursor_y*m_width;i<m_width*m_height;i++)
    m_surface[i] = tagChar(' ');
}

void GUITerminal::OutputRawString(std::string txt) {
  PutString(txt);
}

char* GUITerminal::getLine(std::string aprompt) {
  char *ret = KeyManager::getLine(aprompt);
  emit CommandLine(QString(ret));
  return ret;
}
