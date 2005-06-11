#define CURSORBIT 1
#define SELECTBIT 2
#include <string>

class tagChar
{
 public:
  char v;
  char flags;
  tagChar(): v(' '), flags(0) {};
  tagChar(char t): v(t), flags(0) {};
  bool noflags() {return(flags == 0);};
  bool cursor() {return(flags & CURSORBIT);};
  void toggleCursor() {flags ^= CURSORBIT;};
  void setCursor() {flags |= CURSORBIT;};
  void clearCursor() {flags &= ~CURSORBIT;};
  void clearSelection() {flags &= ~SELECTBIT;};
  void setSelection() {flags |= SELECTBIT;};
  bool operator == (const tagChar& b) {return (v==b.v) && (flags==b.flags);};
};

class TermWidget {
protected:
  tagChar* m_surface;  // width x height
  int m_height;        // height of terminal in characters
  int m_width;         // width of terminal in characters
  int m_cursor_x;      // column position of the cursor
  int m_cursor_y;      // row position of the cursor
  tagChar* m_onscreen; // width x height - contains contents of the screen
  tagChar* m_history;  // width x scrollheight - contains history of terminal
  int m_char_w;        // width of a character
  int m_char_h;        // height of a character
  int m_char_decent;   // descent of a character
  int m_active_width;  // width of the active text region.
  bool m_blink_skip;
  bool m_clearall;
  bool cursorOn;
  bool blinkEnable;
  int m_scrollback;
  int m_history_lines;
  bool m_scrolling;
  bool m_mousePressed;
  int selectionStart;
  int selectionStop;
  int m_scroll_offset;
  int m_scrollmin, m_scrollmax, m_scrollline, m_scrollpage;
  bool m_firsttime;
 public:
  void scrollBack(int val);
  TermWidget();
  virtual ~TermWidget();
  void resizeTextSurface();
  //  virtual void ProcessChar(int c) = 0;
  void PutString(std::string txt);
  void blink();
  void setCursor(int x, int y);
  int  getTextWidth() {return m_width;};
  void Initialize();
  //TK dependant functions
public:
  void setScrollbar(int val);
  void OnResize();
  void OnKeyPress(int key);
  void OnMouseDown(int x, int y);
  void OnMouseDrag(int x, int y);
  void OnMouseUp(int x, int y);
  void OnScroll(int val);
  void DrawContent();
  
  virtual int GetHeight() = 0;
  virtual int GetWidth() = 0;
  virtual void InstallEventTimers() = 0;
  virtual void ScrollLineUp() = 0;
  virtual void ScrollLineDown() = 0;
  virtual void SetScrollBarValue(int val) = 0;
  virtual void SetupScrollBar(int minval, int maxval, int step, int page, int val) = 0;
  virtual void BeginDraw() = 0;
  virtual void PutTagChar(int x, int y, tagChar g) = 0;
  virtual void EndDraw() = 0;
  virtual void setFont(int size) = 0;
};

