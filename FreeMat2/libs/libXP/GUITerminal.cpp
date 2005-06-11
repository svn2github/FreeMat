#include "GUITerminal.hpp"
#include <qapplication.h>

void GUITerminal::MoveDown() {
  setCursor(m_cursor_x,m_cursor_y+1);
}

void GUITerminal::MoveUp() {
  setCursor(m_cursor_x,m_cursor_y-1);
}

void GUITerminal::MoveLeft() {
  setCursor(m_cursor_x-1,m_cursor_y);
}

void GUITerminal::MoveRight() {
  setCursor(m_cursor_x+1,m_cursor_y);
}

void GUITerminal::MoveBOL() {
  setCursor(0,m_cursor_y);
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
