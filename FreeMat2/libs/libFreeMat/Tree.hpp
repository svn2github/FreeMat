#ifndef __Tree_hpp__
#define __Tree_hpp__

#include <string>
#include "Token.hpp"
#include "Array.hpp"
#include <QSharedData>
#include <QSharedDataPointer>

using namespace std;


class tree;
class Serialize;

typedef QList<tree> treeVector;

class tree_node : public QSharedData {
public:
  Token node;
  treeVector children;
  void print() const;
  void Rename(byte newtok);
  unsigned context() const {return node.Position();}
  tree_node();
};

class tree {
private:
  QSharedDataPointer<tree_node> tptr;
public:
  tree();
  tree(const Token& tok);
  void print() const;
  inline void Rename(byte newtok) {tptr->Rename(newtok);}
  inline bool valid() const {return !(tptr->node.Is(TOK_INVALID));}
  //  void operator=(const tree &copy);
  bool operator==(const tree &copy);
  inline unsigned context() const {if (valid()) return tptr->context(); else return 0;}
  inline tree first() const {if (valid()) return tptr->children.front(); else return tree();}
  inline tree second() const {return child(1);}
  inline bool is(byte tok) const {return (token()==tok);}
  inline byte token() const {if (valid()) return tptr->node.Value(); else return 0;}
  inline unsigned numchildren() const {if (valid()) return tptr->children.size(); else return 0;}
  inline bool haschildren() const {return numchildren() > 0;}
  inline string text() const {if (valid()) return tptr->node.Text(); else return std::string();}
  inline Array array() const {if (valid()) return tptr->node.GetArray(); else return Array();}
  inline const treeVector& children() const {return tptr->children;}
  inline tree last() const {if (valid()) return tptr->children.back(); else return tree();}
  inline tree child(unsigned n) const {if (valid()) return tptr->children.at(n); else return tree();}
  inline Token& node() {return tptr->node;}
  inline const Token& node() const {return tptr->node;}
  inline void addChild(const tree &child) {tptr->children.push_back(child);}
};

tree mkLeaf(const Token& tok);
tree mkLeafWithLiterals(const Token& tok);
tree mkLeaf(byte a, unsigned pos);
tree mkNode(const Token& tok, tree arg1, tree arg2);
tree mkNode(const Token& tok, tree arg1);
tree first(tree root);
tree second(tree root); 
void addChild(tree &root, tree child);
void addChild(tree &root, tree child1, tree child2);

void FreezeTree(tree root, Serialize *s);
tree ThawTree(Serialize *s);

#endif
