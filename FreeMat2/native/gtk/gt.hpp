#ifndef __gt_hpp__
#define __gt_hpp__
// Demos a base window class for GTK.  The base window class must be able to:
// Draw itself using double buffering to the screen
// Receive mouse commands
// Support hiding, showing, focus, select, etc.

#include <gtk/gtk.h>
#include <string>
#include <iostream>
#include "../win32/TermWidget.hpp"

class GTKTerminal : public TermWidget {
  GtkWidget *window;
  GdkPixmap *pixmap;
  GtkWidget *drawing_area;
  GtkWidget *scroll_bar;
  GtkObject *adj1;
  PangoFontDescription *myFont;
  PangoContext *cntxt;
  PangoLayout *pango;
  friend gint expose_event(GtkWidget *widget, GdkEventExpose *event);
  friend gint configure_event(GtkWidget *widget, GdkEventConfigure *event);
  friend gint scroll_changed(GtkWidget *widget, gpointer *data);
public:
  GTKTerminal(int width, int height, std::string title);
  ~GTKTerminal();
  virtual int GetHeight();
  virtual int GetWidth();
  virtual void InstallEventTimers();
  virtual void SetScrollBarValue(int val);
  virtual void SetupScrollBar(int minval, int maxval, int step, int page, int val);
  virtual void setFont(int size);
  virtual void ScrollLineUp();
  virtual void ScrollLineDown();
  virtual void BeginDraw() {};
  virtual void EndDraw() {};
  virtual void PutTagChar(int x, int y, tagChar g);
  virtual void Show();
  virtual void Hide();
};

#endif
