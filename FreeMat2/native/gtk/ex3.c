#include <gtk/gtk.h>

#define IMAGE_WIDTH     256
#define IMAGE_HEIGHT    256

guchar rgbbuf1[IMAGE_WIDTH * IMAGE_HEIGHT * 3];
guchar rgbbuf2[IMAGE_WIDTH * IMAGE_HEIGHT * 3];

gboolean on_darea_expose (GtkWidget *widget,
                          GdkEventExpose *event,
                          gpointer user_data);

GtkWidget* newImageDisplayWindow(guchar* imge, int width, int height) {
  GtkWidget *window, *darea;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  darea = gtk_drawing_area_new ();
  gtk_widget_set_size_request (darea, IMAGE_WIDTH, IMAGE_HEIGHT);
  gtk_container_add (GTK_CONTAINER (window), darea);
  gtk_signal_connect (GTK_OBJECT (darea), "expose-event",
                      GTK_SIGNAL_FUNC (on_darea_expose), imge);
  gtk_widget_show_all (window);
}

int
main (int argc, char *argv[])
{
  GtkWidget *pixwin1;
  GtkWidget *pixwin2;
  gint x, y;
  guchar *pos;

  gtk_init (&argc, &argv);

  pixwin1 = newImageDisplayWindow(rgbbuf1,IMAGE_WIDTH,IMAGE_HEIGHT);
  pixwin2 = newImageDisplayWindow(rgbbuf2,IMAGE_WIDTH,IMAGE_HEIGHT);

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
  gtk_main ();
  return 0;
}


gboolean
on_darea_expose (GtkWidget *widget,
                 GdkEventExpose *event,
                 gpointer user_data)
{
  gdk_draw_rgb_image (widget->window, widget->style->fg_gc[GTK_STATE_NORMAL],
                      0, 0, IMAGE_WIDTH, IMAGE_HEIGHT,
                      GDK_RGB_DITHER_MAX, user_data, IMAGE_WIDTH * 3);

  return TRUE;
}
