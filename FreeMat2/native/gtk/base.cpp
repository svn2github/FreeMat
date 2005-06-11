// Demos a base window class for GTK.  The base window class must be able to:
// Draw itself using double buffering to the screen
// Receive mouse commands
// Support hiding, showing, focus, select, etc.

#include <gtk/gtk.h>
#include <string>
#include <iostream>
#include "TermWidget.hpp"

class XPWindow {
public:
  GtkWidget *window;
  GdkPixmap *pixmap;
  GtkWidget *drawing_area;
public:
  XPWindow(int width, int height, std::string title);
  XPWindow() {}
  virtual ~XPWindow();
  virtual void OnDraw() {}
  virtual void OnResize() {}
  virtual void Show();
  virtual void Hide();
  virtual void Raise();
  virtual void OnMouseDown(int x, int y) {}
  virtual void OnMouseDrag(int x, int y);
  virtual void OnMouseUp(int x, int y) {}
  virtual void OnKeyDown(int key) {}
  virtual int GetHeight();
  virtual int GetWidth();
};

void XPWindow::OnMouseDrag(int x, int y) {
  gdk_draw_rectangle(pixmap, window->style->black_gc,
		     TRUE, x-2,y-2,4,4);
  gtk_widget_queue_draw_area(window, x-2, y-2, 4, 4);
}

void XPWindow::Raise() {
  // Don't know this one!
}

void XPWindow::Show() {
  gtk_widget_show(drawing_area);
  gtk_widget_show(window);
}

void XPWindow::Hide() {
  gtk_widget_hide(drawing_area);
  gtk_widget_hide(window);
}

int XPWindow::GetHeight() {
  return window->allocation.height;
}

int XPWindow::GetWidth() {
  return window->allocation.width;
}

static gint expose_event(GtkWidget *widget,
			 GdkEventExpose *event) {
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  gdk_draw_drawable (widget->window,
		     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		     winptr->pixmap,
		     event->area.x, event->area.y,
		     event->area.x, event->area.y,
		     event->area.width, event->area.height);
  return FALSE;
}


static gint button_press_event(GtkWidget *widget,
			       GdkEventButton *event) {
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  if (event->button == 1)
    winptr->OnMouseDown(event->x, event->y);
  return TRUE;
}

static gint button_release_event(GtkWidget *widget,
				 GdkEventButton *event) {
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  if (event->button == 1)
    winptr->OnMouseUp(event->x, event->y);
  return TRUE;
}

static gint motion_notify_event(GtkWidget *widget,
				GdkEventMotion *event) {
  if (!(event->state & GDK_BUTTON1_MASK)) return FALSE;
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  winptr->OnMouseDrag(event->x, event->y);
  return TRUE;
}

static gint keypress_event(GtkWidget *widget,
			   GdkEventKey *event) {
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  winptr->OnKeyDown(event->keyval);
  return TRUE;
}

/* Create a new backing pixmap of the appropriate size */
static gint configure_event( GtkWidget         *widget,
                             GdkEventConfigure *event )
{
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  if (winptr->pixmap)
    g_object_unref (winptr->pixmap);
  
  winptr->pixmap = gdk_pixmap_new (widget->window,
				   widget->allocation.width,
				   widget->allocation.height,
				   -1);
  gdk_draw_rectangle (winptr->pixmap,
		      widget->style->white_gc,
		      TRUE,
		      0, 0,
		      widget->allocation.width,
		      widget->allocation.height);
  winptr->OnResize();
  return TRUE;
}


XPWindow::XPWindow(int width, int height, std::string title) {
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_usize(window, width, height);
  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawing_area,width,height);
  gtk_container_add(GTK_CONTAINER(window),drawing_area);
  g_object_set_data(G_OBJECT(drawing_area),"this",this);
  g_object_set_data(G_OBJECT(window),"this",this);
  g_signal_connect (G_OBJECT (drawing_area), "expose_event",
		    G_CALLBACK (expose_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area),"configure_event",
		    G_CALLBACK (configure_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "motion_notify_event",
		    G_CALLBACK (motion_notify_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "button_press_event",
		    G_CALLBACK (button_press_event), NULL);
  g_signal_connect (G_OBJECT (window), "keypress_event",
		    G_CALLBACK (keypress_event), NULL);
  gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_POINTER_MOTION_MASK);
  pixmap = NULL;
}

XPWindow::~XPWindow() {
}

class XPScrolledWindow : public XPWindow {
 public:
  GtkWidget *scroll_bar;
  GtkObject *adj1;
 public:
  XPScrolledWindow(int width, int height, std::string title);
  virtual ~XPScrolledWindow();
};

XPScrolledWindow::~XPScrolledWindow() {
}

XPScrolledWindow::XPScrolledWindow(int width, int height, std::string title) {
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  GtkWidget* hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  gtk_container_add(GTK_CONTAINER(window), hbox);
  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawing_area,width,height);
  gtk_widget_show(drawing_area);
  gtk_box_pack_start(GTK_BOX(hbox),drawing_area,TRUE,TRUE,0);
  // value, lower, upper, step increment, page increment, page size
  adj1 = gtk_adjustment_new(0,1,10,1,5,5);
  scroll_bar = gtk_vscrollbar_new(GTK_ADJUSTMENT(adj1));
  gtk_box_pack_start(GTK_BOX(hbox),scroll_bar,FALSE,FALSE,0);
  gtk_widget_show(scroll_bar);
  g_object_set_data(G_OBJECT(drawing_area),"this",this);
  g_object_set_data(G_OBJECT(window),"this",this);
  g_signal_connect (G_OBJECT (drawing_area), "expose_event",
		    G_CALLBACK (expose_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area),"configure_event",
		    G_CALLBACK (configure_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "motion_notify_event",
		    G_CALLBACK (motion_notify_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "button_press_event",
		    G_CALLBACK (button_press_event), NULL);
  g_signal_connect (G_OBJECT (window), "key_press_event",
		    G_CALLBACK (keypress_event), NULL);
  gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_POINTER_MOTION_MASK
			 | GDK_KEY_PRESS_MASK);
  gtk_widget_set_events (window,GDK_KEY_PRESS_MASK);
  gtk_widget_grab_focus(window);
  pixmap = NULL;
}

class GTKTermWidget : public XPScrolledWindow, public TermWidget {
  PangoFontDescription *myFont;
  PangoContext *cntxt;
  PangoLayout *pango;
public:
  GTKTermWidget(int width, int height, std::string title);
  virtual void InstallEventTimers();
  virtual void SetScrollBarValue(int val);
  virtual void SetupScrollBar(int minval, int maxval, int step, int page, int val);
  virtual void DrawContent();  
  virtual void setFont(int size);
  virtual int GetHeight();
  virtual int GetWidth();
  virtual void ScrollLineUp();
  virtual void ScrollLineDown();
  virtual void OnKeyDown(int key);
  virtual void OnMouseDown(int x, int y);
  virtual void OnMouseDrag(int x, int y);
  virtual void OnMouseUp(int x, int y);
  virtual void OnResize();
};

void GTKTermWidget::OnMouseDown(int x, int y) {
  TermWidget::OnMouseDown(x,y);
}

void GTKTermWidget::OnMouseUp(int x, int y) {
  TermWidget::OnMouseUp(x,y);
}

void GTKTermWidget::OnMouseDrag(int x, int y) {
  TermWidget::OnMouseDrag(x,y);
}

void GTKTermWidget::OnKeyDown(int key) {
  TermWidget::OnKeyPress(key);
}

void GTKTermWidget::OnResize() {
  TermWidget::OnResize();
}

static gint scroll_changed(GtkWidget *widget, gpointer *data) {
  // Get the scroll value
  GTKTermWidget *p = (GTKTermWidget*) data;
  std::cout << "this = " << p << "\n";
  int scrollvalue = GTK_ADJUSTMENT(p->adj1)->value;
  std::cout << "scroll val = " << scrollvalue << "\n";
  p->OnScroll(scrollvalue);
  return TRUE;  
}

GTKTermWidget::GTKTermWidget(int width, int height, std::string title) :
  XPScrolledWindow(width, height, title) {
  cntxt = gtk_widget_get_pango_context(window);
  pango = pango_layout_new(cntxt);  
  TermWidget::Initialize();
  InstallEventTimers();
  g_signal_connect(G_OBJECT(adj1), "value_changed", G_CALLBACK(scroll_changed), this);
  SetupScrollBar(0,1,1,1,0);
}

int GTKTermWidget::GetWidth() {
  return XPWindow::GetWidth() - 16;
}

int GTKTermWidget::GetHeight() {
  return XPWindow::GetHeight();
}

void GTKTermWidget::ScrollLineUp() {
  gtk_adjustment_set_value(GTK_ADJUSTMENT(adj1),GTK_ADJUSTMENT(adj1)->value - 1);
}

void GTKTermWidget::ScrollLineDown() {
  gtk_adjustment_set_value(GTK_ADJUSTMENT(adj1),GTK_ADJUSTMENT(adj1)->value + 1);
}

static gint blink_cb(gpointer data) {
  GTKTermWidget* ptr = (GTKTermWidget*) data;
  ptr->blink();
  return TRUE;
}

static gint refresh_cb(gpointer data) {
  GTKTermWidget* ptr = (GTKTermWidget*) data;
  ptr->DrawContent();
  return TRUE;
}

void GTKTermWidget::InstallEventTimers() {
  g_timeout_add(100,refresh_cb,this);
  g_timeout_add(1000,blink_cb,this);
}

void GTKTermWidget::SetScrollBarValue(int val) {
  gtk_adjustment_set_value(GTK_ADJUSTMENT(adj1),val);
}

void GTKTermWidget::SetupScrollBar(int minval, int maxval, int step, int page, int val) {
  GTK_ADJUSTMENT(adj1)->lower = minval;
  GTK_ADJUSTMENT(adj1)->upper = maxval;
  GTK_ADJUSTMENT(adj1)->step_increment = step;
  GTK_ADJUSTMENT(adj1)->page_increment = page;
  GTK_ADJUSTMENT(adj1)->page_size = 1;
  gtk_adjustment_set_value(GTK_ADJUSTMENT(adj1),val);
}

void GTKTermWidget::DrawContent() {
  if (m_width == 0) return;
  for (int i=0;i<m_height;i++) {
    int j=0;
    while (j<m_width) {
      // skip over characters that do not need to be redrawn
      while ((j < m_width) && 
	     (m_onscreen[i*m_width+j] == m_surface[i*m_width+j])) j++;
      if (j < m_width) {
	tagChar g = m_surface[i*m_width+j];
	if (m_scrolling) 
	  g.flags &= ~CURSORBIT;
	pango_layout_set_text(pango, &g.v, 1);
	if (g.noflags()) {
	  gdk_draw_rectangle(pixmap, window->style->white_gc,
			     TRUE, j*m_char_w, i*m_char_h, 
			     m_char_w, m_char_h);
	  gdk_draw_layout(pixmap, window->style->black_gc,
			  j*m_char_w, i*m_char_h, pango);
	  gtk_widget_queue_draw_area(window, j*m_char_w, i*m_char_h, 
				     m_char_w, m_char_h);
	} else if (g.cursor()) {
	  gdk_draw_rectangle(pixmap, window->style->black_gc,
			     TRUE, j*m_char_w, i*m_char_h, 
			     m_char_w, m_char_h);
	  gdk_draw_layout(pixmap, window->style->white_gc,
			  j*m_char_w, i*m_char_h, pango);
	  gtk_widget_queue_draw_area(window, j*m_char_w, i*m_char_h, 
				     m_char_w, m_char_h);
	} else {
	  gdk_draw_rectangle(pixmap, window->style->bg_gc[GTK_STATE_SELECTED],
			     TRUE, j*m_char_w, i*m_char_h, 
			     m_char_w, m_char_h);
	  gdk_draw_layout(pixmap, window->style->fg_gc[GTK_STATE_SELECTED],
			  j*m_char_w, i*m_char_h, pango);
	  gtk_widget_queue_draw_area(window, j*m_char_w, i*m_char_h, 
				     m_char_w, m_char_h);
	}
	m_onscreen[i*m_width+j] = g;
	j++;
      }
    }
  }
}

void GTKTermWidget::setFont(int size) {
  if (myFont)
    pango_font_description_free(myFont);
  myFont = pango_font_description_new();
  pango_font_description_set_family( myFont, "monospace" );
  pango_font_description_set_size( myFont, size*PANGO_SCALE);
  pango_layout_set_font_description( pango, myFont);
  pango_layout_set_text(pango, "W", 1);
  pango_layout_get_pixel_size(pango, &m_char_w, &m_char_h);
}

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);
  XPWindow *win = new GTKTermWidget(400,400,"Hello");
  win->Show();
  gtk_main();
  return 0;
}
