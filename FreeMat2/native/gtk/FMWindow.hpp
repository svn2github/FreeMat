#ifndef __FMWindow_hpp__
#define __FMWindow_hpp__

#include <gtk/gtk.h>
#include <string>

//
// Class FMWindow is a fairly minimal window.  It has the ability to
// display an RGB image and get basic event feedback.  From this window,
// a plot window, image window, etc. can easily be derived.
class FMWindow {
  GtkWidget *m_window, *m_darea;
  guchar *m_rgbdata;
  gint m_image_width, m_image_height;
  gint m_width, m_height;
  
  gboolean Redraw(GtkWidget *widget, GdkEventExpose *event);
  static gboolean gRedraw(GtkWidget *widget, GdkEventExpose *event, gpointer thisptr);
  static gboolean gOnClose(GtkWidget *widget, GdkEvent *event, gpointer thisptr);
  static gint gOnConfigure(GtkWidget *widget, GdkEventConfigure *event, 
			   gpointer thisptr);
  static gint gOnButton(GtkWidget *widget, GdkEventButton *event, gpointer thisptr);
  void UpdateSize(int width, int height);
 public:
  FMWindow();
  ~FMWindow();
  void SetImage(unsigned char *data, int width, int height);
  void Show();
  void Hide();
  void Refresh();
  virtual void OnClose();
  virtual void OnMouseLeftDown(int x, int y);
  virtual void OnResize();
  void SetTitle(std::string title);
};


#endif
