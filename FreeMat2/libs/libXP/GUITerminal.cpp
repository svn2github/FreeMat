#include "GUITerminal.hpp"
#include <qapplication.h>

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
