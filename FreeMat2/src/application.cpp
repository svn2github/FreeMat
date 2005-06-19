#include "application.h"
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qfiledialog.h>
#include <qclipboard.h>
#include <iostream>
#include <qfontdialog.h>
#include <qsettings.h>
#include "Core.hpp"

#include "filesave.xpm"
#include "../libs/libXP/freemat-2.xpm"

ApplicationWindow::~ApplicationWindow() {
}

ApplicationWindow::ApplicationWindow() :
  QMainWindow(0,"FreeMat v2.0", WDestructiveClose | WGroupLeader) {
  QPixmap myIcon = QPixmap(freemat_2);
  setIcon(myIcon);

  QPixmap saveIcon;
  saveIcon = QPixmap(filesave);
  QPopupMenu *file = new QPopupMenu(this);
  menuBar()->insertItem("&File", file);
  file->insertItem(saveIcon,"&Save Transcript",this,SLOT(save()),CTRL+Key_S);
  file->insertItem("&Quit",qApp,SLOT(closeAllWindows()),CTRL+Key_Q);
  QPopupMenu *edit = new QPopupMenu(this);
  menuBar()->insertItem("&Edit", edit);
  edit->insertItem("&Copy",this,SLOT(copy()),CTRL+Key_C);
  edit->insertItem("&Paste",this,SLOT(paste()),CTRL+Key_V);
  edit->insertSeparator();
  edit->insertItem("&Font",this,SLOT(font()),CTRL+Key_F);
  QPopupMenu *help = new QPopupMenu(this);
  menuBar()->insertItem("&Help", help);
  help->insertItem("&About",this,SLOT(about()));
  help->insertItem("Online &Manual",this,SLOT(manual()),Key_F1);
  help->insertItem("About &Qt", this, SLOT(aboutQt()));
}

void ApplicationWindow::closeEvent(QCloseEvent* ce) {
  ce->accept();
  exit(0);
  return;
}

void ApplicationWindow::SetGUITerminal(GUITerminal* term) {
  m_term = term;
  setCentralWidget(term);
  setMinimumSize(400,300);
  QSettings settings;
  settings.setPath("FreeMat","FreeMat");
  QString font = settings.readEntry("/terminal/font");
  if (!font.isNull()) {
    QFont new_font;
    new_font.fromString(font);
    m_term->setFont(new_font);
  }
  term->show();
}

void ApplicationWindow::save() {
  QString fn = QFileDialog::getSaveFileName(QString::null, 
					    QString::null, this);
  if (!fn.isEmpty()) {
    FILE *fp;
    fp = fopen(fn.ascii(),"w");
    if (!fp) {
      char buffer[1000];
      sprintf(buffer,"Unable to save transcript to file %s!",
	      fn.ascii());
      QMessageBox::information(this,"Error",buffer,
			       QMessageBox::Ok);
      return;
    }
    int history_count;
    int width;
    char *textbuffer;
    textbuffer = m_term->getTextSurface(history_count, width);
    char *linebuf = new char[width+1];
    for (int i=0;i<history_count;i++) {
      // scan backwards for last non ' ' char
      int j=width-1;
      while ((j>0) && (textbuffer[i*width+j] == ' '))
	j--;
      j++;
      memcpy(linebuf,textbuffer+i*width,j*sizeof(char));
      linebuf[j] = 0;
#ifdef WIN32
      fprintf(fp,"%s\r\n",linebuf);
#else
      fprintf(fp,"%s\n",linebuf);
#endif
    }
    fclose(fp);
  }
}

void ApplicationWindow::copy() {
  char *copytextbuf = m_term->getSelectionText();
  if (!copytextbuf) return;
  QClipboard *cb = QApplication::clipboard();
  cb->setText(copytextbuf, QClipboard::Clipboard);
  free(copytextbuf);
}

void ApplicationWindow::paste() {
  QClipboard *cb = QApplication::clipboard();
  QString text;
  if (cb->supportsSelection())
    text = cb->text(QClipboard::Selection);
  if (text.isNull())
    text = cb->text(QClipboard::Clipboard);
  if (!text.isNull()) {
    const char *cp = text.ascii();
    while (*cp) 
      m_term->ProcessChar(*cp++);
  }
}

void ApplicationWindow::font() {
  QFont old_font = m_term->getFont();
  bool ok;
  QFont new_font = QFontDialog::getFont(&ok, old_font, this);
  if (ok) {
    QSettings settings;
    settings.setPath("FreeMat","FreeMat");
    settings.writeEntry("/terminal/font",new_font.toString());
    m_term->setFont(new_font);
  }
}

void ApplicationWindow::about() {
}

void ApplicationWindow::manual() {
  ArrayVector dummy;
  HelpWinFunction(0,dummy);
}

void ApplicationWindow::aboutQt() {
  QMessageBox::aboutQt(this, "FreeMat");
}
