#include <gtk/gtk.h>

#define IMAGE_WIDTH     256
#define IMAGE_HEIGHT    256


//
// Class FMWindow is a fairly minimal window.  It has the ability to
// display an RGB image and get basic event feedback.  From this window,
// a plot window, image window, etc. can easily be derived.
class FMWindow {
  GtkWidget *window, *darea;
  gboolean Redraw(GtkWidget *widget, GdkEventExpose *event);
 public:
  FMWindow();
  ~FMWindow();
  virtual void SetImage(unsigned char *data, int width, int height);
  virtual void Show();
  virtual void Hide();
  virtual void OnClose();
  virtual void OnMouseLeftDown();
};

gboolean FMWindowRedrawCallback(GtkWidget *widget, GdkEventExpose *event, 
				gpointer thisptr) {
  return (((FMWindow*) thisptr)->Redraw(widget,event));
}



class ImageDisplayWindow {
  guchar *m_rgbdata;
  int m_width, m_height;
  GtkWidget *window, *darea;

  // Global remapper class for the callback
  static gboolean  GlobalRedraw(GtkWidget *widget, GdkEventExpose *event, 
				gpointer thisptr);
  static void GlobalOnClose();
  static void GlobalOnMouseLeftDown();
 public:
  ImageDisplayWindow(guchar* data, int width, int height);
  ~ImageDisplayWindow();
  gboolean Redraw(GtkWidget *widget, GdkEventExpose *event);
  void Show();
  void Hide();
  void OnClose();
  void OnMouseLeftDown();
};

gboolean ImageDisplayWindow::GlobalRedraw(GtkWidget *widget, 
					  GdkEventExpose *event, 
					  gpointer thisptr) {
  return (((ImageDisplayWindow*) thisptr)->Redraw(widget,event));
}

ImageDisplayWindow::ImageDisplayWindow(guchar* data, int width, int height) {
  m_rgbdata = data;
  m_width = width;
  m_height = height;
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  darea = gtk_drawing_area_new ();
  gtk_widget_set_size_request (darea, IMAGE_WIDTH, IMAGE_HEIGHT);
  gtk_container_add (GTK_CONTAINER (window), darea);
  g_signal_connect (G_OBJECT(darea), "expose-event",
		    G_CALLBACK(ImageDisplayWindow::GlobalRedraw), this);
}

ImageDisplayWindow::~ImageDisplayWindow() {
  gtk_widget_destroy(window);
}

void ImageDisplayWindow::Show() {
  gtk_widget_show_all(window);
}

void ImageDisplayWindow::Hide() {
  gtk_widget_hide_all(window);
}

gboolean ImageDisplayWindow::Redraw(GtkWidget *widget, GdkEventExpose *event) {
  gdk_draw_rgb_image (widget->window, widget->style->fg_gc[GTK_STATE_NORMAL],
                      0, 0, m_width, m_height,
                      GDK_RGB_DITHER_MAX, m_rgbdata, m_width * 3);
  return TRUE;
}

guchar rgbbuf1[IMAGE_WIDTH * IMAGE_HEIGHT * 3];
guchar rgbbuf2[IMAGE_WIDTH * IMAGE_HEIGHT * 3];

int
main (int argc, char *argv[]) {
  ImageDisplayWindow *win1, *win2, *win3;
  gint x, y;
  guchar *pos;

  gtk_init (&argc, &argv);

  win1 = new ImageDisplayWindow(rgbbuf1,IMAGE_WIDTH,IMAGE_HEIGHT);
  win2 = new ImageDisplayWindow(rgbbuf2,IMAGE_WIDTH,IMAGE_HEIGHT);
  win3 = new ImageDisplayWindow(rgbbuf2,IMAGE_WIDTH,IMAGE_HEIGHT);
  delete win3;
  win1->Show();
  win2->Hide();

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


