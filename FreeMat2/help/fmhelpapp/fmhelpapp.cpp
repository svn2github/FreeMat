#include <QApplication>
#include <QTextEdit>
#include <QTextStream>
#include <QPushButton>
#include <QVBoxLayout>
#include <QtDebug>
#include <QDir>
#include "KeyManager.hpp"
#include "Module.hpp"
#include "Class.hpp"
#include "LoadCore.hpp"
#include "LoadFN.hpp"
#include "GraphicsCore.hpp"
#include "File.hpp"
#include "Context.hpp"

QTextEdit *m_text;

QString output;
QStringList input;

void OutputText(QString str) {
  m_text->insertPlainText(str);
  //  m_text->setCursor(m_text->textCursor().movePosition(QCursor::End));
  m_text->ensureCursorVisible();
  qDebug() << str;
}

void Halt(QString emsg) {
  OutputText(emsg);
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
};

void GroupWriter::RegisterWriter(HelpWriter*ptr) {
  cast.push_back(ptr);
}

void GroupWriter::EndModule() {
  for (unsigned i=0;i<cast.size();i++) cast[i]->EndModule();
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

class HTMLWriter : public HelpWriter {
  QFile *myfile;
  QTextStream *mystream;
  QMultiMap<QString, QString> sectables;
  QStringList eqnlist;
  bool verbatim;
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
};

#if 0
// NOTE - GPL'ed code from doxygen. 
void FormulaList::generateBitmaps(const char *path)
{
  int x1,y1,x2,y2;
  QDir d(path);
  // store the original directory
  if (!d.exists()) { err("Error: Output dir %s does not exist!\n",path); exit(1); }
  QCString oldDir = convertToQCString(QDir::currentDirPath());
  // go to the html output directory (i.e. path)
  QDir::setCurrent(d.absPath());
  QDir thisDir;
  // generate a latex file containing one formula per page.
  QCString texName="_formulas.tex";
  QList<int> pagesToGenerate;
  pagesToGenerate.setAutoDelete(TRUE);
  FormulaListIterator fli(*this);
  Formula *formula;
  QFile f(texName);
  bool formulaError=FALSE;
  if (f.open(IO_WriteOnly))
  {
    QTextStream t(&f);
    if (Config_getBool("LATEX_BATCHMODE")) t << "\\batchmode" << endl;
    t << "\\documentclass{article}" << endl;
    t << "\\usepackage{epsfig}" << endl; // for those who want to include images
    const char *s=Config_getList("EXTRA_PACKAGES").first();
    while (s)
    {
      t << "\\usepackage{" << s << "}\n";
      s=Config_getList("EXTRA_PACKAGES").next();
    }
    t << "\\pagestyle{empty}" << endl; 
    t << "\\begin{document}" << endl;
    int page=0;
    for (fli.toFirst();(formula=fli.current());++fli)
    {
      QCString resultName;
      resultName.sprintf("form_%d.png",formula->getId());
      // only formulas for which no image exists are generated
      QFileInfo fi(resultName);
      if (!fi.exists())
      {
        // we force a pagebreak after each formula
        t << formula->getFormulaText() << endl << "\\pagebreak\n\n";
        pagesToGenerate.append(new int(page));
      }
      page++;
    }
    t << "\\end{document}" << endl;
    f.close();
  }
  if (pagesToGenerate.count()>0) // there are new formulas
  {
    //printf("Running latex...\n");
    //system("latex _formulas.tex </dev/null >/dev/null");
    QCString latexCmd = Config_getString("LATEX_CMD_NAME");
    if (latexCmd.isEmpty()) latexCmd="latex";
    if (iSystem(latexCmd,"_formulas.tex")!=0)
    {
      err("Problems running latex. Check your installation or look for typos in _formulas.tex!\n");
      formulaError=TRUE;
      //return;
    }
    //printf("Running dvips...\n");
    QListIterator<int> pli(pagesToGenerate);
    int *pagePtr;
    int pageIndex=1;
    for (;(pagePtr=pli.current());++pli,++pageIndex)
    {
      int pageNum=*pagePtr;
      msg("Generating image form_%d.png for formula\n",pageNum);
      char dviArgs[4096];
      QCString formBase;
      formBase.sprintf("_form%d",pageNum);
      // run dvips to convert the page with number pageIndex to an
      // encapsulated postscript.
      sprintf(dviArgs,"-q -D 600 -E -n 1 -p %d -o %s.eps _formulas.dvi",
          pageIndex,formBase.data());
      if (iSystem("dvips",dviArgs)!=0)
      {
        err("Problems running dvips. Check your installation!\n");
        return;
      }
      // now we read the generated postscript file to extract the bounding box
      QFileInfo fi(formBase+".eps");
      if (fi.exists())
      {
        QCString eps = fileToString(formBase+".eps");
        int i=eps.find("%%BoundingBox:");
        if (i!=-1)
        {
          sscanf(eps.data()+i,"%%%%BoundingBox:%d %d %d %d",&x1,&y1,&x2,&y2);
        }
        else
        {
          err("Error: Couldn't extract bounding box!\n");
        }
      } 
      // next we generate a postscript file which contains the eps
      // and displays it in the right colors and the right bounding box
      f.setName(formBase+".ps");
      if (f.open(IO_WriteOnly))
      {
        QTextStream t(&f);
        t << "1 1 1 setrgbcolor" << endl;  // anti-alias to white background
        t << "newpath" << endl;
        t << "-1 -1 moveto" << endl;
        t << (x2-x1+2) << " -1 lineto" << endl;
        t << (x2-x1+2) << " " << (y2-y1+2) << " lineto" << endl;
        t << "-1 " << (y2-y1+2) << " lineto" <<endl;
        t << "closepath" << endl;
        t << "fill" << endl;
        t << -x1 << " " << -y1 << " translate" << endl;
        t << "0 0 0 setrgbcolor" << endl;
        t << "(" << formBase << ".eps) run" << endl;
        f.close();
      }
      // scale the image so that it is four times larger than needed.
      // and the sizes are a multiple of four.
      const double scaleFactor = 16.0/3.0; 
      int gx = (((int)((x2-x1)*scaleFactor))+3)&~2;
      int gy = (((int)((y2-y1)*scaleFactor))+3)&~2;
      // Then we run ghostscript to convert the postscript to a pixmap
      // The pixmap is a truecolor image, where only black and white are
      // used.  
#if defined(_WIN32) && !defined(__CYGWIN__)
      char gsArgs[256];
      sprintf(gsArgs,"-q -g%dx%d -r%dx%dx -sDEVICE=ppmraw "
                     "-sOutputFile=%s.pnm -dNOPAUSE -dBATCH -- %s.ps",
                     gx,gy,(int)(scaleFactor*72),(int)(scaleFactor*72),
                     formBase.data(),formBase.data()
             );
      // gswin32 is a GUI api which will pop up a window and run
      // asynchronously. To prevent both, we use ShellExecuteEx and
      // WaitForSingleObject (thanks to Robert Golias for the code)
      SHELLEXECUTEINFO sInfo = {
        sizeof(SHELLEXECUTEINFO),   /* structure size */
        SEE_MASK_NOCLOSEPROCESS,    /* leave the process running */
        NULL,                       /* window handle */
        NULL,                       /* action to perform: open */
        "gswin32.exe",              /* file to execute */
        gsArgs,                     /* argument list */ 
        NULL,                       /* use current working dir */
        SW_HIDE,                    /* minimize on start-up */
        0,                          /* application instance handle */
        NULL,                       /* ignored: id list */
        NULL,                       /* ignored: class name */
        NULL,                       /* ignored: key class */
        0,                          /* ignored: hot key */
        NULL,                       /* ignored: icon */
        NULL                        /* resulting application handle */
      };
      if (!ShellExecuteEx(&sInfo))
      {
        err("Problem running ghostscript. Check your installation!\n");
        return;
      }
      else if (sInfo.hProcess)      /* executable was launched, wait for it to finish */
      {
        WaitForSingleObject(sInfo.hProcess,INFINITE); 
      }
#else
      char gsArgs[4096];
      sprintf(gsArgs,"-q -g%dx%d -r%dx%dx -sDEVICE=ppmraw "
                    "-sOutputFile=%s.pnm -dNOPAUSE -dBATCH -- %s.ps",
                    gx,gy,(int)(scaleFactor*72),(int)(scaleFactor*72),
                    formBase.data(),formBase.data()
             );
      if (iSystem("gs",gsArgs)!=0)
      {
        err("Problem running ghostscript. Check your installation!\n");
        return;
      }
#endif
      f.setName(formBase+".pnm");
      uint imageX=0,imageY=0;
      // we read the generated image again, to obtain the pixel data.
      if (f.open(IO_ReadOnly))
      {
        QTextStream t(&f);
        QCString s;
        if (!t.eof())
          s=t.readLine();
        if (s.length()<2 || s.left(2)!="P6")
          err("Error: ghostscript produced an illegal image format!");
        else
        {
          // assume the size if after the first line that does not start with
          // # excluding the first line of the file.
          while (!t.eof() && (s=t.readLine()) && !s.isEmpty() && s.at(0)=='#');
          sscanf(s,"%d %d",&imageX,&imageY);
        }
        if (imageX>0 && imageY>0)
        {
          //printf("Converting image...\n");
          char *data = new char[imageX*imageY*3]; // rgb 8:8:8 format
          uint i,x,y,ix,iy;
          f.readBlock(data,imageX*imageY*3);
          Image srcImage(imageX,imageY),
                filteredImage(imageX,imageY),
                dstImage(imageX/4,imageY/4);
          uchar *ps=srcImage.getData();
          // convert image to black (1) and white (0) index.
          for (i=0;i<imageX*imageY;i++) *ps++= (data[i*3]==0 ? 1 : 0);
          // apply a simple box filter to the image 
          static int filterMask[]={1,2,1,2,8,2,1,2,1};
          for (y=0;y<srcImage.getHeight();y++)
          {
            for (x=0;x<srcImage.getWidth();x++)
            {
              int s=0;
              for (iy=0;iy<2;iy++)
              {
                for (ix=0;ix<2;ix++)
                {
                  s+=srcImage.getPixel(x+ix-1,y+iy-1)*filterMask[iy*3+ix];
                }
              }
              filteredImage.setPixel(x,y,s);
            }
          }
          // down-sample the image to 1/16th of the area using 16 gray scale
          // colors.
          // TODO: optimize this code.
          for (y=0;y<dstImage.getHeight();y++)
          {
            for (x=0;x<dstImage.getWidth();x++)
            {
              int xp=x<<2;
              int yp=y<<2;
              int c=filteredImage.getPixel(xp+0,yp+0)+
                    filteredImage.getPixel(xp+1,yp+0)+
                    filteredImage.getPixel(xp+2,yp+0)+
                    filteredImage.getPixel(xp+3,yp+0)+
                    filteredImage.getPixel(xp+0,yp+1)+
                    filteredImage.getPixel(xp+1,yp+1)+
                    filteredImage.getPixel(xp+2,yp+1)+
                    filteredImage.getPixel(xp+3,yp+1)+
                    filteredImage.getPixel(xp+0,yp+2)+
                    filteredImage.getPixel(xp+1,yp+2)+
                    filteredImage.getPixel(xp+2,yp+2)+
                    filteredImage.getPixel(xp+3,yp+2)+
                    filteredImage.getPixel(xp+0,yp+3)+
                    filteredImage.getPixel(xp+1,yp+3)+
                    filteredImage.getPixel(xp+2,yp+3)+
                    filteredImage.getPixel(xp+3,yp+3);
              // here we scale and clip the color value so the
              // resulting image has a reasonable contrast
              dstImage.setPixel(x,y,QMIN(15,(c*15)/(16*10)));
            }
          }
          // save the result as a png
          QCString resultName;
          resultName.sprintf("form_%d.png",pageNum);
          // the option parameter 1 is used here as a temporary hack
          // to select the right color palette! 
          dstImage.save(resultName,1);
          delete[] data;
        }
        f.close();
      } 
      // remove intermediate image files
      thisDir.remove(formBase+".eps");
      thisDir.remove(formBase+".pnm");
      thisDir.remove(formBase+".ps");
    }
    // remove intermediate files produced by latex
    thisDir.remove("_formulas.dvi");
    thisDir.remove("_formulas.log");
    thisDir.remove("_formulas.aux");
  }
  // remove the latex file itself
  if (!formulaError) thisDir.remove("_formulas.tex");
  // write/update the formula repository so we know what text the 
  // generated pngs represent (we use this next time to avoid regeneration
  // of the pngs, and to avoid forcing the user to delete all pngs in order
  // to let a browser refresh the images).
  f.setName("formula.repository");
  if (f.open(IO_WriteOnly))
  {
    QTextStream t(&f);
    for (fli.toFirst();(formula=fli.current());++fli)
    {
      t << "\\form#" << formula->getId() << ":" << formula->getFormulaText() << endl;
    }
    f.close();
  }
  // reset the directory to the original location.
  QDir::setCurrent(oldDir);
}
#endif

void HTMLWriter::EndModule() {
  *mystream << "</BODY>\n";
  *mystream << "</HTML>\n";
  if (myfile) delete myfile;
  if (mystream) delete mystream;
}

void HTMLWriter::BeginModule(QString modname, QString moddesc, QString secname) {
  myfile = new QFile(modname.toLower() + ".html");
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
  sectables.insert(secname,modname);
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
  *mystream << "</PRE>\n";
}

void HTMLWriter::DoFigure(QString name) {
  *mystream << "<P>\n";
  *mystream << "<DIV ALIGN=\"CENTER\">\n";
  *mystream << "<IMG SRC=\"" << name << ".png\">\n";
  *mystream << "</DIV>\n";
}

void HTMLWriter::DoEquation(QString name) {
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
};

void LatexWriter::EndModule() {
  if (myfile) delete myfile;
  if (mystream) delete mystream;
}

void LatexWriter::BeginModule(QString modname, QString moddesc, QString secname) {
  myfile = new QFile(modname.toLower() + ".tex");
  if (!myfile->open(QFile::WriteOnly))
    Halt("Unable to open " + modname + ".tex for output");
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
  LoadGraphicsCoreFunctions(context);  
  InitializeFigureSubsystem();
  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");
  m_term->setContext(context);
  if (envPtr)
    m_term->setPath(std::string(envPtr));
  else 
    m_term->setPath(std::string(""));
  m_term->setPath("../../MFiles");
  WalkTree *twalk = new WalkTree(context,m_term);
  return twalk;
}

QString EvaluateCommands(QStringList cmds) {
  input = cmds;
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
  delete twalk;
  delete m_term;
  return output;
}

void GUISetup() {
  QWidget *m_main = new QWidget;
  m_text = new QTextEdit;
  m_text->setReadOnly(true);
  m_text->resize(400,400);
  m_text->show();
  m_text->setFontFamily("Courier");
  QPushButton *quit = new QPushButton("Quit");
  QWidget::connect(quit,SIGNAL(clicked()),qApp,SLOT(quit()));
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(m_text);
  layout->addWidget(quit);
  m_main->setLayout(layout);
  m_main->show();
}

QString MustMatch(QRegExp re, QString source) {
  if (re.indexIn(source) < 0)
    Halt("Bad line: " + source);
  return re.cap(1);
}

bool TestMatch(QRegExp re, QString source) {
  return (re.indexIn(source) >= 0);
}

void CloseAllHandleWindows() {
  
}

void ProcessFile(QFileInfo fileinfo, HelpWriter *out) {
  QRegExp docblock_pattern("^\\s*//!");
  QRegExp modulename_pattern("^\\s*//@Module\\s*(\\b\\w+\\b)");
  QRegExp moduledesc_pattern("^\\s*//@Module\\s*(\\b.*)");
  QRegExp sectioname_pattern("^\\s*//@@Section\\s*(\\b\\w+\\b)");
  QRegExp groupname_pattern("^\\s*//@@(.*)");
  QRegExp execin_pattern("^\\s*//@<");
  QRegExp execout_pattern("^\\s*//@>");
  QRegExp verbatimin_pattern("^\\s*//@\\[");
  QRegExp verbatimout_pattern("^\\s*//@\\]");
  QRegExp ccomment_pattern("^\\s*//(.*)");
  QRegExp figure_pattern("^\\s*//@figure\\s*(\\b\\w+\\b)");
  QRegExp eqnin_pattern("^\\s*//\\s*\\\\\\[");
  QRegExp eqnout_pattern("^\\s*//\\s*\\\\\\]");
    
  context = new Context;
  modulename_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  moduledesc_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  sectioname_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  groupname_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  figure_pattern.setCaseSensitivity(Qt::CaseInsensitive);
  if (fileinfo.suffix() == "mpp") {
    OutputText("Processing File " + fileinfo.absoluteFilePath() + "...\n");
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
	  out->BeginModule(modname,moddesc,secname);
	  line = fstr.readLine(0);
	  while (!fstr.atEnd() && !TestMatch(docblock_pattern,line)) {
	    QString groupname(MustMatch(groupname_pattern,line));
	    out->BeginGroup(groupname);
	    line = fstr.readLine(0);
	    while (!fstr.atEnd() && !TestMatch(groupname_pattern,line) && !TestMatch(docblock_pattern,line)) {
	      if (TestMatch(execin_pattern,line)) {
		QStringList cmdlist;
		line = fstr.readLine(0);
		while (!fstr.atEnd() && !TestMatch(execout_pattern,line)) {
		  if (TestMatch(ccomment_pattern,line)) 
		    cmdlist.push_back(MustMatch(ccomment_pattern,line)+"\n");
		  line = fstr.readLine(0);
		}
		cmdlist.push_back("quit;\n");
		QString resp(EvaluateCommands(cmdlist));
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
		QString eqn = line + "\n";
		while (!fstr.atEnd() && !TestMatch(eqnout_pattern,line)) {
		  eqn += MustMatch(ccomment_pattern,line)+"\n";
		  line = fstr.readLine(0);
		}
		out->DoEquation(eqn);
		if (fstr.atEnd())
		  Halt("Unmatched equation block detected!");
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
	}
      }
      out->EndModule();
    }
  }
  qApp->processEvents();
  CloseAllHandleWindows();
  delete context;
}

void ProcessDir(QDir dir, HelpWriter *out) {
  OutputText("Processing Directory " + dir.absolutePath() + "...\n");
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

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  GUISetup();
  LatexWriter texout;
  HTMLWriter htmlout;
  GroupWriter out;
  out.RegisterWriter(&texout);
  out.RegisterWriter(&htmlout);
  ProcessDir(QDir("."),&out);
  //  return app.exec();
  return 0;
}
