#include "FMWindow.hpp"

// Glue code to map generic callback to member function of an instance
gboolean FMWindow::gRedraw(GtkWidget *widget, GdkEventExpose *event, 
			   gpointer thisptr) {
  return (((FMWindow*) thisptr)->Redraw(widget,event));
}

gboolean FMWindow::gOnClose(GtkWidget *widget, GdkEvent *event,
			    gpointer thisptr) {
  ((FMWindow*) thisptr)->OnClose();
  return FALSE;
}

gint FMWindow::gOnConfigure(GtkWidget *widget, GdkEventConfigure *event,
			    gpointer thisptr) {
  ((FMWindow*) thisptr)->UpdateSize(event->width,event->height);
  return TRUE;
}

gint FMWindow::gOnButton(GtkWidget *widget, GdkEventButton *event, gpointer thisptr) {
  ((FMWindow*) thisptr)->OnMouseLeftDown(event->x,event->y);
  return TRUE;
}

void FMWindow::UpdateSize(int width, int height) {
  if (width != m_width || height != m_height) {
    m_width = width;
    m_height = height;
    OnResize();
  }
}

FMWindow::FMWindow() {
  m_rgbdata = NULL;
  m_image_width = 200;
  m_image_height = 200;
  m_width = 0;
  m_height = 0;
  m_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  m_darea = gtk_drawing_area_new ();
  gtk_widget_set_size_request (m_darea, 200, 200);
  gtk_container_add(GTK_CONTAINER (m_window), m_darea);
  gtk_widget_set_events(m_darea, GDK_EXPOSURE_MASK | 
			GDK_BUTTON_PRESS_MASK);
  g_signal_connect(G_OBJECT(m_darea), "expose-event",
		    G_CALLBACK(FMWindow::gRedraw), this);
  g_signal_connect(G_OBJECT(m_window), "delete-event",
		   G_CALLBACK(FMWindow::gOnClose), this);
  g_signal_connect(G_OBJECT(m_window), "configure_event",
		   G_CALLBACK(FMWindow::gOnConfigure), this);
  g_signal_connect(G_OBJECT(m_window), "button_press_event",
		   G_CALLBACK(FMWindow::gOnButton), this);
}

FMWindow::~FMWindow() {
  gtk_widget_destroy(m_window);  
}

void FMWindow::SetImage(unsigned char *data, int width, int height) {
  m_rgbdata = data;
  m_image_width = width;
  m_image_height = height;
  gtk_widget_set_size_request (m_darea, width, height);
  Refresh();
}

void FMWindow::Refresh() {
  gtk_widget_queue_draw(m_window);
}

void FMWindow::Show() {
  gtk_widget_show_all(m_window);
}

void FMWindow::Hide() {
  gtk_widget_hide_all(m_window);
}

void FMWindow::OnClose() {
//   g_print("Close now\n");
}

void FMWindow::OnMouseLeftDown(int x, int y) {
//   g_print("mouse down %d %d\n",x,y);
}

void FMWindow::OnResize() {
//   g_print("Resize %d %d\n",m_width,m_height);
}

void FMWindow::SetTitle(std::string title) {
  gtk_window_set_title(GTK_WINDOW(m_window),title.c_str());
}

gboolean FMWindow::Redraw(GtkWidget *widget, GdkEventExpose *event) {
  if (m_rgbdata)
    gdk_draw_rgb_image (widget->window, widget->style->fg_gc[GTK_STATE_NORMAL],
			0, 0, m_image_width, m_image_height,
			GDK_RGB_DITHER_MAX, m_rgbdata, m_image_width * 3);
  return TRUE;
}
