// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "Socket.hpp"
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "Exception.hpp"
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>

namespace FreeMat {

  Socket::Socket(int fd) {
    sockfd = fd;
  }
  
  Socket::Socket(char *address, int portNum) {
    struct sockaddr_in sin;
    // Get a socket
    if ((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
      throw Exception("Panic: Unable to get a TCP socket!\n");
    
    // Convert the address to a hostent structure
    struct hostent *host;
    if ((host = gethostbyname(address)) == NULL)
      throw Exception("Requested address not found!\n");
    
    // Zero out the socket address
    memset((char*) &sin, 0, sizeof(sin));
    memcpy((char*) &sin.sin_addr, host->h_addr, host->h_length);
    sin.sin_family = host->h_addrtype;
    sin.sin_port = htons(portNum);
  
    if (connect(sockfd, (struct sockaddr *)&sin, sizeof(sin)) == -1)
      throw Exception("Unable to connect socket to specified address");
  }

  Socket::~Socket() {
    close(sockfd);
  }

  void Socket::writeBytes(const void* data, int len) {
    int bytesOut = 0;
    int written;
    while (bytesOut < len) {
      written = send(sockfd, ((char*) data)+bytesOut, len-bytesOut, 0);
      if (written == -1)
	throw Exception("Unable to write to socket - connection error");
      if (written == 0)
	throw Exception("Unable to write to socket - connection closed");
      bytesOut += written;
    }
  }

  void Socket::readBytes(void* data, int len) {
    int bytesIn = 0;
    int readIn;
    while (bytesIn < len) {
      readIn = recv(sockfd, ((char*)data)+bytesIn, len-bytesIn, 0);
      if (readIn == -1)
	throw Exception("Unable to read from socket - connection error");
      if (readIn == 0)
	throw Exception("Unable to read from socket - connection closed");
      bytesIn += readIn;
    }
  }
}
