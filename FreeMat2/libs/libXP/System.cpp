#include "System.hpp"
#include "Exception.hpp"
#include <stdlib.h>

#ifdef WIN32
#include <stdio.h> 
#include <windows.h> 
#endif

namespace FreeMat {

#ifdef WIN32

#define MAX 1024
#define BUFSIZE 4096 

  std::vector<std::string> DoSystemCallCaptured(std::string cmd) {
    std::vector<std::string> retval;
    HANDLE hChildStdinRd, hChildStdinWr, hChildStdinWrDup, 
      hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup, 
      hInputFile, hSaveStdin, hSaveStdout; 

    PROCESS_INFORMATION piProcInfo; 
    STARTUPINFO siStartInfo;
    BOOL bFuncRetn = FALSE; 
    SECURITY_ATTRIBUTES saAttr; 
    BOOL fSuccess; 
    
    // Set the bInheritHandle flag so pipe handles are inherited. 
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 
    // Save the handle to the current STDOUT. 
    hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
    // Create a pipe for the child process's STDOUT. 
    if (! CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) 
      throw Exception("Stdout pipe creation failed\n"); 
    // Set a write handle to the pipe to be STDOUT. 
    if (! SetStdHandle(STD_OUTPUT_HANDLE, hChildStdoutWr)) 
      throw Exception("Redirecting STDOUT failed"); 
    // Create noninheritable read handle and close the inheritable read 
    // handle. 
    fSuccess = DuplicateHandle(GetCurrentProcess(), hChildStdoutRd,
			       GetCurrentProcess(), &hChildStdoutRdDup , 0,
			       FALSE,
			       DUPLICATE_SAME_ACCESS);
    if( !fSuccess )
      throw Exception("DuplicateHandle failed");
    CloseHandle(hChildStdoutRd);
    // Set up members of the PROCESS_INFORMATION structure. 
    ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
    // Set up members of the STARTUPINFO structure. 
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO); 
    if (!CreateProcess(NULL, "cmd.exe /c dir", NULL, NULL, TRUE, 
		       0, NULL, NULL, &siStartInfo, &piProcInfo))
      throw Exception("Create process failed");
    // After process creation, restore the saved STDIN and STDOUT. 
    if (! SetStdHandle(STD_OUTPUT_HANDLE, hSaveStdout)) 
      throw Exception("Re-redirecting Stdout failed\n"); 
    if (!CloseHandle(hChildStdoutWr)) 
      throw Exception("Closing handle failed"); 
    DWORD dwRead, dwWritten; 
    bool moreOutput = true;
    int readSoFar = 0;
    char *output, *op;
    output = (char*) malloc(BUFSIZE);
    op = output;
    while (moreOutput) {
		BOOL bres = ReadFile( hChildStdoutRdDup, op, BUFSIZE-1, &dwRead, NULL);
      moreOutput = !(bres && (dwRead == 0));
      if (moreOutput) {
	readSoFar += dwRead;
	output = (char*) realloc(output, readSoFar+BUFSIZE);
	op = output + readSoFar;
      }
      *op = '\0';
    } 
    for (char *line=strtok(output,"\n");line;line=strtok(NULL,"\n"))
      retval.push_back(std::string(line));
    free(output);
    return retval;
  }
#else
#include <unistd.h>
#include <errno.h>

#define MAX 1024

std::vector<std::string> DoSystemCallCaptured(std::string cmd) {
  int n, fd[2];
  char *output;
  char *op;
  char *line;
  int readSoFar;
  std::vector<std::string > ret;
  
  if(pipe(fd) < 0) 
    throw FreeMat::Exception("Internal error - unable to set up pipe for system call!!");
  switch(fork()) {
  case -1:
    throw FreeMat::Exception("Internal error - unable to fork system call!!");
  case 0:                 /* child */
    close(fd[0]);
    dup2(fd[1], fileno(stdout));
    execlp("sh", "sh", "-c", cmd.c_str(), NULL);
  default:                /* parent */
    close(fd[1]);
    output = (char*) malloc(MAX);
    op = output;
    readSoFar = 0;
    bool moreOutput = true;
    while (moreOutput) {
      n = read(fd[0], op, MAX - 1);
      if ((n == 0) && (errno != EINTR))
	moreOutput = false;
      else {
	readSoFar += n;
	output = (char*) realloc(output,readSoFar+MAX);
	op = output + readSoFar;
      }
    }
    *op = '\0';
    close(fd[0]);
  }
  for (line=strtok(output,"\n");line;line=strtok(NULL,"\n"))
    ret.push_back(std::string(line));
  free(output);
  return ret;
}
#endif
}
