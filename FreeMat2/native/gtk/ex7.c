#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef struct termios Termios;

Termios SetTerminalRawMode() {
  Termios oldattr, newattr;

  tcgetattr(STDIN_FILENO, &oldattr);
  newattr = oldattr;
  newattr.c_lflag &= ~(ECHO | ICANON | IEXTEN);
  newattr.c_iflag &= ~(ICRNL | INPCK | ISTRIP);
  newattr.c_cflag &= ~(CSIZE | PARENB);
  newattr.c_cflag |= CS8;
  newattr.c_oflag &= ~(OPOST);
  newattr.c_cc[VMIN] = 1;
  while (tcsetattr(STDIN_FILENO, TCSADRAIN, &newattr)) {
    if (errno != EINTR) {
      fprintf(stderr, "Unable to set up terminal attributes: tcsetattr error: %s\n", strerror(errno));
      exit(1);
    }
  }
  return oldattr;
}

void RestoreTerminalMode(Termios oldattr) {
  while (tcsetattr(STDIN_FILENO, TCSADRAIN, &oldattr)) {
    if (errno != EINTR) {
      fprintf(stderr, "Unable to set up terminal attributes: tcsetattr error: %s\n", strerror(errno));
      exit(1);
    }
  }
}

char ReadCharacter() {
  char c;

  while (read(STDIN_FILENO, (void *)&c, 1) != 1) {
    if(errno != EINTR) {
      fprintf(stderr,"Unable to read from input!");
      exit(1);
    };
  }
  return c;
}

void WriteCharacter(char c) {
  while (write(STDIN_FILENO, (void *)&c, 1) != 1) {
    if(errno != EINTR) {
      fprintf(stderr,"Unable to write to output!");
      exit(1);
    };
  }
}

int main() {
  Termios oldattr;
  int quitflag;
  char c;

  oldattr = SetTerminalRawMode();
  quitflag = 0;
  while (!quitflag) {
    c = ReadCharacter();
    quitflag = (c=='q');
    WriteCharacter(c);
  }
  RestoreTerminalMode(oldattr);
  return 0;
}
