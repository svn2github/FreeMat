#include <gtk/gtk.h>
#include <fcntl.h>
#include "FMWindow.hpp"
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

guchar rgbbuf1[IMAGE_WIDTH * IMAGE_HEIGHT * 3];
guchar rgbbuf2[IMAGE_WIDTH * IMAGE_HEIGHT * 3];

char *inbuf;
int inbuf_size, inbuf_pos;
#define INIT_BUF_SIZE 512

typedef struct termios Termios;

Termios SetTerminalRawMode() {
  Termios oldattr, newattr;

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
    };  }
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

static char *find_char(char ch, const char *ptr, int len)
{
  int i;
  char *cp;

  for (cp = (char *) ptr, i = 0; i < len; i++, cp++)
    if (*cp == ch) return cp;

  return NULL;
}

/* This callback is called when we get something from stdin */
static void cb_stdin(gpointer data, gint source, GdkInputCondition condition) {
  int want_to_read, num_read, next_size;
  char *nl_ptr, *retstr;

  do
  {
    if (inbuf_pos == inbuf_size)
    {
      /* inbuf is full, so allocate more space for it */
      inbuf_size *= 2;
      inbuf = (char*) realloc(inbuf, inbuf_size * sizeof(char)); 
    }

    want_to_read = inbuf_size - inbuf_pos;
    /* read(2) is undefined if count > SSIZE_MAX */
    if (want_to_read > SSIZE_MAX)
      want_to_read = SSIZE_MAX;

    /* This doesn't block, because we are in nonblocking mode */
    num_read = read(source, &inbuf[inbuf_pos], want_to_read);

    if (num_read < 0) /* Read returned an error */
    {
      if (errno == EINTR || errno == EAGAIN)
	return; /* We're ok, but we got no data, so break out */

      /* (XXX) This should be done with one call to XtAppError... */
      fprintf(stderr,"read from stdin returned an error.\n");
    }

    if (num_read == 0) /* End of file */
    {
      /* An internal command to notify the widget of EOF */
      printf("EOF found\n");
      exit(0);
    }

    inbuf_pos += num_read;
    
    while (inbuf_pos > 0) /* While there are read characters... */
    {
      if ((nl_ptr = find_char('\n', inbuf, inbuf_pos)) == NULL)
	break; /* No full lines in inbuf */
      else
      {
	/* We found a newline, so send off the command preceding it */
	
	*nl_ptr = 0; /* Remove the newline */
	
	/* Send the command to the DrawCanvas widget */
	printf("command is %s\n",inbuf);
	
	/*
	 * Move the next command (after the newline we found), if any,
	 * to the start of inbuf, and set inbuf_pos
	 */
	next_size = &inbuf[inbuf_pos - 1] - nl_ptr;
	if (next_size > 0)
	  memmove(inbuf, nl_ptr + 1, next_size);
	inbuf_pos = next_size;
      }
    }
  } while(1); /* Keep on reading as long as we get something */
}


void KeyProc(gpointer data, gint source, GdkInputCondition condition) {
  char c;
  while (read(source, &c, 1) == 1) {
    std::cout << "Got character " << c << "\n";
  }
}

class Terminal {
  int state;
  Termios oldattr;
  Termios newattr;
  char *left, *right, *up, *down, *home, *bol;
  char *clear_eol, *clear_eod;
  char *u_arrow, *d_arrow, *l_arrow, *r_arrow;
 public:
  Terminal();
  ~Terminal();
};

// Set up the terminal in raw mode, and initialize the control
// strings.
Terminal::Terminal() {
  tcgetattr(STDIN_FILENO, &oldattr);
  newattr = oldattr;
  newattr.c_lflag &= ~(ECHO | ICANON | IEXTEN);
  newattr.c_iflag &= ~(ICRNL | INPCK | ISTRIP);
  newattr.c_cflag &= ~(CSIZE | PARENB);
  newattr.c_cflag |= CS8;
  newattr.c_oflag &= ~(OPOST);  
  newattr.c_cc[VMIN] = 1;
  newattr.c_cc[VTIME] = 0;
  while (tcsetattr(STDIN_FILENO, TCSADRAIN, &newattr)) {
    if (errno != EINTR) {
      fprintf(stderr, "Unable to set up terminal attributes: tcsetattr error: %s\n", strerror(errno));
      exit(1);
    }
  }
}

// Restore the original terminal setttings
Terminal::~Terminal() {
  
}

void main_proc(gpointer data, gint source, GdkInputCondition condition) {
  char c;
  while (read(source, &c, 1) == 1) {
    printf("Got character %x\n\r",c);
  }
}

int main (int argc, char *argv[]) {
  FMWindow *win1, *win2;
  gint x, y;
  guchar *pos;

  gtk_init (&argc, &argv);

  win1 = new FMWindow;
  win2 = new FMWindow;

  /* Set up the RGB buffer. */
  pos = rgbbuf1;
  for (y = 0; y < IMAGE_HEIGHT; y++)
    {
      for (x = 0; x < IMAGE_WIDTH; x++)
        {
          *pos++ = x - x % 32;                  /* Red. */
          *pos++ = (x / 32) * 4 + y - y % 32;   /* Green. */
          *pos++ = y - y % 32;                  /* Blue. */
        }
    }
  /* Set up the RGB buffer. */
  pos = rgbbuf2;
  for (y = 0; y < IMAGE_HEIGHT; y++)
    {
      for (x = 0; x < IMAGE_WIDTH; x++)
        {
          *pos++ = x - x % 16;                  /* Red. */
          *pos++ = (x / 16) * 4 + y - y % 16;   /* Green. */
          *pos++ = y - y % 16;                  /* Blue. */
        }
    }
  win2->Show();
  win2->SetImage(rgbbuf2, IMAGE_HEIGHT, IMAGE_WIDTH);
  win1->Show();
  win1->SetTitle("Image 1");
  win2->SetTitle("Image 2");
  //  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  gdk_input_add(STDIN_FILENO, GDK_INPUT_READ, main_proc, NULL);
  inbuf_size = INIT_BUF_SIZE;
  inbuf_pos = 0;
  inbuf = (char*) malloc(inbuf_size*sizeof(char));
  Termios oldattr;
  oldattr = SetTerminalRawMode();
  printf("--> ");
  fflush(stdout);
  gtk_main();
  RestoreTerminalMode(oldattr);
  return 0;
}


