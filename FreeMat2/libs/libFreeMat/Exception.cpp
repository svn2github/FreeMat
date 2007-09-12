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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "Exception.hpp"
#include "Malloc.hpp"
#include "Interpreter.hpp"

Exception::Exception(std::string msg_in) {
  msg = msg_in; 
  handled = false;
}

void Exception::printMe(Interpreter *eval) {
  eval->errorMessage(msg);
}

bool Exception::matches(string tst_msg) {
  return (tst_msg == msg);
}

string Exception::getMessageCopy() {
  return msg;
}

