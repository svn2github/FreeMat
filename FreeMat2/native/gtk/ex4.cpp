#include <gtk/gtk.h>
#include <fcntl.h>
#include "FMWindow.hpp"
#include <iostream>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

guchar rgbbuf1[IMAGE_WIDTH * IMAGE_HEIGHT * 3];
guchar rgbbuf2[IMAGE_WIDTH * IMAGE_HEIGHT * 3];

char *inbuf;
int inbuf_size, inbuf_pos;
#define INIT_BUF_SIZE 512

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
  while (read(source, &c, 1)) {
    std::cout << "Got character " << c << "\n";
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
  fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
  gdk_input_add(STDIN_FILENO, GDK_INPUT_READ, cb_stdin, NULL);
  inbuf_size = INIT_BUF_SIZE;
  inbuf_pos = 0;
  inbuf = (char*) malloc(inbuf_size*sizeof(char));
  
  gtk_main();
  return 0;
}


