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
#include "HelpWidget.hpp"
#include "fmhelpapp.hpp"
#include <QtGui>

QTextEdit *m_text;

QString output;
QStringList input;
QMap<QString, QString> section_descriptors;
int moduledepth = 0;

void TermOutputText(QString str) {
  //  m_text->textCursor().movePosition(QTextCursor::End);
  m_text->insertPlainText(str);
  m_text->ensureCursorVisible();
  //  qDebug() << str;
  qApp->processEvents();
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
  QFile file("html/" + secname + ".html");
  if (!file.open(QFile::WriteOnly))
    Halt("Unable to open html/" + secname + ".html for output");
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
  for (unsigned i=0;i<modinfo.size();i++)
    f << "<LI> <A HREF=" << modinfo[i][0] << ".html> " << modinfo[i][0] << "</A> " << modinfo[i][1] << "</LI>\n";
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
  QFile file("html/index.html");
  if (!file.open(QFile::WriteOnly))
    Halt("Unable to open html/index.html for output");
  QTextStream f(&file);
  f << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n";
  f << "\n";
  f << "<HTML>\n";
  f << "<HEAD>\n";
  f << "<TITLE>" << "FreeMat v2.0 Documentation" << "</TITLE>\n";
  f << "</HEAD>\n";
  f << "<BODY>\n";
  f << "<H2>" << "FreeMat v2.0 Documentation" << "</H2>\n";
  f << "<P>\n";
  f << "<H1> Documentation Sections </H1>\n";
  f << "<UL>\n";
  foreach (QString secname, sections) 
    f << "<LI> <A HREF=" << secname << ".html> " << 
      section_descriptors.value(secname) << "</A> </LI>\n";
  f << "</UL>\n";
  f << "</BODY>\n";
  f << "</HTML>\n";
  // Build the module list
  QFile file2("html/modules.txt");
  if (!file2.open(QFile::WriteOnly | QIODevice::Text))
    Halt("Unable to open html/modules.txt for output");
  QList<QStringList> moduledlist(sectables.values());
  QStringList modulenames;
  for (unsigned k=0;k<moduledlist.size();k++)
    modulenames << moduledlist[k][0].toLower();
  modulenames.sort();
  QTextStream f2(&file2);
  for (unsigned k=0;k<modulenames.size();k++) {
    f2 << modulenames[k] << endl;
  }
  // Build the section index
  QFile file3("html/sectable.txt");
  if (!file3.open(QFile::WriteOnly | QIODevice::Text))
    Halt("Unable to open html/sectable.txt for output");
  QTextStream f3(&file3);
  foreach (QString secname, sections) {
    QString secdesc(section_descriptors.value(secname));
    QList<QStringList> modules(sectables.values(secname));
    QStringList moduleList;
    for (unsigned m=0;m<modules.size();m++)
      moduleList << modules[m][1];
    moduleList.sort();
    f3 << secdesc << "\n";
    for (unsigned m=0;m<moduleList.size();m++) {
      f3 << "+" << moduleList[m] << "\n";
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
  QFile file("tmp/" + modulename+"_eqn.tex");
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
  latex.setWorkingDirectory("tmp");
  latex.start("latex",QStringList() << (modulename + "_eqn.tex"));
  if (!latex.waitForFinished())
    Halt("LaTeX for " + modulename + "_eqn.tex never returned");
  QProcess dvipng;
  dvipng.setWorkingDirectory("tmp");
  dvipng.start("dvipng",QStringList() << "-T tight" << (modulename + "_eqn.dvi"));
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
  myfile = new QFile("html/" + modulename + ".html");
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
  *mystream << "Section: <A HREF=" << secname.toLower() << ".html> " << section_descriptors.value(secname.toLower()) << "</A>\n";
  verbatim = false;
}

void HTMLWriter::BeginGroup(QString groupname) {
  *mystream << "<H3>" << groupname << "</H3>\n";
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
  *mystream << ftxt << "\n";
  *mystream << "</PRE>\n";
  *mystream << "<P>\n";
}

class LatexWriter : public HelpWriter {
  QFile *myfile;
  QTextStream *mystream;
  QMultiMap<QString, QString> sectables;
  bool verbatim;
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
};


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
  myfile = new QFile("latex/" + modname.toLower() + ".tex");
  if (!myfile->open(QFile::WriteOnly)) {
    Halt("Unable to open " + modname + ".tex for output " + QString().sprintf("%d",myfile->error()) + " depth = " + QString().sprintf("%d",moduledepth));
  }
  mystream = new QTextStream(myfile);
  *mystream << "\\subsection{" + moddesc + "}\n\n";
  sectables.insert(secname,modname);
  verbatim = false;
}

void LatexWriter::BeginGroup(QString groupname) {
  *mystream << "\\subsubsection{" + groupname + "}\n\n";
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
  *mystream << "\\centerline{\\includegraphics[width=8cm]{name}}\n";
}

void LatexWriter::DoEquation(QString eqn) {
  *mystream << "\\[\\n" << eqn << "\\]\\n";
}

void LatexWriter::DoFile(QString filename, QString ftext) {
  *mystream << "\\begin{verbatim}\n";
  *mystream << "     " << filename << "\n";
  *mystream << ftext;
  *mystream << "\\end{verbatim}\n";
}

namespace FreeMat {
  class HelpTerminal : public KeyManager {
  public:
    HelpTerminal() {}
    virtual ~HelpTerminal() {}
    virtual void Initialize() {}
    virtual void RestoreOriginalMode() {}
    virtual void OutputRawString(std::string txt) {
      output += txt.c_str();
    }
    virtual void ResizeEvent() {}
    virtual void MoveDown() {
      output += "\n";
    }
    virtual char* getLine(std::string aprompt) {
      if (input.empty()) return 0;
      QString txt(input[0]);
      input.removeFirst();
      char *rettxt = strdup(qPrintable(txt));
      if (!input.empty())
	output += aprompt.c_str() + txt;
      return (strdup(qPrintable(txt)));      
    }
    virtual void MoveUp() {};
    virtual void MoveRight() {};
    virtual void MoveLeft() {};
    virtual void ClearEOL() {};
    virtual void ClearEOD() {};
    virtual void MoveBOL() {};
    virtual int getTerminalWidth() {return 80;}
  };
};

HelpTerminal *m_term;
using namespace FreeMat;
Context *context;

WalkTree* GetInterpreter() {
  m_term = new HelpTerminal;
  LoadModuleFunctions(context);
  LoadClassFunction(context);
  LoadCoreFunctions(context);
  LoadFNFunctions(context);
  LoadHandleGraphicsFunctions(context);  
  //   const char *envPtr;
  //   envPtr = getenv("FREEMAT_PATH");
  m_term->setContext(context);
  //   if (envPtr)
  //     m_term->setPath(std::string(envPtr));
  //   else 
  //     m_term->setPath(std::string(""));
  m_term->setPath("../../MFiles");
  WalkTree *twalk = new WalkTree(context,m_term);
  return twalk;
}

QString EvaluateCommands(QStringList cmds, int expectedCount, QString modulename, QString file) {
  input = cmds;
  output.clear();
  WalkTree* twalk = GetInterpreter();
  try {
    while (!input.empty()) {
      try {
	twalk->evalCLI();
      } catch (WalkTreeRetallException) {
	twalk->clearStacks();
      } catch (WalkTreeReturnException &e) {
      } catch (WalkTreeQuitException) {
      }
    }
  } catch (WalkTreeQuitException &e) {
  } catch (std::exception& e) {
    std::cout << "Exception caught: " << e.what() << "\n";
  }
  if (twalk->getErrorCount() != expectedCount) 
    Halt("Error: Got " + QString().sprintf("%d",twalk->getErrorCount()) + ", expected " + QString().sprintf("%d",expectedCount) + " in module " + modulename + " in file " + file + "\n\nOutput Follows:\n"+output);
  ShutdownHandleGraphics();
  delete twalk;
  delete m_term;
  QRegExp mprintre("(--> mprint[^\\n]*)");
  output = output.replace(mprintre,"");
  return output;
}

void ConsoleWidget::exitNow() {
  exit(1);
}

void ConsoleWidget::LaunchHelpWindow() {
  HelpWindow *m_helpwin = new HelpWindow("/sandbox/freemat2/help/fmhelpapp/html");
  m_helpwin->show();
}


ConsoleWidget::ConsoleWidget() : QWidget() {
  m_text = new QTextEdit;
  m_text->setReadOnly(true);
  resize(600,400);
  m_text->setFontFamily("Courier");
  QPushButton *run = new QPushButton("Run");
  QPushButton *quit = new QPushButton("Quit");
  QPushButton *test = new QPushButton("Test");
  QWidget::connect(quit,SIGNAL(clicked()),this,SLOT(exitNow()));
  QWidget::connect(test,SIGNAL(clicked()),this,SLOT(LaunchHelpWindow()));
  QWidget::connect(run,SIGNAL(clicked()),this,SLOT(Run()));
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(m_text);
  QWidget *buttons = new QWidget;
  QHBoxLayout *hlayout = new QHBoxLayout;
  hlayout->addWidget(run);
  hlayout->addWidget(test);
  hlayout->addWidget(quit);
  buttons->setLayout(hlayout);
  layout->addWidget(buttons);
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
	QString line(fstr.readLine(0));
	if (TestMatch(docblock_pattern,line)) {
	  QString line(fstr.readLine(0));
	  QString modname(MustMatch(modulename_pattern,line));
	  QString moddesc(MustMatch(moduledesc_pattern,line));
	  line = fstr.readLine(0);
	  QString secname(MustMatch(sectioname_pattern,line));
	  qDebug("Module " + modname + " " + moddesc + " " + secname);
	  secname = secname.toLower();
	  out->BeginModule(modname,moddesc,secname);
	  line = fstr.readLine(0);
	  while (!fstr.atEnd() && !TestMatch(docblock_pattern,line)) {
	    QString groupname(MustMatch(groupname_pattern,line));
	    out->BeginGroup(groupname);
	    line = fstr.readLine(0);
	    while (!fstr.atEnd() && !TestMatch(groupname_pattern,line) && !TestMatch(docblock_pattern,line)) {
	      if (TestMatch(execin_pattern,line)) {
		line = MustMatch(execin_pattern,line);
		int ErrorsExpected = 0;
		if (!line.isEmpty())
		  ErrorsExpected = line.toInt();
		QStringList cmdlist;
		line = fstr.readLine(0);
		while (!fstr.atEnd() && !TestMatch(execout_pattern,line)) {
		  if (TestMatch(ccomment_pattern,line)) 
		    cmdlist.push_back(MustMatch(ccomment_pattern,line)+"\n");
		  line = fstr.readLine(0);
		}
		cmdlist.push_back("quit;\n");
		QString resp(EvaluateCommands(cmdlist,ErrorsExpected,modname,fileinfo.absoluteFilePath()));
		out->BeginVerbatim();
		out->OutputText(resp);
		out->EndVerbatim();
		if (fstr.atEnd())
		  Halt("Unmatched docblock detected!");
		line = fstr.readLine(0);
	      } else if (TestMatch(verbatimin_pattern,line)) {
		line = fstr.readLine(0);
		out->BeginVerbatim();
		while (!fstr.atEnd() && !TestMatch(verbatimout_pattern,line)) {
		  out->OutputText(MustMatch(ccomment_pattern,line)+"\n");
		  line = fstr.readLine(0);
		}
		out->EndVerbatim();
		if (fstr.atEnd())
		  Halt("Unmatched verbatim block detected!");
		line = fstr.readLine(0);
	      } else if (TestMatch(figure_pattern,line)) {
		out->DoFigure(MustMatch(figure_pattern,line));
		line = fstr.readLine(0);
	      } else if (TestMatch(eqnin_pattern,line)) {
		line = fstr.readLine(0);
		QString eqn; 
		while (!fstr.atEnd() && !TestMatch(eqnout_pattern,line)) {
		  eqn += MustMatch(ccomment_pattern,line)+"\n";
		  line = fstr.readLine(0);
		}
		out->DoEquation(eqn);
		if (fstr.atEnd())
		  Halt("Unmatched equation block detected!");
		line = fstr.readLine(0);
	      } else if (TestMatch(fnin_pattern,line)) {
		QString fname(MustMatch(fnin_pattern,line));
		line = fstr.readLine(0);
		QString fn;
		while (!fstr.atEnd() && !TestMatch(fnout_pattern,line)) {
		  fn += MustMatch(ccomment_pattern,line)+"\n";
		  line = fstr.readLine(0);
		}
		QFile *myfile = new QFile(fname);
		if (!myfile->open(QFile::WriteOnly))
		  Halt("Unable to open "+fname+" for output");
		QTextStream *t = new QTextStream(myfile);
		*t << fn;
		delete t;
		delete myfile;
		out->DoFile(fname,fn);
		if (fstr.atEnd())
		  Halt("Unmatched function block detected!");
	      } else if (TestMatch(enumeratein_pattern,line)) {
		line = fstr.readLine(0);
		QStringList itemlist;
		while(!fstr.atEnd() && !TestMatch(enumerateout_pattern,line)) {
		  QString item(MustMatch(item_pattern,line));
		  line = fstr.readLine(0);
		  while (!TestMatch(item_pattern,line) && 
			 !TestMatch(enumerateout_pattern,line) && 
			 !fstr.atEnd()) {
		    item += MustMatch(ccomment_pattern,line);
		    line = fstr.readLine(0);
		  }
		  item += "\n";
		  itemlist << item;
		}
		if (fstr.atEnd())
		  Halt("Unmatched enumeration block");
		out->DoEnumerate(itemlist);
		line = fstr.readLine(0);
	      } else if (TestMatch(itemizein_pattern,line)) {
		line = fstr.readLine(0);
		QStringList itemlist;
		while(!fstr.atEnd() && !TestMatch(itemizeout_pattern,line)) {
		  QString item(MustMatch(item_pattern,line));
		  line = fstr.readLine(0);
		  while (!TestMatch(item_pattern,line) && 
			 !TestMatch(itemizeout_pattern,line) && 
			 !fstr.atEnd()) {
		    item += MustMatch(ccomment_pattern,line);
		    line = fstr.readLine(0);
		  }
		  item += "\n";
		  itemlist << item;
		}
		if (fstr.atEnd())
		  Halt("Unmatched itemize block");
		out->DoItemize(itemlist);
		line = fstr.readLine(0);
	      } else {
		if (TestMatch(ccomment_pattern,line)) 
		  line = MustMatch(ccomment_pattern,line);
		out->OutputText(line+"\n");
		line = fstr.readLine(0);
	      }
	    }
	    if (fstr.atEnd())
	      Halt("Unmatched docblock detected!");
	  }
	  if (fstr.atEnd())
	    Halt("Unmatched docblock detected!");
	  out->EndModule();
	}
      }
    }
  }
  qApp->processEvents();
  delete context;
}

void ProcessDir(QDir dir, HelpWriter *out) {
  TermOutputText("Processing Directory " + dir.absolutePath() + "...\n");
  dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  QFileInfoList list = dir.entryInfoList();
  for (unsigned i=0;i<list.size();i++) {
    QFileInfo fileInfo = list.at(i);
    if (fileInfo.isDir())
      ProcessDir(QDir(fileInfo.absoluteFilePath()),out);
    else
      ProcessFile(fileInfo,out);
  }
  qApp->processEvents();
}

void ReadSectionDescriptors() {
  QRegExp re("(\\b\\w+\\b)\\s*(.*)");
  TermOutputText("Reading section descriptors...\n");
  QFile file("section_descriptors.txt");
  if (!file.open(QFile::ReadOnly))
    Halt("Unable to open section descriptors file for reading");
  QTextStream f(&file);
  while (!f.atEnd()) {
    QString line(f.readLine(0));
    if (re.indexIn(line) < 0)
      Halt("Bad line: " + line);
    section_descriptors.insert(re.cap(1),re.cap(2));
    TermOutputText("Section Descriptor: " + re.cap(1) + " : " + re.cap(2) + "\n");
  }
}

void ConsoleWidget::Run() {
  ReadSectionDescriptors();
  LatexWriter texout;
  HTMLWriter htmlout;
  GroupWriter out;
  out.RegisterWriter(&texout);
  out.RegisterWriter(&htmlout);
  ProcessDir(QDir("../../MFiles"),&out); 
  ProcessDir(QDir("../../libs"),&out); 
  out.WriteIndex();
  TermOutputText("\n\nDone!\n");
}

void ReadModuleNames() {
  QFile file("html/modules.txt");
  if (!file.open(QFile::ReadOnly))
    Halt("Unable to open module list");
  QTextStream f(&file);
  while (!f.atEnd()) {
    QString line(f.readLine(0));
    qDebug(line);
  }
}

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  ConsoleWidget *m_main = new ConsoleWidget;
  m_main->show();
  // Get the section table
  ReadModuleNames();
  return app.exec();
  return 0;
}
