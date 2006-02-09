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

namespace FreeMat {

  int exceptionCount = 0;

  Exception::Exception(const char*msg_in) { 
    msg = strdup(msg_in); 
    exceptionCount++;
  }

  Exception::Exception(std::string msg_in) {
    msg = strdup(msg_in.c_str()); 
    exceptionCount++;
  }

  Exception::~Exception() {
    free(msg);
  }

  Exception::Exception(const Exception& copy) {
    msg = strdup(copy.msg);
    exceptionCount++;
  }

  void Exception::operator=(const Exception &copy) {
    if (this == &copy) return;
    if (msg != NULL)
      free(msg);
    msg = strdup(copy.msg);
  }

  void Exception::printMe(Interface *io) {
    io->errorMessage(msg);
  }
  
  bool Exception::matches(const char *tst_msg) {
    return (strcmp(tst_msg,msg) == 0);
  }

  char* Exception::getMessageCopy() {
    char* rp;
    rp = (char*) malloc(strlen(msg)+1);
    strcpy(rp,msg);
    return rp;
  }
}
