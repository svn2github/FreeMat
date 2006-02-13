/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "GUITerminal.hpp"
#include <QDebug>
#include <qapplication.h>

void GUITerminal::resizeEvent( QResizeEvent *e ) {
  QTTerm::resizeEvent(e);
  setTerminalWidth(getTextWidth());
}

GUITerminal::GUITerminal(QWidget *parent, KeyManager *sink) :
  QTTerm(parent,NULL), BaseTerminal(sink) {
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

void GUITerminal::QueueString(QString t) {
  const char *cp = t.toAscii().constData();
  while (*cp) {
    ProcessChar(*cp++);
  }
}


void GUITermianl::ProcessChar(int c) {
  
}
