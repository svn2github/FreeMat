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
#include "DumbTerminal.hpp"
#include <qapplication.h>
#include <qeventloop.h>

DumbTerminal::DumbTerminal() {
}

DumbTerminal::~DumbTerminal() {
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

void DumbTerminal::DoRead() {
  char c;
  while (read(STDIN_FILENO, &c, 1) == 1)
    emit OnChar(c);
}
