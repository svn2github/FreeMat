// Demos a base window class for GTK.  The base window class must be able to:
// Draw itself using double buffering to the screen
// Receive mouse commands
// Support hiding, showing, focus, select, etc.

#include "gt.hpp"

gint expose_event(GtkWidget *widget,
			 GdkEventExpose *event) {
  GTKTerminal *winptr;
  winptr = (GTKTerminal*) g_object_get_data(G_OBJECT(widget),"this");
  gdk_draw_drawable (widget->window,
		     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		     winptr->pixmap,
		     event->area.x, event->area.y,
		     event->area.x, event->area.y,
		     event->area.width, event->area.height);
  return FALSE;
}


gint button_press_event(GtkWidget *widget,
			       GdkEventButton *event) {
  GTKTerminal *winptr;
  winptr = (GTKTerminal*) g_object_get_data(G_OBJECT(widget),"this");
  if (event->button == 1)
    winptr->OnMouseDown(event->x, event->y);
  return TRUE;
}

gint button_release_event(GtkWidget *widget,
				 GdkEventButton *event) {
  GTKTerminal *winptr;
  winptr = (GTKTerminal*) g_object_get_data(G_OBJECT(widget),"this");
  if (event->button == 1)
    winptr->OnMouseUp(event->x, event->y);
  return TRUE;
}

gint motion_notify_event(GtkWidget *widget,
				GdkEventMotion *event) {
  if (!(event->state & GDK_BUTTON1_MASK)) return FALSE;
  GTKTerminal *winptr;
  winptr = (GTKTerminal*) g_object_get_data(G_OBJECT(widget),"this");
  winptr->OnMouseDrag(event->x, event->y);
  return TRUE;
}

gint keypress_event(GtkWidget *widget,
			   GdkEventKey *event) {
  GTKTerminal *winptr;
  winptr = (GTKTerminal*) g_object_get_data(G_OBJECT(widget),"this");
  winptr->OnKeyPress(event->keyval);
  return TRUE;
}

/* Create a new backing pixmap of the appropriate size */
gint configure_event( GtkWidget         *widget,
                             GdkEventConfigure *event )
{
  GTKTerminal *winptr;
  winptr = (GTKTerminal*) g_object_get_data(G_OBJECT(widget),"this");
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

gint scroll_changed(GtkWidget *widget, gpointer *data) {
  // Get the scroll value
  GTKTerminal *p = (GTKTerminal*) data;
  int scrollvalue = GTK_ADJUSTMENT(p->adj1)->value;
  p->OnScroll(scrollvalue);
  return TRUE;  
}

int GTKTerminal::GetHeight() {
  return (drawing_area->allocation.height);
}

int GTKTerminal::GetWidth() {
  return (drawing_area->allocation.width);
}

GTKTerminal::GTKTerminal(int width, int height, std::string title) {
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
  cntxt = gtk_widget_get_pango_context(window);
  pango = pango_layout_new(cntxt);  
  gtk_widget_show(drawing_area);
  gtk_widget_show(window);
  TermWidget::Initialize();
  InstallEventTimers();
  g_signal_connect(G_OBJECT(adj1), "value_changed", G_CALLBACK(scroll_changed), this);
}

GTKTerminal::~GTKTerminal() {
}

void GTKTerminal::Show() {
  gtk_widget_show(drawing_area);
  gtk_widget_show(window);
}

void GTKTerminal::Hide() {
  gtk_widget_hide(drawing_area);
  gtk_widget_hide(window);
}

void GTKTerminal::ScrollLineUp() {
  gtk_adjustment_set_value(GTK_ADJUSTMENT(adj1),GTK_ADJUSTMENT(adj1)->value - 1);
}

void GTKTerminal::ScrollLineDown() {
  gtk_adjustment_set_value(GTK_ADJUSTMENT(adj1),GTK_ADJUSTMENT(adj1)->value + 1);
}

gint blink_cb(gpointer data) {
  GTKTerminal* ptr = (GTKTerminal*) data;
  ptr->blink();
  return TRUE;
}

gint refresh_cb(gpointer data) {
  GTKTerminal* ptr = (GTKTerminal*) data;
  ptr->DrawContent();
  return TRUE;
}

void GTKTerminal::InstallEventTimers() {
  g_timeout_add(100,refresh_cb,this);
  g_timeout_add(1000,blink_cb,this);
}

void GTKTerminal::SetScrollBarValue(int val) {
  gtk_adjustment_set_value(GTK_ADJUSTMENT(adj1),val);
}

void GTKTerminal::SetupScrollBar(int minval, int maxval, int step, int page, int val) {
  GTK_ADJUSTMENT(adj1)->lower = minval;
  GTK_ADJUSTMENT(adj1)->upper = maxval+page;
  GTK_ADJUSTMENT(adj1)->step_increment = step;
  GTK_ADJUSTMENT(adj1)->page_increment = page;
  GTK_ADJUSTMENT(adj1)->page_size = page;
  gtk_adjustment_set_value(GTK_ADJUSTMENT(adj1),val);
}

void GTKTerminal::PutTagChar(int x, int y, tagChar g) {
  pango_layout_set_text(pango, &g.v, 1);
  if (g.noflags()) {
    gdk_draw_rectangle(pixmap, window->style->white_gc,
		       TRUE, x, y, 
		       m_char_w, m_char_h);
    gdk_draw_layout(pixmap, window->style->black_gc,
		    x, y, pango);
    gtk_widget_queue_draw_area(window, x, y, 
			       m_char_w, m_char_h);
  } else if (g.cursor()) {
    gdk_draw_rectangle(pixmap, window->style->black_gc,
		       TRUE, x, y, 
		       m_char_w, m_char_h);
    gdk_draw_layout(pixmap, window->style->white_gc,
		    x, y, pango);
    gtk_widget_queue_draw_area(window, x, y, 
			       m_char_w, m_char_h);
  } else {
    gdk_draw_rectangle(pixmap, window->style->bg_gc[GTK_STATE_SELECTED],
		       TRUE, x, y, 
		       m_char_w, m_char_h);
    gdk_draw_layout(pixmap, window->style->fg_gc[GTK_STATE_SELECTED],
		    x, y, pango);
    gtk_widget_queue_draw_area(window, x, y, 
			       m_char_w, m_char_h);
  }
}

void GTKTerminal::setFont(int size) {
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
  GTKTerminal *win = new GTKTerminal(400,400,"Hello");
  win->Show();
  gtk_main();
  return 0;
}
