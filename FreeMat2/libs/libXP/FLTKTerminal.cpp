#ifdef WIN32
#include <windows.h>
#include <htmlhelp.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <glob.h>
#include <unistd.h>
#endif

#include "FLTKTerminal.hpp"
#include "FL/Fl.H"
#include "FL/x.H"
#include "FL/Enumerations.H"
#include "FL/Fl_Widget.H"
#include "FL/Fl_Help_Dialog.H"
#include "FL/Fl_File_Chooser.H"
#include "WalkTree.hpp"
#include <algorithm>

const char *g_helppath;
static Fl_Help_Dialog* hlpd = NULL;
static Fl_File_Chooser* fc = NULL;


//The scrollback buffer length - later to be put in the prefs file
#ifdef __APPLE__
#define SCROLLBACK 500
#else
#define SCROLLBACK 5000
#endif
// \r handling

void FLTKTerminalWidget::blinkCB(void* data) {
  ((FLTKTerminalWidget*) data)->blink();
  Fl::add_timeout(1, FLTKTerminalWidget::blinkCB, data);
}

void FLTKTerminalWidget::blink() {
  if (blinkactive) {
    blinkon = !blinkon;
    int X, Y;
    position_to_xy(mCursorPos, &X, &Y);
    damage(1, X, Y, fl_width('W'), mMaxsize);
  }
  blinkactive = true;
}

Fl_Preferences app(Fl_Preferences::USER, "freemat", "main/font");

FLTKTerminalWidget::FLTKTerminalWidget(int x, int y, int w, int h, const char *label) : textInitialized(false),
  Fl_Text_Display(x,y,w,h,label) {
  textfont(FL_COURIER);
  textsize(12);
  textbuf = new Fl_Text_Buffer;
  buffer(textbuf);
  cursor_style(Fl_Text_Display::CARET_CURSOR);
  //  show_cursor();
  linecount = 0;
  m_width = 80;
  m_height = 25;
  Fl::add_timeout(1, FLTKTerminalWidget::blinkCB, this);
  blinkon = true;
  blinkactive = false;
  crflag = false;
}

#define LEFT_MARGIN 3
#define RIGHT_MARGIN 3

void FLTKTerminalWidget::draw() {
  int X, Y;
  position_to_xy(mCursorPos, &X, &Y);
  Fl_Text_Display::draw();
  fl_push_clip(text_area.x-LEFT_MARGIN,
	       text_area.y,
	       text_area.w+LEFT_MARGIN+RIGHT_MARGIN,
	       text_area.h);
  if (blinkon) {
    fl_color(textcolor());
    fl_rectf(X, Y, fl_width('W'), mMaxsize);
  } else {
    fl_color(color());
    fl_rectf(X, Y, fl_width('W'), mMaxsize);
    fl_color(textcolor());
    if (mCursorPos < buffer()->length())
      fl_draw(buffer()->text_range(mCursorPos,mCursorPos+1),
	      1, X, Y + mMaxsize - fl_descent());
  }
  fl_pop_clip();
}

FLTKTerminalWidget::~FLTKTerminalWidget() {
}


char* FLTKTerminalWidget::getLine(const char*prompt) {
  if (enteredLines.empty()) {
    m_prompt = prompt;
    promptlen = strlen(prompt);
    int line, col;
    position_to_linecol(buffer()->length(),&line,&col);
    if (col != 0) outputText("\n");
    buffer()->append(prompt);
    insert_position(buffer()->length());
    history_ptr = -1;
	enteredLinesChanged = false;
    while(!enteredLinesChanged)
      Fl::wait();
  }
  std::string theline(enteredLines.front());
  enteredLines.pop_front();
  char *cp = strdup(theline.c_str());
  return cp;
}

void FLTKTerminalWidget::RegisterInterrupt() {
  sigInterrupt(0);
}

int FLTKTerminalWidget::getTerminalWidth() {
  return m_width;
}

void FLTKTerminalWidget::adjustScrollPosition() {
  if (textInitialized) {
    int top;
    if (linecount > SCROLLBACK) {
      int startpos = 0;
      int stoppos = 0;
      for (int i=0;i<linecount-SCROLLBACK;i++)
	stoppos += buffer()->line_end(stoppos)+1;
      buffer()->remove(startpos,stoppos);
      linecount = SCROLLBACK;
    }
    top = linecount - m_height + 4;
    top = (top < 0) ? 0 : top;
    scroll(top,0);
    Fl::flush();
  }
}

void FLTKTerminalWidget::dopaste() {
  Fl::paste(*this,1);
}

void FLTKTerminalWidget::docopy() {
  if (!buffer()->selected()) return;
  const char *copy = buffer()->selection_text();
  if (!*copy) return;
#ifdef WIN32
  // Need to add '\r' to the '\n'
  int newline_count = 0;
  const char *cp = copy;
  while (*cp) {
    if (*cp == '\n') newline_count++;
    cp++;
  }
  cp = copy;
  char *cbuf = (char*) malloc(strlen(copy)+1+newline_count);
  memset(cbuf,0,strlen(copy)+1+newline_count);
  char *dp = cbuf;
  while (*cp) {
    if (*cp == '\n') {
      *dp++ = '\r';
      *dp++ = *cp;
    } else 
      *dp++ = *cp;
    cp++;
  }
  Fl::copy(cbuf,strlen(cbuf),1);
  free((void*) copy);
  free((void*) cbuf);
#else
  if (*copy) Fl::copy(copy,strlen(copy),1);
  free((void*) copy);
#endif
}

// The output logic works like this...  We start copying
// text to the temporary buffer.  If we encounter a 
// bare new line or run out of characters, we write the
// temporary buffer to the output.  If we encounter a
// \r, we don't output it, and set the crflag.  If
// So the logic is:
//
//   crflag = true
//      character == '\n'? clear crflag
//      character == '\r'? crflag = true
//      character == other: delete current line, crflag = false
//   crflag = false
//      character == '\n'? flush buffer
//      character == '\r'? flush buffer, crflag = true
//      character == other: add char to buffer, continue
void FLTKTerminalWidget::outputText(const char *txt) {
  char *buf;
  bool linecount_changed;
  buf = (char*) malloc(strlen(txt)+1);
  memset(buf,0,strlen(txt)+1);
  linecount_changed = false;
  const char *cp = txt;
  char *dp = buf;
  while (*cp) {
    if (crflag) {
      if (*cp == '\n') {
	buffer()->append("\n");
	crflag = false;
	linecount++;
	linecount_changed = true;
      } else if (*cp == '\r')
	crflag = true;
      else {
	crflag = false;
	buffer()->replace(buffer()->line_start(buffer()->length()),
			  buffer()->line_end(buffer()->length()),"");
	memset(buf,0,strlen(txt)+1);
	dp = buf;
	*dp++ = *cp;
      }
    } else {
      if (*cp == '\n') {
	*dp = '\n';
	buffer()->append(buf);
	memset(buf,0,strlen(txt)+1);
	dp = buf;
	linecount++;
	linecount_changed = true;
      } else if (*cp == '\r') {
	buffer()->append(buf);
	memset(buf,0,strlen(txt)+1);
	dp = buf;
	crflag = true;
      } else {
	*dp++ = *cp;
      }
    }
    cp++;
  }
  buffer()->append(buf);
  //  insert_position(buffer()->length());
  if (linecount_changed) 
    adjustScrollPosition();
  textInitialized = true;
  blinkactive = false;
  blinkon = true;
}

void FLTKTerminalWidget::resize(int X, int Y, int W, int H) {
  Fl_Text_Display::resize(X,Y,W,H);
  if (textInitialized) {
    m_width = w()/fl_width('W');
    m_height = h()/fl_height();
  }
}

void FLTKTerminalWidget::outputMessage(const char*msg) {
  outputText(msg);
}

void FLTKTerminalWidget::errorMessage(const char*msg) {
  outputText("Error: ");
  outputText(msg);
  outputText("\n");
}

void FLTKTerminalWidget::warningMessage(const char*msg) {
  outputText("Warning: ");
  outputText(msg);
  outputText("\n");
}

void FLTKTerminalWidget::adjustInsertPosition() {
int line, col;
position_to_linecol(insert_position(),&line,&col);
if ((linecount+1) != line)
    insert_position(buffer()->length());
  adjustScrollPosition();
}

int FLTKTerminalWidget::leftkey() {
  int line, col;
  adjustInsertPosition();
  position_to_linecol(insert_position(),&line,&col);
  if (col > promptlen)
    insert_position(insert_position()-1);
  return 1;
}

int FLTKTerminalWidget::rightkey() {
  adjustInsertPosition();
  if (insert_position() < buffer()->length())
    insert_position(insert_position()+1);
  return 1;
}

int FLTKTerminalWidget::ctrlc() {
  printf("CTRL-C received!!\r\n");
  sigInterrupt(1);
  return 1;
}

int FLTKTerminalWidget::ctrla() {
  adjustInsertPosition();
  insert_position(buffer()->line_start(insert_position()) + promptlen);
  return 1;
}

int FLTKTerminalWidget::ctrld() {
  adjustInsertPosition();
  buffer()->remove(insert_position(),insert_position()+1);
  return 1;
}

int FLTKTerminalWidget::ctrle() {
  adjustInsertPosition();
  insert_position(buffer()->length());
  return 1;
}

int FLTKTerminalWidget::ctrlk() {
  adjustInsertPosition();
  int cutin, cutout;
  cutin = insert_position();
  cutout = buffer()->line_end(insert_position());
  cutbuff = buffer()->text_range(cutin,cutout);
  buffer()->remove(cutin,cutout);
  return 1;
}

int FLTKTerminalWidget::ctrly() {
  adjustInsertPosition();
  buffer()->insert(insert_position(),cutbuff.c_str());
  insert_position(insert_position()+cutbuff.size());
  return 1;
}

int FLTKTerminalWidget::tab() {
  int line, col;
  position_to_linecol(insert_position(),&line,&col);
  if ((col > promptlen) && (buffer()->character(insert_position()-1) != ' ') &&
      (buffer()->character(insert_position()-1) != '\t'))
    CompleteWord();
  else {
    buffer()->insert(insert_position(),"\t");
    insert_position(insert_position()+1);
  }
  return 1;
}

int FLTKTerminalWidget::upkey() {
  char tbuf[4096];
  adjustInsertPosition();
  if (history.size() == 0) return 1;
  if (history_ptr == -1)
    history_ptr = history.size() - 2;
  else {
    if (history_ptr == 0)
      history_ptr = history.size();
    history_ptr--;
  }
  sprintf(tbuf,"%s%s",m_prompt,history[history_ptr].c_str());
  tbuf[strlen(tbuf)-1] = 0;
  buffer()->replace(line_start(insert_position()),
		    line_start(insert_position())+strlen(tbuf),
		    tbuf);
  buffer()->remove(line_start(insert_position())+strlen(tbuf),
		   buffer()->line_end(insert_position()));
  return 1;
}

int FLTKTerminalWidget::downkey() {
  char tbuf[4096];
  adjustInsertPosition();
  if (history.size() == 0) return 1;
  history_ptr++;
  if (history_ptr >= history.size())
    history_ptr = 0;
  sprintf(tbuf,"%s%s",m_prompt,history[history_ptr].c_str());
  tbuf[strlen(tbuf)-1] = 0;
  buffer()->replace(line_start(insert_position()),
		    line_start(insert_position())+strlen(tbuf),
		    tbuf);
  buffer()->remove(line_start(insert_position())+strlen(tbuf),
		   buffer()->line_end(insert_position()));
  return 1;
}

int FLTKTerminalWidget::backspace() {
  int line, col;
  adjustInsertPosition();
  position_to_linecol(insert_position(),&line,&col);
  if (col > promptlen)
    buffer()->remove(insert_position()-1,insert_position());
  return 1;
}

int FLTKTerminalWidget::enter() {
  char tbuf[4096];
  adjustInsertPosition();
  strcpy(tbuf,buffer()->line_text(insert_position()));
  strcat(tbuf,"\n");
  enteredLines.push_back(tbuf+promptlen);
  outputText("\n");
  if (strlen(tbuf+promptlen) > 1) {
    if (history.size() > 0)
      history.pop_back();
    history.push_back(tbuf+promptlen);
    history.push_back(" ");
  }
  promptlen = 0;
  enteredLinesChanged = true;
  return 1;
}

void FLTKTerminalWidget::fontsize(int sze) {
  sizetext = sze;
  textsize(sze);
  redraw();
  app.set("textsize",sizetext);
}

int FLTKTerminalWidget::handleascii(int key) {
  adjustInsertPosition();
  if ((key>=32) && (key <129)) {
	buffer()->insert(insert_position(),Fl::event_text());
    insert_position(insert_position()+1);
  } 
  blinkactive = false;
  blinkon = true;
  return 1;
}

int FLTKTerminalWidget::handle_key() {
  int key;
  key = Fl::event_key();
  if (((key == 'a') && (Fl::event_state() & FL_CTRL)) ||
      (key == FL_Home))
    return ctrla();
  if (((key == 'e') && (Fl::event_state() & FL_CTRL)) ||
      (key == FL_End))
    return ctrle();
  if ((key == 'd') && (Fl::event_state() & FL_CTRL))
    return ctrld();
  if ((key == 'k') && (Fl::event_state() & FL_CTRL))
    return ctrlk();
  if ((key == 'y') && (Fl::event_state() & FL_CTRL))
    return ctrly();
  if ((key == 'c') && (Fl::event_state() & FL_CTRL))
    return ctrlc();
  if ((key >= FL_KP) && (key <= FL_KP_Last))
    key -= FL_KP;
  switch(key) {
  case FL_Tab:
    return tab();
    break;
  case FL_BackSpace:    
    return backspace();
    break;
  case FL_Enter:
    return enter();
    break;
  case FL_Left:
    return leftkey();
    break;
  case FL_Right:
    return rightkey();
    break;
  case FL_Up:
    return upkey();
    break;
  case FL_Down:
    return downkey();
    break;
  case FL_Page_Up:
    for (int i=0;i<m_height;i++)
      move_up();
    show_insert_position();
    break;
  case FL_Page_Down:
    for (int i=0;i<m_height;i++)
      move_down();
    show_insert_position();
    break;
  default: 
    handleascii(key);
  }
  return 1;
}


void FLTKTerminalWidget::ListCompletions(std::vector<std::string> completions) {
  int maxlen;    /* The length of the longest matching string */
  int width;     /* The width of a column */
  int ncol;      /* The number of columns to list */
  int nrow;      /* The number of rows needed to list all of the matches */
  int row,col;   /* The row and column being written to */
  int i;
  /*
   * Not enough space to list anything?
   */
  if(m_width < 1)
    return;
  /*
   * Work out the maximum length of the matching strings.
   */
  maxlen = 0;
  for(i=0; i<completions.size(); i++) {
    int len = completions[i].length();
    if(len > maxlen)
      maxlen = len;
  };
  /*
   * Nothing to list?
   */
  if(maxlen == 0)
    return;
  /*
   * Split the available terminal width into columns of maxlen + 2 characters.
   */
  width = maxlen + 2;
  ncol = m_width / width;
  /*
   * If the column width is greater than the terminal width, the matches will
   * just have to overlap onto the next line.
   */
  if(ncol < 1)
    ncol = 1;
  /*
   * How many rows will be needed?
   */
  nrow = (completions.size() + ncol - 1) / ncol;
  /*
   * Print the matches out in ncol columns, sorted in row order within each
   * column.
   */
  for(row=0; row < nrow; row++) {
    for(col=0; col < ncol; col++) {
      int m = col*nrow + row;
      if(m < completions.size()) {
	char buffer[4096];
	sprintf(buffer, "%s%-*s%s", completions[m].c_str(),
		(int) (ncol > 1 ? maxlen - completions[m].length():0),
		"", col<ncol-1 ? "  " : "\n");
	outputText(buffer);
      } else {
	outputText("\n");
	break;
      };
    };
  };
}

//FIXME - move to separate file.
static std::string GetCommonPrefix(std::vector<std::string> matches,
			    std::string tempstring) {
  int minlength;
  int prefixlength;
  bool allmatch;
  std::string templ;
  int i, j;

  minlength = matches[0].size();
  for (i=0;i<matches.size();i++)
    minlength = (minlength < matches[i].size()) ? 
      minlength : matches[i].size();
  prefixlength = minlength;
  templ = matches[0];
  for (i=0;i<matches.size();i++) {
    j = 0;
    allmatch = true;
    while (allmatch && (j<prefixlength)) {
      std::string mtch(matches[i]);
      allmatch = (mtch[j] == templ[j]);
      if (allmatch) j++;
    }
    prefixlength = (j < prefixlength) ? j : prefixlength;
  }
  if (prefixlength <= tempstring.length())
    return (std::string(""));
  else
    return(templ.substr(tempstring.length(),prefixlength-tempstring.length()));
}

void FLTKTerminalWidget::CompleteWord() {
  int redisplay = 0;
  int suffix_len;         /* The length of the completion extension */
  int cont_len;           /* The length of any continuation suffix */
  char *line_start;
  int relative_pos;
  int nextra;

  line_start = buffer()->line_text(insert_position());
  relative_pos = insert_position() - buffer()->line_start(insert_position());

  std::vector<std::string> matches;
  /*
   * Perform the completion.
   */
  std::string tempstring;
  matches = GetCompletions(line_start,relative_pos,tempstring);
  if(matches.size() >= 1) {
    /*
     * If there any ambiguous matches, report them, starting on a new line.
     */
    if(matches.size() > 1) {
      outputText("\n");
      ListCompletions(matches);
      buffer()->append(line_start);
      insert_position(relative_pos+buffer()->line_start(buffer()->length()));
    };
    /*
     * Find the common prefix
     */
    std::string prefix;
    prefix = GetCommonPrefix(matches, tempstring);
    /*
     * Get the length of the suffix and any continuation suffix to add to it.
     */
    suffix_len = prefix.length(); // This is supposed to be the length of the filename extension...
    cont_len = 0;
    /*
     * Work out the number of characters that are to be added.
     */
    nextra = suffix_len + cont_len;
    /*
     * Is there anything to be added?
     */
    buffer()->insert(insert_position(),prefix.c_str());
    insert_position(insert_position()+suffix_len);
  }
  return;
}



int FLTKTerminalWidget::handle(int event) {
	if (event == FL_PASTE) {
		if (!Fl::event_text()) {
			return 1;
		}
		const char *cp = Fl::event_text();
		while (*cp) {
			if (*cp == '\n') {
				enter();
				cp++;
			} else {
				char tbuf[2];
				tbuf[0] = *cp++;
				tbuf[1] = 0;
			    adjustInsertPosition();
               	buffer()->insert(insert_position(),tbuf);
				insert_position(insert_position()+1);
			}
		}
		return 1;
	}
  if (event == FL_KEYBOARD) {
    return handle_key();
  }
  return(Fl_Text_Display::handle(event));
}

void quit_cb(Fl_Widget*, void*) {
  exit(0);
}

void save_cb(Fl_Widget*, void* w) {
  char *newfile;
  newfile = fl_file_chooser("Save To Text File", "*", NULL);
  if (newfile) {
    FILE *fp = fopen(newfile,"w");
    if (fp) {
      FLTKTerminalWindow *win = (FLTKTerminalWindow*) w;
      const char *txt = win->term()->buffer()->text();
      fwrite(txt,sizeof(char),strlen(txt),fp);
      fclose(fp);
    }
  }
}

void help_cb(Fl_Widget*, void*) {
#if defined(WIN32)
  HtmlHelp(NULL,g_helppath,HH_DISPLAY_TOC,NULL);
#elif defined(__APPLE__)
  // Help is an HTML file 
  AHGotoPage(NULL,
	     CFStringCreateWithBytes(NULL,g_helppath,
				     strlen(g_helppath),0,false),
	     NULL);
#else
  if (hlpd)
    hlpd->show();
  else {
    hlpd = new Fl_Help_Dialog;
    hlpd->load(g_helppath);
    hlpd->show();
  }
#endif
}

void close_about_cb(Fl_Widget*, void *w) {
  Fl_Window *win = (Fl_Window*) w;
  win->hide();
}

void about_cb(Fl_Widget*, void*) {
  Fl_Window *about_dlg = new Fl_Window(240,115,"About FreeMat");
  Fl_Box *box1 = new Fl_Box(57,20,120,14,"FreeMat Version 1.10");
  box1->align(FL_ALIGN_CENTER);
  Fl_Box *box2 = new Fl_Box(57,38,120,14,"Copyright 2002-2005 by Samit Basu");
  box2->align(FL_ALIGN_CENTER);
  Fl_Box *box3 = new Fl_Box(57,56,120,14,"http://freemat.sf.net");
  box3->align(FL_ALIGN_CENTER);
  Fl_Button *ok_button = new Fl_Button(82, 78, 50, 20, "OK");
  ok_button->callback(close_about_cb,about_dlg);
  about_dlg->end();
  about_dlg->show();
}

void copy_cb(Fl_Widget*, void* w) {
  FLTKTerminalWindow *win = (FLTKTerminalWindow*) w;
  win->term()->docopy();
}

void paste_cb(Fl_Widget*, void*w) {
  FLTKTerminalWindow *win = (FLTKTerminalWindow*) w;
  win->term()->dopaste();
}

void font_cb(Fl_Widget* wb, void*w) {
  Fl_Menu_* mw = (Fl_Menu_*)wb;
  const Fl_Menu_Item* m = mw->mvalue();
  FLTKTerminalWindow *win = (FLTKTerminalWindow*) w;
  if (m) {
    int fontsize;
    fontsize = atoi(m->label());
    win->term()->fontsize(fontsize);
  }
}

Fl_Menu_Item menuitems[] = {
{ "&File", 0, 0, 0, FL_SUBMENU },
{ "&Save Transcript", 0, (Fl_Callback *)save_cb },
{ "E&xit", 0, (Fl_Callback *)quit_cb, 0 },
{ 0 },
{ "&Edit", 0, 0, 0, FL_SUBMENU },
{ "&Copy",  0, (Fl_Callback *)copy_cb },
{ "&Paste",  0 , (Fl_Callback *)paste_cb },
{ "&Font Size", 0, 0, 0, FL_SUBMENU },
{ "8", 0, (Fl_Callback *)font_cb , (void *) 8, FL_MENU_RADIO},
{ "10", 0, (Fl_Callback *)font_cb , (void *) 10, FL_MENU_RADIO},
{ "12", 0, (Fl_Callback *)font_cb , (void *) 12, FL_MENU_RADIO|FL_MENU_VALUE},
{ "14", 0, (Fl_Callback *)font_cb , (void *) 14, FL_MENU_RADIO},
{ "16", 0, (Fl_Callback *)font_cb , (void *) 16, FL_MENU_RADIO},
{ "18", 0, (Fl_Callback *)font_cb , (void *) 18, FL_MENU_RADIO},
{ "20", 0, (Fl_Callback *)font_cb , (void *) 20, FL_MENU_RADIO},
{ 0 },
{ 0 },
{ "&Help", 0, 0, 0, FL_SUBMENU },
{ "&Online Help",  0, (Fl_Callback *)help_cb },
{ "&About FreeMat", 0, (Fl_Callback *)about_cb },
{ 0 },
{ 0 }
};

void quit_check_cb(Fl_Widget*, void*) {
  if (!fl_ask("Are you sure you want to quit?")) return;
  exit(0);
}

FLTKTerminalWindow::FLTKTerminalWindow(int w, int h, const char* t, const char *help_path) :
  Fl_Double_Window(w,h,t) {
  g_helppath = help_path;
  begin();
  m_menu = new Fl_Menu_Bar(0,0,w,30);
  m_menu->box(FL_NO_BOX);
  m_menu->copy(menuitems,this);
  m_term = new FLTKTerminalWidget(0,30,w,h-30);
  int sizetext;
  app.get("textsize",sizetext,12);
  char buffer[100];
  sprintf(buffer,"Edit/Font Size/%d",sizetext);
  for (int k=0;k<6;k++)
    m_menu->mode(8+k,FL_MENU_RADIO);
  m_menu->mode(8+(sizetext-8)/2,FL_MENU_RADIO|FL_MENU_VALUE);
  //  const Fl_Menu_Item *p = m_menu->find_item("buffer");
  //  printf("menu item is %x\r\n",p);
  //  if (p)
  //    p->setonly();
  m_term->fontsize(sizetext);
  end();
  resizable(m_term);
  callback(quit_check_cb);
}

FLTKTerminalWindow::~FLTKTerminalWindow() {
}

FLTKTerminalWidget* FLTKTerminalWindow::term() {
  return m_term;
}
