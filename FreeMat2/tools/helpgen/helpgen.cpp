/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <QApplication>
#include <QTextEdit>
#include <QTextStream>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtDebug>
#include <QDir>
#include <QProcess>
#include "KeyManager.hpp"
#include "Module.hpp"
#include "Class.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "HandleCommands.hpp"
#include "File.hpp"
#include "Context.hpp"
#include "helpgen.hpp"
#include <QtGui>
#include "Common.hpp"

QTextEdit *m_text;

QStringList input;
QStringList generatedFileList;
QMap<QString, QString> section_descriptors;
QStringList sectionOrdered;
int moduledepth = 0;
QString sourcepath;


HelpHandler::HelpHandler(Interpreter *interp, 
			 QStringList cmds) {
  m_interp = interp;
  m_cmds = cmds;
}

void HelpHandler::AddOutputString(string txt) {
  m_text += txt;
}

void HelpHandler::SetPrompt(string prompt) {
  m_text += prompt;
  m_interp->ExecuteLine(m_cmds.front().toStdString());
  m_text += m_cmds.front().toStdString();
  m_cmds.erase(m_cmds.begin());
}

void HelpHandler::DoGraphicsCall(FuncPtr f, ArrayVector m, int narg) { 
  try {
    ArrayVector n(f->evaluateFunction(m_interp,m,narg));
    m_interp->RegisterGfxResults(n);
  } catch (Exception& e) {
    m_interp->RegisterGfxError(e.getMessageCopy());
  }
}


void TermOutputText(QString str) {
  //  m_text->textCursor().movePosition(QTextCursor::End);
  m_text->insertPlainText(str);
  m_text->ensureCursorVisible();
  //  qDebug() << str;
  qApp->processEvents();
}


Context *context;
Interpreter *m_interp;
HelpHandler *m_hlp;

Interpreter* GetInterpreter(QStringList cmds) {
  QString pth(sourcepath+"/toolbox");
  QStringList pthList(GetRecursiveDirList(pth));
  Interpreter *twalk = new Interpreter(context);
  m_hlp = new HelpHandler(twalk,cmds);
  twalk->setUserPath(pthList);
  twalk->rescanPath();
  twalk->AutoStop(false);
  twalk->setGreetingFlag(true);
  qRegisterMetaType<string>("string");
  qRegisterMetaType<FuncPtr>("FuncPtr");
  qRegisterMetaType<ArrayVector>("ArrayVector");
  QObject::connect(twalk,SIGNAL(outputRawText(string)),
		   m_hlp,SLOT(AddOutputString(string)));
  QObject::connect(twalk,SIGNAL(SetPrompt(string)),
		   m_hlp,SLOT(SetPrompt(string)));
  QObject::connect(twalk,SIGNAL(doGraphicsCall(FuncPtr,ArrayVector,int)),
		   m_hlp,SLOT(DoGraphicsCall(FuncPtr,ArrayVector,int)));
  twalk->setTerminalWidth(80);
  m_interp = twalk;
  return twalk;
}

void Halt(QString emsg) {
  TermOutputText(emsg);
  QEventLoop m_loop;
  m_loop.exec();  
  exit(0);
}

// Base class for all Help output formats (text, html, latex)
class HelpWriter {
 public:
  HelpWriter() {}
  virtual ~HelpWriter() {}
  virtual void BeginModule(QString modname, QString moddesc, QString secname) {}
  virtual void BeginGroup(QString groupname) {}
  virtual void BeginVerbatim() {};
  virtual void OutputText(QString text) {};
  virtual void EndVerbatim() {};
  virtual void EndModule() {}
  virtual void DoFigure(QString figname) {};
  virtual void DoEquation(QString eqn) {};
  virtual void DoFile(QString filename, QString filetext) {};
  virtual void DoEnumerate(QStringList) {};
  virtual void DoItemize(QStringList) {};
  virtual void WriteIndex() {};
};

class GroupWriter : public HelpWriter {
  std::vector<HelpWriter*> cast;
 public:
  GroupWriter() {}
  void RegisterWriter(HelpWriter*ptr);
  virtual ~GroupWriter() {}
  virtual void BeginModule(QString modname, QString moddesc, QString secname);
  virtual void BeginGroup(QString groupname);
  virtual void BeginVerbatim();
  virtual void OutputText(QString text);
  virtual void EndVerbatim();
  virtual void EndModule();
  virtual void DoFigure(QString figname);
  virtual void DoEquation(QString eqn);
  virtual void DoFile(QString filename, QString filetext);
  virtual void DoEnumerate(QStringList);
  virtual void DoItemize(QStringList);
  virtual void WriteIndex();
};

void GroupWriter::RegisterWriter(HelpWriter*ptr) {
  cast.push_back(ptr);
}

void GroupWriter::EndModule() {
  for (unsigned i=0;i<cast.size();i++) cast[i]->EndModule();
}

void GroupWriter::WriteIndex() {
  for (unsigned i=0;i<cast.size();i++)
    cast[i]->WriteIndex();
}

void GroupWriter::BeginModule(QString modname, QString moddesc, QString secname) {
  for (unsigned i=0;i<cast.size();i++) 
    cast[i]->BeginModule(modname,moddesc,secname);
}

void GroupWriter::BeginGroup(QString groupname) {
  for (unsigned i=0;i<cast.size();i++) 
    cast[i]->BeginGroup(groupname);
}

void GroupWriter::BeginVerbatim() {
  for (unsigned i=0;i<cast.size();i++) 
    cast[i]->BeginVerbatim();
}

void GroupWriter::OutputText(QString text) {
  for (unsigned i=0;i<cast.size();i++)
    cast[i]->OutputText(text);
}

void GroupWriter::EndVerbatim() {
  for (unsigned i=0;i<cast.size();i++) 
    cast[i]->EndVerbatim();
}

void GroupWriter::DoFigure(QString name) {
  for (unsigned i=0;i<cast.size();i++) 
    cast[i]->DoFigure(name);
}

void GroupWriter::DoEquation(QString eqn) {
  for (unsigned i=0;i<cast.size();i++) 
    cast[i]->DoEquation(eqn);
}

void GroupWriter::DoFile(QString filename, QString filetext) {
  for (unsigned i=0;i<cast.size();i++) 
    cast[i]->DoFile(filename,filetext);
}

void GroupWriter::DoEnumerate(QStringList lst) {
  for (unsigned i=0;i<cast.size();i++) 
    cast[i]->DoEnumerate(lst);
}

void GroupWriter::DoItemize(QStringList lst) {
  for (unsigned i=0;i<cast.size();i++) 
    cast[i]->DoItemize(lst);
}

class HTMLWriter : public HelpWriter {
  QFile *myfile;
  QTextStream *mystream;
  QMultiMap<QString, QStringList> sectables;
  QStringList eqnlist;
  bool verbatim;
  QString modulename;
  bool ignore;
 public:
  HTMLWriter() {myfile = NULL; mystream = NULL;}
  virtual ~HTMLWriter() {}
  void BeginModule(QString modname, QString moddesc, QString secname);
  void BeginGroup(QString groupname);
  void BeginVerbatim();
  void OutputText(QString text);
  QString ExpandCodes(QString text);
  void DoFigure(QString figname);
  void EndVerbatim();
  void EndModule();
  void DoEquation(QString eqn);
  void DoFile(QString filename, QString filetext);
  void DoEnumerate(QStringList);
  void DoItemize(QStringList);
  void GenerateEquations();
  void WriteIndex();
  void WriteSectionTable(QString secname, QList<QStringList> modinfo);
};

void HTMLWriter::WriteSectionTable(QString secname, QList<QStringList> modinfo) {
  secname = secname.toLower();
  QString secdesc(section_descriptors.value(secname));
  if (secdesc.isEmpty())
    TermOutputText("Warning: No section descriptor for " + secname + "!\n");
  QFile file(sourcepath+"/help/html/sec_" + secname + ".html");
  if (!file.open(QFile::WriteOnly))
    Halt("Unable to open "+sourcepath+"/help/html/" + secname + ".html for output");
  QTextStream f(&file);
  f << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n";
  f << "\n";
  f << "<HTML>\n";
  f << "<HEAD>\n";
  f << "<TITLE>" << secdesc << "</TITLE>\n";
  f << "</HEAD>\n";
  f << "<BODY>\n";
  f << "<H2>" << secdesc << "</H2>\n";
  f << "<P>\n";
  f << "<A HREF=index.html> Main Index </A>\n";
  f << "<P>\n";
  f << "<UL>\n";
  QMap<QString, QString> modmap;
  QStringList moduleList;
  for (unsigned m=0;m<modinfo.size();m++) {
    moduleList << modinfo[m][0].toLower();
    modmap[modinfo[m][0].toLower()] = modinfo[m][1];
  }
  moduleList.sort();
  for (unsigned i=0;i<moduleList.size();i++)
    f << "<LI> <A HREF=" << secname.toLower() + "_" + moduleList[i] << ".html> " << moduleList[i] << "</A> " << modmap[moduleList[i]] << "</LI>\n";
  f << "</UL>\n";
  f << "</BODY>\n";
  f << "</HTML>\n";
}

void HTMLWriter::WriteIndex() {
  TermOutputText("Writing index\n");
  // Write the section pages
  QList<QString> sections_multi(sectables.keys());
  QSet<QString> sections;
  for (int i=0;i<sections_multi.size();i++)
    sections << sections_multi[i];
  foreach (QString secname, sections) {
    QList<QStringList> modules(sectables.values(secname));
    WriteSectionTable(secname,modules);
  }
  QFile file(sourcepath+"/help/html/index.html");
  if (!file.open(QFile::WriteOnly))
    Halt("Unable to open "+sourcepath+"/help/html/index.html for output");
  QTextStream f(&file);
  f << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n";
  f << "\n";
  f << "<HTML>\n";
  f << "<HEAD>\n";
  f << "<TITLE>" << Interpreter::getVersionString().c_str();
  f << " Documentation" << "</TITLE>\n";
  f << "</HEAD>\n";
  f << "<BODY>\n";
  f << "<H1>" << Interpreter::getVersionString().c_str();
  f << " Documentation" << "</H1>\n";
  f << "<P>\n";
  f << "<H2> Documentation Sections </H2>\n";
  f << "<UL>\n";
  foreach (QString secname, sectionOrdered) 
    f << "<LI> <A HREF=sec_" << secname.toLower() << ".html> " << 
    section_descriptors.value(secname) << "</A> </LI>\n";
  f << "</UL>\n";
  f << "</BODY>\n";
  f << "</HTML>\n";
  // Build the module list
  QFile file2(sourcepath+"/help/html/modules.txt");
  if (!file2.open(QFile::WriteOnly | QIODevice::Text))
    Halt("Unable to open "+sourcepath+"/help/html/modules.txt for output");
  // Loop over the sections
  QStringList modulenames;
  foreach (QString secname, sections) {
    // Get the modules for this section
    QList<QStringList> moduledlist(sectables.values(secname));
    for (unsigned k=0;k<moduledlist.size();k++)
      modulenames << moduledlist[k][0].toLower() + " (" + secname + ")";
  }
  modulenames.sort();
  QTextStream f2(&file2);
  for (unsigned k=0;k<modulenames.size();k++) {
    f2 << modulenames[k] << endl;
  }
  // Build the section index
  QFile file3(sourcepath+"/help/html/sectable.txt");
  if (!file3.open(QFile::WriteOnly | QIODevice::Text))
    Halt("Unable to open "+sourcepath+"/help/html/sectable.txt for output");
  QTextStream f3(&file3);
  foreach (QString secname, sectionOrdered) {
    QString secdesc(section_descriptors.value(secname));
    QList<QStringList> modules(sectables.values(secname));
    QStringList moduleList;
    for (unsigned m=0;m<modules.size();m++)
      moduleList << modules[m][1];
    moduleList.sort();
    f3 << secdesc << "\n";
    for (unsigned m=0;m<moduleList.size();m++) {
      f3 << "+" << " (" << secname << ")" << moduleList[m] << "\n";
    }
  }
}

void HTMLWriter::DoItemize(QStringList lst) {
  *mystream << "<UL>\n";
  for (int i=0;i<lst.size();i++)
    *mystream << "<LI> " << ExpandCodes(lst[i]) << " </LI>\n";
  *mystream << "</UL>\n";
}

void HTMLWriter::DoEnumerate(QStringList lst) {
  *mystream << "<OL>\n";
  for (int i=0;i<lst.size();i++)
    *mystream << "<LI> " << ExpandCodes(lst[i]) << " </LI>\n";
  *mystream << "</OL>\n";
}

void HTMLWriter::GenerateEquations() {
  if (eqnlist.empty()) return;
  QFile file(sourcepath+"/help/tmp/" + modulename+"_eqn.tex");
  if (!file.open(QFile::WriteOnly)) 
    Halt("Unable to open " + modulename + "_eqn.tex for output");
  QTextStream f(&file);
  f << "\\documentclass{article}\n";
  f << "\\usepackage{amsmath}\n";
  f << "\\pagestyle{empty}\n";
  f << "\\begin{document}\n";
  for (unsigned i=0;i<eqnlist.size();i++) {
    f << "\\[\n" << eqnlist[i] << "\\]\n";
    f << "\\pagebreak\n";
  }
  f << "\\end{document}\n";
  file.close();
  TermOutputText("Generating equations for " + modulename + "\n");
  QProcess latex;
  latex.setWorkingDirectory(sourcepath+"/help/tmp");
  latex.start("latex",QStringList() << (modulename + "_eqn.tex"));
  if (!latex.waitForFinished())
    Halt("LaTeX for " + modulename + "_eqn.tex never returned");
  QProcess dvipng;
  dvipng.setWorkingDirectory(sourcepath+"/help/html");
  dvipng.start("dvipng",QStringList() << "-T tight" << "../tmp/" + modulename + "_eqn.dvi");
  if (!dvipng.waitForFinished())
    Halt("dvipng for " + modulename + "_eqn.dvi never returned");
}

void HTMLWriter::EndModule() {
  *mystream << "</BODY>\n";
  *mystream << "</HTML>\n";
  if (myfile) delete myfile;
  if (mystream) delete mystream;
  GenerateEquations();
}

void HTMLWriter::BeginModule(QString modname, QString moddesc, QString secname) {
  modulename = modname.toLower();
  eqnlist.clear();
  myfile = new QFile(sourcepath+"/help/html/" + secname + "_" + modulename + ".html");
  if (!myfile->open(QFile::WriteOnly))
    Halt("Unable to open " + modname + ".html for output");
  mystream = new QTextStream(myfile);
  // Output HTML Header
  *mystream << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n";
  *mystream << "\n";
  *mystream << "<HTML>\n";
  *mystream << "<HEAD>\n";
  *mystream << "<TITLE>" << moddesc << "</TITLE>\n";
  *mystream << "</HEAD>\n";
  *mystream << "<BODY>\n";
  *mystream << "<H2>" << moddesc << "</H2>\n";
  sectables.insert(secname,QStringList() << modname << moddesc);
  *mystream << "<P>\n";
  *mystream << "Section: <A HREF=sec_" << secname.toLower() << ".html> " << section_descriptors.value(secname.toLower()) << "</A>\n";
  verbatim = false;
}

void HTMLWriter::BeginGroup(QString groupname) {
  if (groupname.toLower() == "tests")
    ignore = true;
  else {
    *mystream << "<H3>" << groupname << "</H3>\n";
    ignore = false;
  }
}

void HTMLWriter::BeginVerbatim() {
  *mystream << "<PRE>\n";
  verbatim = true;
}

QString HTMLWriter::ExpandCodes(QString text) {
  QRegExp code_pattern("@\\|([^\\|]*)\\|");
  text = text.replace(code_pattern,"<code>\\1</code>"); 
  if (text == "\n") text = "<P>\n";
  return text;
}

QString LatinFilter(QString text) {
  text = text.replace(QRegExp("&"),"&amp;");
  text = text.replace(QRegExp("<"),"&lt;");
  text = text.replace(QRegExp(">"),"&gt;");
  return text;
}

void HTMLWriter::OutputText(QString text) {
  if (ignore) return;
  if (verbatim)
    *mystream << LatinFilter(text);
  else
    *mystream << ExpandCodes(LatinFilter(text));
}

void HTMLWriter::EndVerbatim() {
  verbatim = false;
  *mystream << "</PRE>\n<P>\n";
}

void HTMLWriter::DoFigure(QString name) {
  *mystream << "<P>\n";
  *mystream << "<DIV ALIGN=\"CENTER\">\n";
  *mystream << "<IMG SRC=\"" << name << ".png\">\n";
  *mystream << "</DIV>\n";
  *mystream << "<P>\n";
}

void HTMLWriter::DoEquation(QString eqn) {
  eqnlist << eqn;
  *mystream << "<P>\n";
  *mystream << "<DIV ALIGN=\"CENTER\">\n";
  *mystream << "<IMG SRC=\"" << modulename << "_eqn" << eqnlist.count() << ".png\">\n";
  *mystream << "</DIV>\n";
  *mystream << "<P>\n";
}

void HTMLWriter::DoFile(QString fname, QString ftxt) {
  *mystream << "<P>\n<PRE>\n";
  *mystream << "     " << fname << "\n";
  *mystream << LatinFilter(ftxt) << "\n";
  *mystream << "</PRE>\n";
  *mystream << "<P>\n";
}

class TestWriter : public HelpWriter {
  QFile *myfile;
  QTextStream *mystream;
  QMultiMap<QString, QStringList> sectables;
  bool verbatim;
  bool ignore;
  QString module;
  int num;
 public:
  TestWriter();
  virtual ~TestWriter() {}
  void BeginModule(QString modname, QString moddesc, QString secname);
  void BeginGroup(QString groupname);
  void OutputText(QString text);
  void WriteIndex();
};


void TestWriter::BeginModule(QString modname, QString moddesc, QString secname) {
  module = modname;
}

TestWriter::TestWriter() {
  myfile = new QFile(sourcepath+"/help/test/test_functions.m");
  if (!myfile->open(QFile::WriteOnly))
    Halt("Unable to open help/test/test_functions.m for output");
  mystream = new QTextStream(myfile);
  *mystream << "% Regression test function for " << Interpreter::getVersionString().c_str() << "\n";
  *mystream << "% This function is autogenerated by helpgen.\n";
  *mystream << "function test_functions\n";
}

void TestWriter::BeginGroup(QString groupname) {
  if (groupname.toLower() != "tests") {
    ignore = true;
  } else {
    ignore = false;
  }
  num = 1;
}

void TestWriter::OutputText(QString text) {
  if (ignore) return;
  QRegExp test_pattern("@\\{\"([^\"]*)\",\"([^\"]*)\",\"([^\"]*)\"\\}");
  if (test_pattern.indexIn(text) < 0)
    Halt("Bad line: " + text);
  *mystream << "\n";
  *mystream << test_pattern.cap(1) << ";\n";
  *mystream << "q=" << test_pattern.cap(2) << ";\n";
  *mystream << "test_" << test_pattern.cap(3) << "(q,y,'"<< module.toLower() << " " << num <<"');\n";
  *mystream << "\n";
  num++;
}

void TestWriter::WriteIndex() {
  
  delete mystream;
  delete myfile;
}

class TextWriter : public HelpWriter {
  QFile *myfile;
  QTextStream *mystream;
  QMultiMap<QString, QStringList> sectables;
  bool verbatim;
  bool ignore;
 public:
  TextWriter() {myfile = NULL; mystream = NULL;}
  virtual ~TextWriter() {}
  void BeginModule(QString modname, QString moddesc, QString secname);
  void BeginGroup(QString groupname);
  void BeginVerbatim();
  void OutputText(QString text);
  QString ExpandCodes(QString text);
  void DoFigure(QString figname);
  void EndVerbatim();
  void EndModule();
  void DoEquation(QString eqn);
  void DoFile(QString filename, QString filetext);
  void DoEnumerate(QStringList);
  void DoItemize(QStringList);
  void WriteIndex();
};

void TextWriter::WriteIndex() {
#if 0
  QFile file("latex/main.mdc");
  if (!file.open(QFile::WriteOnly))
    Halt("Unable to open latex/main.tex for output");
  QTextStream f(&file);
  f << "\\documentclass{book}\n";
  f << "\\usepackage{graphicx}\n";
  f << "\\usepackage{amsmath}\n";
  f << "\\renewcommand{\\topfraction}{0.9}\n";
  f << "\\renewcommand{\\floatpagefraction}{0.9}\n";
  f << "\\oddsidemargin 0.0in\n";
  f << "\\evensidemargin 1.0in\n";
  f << "\\textwidth 6.0in\n";
  f << "\\title{" << Interpreter::getVersionString().c_str() << " Documentation}\n";
  f << "\\author{Samit Basu}\n";
  f << "\\begin{document}\n";
  f << "\\maketitle\n";
  f << "\\tableofcontents\n";
  QList<QString> sections_multi(sectables.keys());
  QSet<QString> sections;
  for (int i=0;i<sections_multi.size();i++)
    sections << sections_multi[i];
  foreach (QString secname, sections) {
    QList<QStringList> modules(sectables.values(secname));
    QString secdesc(section_descriptors.value(secname));
    QStringList moduleList;
    for (unsigned m=0;m<modules.size();m++)
      moduleList << modules[m][0].toLower();
    moduleList.sort();
    f << "\\chapter{" << secdesc << "}\n";
    for (unsigned m=0;m<moduleList.size();m++) {
      f << "\\input{" << moduleList[m] << "}\n";
    }
  }
  f << "\\end{document}\n";
#endif
}

void TextWriter::DoItemize(QStringList lst) {
  if (ignore) return;
  for (int i=0;i<lst.size();i++)
    *mystream << "  - " << ExpandCodes(lst[i]);
}

void TextWriter::DoEnumerate(QStringList lst) {
  if (ignore) return;
  for (int i=0;i<lst.size();i++)
    *mystream << "  " << (i+1) << ". " << ExpandCodes(lst[i]);
}

void TextWriter::EndModule() {
  if (myfile) delete myfile;
  if (mystream) delete mystream;
  moduledepth--;
}

void TextWriter::BeginModule(QString modname, QString moddesc, QString secname) {
  //  qDebug() << "Module " << modname << " " << moddesc << " " << secname;
  moduledepth++;
  myfile = new QFile(sourcepath+"/help/text/" + modname.toLower() + ".mdc");
  if (!myfile->open(QFile::Append)) {
    Halt("Unable to open " + modname + ".mdc for output " + QString().sprintf("%d",myfile->error()) + " depth = " + QString().sprintf("%d",moduledepth));
  }
  mystream = new QTextStream(myfile);
  *mystream << modname << " " << moddesc << "\n";
  *mystream << "\n";
  *mystream << "\n";
  sectables.insert(secname,QStringList() << modname << moddesc);
  verbatim = false;
  ignore = true;
}

void TextWriter::BeginGroup(QString groupname) {
  if (groupname.toLower() != "usage") {
    ignore = true;
  } else {
    ignore = false;
  }
  OutputText("Usage\n\n");
}

void TextWriter::BeginVerbatim() {
  verbatim = true;
  OutputText("\n");
}

QString TextWriter::ExpandCodes(QString text) {
  QRegExp code_pattern("@\\|([^\\|]*)\\|");
  QRegExp ret_pattern("\\n");
  text = text.replace(code_pattern,"\\1");
  text = text.replace(ret_pattern,"\n ");
  return text;
}

void TextWriter::OutputText(QString text) {
  if (ignore) return;
  *mystream << ExpandCodes(text);
}

void TextWriter::EndVerbatim() {
  OutputText("\n");
  verbatim = false;
}

void TextWriter::DoFigure(QString name) {
  //  OutputText("<Figure Skipped>\n");
}

void TextWriter::DoEquation(QString eqn) {
  //  OutputText("<Equation Skipped>\n");
}

void TextWriter::DoFile(QString filename, QString ftext) {
  //  OutputText("<File Skipped>\n");
}

class LatexWriter : public HelpWriter {
  QFile *myfile;
  QTextStream *mystream;
  QMultiMap<QString, QStringList> sectables;
  bool verbatim;
  bool ignore;
 public:
  LatexWriter() {myfile = NULL; mystream = NULL;}
  virtual ~LatexWriter() {}
  void BeginModule(QString modname, QString moddesc, QString secname);
  void BeginGroup(QString groupname);
  void BeginVerbatim();
  void OutputText(QString text);
  QString ExpandCodes(QString text);
  void DoFigure(QString figname);
  void EndVerbatim();
  void EndModule();
  void DoEquation(QString eqn);
  void DoFile(QString filename, QString filetext);
  void DoEnumerate(QStringList);
  void DoItemize(QStringList);
  void WriteIndex();
};

void LatexWriter::WriteIndex() {
  QFile file(sourcepath+"/help/latex/main.tex");
  if (!file.open(QFile::WriteOnly))
    Halt("Unable to open "+sourcepath+"/latex/main.tex for output");
  QTextStream f(&file);
  f << "\\documentclass{book}\n";
  f << "\\usepackage{graphicx}\n";
  f << "\\usepackage{amsmath}\n";
  f << "\\renewcommand{\\topfraction}{0.9}\n";
  f << "\\renewcommand{\\floatpagefraction}{0.9}\n";
  f << "\\oddsidemargin 0.0in\n";
  f << "\\evensidemargin 1.0in\n";
  f << "\\textwidth 6.0in\n";
  f << "\\title{" << Interpreter::getVersionString().c_str() << " Documentation}\n";
  f << "\\author{Samit Basu}\n";
  f << "\\begin{document}\n";
  f << "\\maketitle\n";
  f << "\\tableofcontents\n";
  foreach (QString secname, sectionOrdered) {
    QList<QStringList> modules(sectables.values(secname));
    QString secdesc(section_descriptors.value(secname));
    QStringList moduleList;
    for (unsigned m=0;m<modules.size();m++)
      moduleList << modules[m][0].toLower();
    moduleList.sort();
    f << "\\chapter{" << secdesc << "}\n";
    for (unsigned m=0;m<moduleList.size();m++) {
      f << "\\input{" << secname + "_" + moduleList[m] << "}\n";
    }
  }
  f << "\\end{document}\n";
}

void LatexWriter::DoItemize(QStringList lst) {
  *mystream << "\\begin{itemize}\n";
  for (int i=0;i<lst.size();i++)
    *mystream << "\\item " << ExpandCodes(lst[i]);
  *mystream << "\\end{itemize}\n";
}

void LatexWriter::DoEnumerate(QStringList lst) {
  *mystream << "\\begin{enumerate}\n";
  for (int i=0;i<lst.size();i++)
    *mystream << "\\item " << ExpandCodes(lst[i]);
  *mystream << "\\end{enumerate}\n";
}

void LatexWriter::EndModule() {
  if (myfile) delete myfile;
  if (mystream) delete mystream;
  moduledepth--;
}

void LatexWriter::BeginModule(QString modname, QString moddesc, QString secname) {
  moduledepth++;
  QString filename(sourcepath+"/help/latex/" + secname.toLower() + "_" + modname.toLower() + ".tex");
  myfile = new QFile(filename);
  if (!myfile->open(QFile::WriteOnly)) {
    Halt("Unable to open " + filename + " for output " + QString().sprintf("%d",myfile->error()) + " depth = " + QString().sprintf("%d",moduledepth));
  }
  mystream = new QTextStream(myfile);
  *mystream << "\\section{" + moddesc + "}\n\n";
  sectables.insert(secname,QStringList() << modname << moddesc);
  verbatim = false;
}

void LatexWriter::BeginGroup(QString groupname) {
  if (groupname.toLower() == "tests")
    ignore = true;
  else {
    ignore = false;
    *mystream << "\\subsection{" + groupname + "}\n\n";
  }
}

void LatexWriter::BeginVerbatim() {
  *mystream << "\\begin{verbatim}\n";
  verbatim = true;
}

QString LatexWriter::ExpandCodes(QString text) {
  QRegExp code_pattern("@\\|([^\\|]*)\\|");
  return text.replace(code_pattern,"\\verb|\\1|"); 
}

void LatexWriter::OutputText(QString text) {
  if (ignore) return;
  if (verbatim)
    *mystream << text;
  else
    *mystream << ExpandCodes(text);
}

void LatexWriter::EndVerbatim() {
  verbatim = false;
  *mystream << "\\end{verbatim}\n";
}

void LatexWriter::DoFigure(QString name) {
  *mystream << "\n\n\\centerline{\\includegraphics[width=8cm]{" << name << "}}\n\n";
}

void LatexWriter::DoEquation(QString eqn) {
  *mystream << "\\[\n" << eqn << "\\]\n";
}

void LatexWriter::DoFile(QString filename, QString ftext) {
  *mystream << "\\begin{verbatim}\n";
  *mystream << "     " << filename << "\n";
  *mystream << ftext;
  *mystream << "\\end{verbatim}\n";
}

QString EvaluateCommands(QStringList cmds, int expectedCount, QString modulename, QString file) {
  QString output;

  output.clear();

  QString cpath(QDir::currentPath());

  //  qDebug() << "Evaluating:\n" << cmds;
  Interpreter* twalk = GetInterpreter(cmds);
  twalk->start();  
  //   for (int i=0;i<cmds.size();i++)
  //     twalk->ExecuteLine(cmds[i].toStdString());
  //   // Need to 
  while (!twalk->isFinished())
    qApp->processEvents();
  QDir::setCurrent(cpath);
  output = QString::fromStdString(m_hlp->text());
  if (twalk->getErrorCount() != expectedCount) 
    Halt("Error: Got " + QString().sprintf("%d",twalk->getErrorCount()) + ", expected " + QString().sprintf("%d",expectedCount) + " in module " + modulename + " in file " + file + "\n\nOutput Follows:\n"+output);
  //  cout << "Output: " << output.toStdString();
  ShutdownHandleGraphics();
  //  cout << "Output: " << m_hlp->text();
  delete twalk;
  delete m_hlp;
  QRegExp mprintre("(--> mprint[^\\n]*)");
  output = output.replace(mprintre,"");
  QRegExp quitre("(--> quit;)");
  output = output.replace(quitre,"");
  return output;
}

void ConsoleWidget::exitNow() {
  exit(1);
}

void ConsoleWidget::LaunchHelpWindow() {
}


ConsoleWidget::ConsoleWidget() : QWidget() {
  m_text = new QTextEdit;
  m_text->setReadOnly(true);
  resize(600,400);
  m_text->setFontFamily("Courier");
  //   QPushButton *run = new QPushButton("Run");
  //   QPushButton *quit = new QPushButton("Quit");
  //   QPushButton *test = new QPushButton("Test");
  //   QWidget::connect(quit,SIGNAL(clicked()),this,SLOT(exitNow()));
  //   QWidget::connect(test,SIGNAL(clicked()),this,SLOT(LaunchHelpWindow()));
  //   QWidget::connect(run,SIGNAL(clicked()),this,SLOT(Run()));
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(m_text);
  //   QWidget *buttons = new QWidget;
  //   QHBoxLayout *hlayout = new QHBoxLayout;
  //   hlayout->addWidget(run);
  //   hlayout->addWidget(test);
  //   hlayout->addWidget(quit);
  //   buttons->setLayout(hlayout);
  //   layout->addWidget(buttons);
  setLayout(layout);
}

QString MustMatch(QRegExp re, QString source) {
  if (re.indexIn(source) < 0)
    Halt("Bad line: " + source);
  return re.cap(1);
}

bool TestMatch(QRegExp re, QString source) {
  return (re.indexIn(source) >= 0);
}

QString GetLine(QTextStream &strm) {
  QString wrk = strm.readLine(0);
  std::string version(Interpreter::getVersionString());
  QString verstring(QString::fromStdString(version));
  int ndx = verstring.indexOf("v");
  verstring.remove(0,ndx+1);
  wrk.replace("<VERSION_NUMBER>",verstring);
  return wrk;
}

void ProcessFile(QFileInfo fileinfo, HelpWriter *out) {
  QRegExp docblock_pattern;
  QRegExp modulename_pattern;
  QRegExp moduledesc_pattern;
  QRegExp sectioname_pattern;
  QRegExp groupname_pattern;
  QRegExp execin_pattern;
  QRegExp execout_pattern;
  QRegExp verbatimin_pattern;
  QRegExp verbatimout_pattern;
  QRegExp ccomment_pattern;
  QRegExp figure_pattern;
  QRegExp eqnin_pattern;
  QRegExp eqnout_pattern;
  QRegExp fnin_pattern;
  QRegExp fnout_pattern;
  QRegExp enumeratein_pattern;
  QRegExp enumerateout_pattern;
  QRegExp itemizein_pattern;
  QRegExp itemizeout_pattern;
  QRegExp item_pattern;

  std::string version(Interpreter::getVersionString());
  QString verstring(QString::fromStdString(version));
  int ndx = verstring.indexOf("v");
  verstring.remove(0,ndx+1);

  if (fileinfo.suffix() == "cpp") { 
    docblock_pattern = QRegExp("^\\s*//!");
    modulename_pattern = QRegExp("^\\s*//@Module\\s*(\\b\\w+\\b)");
    moduledesc_pattern = QRegExp("^\\s*//@Module\\s*(\\b.*)");
    sectioname_pattern = QRegExp("^\\s*//@@Section\\s*(\\b\\w+\\b)");
    groupname_pattern = QRegExp("^\\s*//@@(.*)");
    execin_pattern = QRegExp("^\\s*//@<(.*)?");
    execout_pattern = QRegExp("^\\s*//@>");
    verbatimin_pattern = QRegExp("^\\s*//@\\[");
    verbatimout_pattern = QRegExp("^\\s*//@\\]");
    ccomment_pattern = QRegExp("^\\s*//(.*)");
    figure_pattern = QRegExp("^\\s*//@figure\\s*(\\b\\w+\\b)");
    // \s*//\s*\\\[
    eqnin_pattern = QRegExp("^\\s*//\\s*\\\\\\[");
    eqnout_pattern = QRegExp("^\\s*//\\s*\\\\\\]");
    fnin_pattern = QRegExp("^\\s*//@\\{\\s*(\\b[\\w\\.]+\\b)");
    fnout_pattern = QRegExp("^\\s*//@\\}");
    // \s*//\\begin\{enumerate\}
    enumeratein_pattern = QRegExp("^\\s*//\\\\begin\\{enumerate\\}");
    enumerateout_pattern = QRegExp("^\\s*//\\\\end\\{enumerate\\}");
    itemizein_pattern = QRegExp("^\\s*//\\\\begin\\{itemize\\}");
    itemizeout_pattern = QRegExp("^\\s*//\\\\end\\{itemize\\}");
    item_pattern = QRegExp("^\\s*//\\s*\\\\item(.*)");
  } else {
    docblock_pattern = QRegExp("^\\s*%!");
    modulename_pattern = QRegExp("^\\s*%@Module\\s*(\\b\\w+\\b)");
    moduledesc_pattern = QRegExp("^\\s*%@Module\\s*(\\b.*)");
    sectioname_pattern = QRegExp("^\\s*%@@Section\\s*(\\b\\w+\\b)");
    groupname_pattern = QRegExp("^\\s*%@@(.*)");
    execin_pattern = QRegExp("^\\s*%@<(.*)?");
    execout_pattern = QRegExp("^\\s*%@>");
    verbatimin_pattern = QRegExp("^\\s*%@\\[");
    verbatimout_pattern = QRegExp("^\\s*%@\\]");
    ccomment_pattern = QRegExp("^\\s*%(.*)");
    figure_pattern = QRegExp("^\\s*%@figure\\s*(\\b\\w+\\b)");
    eqnin_pattern = QRegExp("^\\s*%\\s*\\\\\\[");
    eqnout_pattern = QRegExp("^\\s*%\\s*\\\\\\]");
    fnin_pattern = QRegExp("^\\s*%@\\{\\s*(\\b[\\w\\.]+\\b)");
    fnout_pattern = QRegExp("^\\s*%@\\}");
    enumeratein_pattern = QRegExp("^\\s*%\\\\begin\\{enumerate\\}");
    enumerateout_pattern = QRegExp("^\\s*%\\\\end\\{enumerate\\}");
    itemizein_pattern = QRegExp("^\\s*%\\\\begin\\{itemize\\}");
    itemizeout_pattern = QRegExp("^\\s*%\\\\end\\{itemize\\}");
    item_pattern = QRegExp("^\\s*%\\s*\\\\item(.*)");
  }
  context = new Context;
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadHandleGraphicsFunctions(context);  
  modulename_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  moduledesc_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  sectioname_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  groupname_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  figure_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  if (fileinfo.suffix() == "cpp" | fileinfo.suffix() == "m") {
    TermOutputText("Processing File " + fileinfo.absoluteFilePath() + "...\n");
    if (fileinfo.baseName() == "MPIWrap") {
      TermOutputText("...Skipping MPI routines...\n");
      return;
    }
    QFile file(fileinfo.absoluteFilePath());
    if (file.open(QFile::ReadOnly)) {
      QTextStream fstr(&file);
      while (!fstr.atEnd()) {
	QString line(GetLine(fstr));
	if (TestMatch(docblock_pattern,line)) {
	  QString line(GetLine(fstr));
	  QString modname(MustMatch(modulename_pattern,line));
	  QString moddesc(MustMatch(moduledesc_pattern,line));
	  line = GetLine(fstr);
	  QString secname(MustMatch(sectioname_pattern,line));
	  //	  qDebug("Module " + modname + " " + moddesc + " " + secname);
	  secname = secname.toLower();
	  out->BeginModule(modname,moddesc,secname);
	  line = GetLine(fstr);
	  while (!fstr.atEnd() && !TestMatch(docblock_pattern,line)) {
	    QString groupname(MustMatch(groupname_pattern,line));
	    out->BeginGroup(groupname);
	    line = GetLine(fstr);
	    while (!fstr.atEnd() && !TestMatch(groupname_pattern,line) && !TestMatch(docblock_pattern,line)) {
	      if (TestMatch(execin_pattern,line)) {
		line = MustMatch(execin_pattern,line);
		int ErrorsExpected = 0;
		if (!line.isEmpty())
		  ErrorsExpected = line.toInt();
		QStringList cmdlist;
		line = GetLine(fstr);
		while (!fstr.atEnd() && !TestMatch(execout_pattern,line)) {
		  if (TestMatch(ccomment_pattern,line)) 
		    cmdlist.push_back(MustMatch(ccomment_pattern,line)+"\n");
		  line = GetLine(fstr);
		}
		cmdlist.push_back("quit;\n");
		QString resp(EvaluateCommands(cmdlist,ErrorsExpected,modname,fileinfo.absoluteFilePath()));
		out->BeginVerbatim();
		out->OutputText(resp);
		out->EndVerbatim();
		if (fstr.atEnd())
		  Halt("Unmatched docblock detected!");
		line = GetLine(fstr);
	      } else if (TestMatch(verbatimin_pattern,line)) {
		line = GetLine(fstr);
		out->BeginVerbatim();
		while (!fstr.atEnd() && !TestMatch(verbatimout_pattern,line)) {
		  out->OutputText(MustMatch(ccomment_pattern,line)+"\n");
		  line = GetLine(fstr);
		}
		out->EndVerbatim();
		if (fstr.atEnd())
		  Halt("Unmatched verbatim block detected!");
		line = GetLine(fstr);
	      } else if (TestMatch(figure_pattern,line)) {
		out->DoFigure(MustMatch(figure_pattern,line));
		line = GetLine(fstr);
	      } else if (TestMatch(eqnin_pattern,line)) {
		line = GetLine(fstr);
		QString eqn; 
		while (!fstr.atEnd() && !TestMatch(eqnout_pattern,line)) {
		  eqn += MustMatch(ccomment_pattern,line)+"\n";
		  line = GetLine(fstr);
		}
		out->DoEquation(eqn);
		if (fstr.atEnd())
		  Halt("Unmatched equation block detected!");
		line = GetLine(fstr);
	      } else if (TestMatch(fnin_pattern,line)) {
		QString fname(MustMatch(fnin_pattern,line));
		line = GetLine(fstr);
		QString fn;
		while (!fstr.atEnd() && !TestMatch(fnout_pattern,line)) {
		  fn += MustMatch(ccomment_pattern,line)+"\n";
		  line = GetLine(fstr);
		}
		QFile *myfile = new QFile(fname);
		if (!myfile->open(QFile::WriteOnly))
		  Halt("Unable to open "+fname+" for output");
		QTextStream *t = new QTextStream(myfile);
		*t << fn;
		delete t;
		delete myfile;
		generatedFileList << fname;
		out->DoFile(fname,fn);
		line = GetLine(fstr);
		if (fstr.atEnd())
		  Halt("Unmatched function block detected!");
	      } else if (TestMatch(enumeratein_pattern,line)) {
		line = GetLine(fstr);
		QStringList itemlist;
		while(!fstr.atEnd() && !TestMatch(enumerateout_pattern,line)) {
		  QString item(MustMatch(item_pattern,line));
		  line = GetLine(fstr);
		  while (!TestMatch(item_pattern,line) && 
			 !TestMatch(enumerateout_pattern,line) && 
			 !fstr.atEnd()) {
		    item += MustMatch(ccomment_pattern,line);
		    line = GetLine(fstr);
		  }
		  item += "\n";
		  itemlist << item;
		}
		if (fstr.atEnd())
		  Halt("Unmatched enumeration block");
		out->DoEnumerate(itemlist);
		line = GetLine(fstr);
	      } else if (TestMatch(itemizein_pattern,line)) {
		line = GetLine(fstr);
		QStringList itemlist;
		while(!fstr.atEnd() && !TestMatch(itemizeout_pattern,line)) {
		  QString item(MustMatch(item_pattern,line));
		  line = GetLine(fstr);
		  while (!TestMatch(item_pattern,line) && 
			 !TestMatch(itemizeout_pattern,line) && 
			 !fstr.atEnd()) {
		    item += MustMatch(ccomment_pattern,line);
		    line = GetLine(fstr);
		  }
		  item += "\n";
		  itemlist << item;
		}
		if (fstr.atEnd())
		  Halt("Unmatched itemize block");
		out->DoItemize(itemlist);
		line = GetLine(fstr);
	      } else {
		if (TestMatch(ccomment_pattern,line)) 
		  line = MustMatch(ccomment_pattern,line);
		out->OutputText(line+"\n");
		line = GetLine(fstr);
	      }
	    }
	    if (!TestMatch(docblock_pattern,line) && fstr.atEnd())
	      Halt("Unmatched docblock detected!");
	  }
	  if (!TestMatch(docblock_pattern,line) && fstr.atEnd())
	    Halt("Unmatched docblock detected!");
	  out->EndModule();
	}
      }
    }
  }
  qApp->processEvents();
  delete context;
}

void MergeFile(QFileInfo fileinfo) {
  if (fileinfo.suffix() != "m")
    return;
  // It is a .m file...  Read in the documentation for it
  // from the text directory
  QString headerdocs;
  QFile f(sourcepath+"/help/text/" + fileinfo.baseName() + ".mdc");
  if (f.open(QFile::ReadOnly)) {
    QTextStream s(&f);
    while (!s.atEnd())
      headerdocs += "% " + s.readLine(0) + "\n";
  }
  // Open the file
  QFile h(fileinfo.absoluteFilePath());
  QString newname(fileinfo.absoluteFilePath());
  int p = newname.indexOf("toolbox",0,Qt::CaseInsensitive);
  newname = newname.remove(0,p);
  newname = sourcepath+"/help/"+newname;
  // Create the directory structure if necessary
  QFileInfo fi(newname);
  QDir dir;
  dir.mkpath(fi.absolutePath());
  QFile g(newname);
  if (!h.open(QFile::ReadOnly))
    Halt("Unable to open " + fileinfo.absoluteFilePath() + " for read...\n");
  if (!g.open(QFile::WriteOnly))
    Halt("Unable to open " + newname + " for write...\n");
  QRegExp docblock_pattern("^\\s*%!");
  QTextStream s(&h);
  QTextStream t(&g);
  while (!s.atEnd()) {
    QString line(s.readLine(0));
    if (TestMatch(docblock_pattern,line)) {
      QString line(s.readLine(0));
      while (!TestMatch(docblock_pattern,line))
	line = s.readLine(0);
      t << headerdocs << "\n";
    } else
      t << line << "\n";
  }
}

void MergeMFiles(QDir dir) {
  TermOutputText("Merging M Files in Directory " + dir.absolutePath() + "...\n");
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir())
      MergeMFiles(QDir(fileInfo.absoluteFilePath()));
    else
      MergeFile(fileInfo);
  }
}

void ProcessDir(QDir dir, HelpWriter *out, bool recurse) {
  TermOutputText("Processing Directory " + dir.absolutePath() + "...\n");
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir() && recurse)
      ProcessDir(QDir(fileInfo.absoluteFilePath()),out,recurse);
    else
      ProcessFile(fileInfo,out);
  }
}

void ReadSectionDescriptors() {
  QRegExp re("(\\b\\w+\\b)\\s*(.*)");
  TermOutputText("Reading section descriptors...\n");
  QFile file(sourcepath+"/tools/helpgen/section_descriptors.txt");
  if (!file.open(QFile::ReadOnly))
    Halt("Unable to open section descriptors file for reading");
  QTextStream f(&file);
  while (!f.atEnd()) {
    QString line(f.readLine(0));
    if (re.indexIn(line) < 0)
      Halt("Bad line: " + line);
    section_descriptors.insert(re.cap(1),re.cap(2));
    sectionOrdered << re.cap(1);
    TermOutputText("Section Descriptor: " + re.cap(1) + " : " + re.cap(2) + "\n");
  }
}

void DeleteDirectory(QString dirname) {
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

void MakePDF() {
  QProcess dopdf;
  dopdf.setWorkingDirectory(sourcepath+"/help/latex");
  dopdf.start("pdflatex",QStringList() << "main.tex");
  while (!dopdf.waitForFinished()) {
  }
  dopdf.start("pdflatex",QStringList() << "main.tex");
  while (!dopdf.waitForFinished()) {
  }
}

void ConsoleWidget::Run() {
  QDir dir;
  DeleteDirectory(sourcepath+"/help/html");
  DeleteDirectory(sourcepath+"/help/tmp");
  DeleteDirectory(sourcepath+"/help/latex");
  DeleteDirectory(sourcepath+"/help/text");
  DeleteDirectory(sourcepath+"/help/test");
  DeleteDirectory(sourcepath+"/help/toolbox");
  dir.mkpath(sourcepath+"/help/html");
  dir.mkpath(sourcepath+"/help/tmp");
  dir.mkpath(sourcepath+"/help/latex");
  dir.mkpath(sourcepath+"/help/text");
  dir.mkpath(sourcepath+"/help/test");
  dir.mkpath(sourcepath+"/help/toolbox");
  ReadSectionDescriptors();
  LatexWriter texout;
  HTMLWriter htmlout;
  GroupWriter out;
  TextWriter txtout;
  TestWriter tstout;
  out.RegisterWriter(&texout);
  out.RegisterWriter(&htmlout);
  out.RegisterWriter(&txtout);
  out.RegisterWriter(&tstout);
  ProcessDir(QDir(sourcepath+"/toolbox"),&out,true); 
  ProcessDir(QDir(sourcepath+"/libs"),&out,true); 
  ProcessDir(QDir(sourcepath+"/help"),&out,false); 
  MergeMFiles(QDir(sourcepath+"/toolbox"));
  out.WriteIndex();
  std::string version(Interpreter::getVersionString());
  QString verstring(QString::fromStdString(version));
  int ndx = verstring.indexOf("v");
  verstring.remove(0,ndx+1);
  for (int i=0;i<generatedFileList.size();i++) {
    QFile tmp(generatedFileList[i]);
    tmp.remove();
  }
  // Make the pdf
  // TermOutputText("\nBuilding PDF\n");
  //  MakePDF();
  TermOutputText("\n\nDone!\n");
  qApp->exit();
}

void DoHelpGen(QString sourcep) {
  sourcepath = sourcep;
  ConsoleWidget *m_main = new ConsoleWidget;
  m_main->show();
  QTimer::singleShot(0,m_main,SLOT(Run()));
  qApp->exec();
}

void usage() {
  std::cerr << "Usage: FMHelpGen <path_to_source>\n";
  exit(1);
}

int main(int argc, char *argv[]) {
  QApplication a(argc,argv);
  if (argc != 2) usage();
  DoHelpGen(argv[1]);
  return 0;
}
