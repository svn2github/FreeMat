#include "application.h"
#include <qapplication.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qfiledialog.h>
#include <qclipboard.h>
#include <iostream>
#include <qfontdialog.h>
#include <qsettings.h>
#include "Core.hpp"

#ifdef QT3
#define MAKEASCII(x) x.ascii()
#define QPM QPopupMenu
#define QK(x) x
#include <qpopupmenu.h>
#else
#define MAKEASCII(x) x.toAscii().constData()
#define QPM Q3PopupMenu
#define QK(x) Qt::x
#include <QCloseEvent>
#include <Q3PopupMenu>
#endif

#include "filesave.xpm"
#include "../libs/libXP/freemat-2.xpm"

ApplicationWindow::~ApplicationWindow() {
}

ApplicationWindow::ApplicationWindow() :
#ifdef QT3
  QMainWindow(0,NULL, WDestructiveClose | WGroupLeader) {
#else
  Q3MainWindow(0,NULL, Qt::WDestructiveClose | Qt::WGroupLeader) {
#endif
  QPixmap myIcon = QPixmap(freemat_2);
  setIcon(myIcon);

#ifdef QT3
  setCaption("FreeMat v2.0 Command Window");
#else
  setWindowTitle("FreeMat v2.0 Command Window");
#endif

  QPixmap saveIcon;
  saveIcon = QPixmap(filesave);
  QPM *file = new QPM(this);
  menuBar()->insertItem("&File", file);
  file->insertItem(saveIcon,"&Save Transcript",this,SLOT(save()),QK(CTRL)+QK(Key_S));
  file->insertItem("&Quit",qApp,SLOT(closeAllWindows()),QK(CTRL)+QK(Key_Q));
  QPM *edit = new QPM(this);
  menuBar()->insertItem("&Edit", edit);
  edit->insertItem("&Copy",this,SLOT(copy()),QK(CTRL)+QK(Key_C));
  edit->insertItem("&Paste",this,SLOT(paste()),QK(CTRL)+QK(Key_V));
  edit->insertSeparator();
  edit->insertItem("&Font",this,SLOT(font()),QK(CTRL)+QK(Key_F));
  QPM *help = new QPM(this);
  menuBar()->insertItem("&Help", help);
  help->insertItem("&About",this,SLOT(about()));
  help->insertItem("Online &Manual",this,SLOT(manual()),QK(Key_F1));
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
#ifdef QT3
  QSettings settings;
  settings.setPath("FreeMat","FreeMat");
  QString font = settings.readEntry("/terminal/font");
#else
  QSettings settings("FreeMat","FreeMat");
  QString font = settings.value("terminal/font").toString();
#endif
  if (!font.isNull()) {
    QFont new_font;
    new_font.fromString(font);
    m_term->setFont(new_font);
  }
  term->show();
}

void ApplicationWindow::save() {
#ifdef QT3
  QString fn = QFileDialog::getSaveFileName(QString::null, 
					    QString::null, this);
#else
  QString fn = QFileDialog::getSaveFileName();
#endif
  if (!fn.isEmpty()) {
    FILE *fp;
    fp = fopen(MAKEASCII(fn),"w");
    if (!fp) {
      char buffer[1000];
      sprintf(buffer,"Unable to save transcript to file %s!",
	      MAKEASCII(fn));
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
    const char *cp = MAKEASCII(text);
    while (*cp) 
      m_term->ProcessChar(*cp++);
  }
}

void ApplicationWindow::font() {
  QFont old_font = m_term->getFont();
  bool ok;
  QFont new_font = QFontDialog::getFont(&ok, old_font, this);
  if (ok) {
#ifdef QT3
    QSettings settings;
    settings.setPath("FreeMat","FreeMat");
    settings.writeEntry("/terminal/font",new_font.toString());
#else
    QSettings settings("FreeMat","FreeMat");
    settings.setValue("terminal/font",new_font.toString());
#endif
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
