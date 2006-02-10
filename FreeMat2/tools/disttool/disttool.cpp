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
  QChar last(src.at(src.size()-1));
  QFileInfo fi(dest);
  QDir dir;
  dir.mkpath(fi.absolutePath());
  QFile::copy(src,dest);
}

void CopyDirectory(QString src, QString dest) {
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
  //   QPushButton *winb = new QPushButton("Win32");
  //   QWidget::connect(winb,SIGNAL(clicked()),this,SLOT(WinBundle()));
  //   QPushButton *linuxb = new QPushButton("Linux");
  //   QWidget::connect(linuxb,SIGNAL(clicked()),this,SLOT(LinuxBundle()));
  //   QPushButton *macb = new QPushButton("Mac");
  //   QWidget::connect(macb,SIGNAL(clicked()),this,SLOT(MacBundle()));
  //   QPushButton *quit = new QPushButton("Quit");
  //   QWidget::connect(quit,SIGNAL(clicked()),this,SLOT(exitNow()));
  QVBoxLayout *layout = new QVBoxLayout;
  //   QWidget *buttons = new QWidget;
  //   QHBoxLayout *hlayout = new QHBoxLayout;
  //   hlayout->addWidget(winb);
  //   hlayout->addWidget(linuxb);
  //   hlayout->addWidget(macb);
  //   hlayout->addWidget(quit);
  //   buttons->setLayout(hlayout);
  //   layout->addWidget(buttons);
  layout->addWidget(m_text);
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
#ifdef WIN32
	fname.replace("/","\\");
#endif
	lst += fname;
      }
    }
  }
  return lst;
}

void Execute(QString fname, QStringList args) {
  QProcess exec;
  exec.start(fname,args);
  if (!exec.waitForFinished(60000))
    Halt(fname + " did not finish");
  TermOutputText(exec.readAllStandardOutput());
}

QString ExecuteAndCapture(QString fname, QStringList args) {
  QProcess exec;
  exec.start(fname,args);
  if (!exec.waitForFinished(60000))
    Halt(fname + " did not finish");
  QByteArray outtxt(exec.readAllStandardOutput());
  QString outtxt_string(outtxt);
  return outtxt_string;
}

void Relink(QString frame, QString file) {
  QString qtdir(getenv("QTDIR"));
  Execute("install_name_tool",QStringList() << "-change" << qtdir+"/lib/"+frame+".framework/Versions/4.0/"+frame << "@executable_path/../Frameworks/"+frame+".framework/Versions/4.0/"+frame << file);
}

void InstallFramework(QString frame) {
  QString qtdir(getenv("QTDIR"));
  TermOutputText("Installing Framework: "+frame+"\n");
  Execute("cp",QStringList() << "-R" << qtdir+"/lib/"+frame+".framework" << "../../build/FreeMat.app/Contents/Frameworks/"+frame+".framework");
  //  CopyDirectory(qtdir+"/lib/"+frame+".framework","../../FreeMat.app/Contents/Frameworks/"+frame+".framework");
  Execute("install_name_tool",QStringList() << "-id" << "@executable_path/../Frameworks/"+frame+".framework/Versions/4.0/"+frame << "../../build/FreeMat.app/Contents/Frameworks/"+frame+".framework/Versions/4.0/"+frame);
  Relink(frame,"../../build/FreeMat.app/Contents/MacOs/FreeMat");
}

void CrossLinkFramework(QString dframe, QString lframe) {
  QString qtdir(getenv("QTDIR"));
  TermOutputText("Crosslink Frameworks: " + dframe + " and " + lframe + "\n");
  Relink(lframe,"../../build/FreeMat.app/Contents/Frameworks/"+dframe+".framework/Versions/4.0/"+dframe);
}

void RelinkPlugin(QString plugin, QString frame) {
  TermOutputText("Relinking plugin: " + plugin + " to framework " + frame + "\n");
  Relink(frame,plugin);
}

void RelinkPlugins() {
  QDir dir("../../build/FreeMat.app/Contents/Plugins/imageformats");
  dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo(list[i]);
    RelinkPlugin(fileInfo.absoluteFilePath(),"QtGui");
    RelinkPlugin(fileInfo.absoluteFilePath(),"QtCore");
    RelinkPlugin(fileInfo.absoluteFilePath(),"QtOpenGL");
  }
}

void ConsoleWidget::MacBundle() {
  QString qtdir(getenv("QTDIR"));
  MakeDir("../../build/FreeMat.app/Contents/Frameworks");
  InstallFramework("QtGui");
  InstallFramework("QtCore");
  InstallFramework("QtOpenGL");
  CrossLinkFramework("QtGui","QtCore");
  CrossLinkFramework("QtOpenGL","QtGui");
  CrossLinkFramework("QtOpenGL","QtCore");
  CopyDirectory("../../help/html","../../build/FreeMat.app/Contents/Resources/help/html");
  CopyDirectory("../../help/text","../../build/FreeMat.app/Contents/Resources/help/text");
  CopyDirectory("../../help/MFiles","../../build/FreeMat.app/Contents/Resources/mfiles");
  CopyDirectory(qtdir+"/plugins/imageformats","../../build/FreeMat.app/Contents/Plugins/imageformats");
  RelinkPlugins();
  QFile vfile("../../help/version.txt");
  if (!vfile.open(QFile::ReadOnly))
    Halt("Unable to open ../../help/version.txt for input\n");
  TermOutputText("\n\nDone\n");
}

QString stripWhiteSpace(QString a) {
  QRegExp whitespace(" ");
  int k;
  while ((k = a.indexOf(whitespace,0)) != -1)
    a.remove(k,whitespace.matchedLength());
  return a;
}

bool allWhiteSpace(QString a) {
  a = stripWhiteSpace(a);
  return (a.isEmpty());
}

void ImportLibs(QString program) {
  QString lddOutput(ExecuteAndCapture("ldd",QStringList() << program));
  // Regular Expression to parse output of ldd...
  QRegExp lddlib("=>\\s*([^(]*)");
  QRegExp Xlib("X11R6");
  int k=0;
  while ((k = lddOutput.indexOf(lddlib,k)) != -1) {
    QString lib(stripWhiteSpace(lddlib.cap(1)));
    if (!allWhiteSpace(lib)) {
      TermOutputText("Lib :" + lib);
      if ((lib.indexOf("X11R6") == -1) && (lib.indexOf("/tls/") == -1) && 
	  (lib.indexOf("ncurses") == -1) && (lib.indexOf("libz") == -1) &&
	  (lib.indexOf("libdl") == -1) && (lib.indexOf("libGL") == -1) &&
	  (lib.indexOf("libc.") == -1) && (lib.indexOf("libm.") == -1) &&
	  (lib.indexOf("libpthread.") == -1)) {
 	QFileInfo file(lib);
	CopyFile(lib,"FreeMat/Contents/lib/"+file.fileName());
	// 	Execute("/bin/cp",QStringList() << "-v" << "-R" << lib << file.fileName());
	TermOutputText(" <copy> to FreeMat/Contents/lib/" + file.fileName() + "\n");
      } else
	TermOutputText(" <skip>\n");
    }
    k += lddlib.matchedLength();
  }
}

void ConsoleWidget::LinuxBundle() {
  QFile vfile("../../help/version.txt");
  if (!vfile.open(QFile::ReadOnly))
    Halt("Unable to open ../../help/version.txt for input\n");
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
  CopyFile("../../build/FreeMat","FreeMat/Contents/bin/FreeMatMain");
  // Copy the required libraries
  MakeDir("FreeMat/Contents/lib");
  ImportLibs("../../build/FreeMat");
   // Write out the run script
  QFile *script = new QFile("FreeMat/Contents/bin/FreeMat");
  if (!script->open(QFile::WriteOnly))
    Halt("Unable to open FreeMat/Contents/bin/FreeMat for output\n");
  QTextStream *h = new QTextStream(script);
  *h << "#!/bin/bash\n";
  *h << "mypath=`which $0`\n";
  *h << "mypath=${mypath%/*}\n";
  *h << "declare -x LD_LIBRARY_PATH=$mypath/../lib\n";
  *h << "$mypath/FreeMatMain $*\n";
  delete h;
  delete script;
  Execute("/bin/chmod",QStringList() << "+x" << "FreeMat/Contents/bin/FreeMat");
  CopyDirectory("../../help/html","FreeMat/Contents/Resources/help/html");
  CopyDirectory("../../help/text","FreeMat/Contents/Resources/help/text");
  CopyDirectory("../../help/MFiles","FreeMat/Contents/Resources/mfiles");
  QString qtdir(getenv("QTDIR"));
  CopyFile(qtdir+"/plugins/imageformats/libqjpeg.so","FreeMat/Contents/Plugins/imageformats/libqjpeg.so");
  CopyFile(qtdir+"/plugins/imageformats/libqmng.so","FreeMat/Contents/Plugins/imageformats/libqmng.so");
  QStringList plugs(GetFileList("FreeMat/Contents/Plugins/imageformats",
				QStringList()));
  for (int i=0;i<plugs.size();i++)
    ImportLibs("FreeMat/"+plugs[i]);
  TermOutputText("\n\nDone\n");
  qApp->exit();
}

void ConsoleWidget::WinBundle() {
  QFile vfile("../../help/version.txt");
  if (!vfile.open(QFile::ReadOnly))
    Halt("Unable to open ../../help/version.txt for input\n");
  QTextStream g(&vfile);
  QString versionnum(g.readLine(0));
  MakeDir("FreeMat");
  MakeDir("FreeMat/Contents");
  MakeDir("FreeMat/Contents/bin");
  MakeDir("FreeMat\/Contents/Resources");
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
  qApp->exit();
}

void ConsoleWidget::exitNow() {
  exit(1);
}

int parseFlagArg(int argc, char *argv[], const char* flagstring, bool flagarg) {
  bool flagFound = false;
  int ndx;
  ndx = 1;
  while (!flagFound && ndx < argc) {
    flagFound = strcmp(argv[ndx],flagstring) == 0;
    if (!flagFound) ndx++;
  }
  if (flagFound && flagarg && (ndx == argc-1)) {
    fprintf(stderr,"Error: flag %s requires an argument!\n",flagstring);
    exit(1);
  }
  if (!flagFound)
    ndx = 0;
  return ndx;
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  ConsoleWidget *m_main = new ConsoleWidget;
  int linuxflag;
  int macflag;
  int pcflag;
  linuxflag = parseFlagArg(argc,argv,"-linux",false);
  macflag = parseFlagArg(argc,argv,"-mac",false);
  pcflag = parseFlagArg(argc,argv,"-win",false);
  if (!linuxflag && !macflag && !pcflag)
    return 0;
  m_main->show();
  if (linuxflag) {
    QTimer::singleShot(0,m_main,SLOT(LinuxBundle()));
  } else if (macflag) {
    QTimer::singleShot(0,m_main,SLOT(MacBundle()));
  } else if (pcflag) {
    QTimer::singleShot(0,m_main,SLOT(WinBundle()));
  }
  return app.exec();
  return 0;
}
