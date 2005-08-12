#include "Label.hpp"
#include "Util.hpp"
#include <math.h>
#include <iostream>

// To do LaTeX labels...
//  We can use simple symbol substitution for greek symbols, integrals and summations
//  We then need to add a attributed text string to track the subscripts, and superscripts
//  

namespace FreeMat {
  FormulaTree::FormulaTree(QString text, FormulaTree* supertree, 
			   FormulaTree* subtree, FormulaTree* nexttree) {
    m_supertree = supertree;
    m_subtree = subtree;
    m_nexttree = nexttree;
    m_text = text;
  }

  int depth = 0;

  void FormulaTree::PrintMe() {
    for (int i=0;i<depth;i++)
      std::cout << "  ";
    std::cout << (const char*) m_text << "(" << m_size << "," << m_ascent << ")\n";
    if (m_supertree) {
      depth++;
      m_supertree->PrintMe();
      depth--;
    }
    if (m_subtree) {
      depth++;
      m_subtree->PrintMe();
      depth--;
    }
    if (m_nexttree)
      m_nexttree->PrintMe();
  }

  void FormulaTree::Render(QPainter& gc, Point2D& pos) {
    QFont tmp("Helvetica",m_size);
    gc.setFont(tmp);
    gc.drawText(pos.x,pos.y-m_ascent,m_text);
    pos.x += gc.fontMetrics().boundingRect(m_text).width();
    // draw the super tree
    if (m_supertree)
      m_supertree->Render(gc,pos);
    if (m_subtree)
      m_subtree->Render(gc,pos);
    if (m_nexttree)
      m_nexttree->Render(gc,pos);
  }

  int m_cptr;
  std::vector<QString> m_stringfragments;

  bool match(QString test) {
    return ((m_cptr < m_stringfragments.size()) &&
	    (m_stringfragments[m_cptr] == test));
  }

  FormulaTree* StringToTree();
  
  FormulaTree* Terminal() {
    if (match("{")) {
      m_cptr++;
      FormulaTree* m = StringToTree();
      if (!match("}")) {
	std::cerr << "Unmatched left bracket {\n";
	exit(1);
      }
      m_cptr++;
      return m;
    } else
      return new FormulaTree(m_stringfragments[m_cptr++],NULL,NULL,NULL);
  }
  
  FormulaTree* StringToTree() {
    // The base of the tree should be the
    // first non-empty entry
    if (match("}")) return NULL;
    if (m_cptr >= m_stringfragments.size()) return NULL;
    QString root(m_stringfragments[m_cptr]);
    FormulaTree *super = NULL;
    FormulaTree *sub = NULL;
    m_cptr++;
    if (match("^")) {
      m_cptr++;
      super = Terminal();
      if (match("_")) {
	m_cptr++;
	sub = Terminal();
      }
    } else {
      if (match("_")) {
	m_cptr++;
	sub = Terminal();
      } 
      if (match("^")) {
	m_cptr++;
	super = Terminal();
      }
    }
    return(new FormulaTree(root,super,sub,StringToTree()));
  }
  
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
  Box FormulaTree::GetBox() {
    Box superBox, subBox, rootBox, nextBox;
    if (m_supertree)
      superBox = m_supertree->GetBox();
    if (m_subtree)
      subBox = m_subtree->GetBox();
    // Replace with legitimate code
    if (m_ascent < 0)
      rootBox.depth = -m_ascent;
    rootBox.width = m_size; 
    rootBox.height = m_size+m_ascent;
    if (m_nexttree)
      nextBox = m_nexttree->GetBox();
    Box output;
    output.depth = MIN(superBox.depth,MIN(subBox.depth,MIN(rootBox.depth,
							   nextBox.depth)));
    output.height = MAX(superBox.height,
			MIN(subBox.height,MIN(rootBox.height,
					      nextBox.height)));
    output.width = rootBox.width + nextBox.width + 
      MAX(subBox.width,superBox.width);
    return output;
  }

  void FormulaTree::SizeTree(int size, int ascent) {
    // The root takes the current size
    m_size = size;
    m_ascent = ascent;
    // The superscript takes a smaller size by 60%, and is raised
    // so that the baseline is also 60% of size
    if (m_supertree)
      m_supertree->SizeTree(m_size*0.9,m_ascent + m_size*0.6);
    if (m_subtree)
      m_subtree->SizeTree(m_size*0.9,m_ascent - m_size*0.4);
    if (m_nexttree)
      m_nexttree->SizeTree(size, ascent);
  }

  TexLabel::TexLabel(std::string text) {
    m_rawtext = text;
    CompileRawText();
  }

  void TexLabel::Substitute(QString ecode, QChar rcode) {
    int j = 0;
    while ((j = m_processed_text.indexOf(ecode,0)) != -1) {
      m_processed_text.replace(j,ecode.size(),rcode);
      qDebug("made substitution of %s to %x",(const char*) ecode,
	     rcode.unicode());
    }
  }

  void TexLabel::DoSubstitutions() {
    Substitute("\\Alpha",0x0391);
    Substitute("\\Beta",0x0392);
    Substitute("\\Gamma",0x0393);
    Substitute("\\Delta",0x0394);
    Substitute("\\Epsilon",0x0395);
    Substitute("\\Zeta",0x0396);
    Substitute("\\Eta",0x0397);
    Substitute("\\Theta",0x0398);
    Substitute("\\Iota",0x0399);
    Substitute("\\Kappa",0x039A);
    Substitute("\\Lambda",0x039B);
    Substitute("\\Mu",0x039C);
    Substitute("\\Nu",0x039D);
    Substitute("\\Xi",0x039E);
    Substitute("\\Omicron",0x039F);
    Substitute("\\Pi",0x03A0);
    Substitute("\\Rho",0x03A1);
    Substitute("\\Sigma",0x03A2);
    Substitute("\\Tau",0x03A3);
    Substitute("\\Upsilon",0x03A4);
    Substitute("\\Phi",0x03A5);
    Substitute("\\Chi",0x03A6);
    Substitute("\\Psi",0x03A7);
    Substitute("\\Omega",0x03A8);
    Substitute("\\alpha",0x03B1);
    Substitute("\\beta",0x03B2);
    Substitute("\\gamma",0x03B3);
    Substitute("\\delta",0x03B4);
    Substitute("\\epsilon",0x03B5);
    Substitute("\\zeta",0x03B6);
    Substitute("\\eta",0x03B7);
    Substitute("\\theta",0x03B8);
    Substitute("\\iota",0x03B9);
    Substitute("\\kappa",0x03BA);
    Substitute("\\lambda",0x03BB);
    Substitute("\\mu",0x03BC);
    Substitute("\\nu",0x03BD);
    Substitute("\\xi",0x03BE);
    Substitute("\\omicron",0x03BF);
    Substitute("\\pi",0x03C0);
    Substitute("\\rho",0x03C1);
    Substitute("\\sigma",0x03C3);
    Substitute("\\tau",0x03C4);
    Substitute("\\upsilon",0x03C5);
    Substitute("\\phi",0x03C6);
    Substitute("\\chi",0x03C7);
    Substitute("\\psi",0x03C8);
    Substitute("\\omega",0x03C9);
    Substitute("\\rightarrow",0x2192);
    Substitute("\\leftarrow",0x2190);
    Substitute("\\uparrow",0x2191);
    Substitute("\\downarrow",0x2193);
    Substitute("\\partial",0x2202);
    Substitute("\\nabla",0x2206);
    Substitute("\\Prod",0x220F);
    Substitute("\\prod",0x220F);
    Substitute("\\sum",0x2211);
    Substitute("\\Sum",0x2211);
    Substitute("\\plusminus",0x00B1);
    Substitute("\\infty",0x221E);
    Substitute("\\leq",0x2264);
    Substitute("\\geq",0x2265);
    Substitute("\\neq",0x2260);
    Substitute("\\approx",0x2248);
    Substitute("\\sim",'~');
    Substitute("\\int",0x222B);
  }

  void TexLabel::Stringify() {
    // Break the processed text into string fragments
    // Each fragment consists of a sequence of characters
    // at the same render level (i.e., subscript/superscript
    // history).
    m_cp = 0;
    QString fragment;
    bool singleshot;
    std::vector<QString> stringfrag;
    while (m_cp < m_processed_text.size()) {
      // Is this a bracket?
      if ((m_processed_text[m_cp] == '{') ||
	  (m_processed_text[m_cp] == '}')) {
	// Flush the current string
	stringfrag.push_back(fragment);
	// Reset the fragment buffer
	fragment.clear();
	stringfrag.push_back(QString(m_processed_text[m_cp]));
	singleshot = false;
      } else if ((m_processed_text[m_cp] == '^') ||
		 (m_processed_text[m_cp] == '_')) {
	// If the current char is a ^, then push it as a fragment
	// Flush the current string
	stringfrag.push_back(fragment);
	stringfrag.push_back(QString(m_processed_text[m_cp]));
	fragment.clear();
	singleshot = true;
      } else {
	fragment.push_back(m_processed_text[m_cp]);
	if (singleshot) {
	  stringfrag.push_back(fragment);
	  // Reset the fragment buffer
	  fragment.clear();
	  singleshot = false;
	}
      }
      m_cp++;
    }
    stringfrag.push_back(fragment);
    for (int i=0;i<stringfrag.size();i++) {
      if (!stringfrag[i].isEmpty())
	m_stringfragments.push_back(stringfrag[i]);
    }
  }

  void TexLabel::CompileRawText() {
    // The compiling strategy - we have a sequence of tokens
    // like: a^b_cfg_h - it really is a 3-tree - every token has
    // three slots (a superscript and a subscript slot) and a next token
    //   b
    //  a fg
    //   c  h
    // start with m(a), if n+1-->"^"
    m_processed_text = QString(m_rawtext.c_str());
    DoSubstitutions();
    // Break into strings
    Stringify();
    // Build it into a tree
    m_cptr = 0;
    m_tree = StringToTree();
    m_tree->SizeTree(14,0);
    m_tree->PrintMe();
  }

  int TexLabel::GetCurrentSize() {
    return ((int)(pow(0.8,abs(m_supersub_level))*m_size));
  }

  int TexLabel::GetCurrentYPos() {
    // Get the height of the base character
    QFont Base("Helvetica",m_size);
    QFont Raise("Helvetica",GetCurrentSize());
    QFontMetrics fmBase(Base);
    QFontMetrics fmRaise(Raise);
    QRect szeBase(fmBase.boundingRect("|"));
    QRect szeRaise(fmRaise.boundingRect("|"));
    // Next, we have to calculate how much to shift
    int yShift;
    yShift = szeBase.height() - szeRaise.height();
    return yShift;
  }

  int TexLabel::GetCurrentWidth(QChar a) {
    QFont Raise("Helvetica",GetCurrentSize());
    QFontMetrics fmRaise(Raise);
    QRect szeRaise(fmRaise.boundingRect(a));
    return szeRaise.width();
  }

  Point2D TexLabel::BoundingBox() {
  }

  void TexLabel::Render(QPainter& gc, Point2D pos) {
    m_tree->Render(gc, pos);
    return;
    for (int i=0;i<m_output_text.size();i++) {
      QFont Tmp("Helvetica",m_sizes[i]);
      gc.setFont(Tmp);
      gc.drawText(m_xpos_list[i],m_ypos_list[i],QString(m_output_text[i]));
      qDebug("size: %d at %d,%d, char %x",m_sizes[i],m_xpos_list[i],m_ypos_list[i],m_output_text[i].unicode());
    }
  }

  Label::Label(QWidget* parent, std::string text, char orient) :
    QPWidget(parent,"label") {
    m_text = text;
    m_orientation = orient;
    Point2D m_size(GetTextExtentNoGC(text,12));
    if (m_orientation == 'h')
      setMinimumSize(m_size.x,m_size.y*2);
    else
      setMinimumSize(m_size.y,m_size.x);
    ProcessText();
  }
  
  Label::~Label() {
  }
  
  void Label::ProcessText() {
    m_processed_text = QString(m_text.c_str());
    int j = 0;
    while ((j = m_processed_text.indexOf("\beta",0)) != -1) {
      m_processed_text.replace(j,5,0x03B2);
    }
  }

  void Label::DrawMe(QPainter& gc) {
    Point2D pos(width()/2,0);
    QRect sze(gc.fontMetrics().boundingRect(m_processed_text));
    Point2D ext(sze.width(),sze.height());
    pos.y += ext.y;
    pos.x -= ext.x/2;
    gc.save();
    gc.translate(pos.x,pos.y);
    //    gc.drawText(0,0,m_processed_text);
    gc.restore();

    //    TexLabel tl("\\beta^2-3\\rightarrow\\int\\gamma_4^3");
    //    TexLabel tl("A = H^3_5ello_b-3^2 123 \\beta 4.25");
    TexLabel tl("H^3_5ello_b-3^2+7^{3-4_0}_52^{2^3} 123 \\beta 4.25");
    tl.Render(gc,Point2D(0,15));

//     if (m_orientation == 'h')
//       DrawTextStringAligned(gc,m_text,Point2D(width()/2,0),LR_CENTER,TB_TOP,0);
//     else
//       DrawTextStringAligned(gc,m_text,Point2D(width()/2,height()),LR_LEFT,TB_BOTTOM,90);
  }
}
