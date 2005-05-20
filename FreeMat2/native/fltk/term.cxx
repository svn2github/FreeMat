#include "term.h"
#include <string.h>
#include <iostream>
#include <math.h>

#include <FL/fl_draw.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Scrollbar.H>

#define TMIN(a,b) (((a) < (b)) ? (a) : (b))
#define TMAX(a,b) (((a) > (b)) ? (a) : (b))

void refresh_cb(void* t) {
  ((TermWidget*) t)->refresh();
  Fl::add_timeout(0.03,refresh_cb,t);
}

void blink_cb(void* t) {
  ((TermWidget*) t)->blink();
  Fl::add_timeout(1.0,blink_cb,t);  
}

void scroll_cb(Fl_Widget* o, void* t) {
  ((TermWidget*) t)->scrollBack(((Fl_Valuator*)o)->value());
}

TermWidget::TermWidget(int x, int y, int w, int h, const char *label) : 
  Fl_Widget(x,y,w,h,label) {
  m_surface = NULL;
  m_onscreen = NULL;
  m_history = NULL;
  m_scrollback = 1000;
  m_history_lines = 0;
  m_cursor_x = 0;
  m_cursor_y = 0;
  m_clearall = true;
  //  m_scrollbar = new QScrollBar(Qt::Vertical,this);
  //  m_scrollbar->setRange(0,0);
  //  m_timer_refresh = new QTimer;
  //  QObject::connect(m_timer_refresh, SIGNAL(timeout()), this, SLOT(refresh()));
  //  m_timer_blink = new QTimer;
  //  QObject::connect(m_timer_blink, SIGNAL(timeout()), this, SLOT(blink()));
  //  QObject::connect(m_scrollbar,SIGNAL(valueChanged(int)), this, SLOT(scrollBack(int)));
  setFont(14);
  cursorOn = false;
  blinkEnable = true;
  color(FL_WHITE);
  selection_color(FL_BLUE);
  m_scrolling = false;
  m_mousePressed = false;
  Fl::add_timeout(0.1,refresh_cb,this);
  Fl::add_timeout(1.0,blink_cb,this);
  //  buffer.fill(colorGroup().base());
}

TermWidget::~TermWidget() {
}

void TermWidget::setScroller(Fl_Scrollbar* scrollbar) {
  m_scrollbar = scrollbar;
  adjustScrollbarPosition();  
  //  resizeTextSurface();
}

void TermWidget::scrollBack(int val) {
  if (m_history_lines == 0) return;
  m_scrolling =  (val != m_history_lines);
  m_surface[m_cursor_y*m_width+m_cursor_x].clearCursor();
  m_surface = m_history + (m_scrollback - m_height - (m_history_lines - val))*m_width;
  m_scroll_offset = (m_scrollback - m_height - (m_history_lines - val))*m_width;
}

void TermWidget::blink() {
  if (!blinkEnable) return;
  m_surface[m_cursor_x + m_cursor_y*m_width].toggleCursor();
}

void TermWidget::adjustScrollbarPosition() {
  m_active_width = w() - m_scrollbar->w();
  //  m_scrollbar->resize(QApplication::style().pixelMetric(QStyle::PM_ScrollBarExtent),
  //		      contentsRect().height());
  //  m_active_width = contentsRect().width() - 2 - m_scrollbar->width();
  //  m_scrollbar->move(contentsRect().topRight() - QPoint(m_scrollbar->width()-1,0));
  //  m_scrollbar->show();
}

void TermWidget::refresh() {
  damage(1);
}

void TermWidget::resizeTextSurface() {
  bool firsttime = (m_surface == NULL);
  if (m_surface)
    m_surface[m_cursor_y*m_width+m_cursor_x].clearCursor();  
  int cursor_offset = m_height - 1 - m_cursor_y;
  //  m_timer_refresh->start(30,false);
  //  m_timer_blink->start(1000);
  int new_width = m_active_width/m_char_w;
  int new_height = h()/m_char_h;
  tagChar *new_history = new tagChar[new_width*m_scrollback];
  
  if (m_history) {
    int minwidth = TMIN(new_width,m_width);
    for (int i=0;i<m_scrollback;i++)
      for (int j=0;j<minwidth;j++)
	new_history[i*new_width+j] = m_history[i*m_width+j];
  }
  // Copy old history to new history here
  
  delete[] m_onscreen;
  delete[] m_history;
  m_onscreen = new tagChar[new_width*new_height];
  m_history = new_history;
  m_surface = m_history + (m_scrollback - new_height)*new_width;
  
  if (!firsttime) {
    m_history_lines -= (new_height-m_height);
    m_history_lines = TMAX(0,m_history_lines);
  }

  m_width = new_width;
  m_height = new_height;
  m_clearall = true;
  if (firsttime) {
    setCursor(0,0);
    setScrollbar(0);
  } else {
    m_cursor_y = m_height - 1 - cursor_offset;
    setScrollbar(m_history_lines);
    //    m_scrollbar->setRange(0,m_history_lines);
    //    m_scrollbar->setSteps(1,m_height);
    //    m_scrollbar->setValue(m_history_lines);
  }
  m_scroll_offset = (m_scrollback - m_height)*m_width;
}

void TermWidget::setScrollbar(int val) {
  //  std::cout << "slider size = " << m_height/((double) m_history_lines+m_height) << "\n";
  m_scrollbar->value(val,m_height,0,m_history_lines);
}

void TermWidget::OutputString(std::string txt) {
  if (m_scrolling) 
    setScrollbar(0);
  for (int i=0;i<txt.size();i++) {
    if (txt[i] == '\n' || txt[i] == '\r') {
      setCursor(0,m_cursor_y+1);
    } else {
      m_surface[m_cursor_x + m_cursor_y*m_width] = tagChar(txt[i]);
      setCursor(m_cursor_x+1,m_cursor_y);
    }
  }
  //  m_timer_blink->start(1000);
}

void TermWidget::ProcessChar(char c) {
  if (m_scrolling) 
    setScrollbar(0);
  if (c == 'q')
    exit(0);
  if (c == 'l') {
    setCursor(TMAX(m_cursor_x-1,0),m_cursor_y);
    return;
  }
  if (c != 'd' && c != 'x') {
    char buffer[2];
    buffer[0] = c;
    buffer[1] = 0;
    OutputString(buffer);
  } else if (c == 'd')
    OutputString("Now is the time for all men to come to the aid of their country, and by their aid, assist those who need it, or something like that....");
  else if (c == 'x') {
    for (int i=0;i<100;i++) {
      char buffer[1000];
      sprintf(buffer,"line %d\n",i);
      OutputString(buffer);
    }
  }
  refresh();
}

void TermWidget::resize(int x, int y, int w, int h) {
  Fl_Widget::resize(x,y,w,h);
  adjustScrollbarPosition();
 resizeTextSurface();
}
//   QFrame::resizeEvent(e);
//   QPainter paint;
//   adjustScrollbarPosition();
//   resizeTextSurface();
// }

// void TermWidget::markDirty(QRect &rect) {
//   int xmin, xmax, ymin, ymax;
//   xmin = (int) floor(rect.left()/((float) m_char_w));
//   xmax = (int) ceil(rect.right()/((float) m_char_w));
//   ymin = (int) floor(rect.top()/((float) m_char_h));
//   ymax = (int) ceil(rect.bottom()/((float) m_char_h));
//   xmin = TMIN(m_width-1,TMAX(0,xmin));
//   xmax = TMIN(m_width-1,TMAX(0,xmax));
//   ymin = TMIN(m_height-1,TMAX(0,ymin));
//   ymax = TMIN(m_height-1,TMAX(0,ymax));
//   for (int i=ymin;i<=ymax;i++)
//     for (int j=xmin;j<=xmax;j++)
//       m_onscreen[i*m_width+j] = -1;
// }

// void TermWidget::paintEvent(QPaintEvent *e) {
//   QMemArray<QRect> rects = e->region().rects();
//   for ( uint i = 0; i < rects.count(); i++ ) 
//     markDirty(rects[(int) i]);
//   QPainter paint;
//   paint.begin(this);
//   paintContents(paint);
//   paint.end();
// }

void TermWidget::draw() {
  if (damage() & FL_DAMAGE_ALL) 
    for (int i=0;i<m_width*m_height;i++)
      m_onscreen[i] = tagChar(-1);
  paintContents();
}

// Update the onscreen buffer to match the text buffer surface.
void TermWidget::paintContents() {
  if (!m_surface) return;
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
	if (g.noflags()) {
	  fl_color(FL_WHITE);
	  fl_rectf(j*m_char_w,i*m_char_h,m_char_w,m_char_h);
	  fl_color(FL_BLACK);
	  fl_draw(&g.v,1,j*m_char_w,i*m_char_h+m_char_h-fl_descent());
	} else if (g.cursor()) {
	  fl_color(FL_BLACK);
	  fl_rectf(j*m_char_w,i*m_char_h,m_char_w,m_char_h);
	  fl_color(FL_WHITE);
	  fl_draw(&g.v,1,j*m_char_w,i*m_char_h+m_char_h-fl_descent());
	} else {
	  fl_color(FL_BLUE);
	  fl_rectf(j*m_char_w,i*m_char_h,m_char_w,m_char_h);
	  fl_color(FL_WHITE);
	  fl_draw(&g.v,1,j*m_char_w,i*m_char_h+m_char_h-fl_descent());
	}
	m_onscreen[i*m_width+j] = g;
	j++;
      }
    }
  }
}

void TermWidget::setCursor(int x, int y) {
  if (m_scrolling) 
    setScrollbar(0);
  if (m_surface[m_cursor_y*m_width+m_cursor_x].cursor())
    m_surface[m_cursor_y*m_width+m_cursor_x].toggleCursor();
  m_cursor_x = x;
  m_cursor_y = y;
  m_cursor_y += m_cursor_x/m_width;
  m_cursor_x %= m_width;
  if (m_cursor_y >= m_height) {
    // scroll up - which we do by a single memmove op
    int toscroll = m_cursor_y - m_height + 1;
    for (int i=0;i<(m_scrollback - toscroll)*m_width;i++)
      m_history[i] = m_history[i+toscroll*m_width];
    for (int i=0;i<toscroll*m_width;i++)
      m_history[(m_scrollback - toscroll)*m_width+i] = tagChar();
    m_history_lines = TMIN(m_history_lines+toscroll,m_scrollback-m_height);
    m_cursor_y -= toscroll;
    setScrollbar(m_history_lines);
  }
  m_surface[m_cursor_y*m_width+m_cursor_x].setCursor();
}

void TermWidget::keyPressEvent(int keycode) {
  ProcessChar(keycode);
}

void TermWidget::setFont(int size) {
  fl_font(FL_COURIER,size);
  m_char_h = fl_height();
  m_char_w = fl_width("w");
}

int TermWidget::handle(int event) {
  switch(event) {
  case FL_PUSH:
    mousePressEvent(Fl::event_x(),Fl::event_y());
    return 1;
  case FL_DRAG:
    mouseDragEvent(Fl::event_x(),Fl::event_y());
    return 1;
  case FL_RELEASE:
    mouseReleaseEvent(Fl::event_x(),Fl::event_y());
    return 1;
  case FL_FOCUS:
    return 1;
  case FL_UNFOCUS:
    return 1;
  case FL_KEYBOARD:
    keyPressEvent(Fl::event_key());
    return 1;
  }
  return Fl_Widget::handle(event);
}

void TermWidget::mousePressEvent(int x, int y) {
  m_mousePressed = true;
  // Get the x and y coordinates of the mouse click - map that
  // to a row and column
  int clickcol = x/m_char_w;
  int clickrow = y/m_char_h;
  selectionStart = m_scroll_offset + clickcol + clickrow*m_width;
  selectionStart = TMIN(TMAX(0,selectionStart),m_width*m_scrollback-1);
  selectionStop = selectionStart;
}

void TermWidget::mouseDragEvent( int x, int y) {
  if (y < 0) 
    setScrollbar(m_scrollbar->value()-1);
  if (y > h())
    setScrollbar(m_scrollbar->value()+1);
  // Get the position of the click
  // to a row and column
  int clickcol = x/m_char_w;
  int clickrow = y/m_char_h;
  selectionStop = m_scroll_offset + clickcol + clickrow*m_width;
  selectionStop = TMIN(TMAX(0,selectionStop),m_width*m_scrollback-1);
  // clear the selection bits
  for (int i=0;i<m_width*m_scrollback;i++)
    m_history[i].clearSelection();
  // set the selection bits
  if (selectionStart < selectionStop)
    for (int i=selectionStart;i<selectionStop;i++)
      m_history[i].setSelection();
  else
    for (int i=selectionStop;i<selectionStart;i++)
      m_history[i].setSelection();
}

void TermWidget::mouseReleaseEvent( int x, int y) {
  m_mousePressed = false;
}

int main(int argc, char **argv) {
  Fl_Double_Window window(320,100);
  window.color(FL_WHITE);
  Fl_Group grp(0,0,320,100);
  grp.color(FL_WHITE);
  TermWidget term(0,0,304,100);
  Fl_Scrollbar scroller(304,0,16,100);
  term.setScroller(&scroller);
  scroller.callback(scroll_cb,&term);
  grp.resizable(term);
  window.resizable(grp);
  window.end();
  window.show();
  window.size(400,300);
  return Fl::run();
}
