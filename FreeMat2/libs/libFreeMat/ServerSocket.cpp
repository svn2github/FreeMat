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

#include "ServerSocket.hpp"
#include "Socket.hpp"
#ifdef WIN32
#include <winsock2.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#endif
#include "Exception.hpp"
#include <stdio.h>
#include <errno.h>

namespace FreeMat {
  ServerSocket::ServerSocket(int portNum) {
    struct sockaddr_in sin;
    sin.sin_family=AF_INET;
    sin.sin_port=htons(portNum);
    sin.sin_addr.s_addr=htonl(0);
    
    if ((sockfd=socket(AF_INET,SOCK_STREAM,0)) < 0)
    throw Exception("Unable to get a TCP socket!\n");
    
    int one = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
    
    if (bind(sockfd, (sockaddr *) &sin, sizeof(sin)) < 0) 
      throw Exception("Unable to bind to requested port");
    
    if (listen(sockfd, 5) < 0)
      throw Exception("Unable to convert socket to listening mode");
  }
  
  Socket* ServerSocket::AcceptConnection() {
    struct sockaddr cliaddr;
    int clilen;
    int new_sockfd;
    
  again:
    new_sockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
    if (new_sockfd < 0) {
      perror("Error:");
      if (errno == ECONNABORTED) 
	goto again;
      else
	throw Exception("Accept error.");
    }
    return new Socket(new_sockfd);
  }

  int ServerSocket::getPortNumber() {
    struct sockaddr_in test_addr;
    int len = sizeof(test_addr);
    memset(&test_addr,0,len);
    getsockname(sockfd,(sockaddr *) &test_addr,&len);
    unsigned short portnum = ntohs(test_addr.sin_port);
    return (int) portnum;
  }
  
  ServerSocket::~ServerSocket() {
    close(sockfd);
  }
}
