#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  int tty;
  char buffer;
  
  tty = open("/dev/tty",O_RDWR);
  fcntl(tty, F_SETFL, fcntl(tty, F_GETFL) | O_NONBLOCK);
  if (tty == -1)
    printf("error!!\n");
  while (1) {
    if (read(tty, &buffer, 1))
      write(tty, &buffer, 1);
  }
  return 0;
}
