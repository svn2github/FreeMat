#include <gtk/gtk.h>
#include <fcntl.h>
#include "FMWindow.hpp"
#include "Terminal.hpp"
#include <iostream>
#include <errno.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

guchar rgbbuf1[IMAGE_WIDTH * IMAGE_HEIGHT * 3];
guchar rgbbuf2[IMAGE_WIDTH * IMAGE_HEIGHT * 3];

char *inbuf;
int inbuf_size, inbuf_pos;
#define INIT_BUF_SIZE 512

void keycall(int c) {
  printf("Got character %x\n\r",c);
}

Terminal term;

void main_proc(gpointer data, gint source, GdkInputCondition condition) {
  char c;
  while (read(source, &c, 1) == 1) {
    term.ProcessChar(c);
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
  std::vector<std::string> fnames;
  fnames.push_back("ls");
  fnames.push_back("list");
  fnames.push_back("anotherfunc");
  fnames.push_back("coolio");
  fnames.push_back("sin");
  fnames.push_back("cos");
  term.SetFunctionList(fnames);
  term.ReplacePrompt("--> ");
  term.DisplayPrompt();
  fflush(stdout);
  gtk_main();
  return 0;
}


