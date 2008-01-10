#ifndef __Tree_hpp__
#define __Tree_hpp__

#include <string>
#include "Token.hpp"
#include "Array.hpp"

// A rewrite of the tree class (yet again).  The lesson learned this time is that we want the
// tree class to be as fast as possible.  And that we want to be able to tag the tree with profile
// information.
class Tree;
class JITFunc;

typedef QList<Tree*> TreeList;

class Tree {
public:
  typedef enum {
    UNTRIED,
    FAILED,
    SUCCEEDED
  } JITState_t;
private:
  Token m_node;
  TreeList m_children;
  JITState_t m_jitstate;
  JITFunc *m_jitfunc;
public:
  Tree(): m_node(TOK_INVALID), m_jitstate(UNTRIED), m_jitfunc(NULL)
  {}
  Tree(const Token& tok) : m_node(tok), 
			   m_jitstate(UNTRIED), m_jitfunc(NULL)
  {m_node.fillArray();}
  Tree(byte token, unsigned position) : m_node(Token(token,position)), 
					m_jitstate(UNTRIED), m_jitfunc(NULL)
  {}
  Tree(const Token& tok, Tree* child1, Tree* child2) : m_node(tok), 
						       m_jitstate(UNTRIED), m_jitfunc(NULL)
  {
    m_children.push_back(child1);
    m_children.push_back(child2);
  }
  Tree(const Token& tok, Tree* child1) : m_node(tok), 
					 m_jitstate(UNTRIED), m_jitfunc(NULL)
  {
    m_children.push_back(child1);
  }
  Tree(Serialize *s);
  ~Tree();
  inline const Token& node() const {return m_node;}
  void print() const;
  inline JITState_t JITState() const {return m_jitstate;}
  inline void setJITState(JITState_t t) {m_jitstate = t;}
  inline JITFunc* JITFunction() const {return m_jitfunc;}
  inline void setJITFunction(JITFunc *t) {m_jitfunc = t;}
  inline void rename(byte newtok) {m_node.setValue(newtok);}
  inline unsigned context() const {return m_node.position();}
  inline bool valid() const {return !(m_node.is(TOK_INVALID));}
  bool operator== (const Tree &copy) const;
  inline Tree* first() const {return m_children.front();}
  inline Tree* second() const {return m_children.at(1);}
  inline bool is(byte tok) const {return (token()==tok);}
  inline byte token() const {return m_node.value();}
  inline int numChildren() const {return m_children.size();}
  inline bool hasChildren() const {return (m_children.size()>0);}
  inline string text() const {return m_node.text();}
  inline void setText(string t) {m_node.setText(t);}
  inline Array array() const {return m_node.array();}
  inline const TreeList& children() const {return m_children;}
  inline Tree* last() const {return m_children.back();}
  inline Tree* child(unsigned n) const {return m_children.at(n);}
  inline Token& node() {return m_node;}
  inline void addChild(Tree *t_child) {m_children.push_back(t_child);}
  inline void addChildren(Tree *child1, Tree *child2) {
    m_children.push_back(child1);
    m_children.push_back(child2);
  }
  inline void try_validate() {
    for (int i=0;i<m_children.size();i++) {
      if (m_children.at(i)) 
	m_children.at(i)->try_validate();
      else
	throw Exception("validation failed");
    }
  }
  inline void validate() {
    try {
      try_validate();
    } catch(Exception &e) {
      std::cout << "Tree fails validation!\n";
      print();
    }
  }
  void freeze(Serialize *s) const;
  static Tree* deepTreeCopy(Tree *t);
};

class CodeBlock {
  Tree* m_tree;
public:
  inline CodeBlock(Tree *t, bool needClone = false) : m_tree(t) {if (needClone) clone(); m_tree->validate();}
  inline CodeBlock(const CodeBlock& copy) {m_tree = copy.m_tree; clone();}
  inline CodeBlock() : m_tree(NULL) {}
  inline CodeBlock& operator=(const CodeBlock& copy) {
    if (this == &copy)
      return *this;
    delete m_tree;
    m_tree = copy.m_tree;
    clone();
    return *this;
  }
  inline Tree* tree() {return m_tree;}
  inline ~CodeBlock() {delete m_tree;}
  inline void clone() {m_tree = Tree::deepTreeCopy(m_tree);}
};

typedef QList<CodeBlock> CodeList;

#endif
