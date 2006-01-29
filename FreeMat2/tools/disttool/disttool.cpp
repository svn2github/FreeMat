#include "disttool.hpp"
#include <QtGui>

QTextEdit *m_text;

void TermOutputText(QString str) {
  m_text->insertPlainText(str);
  m_text->ensureCursorVisible();
  qApp->processEvents();
}

void Halt(QString emsg) {
  TermOutputText(emsg);
  QEventLoop m_loop;
  m_loop.exec();  
  exit(0);
}

void MakeDir(QString dir) {
  QDir d;
  TermOutputText("Making Directory " + dir + "\n");
  d.mkpath(dir);
}

void CopyFile(QString src, QString dest) {
  TermOutputText("Copying " + src + " to " + dest + "\n");
  QFile::copy(src,dest);
}

void CopyDirectory(QString src, QString dest) {
  TermOutputText("Copying Directory " + src + " to " + dest + "\n");
  QDir dir(src);
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir())
      CopyDirectory(fileInfo.absoluteFilePath(),dest);
    else
      CopyFile(fileInfo.absoluteFilePath(),dest+"/"+fileInfo.fileName());
  }
}

ConsoleWidget::ConsoleWidget() : QWidget() {
  m_text = new QTextEdit;
  m_text->setReadOnly(true);
  resize(600,400);
  m_text->setFontFamily("Courier");
  QPushButton *winb = new QPushButton("Win32");
  QWidget::connect(winb,SIGNAL(clicked()),this,SLOT(WinBundle()));
  QPushButton *linuxb = new QPushButton("Linux");
  QWidget::connect(linuxb,SIGNAL(clicked()),this,SLOT(LinuxBundle()));
  QPushButton *macb = new QPushButton("Mac");
  QWidget::connect(macb,SIGNAL(clicked()),this,SLOT(MacBundle()));
  QPushButton *quit = new QPushButton("Quit");
  QWidget::connect(quit,SIGNAL(clicked()),this,SLOT(exitNow()));
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(m_text);
  QWidget *buttons = new QWidget;
  QHBoxLayout *hlayout = new QHBoxLayout;
  hlayout->addWidget(winb);
  hlayout->addWidget(linuxb);
  hlayout->addWidget(macb);
  hlayout->addWidget(quit);
  buttons->setLayout(hlayout);
  layout->addWidget(buttons);
  setLayout(layout);
}


QStringList GetFileList(QString src,QStringList lst) {
  QDir dir(src);
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir())
      lst = GetFileList(fileInfo.absoluteFilePath(),lst);
    else {
      if ((fileInfo.suffix() != "nsi") && (fileInfo.suffix() != "in")) {
	QString fname(fileInfo.absoluteFilePath());
	int n = fname.indexOf("Contents/",Qt::CaseInsensitive);
	fname.remove(0,n);
	fname.replace("/","\\");
	lst += fname;
      }
    }
  }
  return lst;
}

void ConsoleWidget::WinBundle() {
  QFile vfile("../helpgen/version.txt");
  if (!vfile.open(QFile::ReadOnly))
    Halt("Unable to open ../helpgen/version.txt for input\n");
  QTextStream g(&vfile);
  QString versionnum(g.readLine(0));
  MakeDir("FreeMat");
  MakeDir("FreeMat/Contents");
  MakeDir("FreeMat/Contents/bin");
  MakeDir("FreeMat/Contents/Resources");
  MakeDir("FreeMat/Contents/Resources/help");
  MakeDir("FreeMat/Contents/Resources/help/html");
  MakeDir("FreeMat/Contents/Resources/help/text");
  MakeDir("FreeMat/Contents/Resources/mfiles");
  CopyFile("../../Release/FreeMat.exe","FreeMat/Contents/bin/FreeMat.exe");
  CopyFile("c:/Qt/4.1.0/bin/QtCore4.dll","FreeMat/Contents/bin/QtCore4.dll");
  CopyFile("c:/Qt/4.1.0/bin/QtGui4.dll","FreeMat/Contents/bin/QtGui4.dll");
  CopyFile("c:/Qt/4.1.0/bin/QtOpenGL4.dll","FreeMat/Contents/bin/QtOpenGL4.dll");
  CopyFile("../../extern/blas/atlas_prebuilt_win32/atlas_blas_P4SSE2.dll",
	   "FreeMat/Contents/bin/blas.dll");
  CopyFile("c:/MinGW/bin/mingwm10.dll","FreeMat/Contents/bin/mingwm10.dll");
  CopyDirectory("../helpgen/html","FreeMat/Contents/Resources/help/html");
  CopyDirectory("../helpgen/text","FreeMat/Contents/Resources/help/text");
  CopyDirectory("../helpgen/MFiles","FreeMat/Contents/Resources/mfiles");
  TermOutputText("Generating NSI file...\n");
  QStringList flist(GetFileList("FreeMat",QStringList()));
  QFile *file_in = new QFile("freemat_nsi.in");
  if (!file_in->open(QFile::ReadOnly))
    Halt("Unable to open FreeMat NSIS template file freemat_nsi.in\n");
  QFile *file_out = new QFile("FreeMat/freemat.nsi");
  if (!file_out->open(QFile::WriteOnly))
    Halt("Unable to open FreeMat NSIS file freemat.nsi for output\n");
  QTextStream *g_in = new QTextStream(file_in);
  QTextStream *g_out = new QTextStream(file_out);
  QStringList dirDel;
  while (!g_in->atEnd()) {
    QString line(g_in->readLine(0));
    if (line.indexOf("<BUNDLE FILES>") >= 0) {
      QString path;
      for (int i=0;i<flist.size();i++) {
	QString fname(flist[i]);
	int pndx = fname.lastIndexOf("\\");
	QString tpath(fname.left(pndx));
	if (path != tpath) {
	  *g_out << "SetOutPath \"$INSTDIR\\"+tpath+"\"\n";
	  path = tpath;
	  dirDel << path;
	}
	*g_out << "  File \"" + flist[i] + "\"\n";
      }
    } else if (line.indexOf("<DELLIST>") >= 0) {
      for (int i=0;i<flist.size();i++) 
	*g_out << "  Delete \"$INSTDIR\\" + flist[i] << "\"\n";
    } else if (line.indexOf("<VERSION_NUMBER>") >= 0) {
      line.replace("<VERSION_NUMBER>",versionnum);
      *g_out << line + "\n";
    } else
      *g_out << line + "\n";
  }
  delete g_in;
  delete g_out;
  delete file_in;
  delete file_out;
  TermOutputText("\n\nDone\n");
}

void ConsoleWidget::LinuxBundle() {
}

void ConsoleWidget::MacBundle() {
}

void ConsoleWidget::exitNow() {
  exit(1);
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  ConsoleWidget *m_main = new ConsoleWidget;
  m_main->show();
  return app.exec();
  return 0;
}
