#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define MAX 1024
#define ERROR(x) {fprintf(stderr,"%s",x); exit(1); }

char*  DoSystemCallCaptured(char * cmd) {
  int n, fd[2];
  char *output;
  char *op;
  char *line;
  int readSoFar;
  
  if(pipe(fd) < 0) 
    ERROR("Internal error - unable to set up pipe for system call!!");
  switch(fork()) {
  case -1:
    ERROR("Internal error - unable to fork system call!!");
  case 0:                 /* child */
    close(fd[0]);
    dup2(fd[1], fileno(stdout));
    execlp("sh", "sh", "-c", cmd, NULL);
  default:                /* parent */
    close(fd[1]);
    output = (char*) malloc(MAX);
    op = output;
    readSoFar = 0;
    while (n = read(fd[0], op, MAX - 1)) {
      readSoFar += n;
      output = (char*) realloc(output,readSoFar+MAX);
      op += n;
    }
    *op = '\0';
    close(fd[0]);
  }
  return output;
}

int main(int argc, char *argv[]) {
  char *t, *s;

  t = DoSystemCallCaptured(argv[1]);
  for (s=strtok(t,"\n");s;s=strtok(NULL,"\n"))
       printf("<%s>\n",s);
  return 0;
}
