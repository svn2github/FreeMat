#include "disttool.hpp"
#include <iostream>
#include <QtGui>

QTextEdit *m_text;
QString sourcepath;
QString buildpath;

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

QString GetVersionString() {
  QFile tfile(sourcepath + "/libs/libFreeMat/WalkTree.cpp");
  if (!tfile.open(QFile::ReadOnly))
    Halt("Unable to open "+sourcepath+"/libs/libFreeMat/WalkTree.cpp for input\n");
  QTextStream g(&tfile);
  QRegExp versionMatch("\\s*//@");
  while (!g.atEnd()) {
    QString line(g.readLine(0));
    if (line.indexOf(versionMatch) >= 0) {
      QString line2(g.readLine(0));
      QRegExp vString("\\s*\"FreeMat v(.*)\"");
      if (vString.indexIn(line2) < 0)
	Halt("Unable to determine version number from the source code\n");
      return vString.cap(1);
    }
  }
  Halt("Unable to determine version number from the source code\n");
}

void MakeDir(QString dir) {
  QDir d;
  //  TermOutputText("Making Directory " + dir + "\n");
  d.mkpath(dir);
}

void CopyFile(QString src, QString dest) {
  QChar last(src.at(src.size()-1));
  QFileInfo fi(dest);
  QDir dir;
  dir.mkpath(fi.absolutePath());
  QFile::copy(src,dest);
  qApp->processEvents();
}

void CopyDirectoryNoRecurse(QString src, QString dest) {
  TermOutputText("Copying Directory " + src + "\n");
  QDir dir(src);
  dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    CopyFile(fileInfo.absoluteFilePath(),dest+"/"+fileInfo.fileName());
  }
}

void CopyDirectory(QString src, QString dest) {
  TermOutputText("Copying Directory " + src + "\n");
  QDir dir(src);
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir()) {
      if ((fileInfo.fileName() != ".svn") &&
	  (!fileInfo.fileName().endsWith("~"))) {
	MakeDir(dest+"/"+fileInfo.fileName());
	CopyDirectory(fileInfo.absoluteFilePath(),dest+"/"+fileInfo.fileName());
      }
    } else
      CopyFile(fileInfo.absoluteFilePath(),dest+"/"+fileInfo.fileName());
  }
}

void DeleteDirectory(QString dirname) {
  TermOutputText("Deleting Directory " + dirname + "\n");
  QDir dir(dirname);
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir())
      DeleteDirectory(fileInfo.absoluteFilePath());
    else
      dir.remove(fileInfo.absoluteFilePath());
  }
  dir.rmdir(dirname);
}

ConsoleWidget::ConsoleWidget() : QWidget() {
  m_text = new QTextEdit;
  m_text->setReadOnly(true);
  resize(600,400);
  m_text->setFontFamily("Courier");
  QVBoxLayout *layout = new QVBoxLayout;
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
  Execute("cp",QStringList() << "-R" << qtdir+"/lib/"+frame+".framework" << buildpath+"/FreeMat.app/Contents/Frameworks/"+frame+".framework");
  //  CopyDirectory(qtdir+"/lib/"+frame+".framework","../../FreeMat.app/Contents/Frameworks/"+frame+".framework");
  Execute("install_name_tool",QStringList() << "-id" << "@executable_path/../Frameworks/"+frame+".framework/Versions/4.0/"+frame << buildpath+"/FreeMat.app/Contents/Frameworks/"+frame+".framework/Versions/4.0/"+frame);
  Relink(frame,buildpath+"/FreeMat.app/Contents/MacOs/FreeMat");
}

void CrossLinkFramework(QString dframe, QString lframe) {
  QString qtdir(getenv("QTDIR"));
  TermOutputText("Crosslink Frameworks: " + dframe + " and " + lframe + "\n");
  Relink(lframe,buildpath+"/FreeMat.app/Contents/Frameworks/"+dframe+".framework/Versions/4.0/"+dframe);
}

void RelinkPlugin(QString plugin, QString frame) {
  TermOutputText("Relinking plugin: " + plugin + " to framework " + frame + "\n");
  Relink(frame,plugin);
}

void RelinkPlugins() {
  QDir dir(buildpath+"/FreeMat.app/Contents/Plugins/imageformats");
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
  MakeDir(buildpath+"/FreeMat.app/Contents/Frameworks");
  InstallFramework("QtGui");
  InstallFramework("QtCore");
  InstallFramework("QtOpenGL");
  CrossLinkFramework("QtGui","QtCore");
  CrossLinkFramework("QtOpenGL","QtGui");
  CrossLinkFramework("QtOpenGL","QtCore");
  CopyDirectory(buildpath+"/help/html",buildpath+"/FreeMat.app/Contents/Resources/help/html");
  CopyDirectory(buildpath+"/help/text",buildpath+"/FreeMat.app/Contents/Resources/help/text");
  CopyDirectory(buildpath+"/help/MFiles",buildpath+"/FreeMat.app/Contents/Resources/mfiles");
  CopyDirectory(qtdir+"/plugins/imageformats",buildpath+"/FreeMat.app/Contents/Plugins/imageformats");
  RelinkPlugins();
  TermOutputText("\n\nDone\n");
  qApp->exit();
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
	CopyFile(lib,buildpath+"/FreeMat"+GetVersionString()+"/Contents/lib/"+file.fileName());
	// 	Execute("/bin/cp",QStringList() << "-v" << "-R" << lib << file.fileName());
	TermOutputText(" <copy> to "+buildpath+"/FreeMat/Contents/lib/" + file.fileName() + "\n");
      } else
	TermOutputText(" <skip>\n");
    }
    k += lddlib.matchedLength();
  }
}


void ConsoleWidget::LinuxBundle() {
  QString versionnum(GetVersionString());
  QString baseDir(buildpath+"/FreeMat" + versionnum);
  MakeDir(baseDir);
  MakeDir(baseDir+"/Contents");
  MakeDir(baseDir+"/Contents/bin");
  MakeDir(baseDir+"/Contents/Resources");
  MakeDir(baseDir+"/Contents/Resources/help");
  MakeDir(baseDir+"/Contents/Resources/help/html");
  MakeDir(baseDir+"/Contents/Resources/help/text");
  MakeDir(baseDir+"/Contents/Resources/help/pdf");
  MakeDir(baseDir+"/Contents/Resources/mfiles");
  CopyFile(buildpath+"/src/FreeMat",baseDir+"/Contents/bin/FreeMatMain");
  // Copy the required libraries
  MakeDir(baseDir+"/Contents/lib");
  ImportLibs(buildpath+"/src/FreeMat");
   // Write out the run script
  QFile *script = new QFile(baseDir+"/Contents/bin/FreeMat");
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
  Execute("/bin/chmod",QStringList() << "+x" << baseDir+"/Contents/bin/FreeMat");
  CopyDirectory(buildpath+"/help/html",baseDir+"/Contents/Resources/help/html");
  CopyDirectory(buildpath+"/help/text",baseDir+"/Contents/Resources/help/text");
  CopyDirectory(buildpath+"/help/MFiles",baseDir+"/Contents/Resources/mfiles");
  CopyFile(buildpath+"/help/latex/main.pdf",baseDir+"/Contents/Resources/pdf/FreeMat"+versionnum+".pdf");
  QString qtdir(getenv("QTDIR"));
  CopyFile(qtdir+"/plugins/imageformats/libqjpeg.so",baseDir+"/Contents/Plugins/imageformats/libqjpeg.so");
  CopyFile(qtdir+"/plugins/imageformats/libqmng.so",baseDir+"/Contents/Plugins/imageformats/libqmng.so");
  QStringList plugs(GetFileList(baseDir+"/Contents/Plugins/imageformats",
				QStringList()));
  for (int i=0;i<plugs.size();i++)
    ImportLibs(baseDir+"/"+plugs[i]);
  TermOutputText("\n\nDone\n");
  qApp->exit();
}

QStringList ReadManifest() {
  QFile man("../../Manifest");
  if (!man.open(QFile::ReadOnly))
    Halt("Unable to open Manifest file");
  QTextStream g(&man);
  QStringList ret;
  while (!g.atEnd()) {
    ret << g.readLine(0);
  }
  return ret;
}

void ConsoleWidget::SrcBundle() {
  QString versionnum(GetVersionString());
  QString baseDir(buildpath+"/FreeMat" + versionnum + "_src");
  DeleteDirectory(baseDir);
  MakeDir(baseDir);
  // Copy the source code directories
  CopyDirectory(sourcepath+"/src",baseDir+"/src");
  CopyDirectory(sourcepath+"/libs",baseDir+"/libs");
  CopyDirectory(sourcepath+"/tools",baseDir+"/tools");
  CopyDirectory(sourcepath+"/MFiles",baseDir+"/MFiles");
  CopyDirectory(sourcepath+"/images",baseDir+"/images");
  CopyDirectory(sourcepath+"/cmake",baseDir+"/cmake");
  CopyDirectory(sourcepath+"/tests",baseDir+"/tests");
  MakeDir(baseDir+"/extern");
  CopyFile(sourcepath+"/extern/build_extern.sh",baseDir+"/extern/build_extern.sh");
  CopyFile(sourcepath+"/extern/build_extern_mingw.sh",baseDir+"/extern/build_extern_mingw.sh");
  CopyFile(sourcepath+"/CMakeLists.txt",baseDir+"/CMakeLists.txt");
  CopyFile(sourcepath+"/COPYING",baseDir+"/COPYING");
  CopyFile(sourcepath+"/INSTALL",baseDir+"/INSTALL");
  CopyFile(sourcepath+"/ChangeLog",baseDir+"/ChangeLog");
  CopyFile(sourcepath+"/README",baseDir+"/README");
  qApp->exit();
}

void ConsoleWidget::WinBundle() {
  QString versionnum(GetVersionString());
  QString baseDir(buildpath+"/FreeMat" + versionnum);
  DeleteDirectory(baseDir);
  MakeDir(baseDir);
  MakeDir(baseDir+"/Contents");
  MakeDir(baseDir+"/Contents/bin");
  MakeDir(baseDir+"/Contents/Resources");
  MakeDir(baseDir+"/Contents/Resources/help");
  MakeDir(baseDir+"/Contents/Resources/help/html");
  MakeDir(baseDir+"/Contents/Resources/help/text");
  MakeDir(baseDir+"/Contents/Resources/mfiles");
  MakeDir(baseDir+"/Contents/Plugins/imageformats");
  QString qtdir(getenv("QTDIR"));
  CopyFile(qtdir+"/plugins/imageformats/qjpeg1.dll",baseDir+"/Contents/Plugins/imageformats/qjpeg1.dll");
  CopyFile(qtdir+"/plugins/imageformats/qmng1.dll",baseDir+"/Contents/Plugins/imageformats/qmng1.dll");
  CopyFile(buildpath+"/src/FreeMat.exe",baseDir+"/Contents/bin/FreeMat.exe");
  CopyFile(buildpath+"/tools/blastune/BlasTuneApp.exe",baseDir+"/Contents/bin/BlasTuneApp.exe");
  CopyFile(buildpath+"/tools/blastune/BlasTune.exe",baseDir+"/Contents/bin/BlasTune.exe");
  CopyFile(qtdir+"/bin/QtCore4.dll",baseDir+"/Contents/bin/QtCore4.dll");
  CopyFile(qtdir+"/bin/QtGui4.dll",baseDir+"/Contents/bin/QtGui4.dll");
  CopyFile(qtdir+"/bin/QtOpenGL4.dll",baseDir+"/Contents/bin/QtOpenGL4.dll");
  CopyFile(sourcepath+"/extern/Root/lib/blas.dll",baseDir+"/Contents/bin/blas.dll");
  //  CopyFile("../../extern/blas/atlas_prebuilt_win32/atlas_blas_P4SSE2.dll",
  //	   baseDir+"/Contents/bin/blas.dll");
  CopyFile("c:/MinGW/bin/mingwm10.dll",baseDir+"/Contents/bin/mingwm10.dll");
  CopyDirectory(buildpath+"/help/html",baseDir+"/Contents/Resources/help/html");
  CopyDirectory(buildpath+"/help/text",baseDir+"/Contents/Resources/help/text");
  CopyDirectory(buildpath+"/help/MFiles",baseDir+"/Contents/Resources/mfiles");
  TermOutputText("Generating NSI file...\n");
  QStringList flist(GetFileList(baseDir+"",QStringList()));
  QFile *file_in = new QFile(sourcepath+"/tools/disttool/freemat_nsi.in");
  if (!file_in->open(QFile::ReadOnly))
    Halt("Unable to open FreeMat NSIS template file freemat_nsi.in\n");
  QFile *file_out = new QFile(baseDir+"/freemat.nsi");
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

void usage() {
  std::cerr << "Usage: DistTool <platform> <path_to_source> <path_to_build> \n";
  std::cerr << "        where <platform> is:\n";
  std::cerr << "           -linux\n";
  std::cerr << "           -mac\n";
  std::cerr << "           -win\n";
  std::cerr << "           -src\n";
  exit(1);
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  ConsoleWidget *m_main = new ConsoleWidget;
  int linuxflag;
  int macflag;
  int pcflag;
  int srcflag;
  linuxflag = parseFlagArg(argc,argv,"-linux",false);
  macflag = parseFlagArg(argc,argv,"-mac",false);
  pcflag = parseFlagArg(argc,argv,"-win",false);
  srcflag = parseFlagArg(argc,argv,"-src",false);
  if (!linuxflag && !macflag && !pcflag && !srcflag)
    usage();
  if (argc < 4) usage();
  sourcepath = argv[2];
  buildpath = argv[3];
  m_main->show();
  if (linuxflag) {
    QTimer::singleShot(0,m_main,SLOT(LinuxBundle()));
  } else if (macflag) {
    QTimer::singleShot(0,m_main,SLOT(MacBundle()));
  } else if (pcflag) {
    QTimer::singleShot(0,m_main,SLOT(WinBundle()));
  } else if (srcflag) {
    QTimer::singleShot(0,m_main,SLOT(SrcBundle()));
  }
  return app.exec();
  return 0;
}
