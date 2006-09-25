#ifndef __Tree_hpp__
#define __Tree_hpp__

#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include "Token.hpp"
#include "Array.hpp"

using namespace std;

class tree;
class Serialize;

typedef vector<tree> treeVector;

class tree_node {
public:
  Token node;
  int owners;
  treeVector children;
  void print() const;
  void Rename(byte newtok);
  tree_node* getCopy();
  unsigned context() const {return node.Position();}
  tree_node();
};

class tree {
  tree_node* tptr;
public:
  tree();
  tree(tree_node* ptr) : tptr(ptr) {}
  tree(const tree& copy);
  //   ~tree();
  tree_node* ptr() {return tptr;}
  void print() const;
  void Rename(byte newtok) {if (tptr) tptr->Rename(newtok);}
  bool valid() const {return (tptr != NULL);}
  //  void operator=(const tree &copy);
  unsigned context() const {if (tptr) return tptr->context(); else return 0;}
  tree first() const {if (tptr) return tptr->children.front(); else return tree();}
  tree second() const {return child(1);}
  bool is(byte tok) const {return (token()==tok);}
  void setBPflag(bool enable) {if (tptr) (tptr->node.SetBPFlag(enable));}
  bool getBPflag() const {if (tptr) return tptr->node.BPFlag(); return false;}
  byte token() const {if (tptr) return tptr->node.Value(); else return 0;}
  unsigned numchildren() const {if (tptr) return tptr->children.size(); else return 0;}
  bool haschildren() const {return numchildren() > 0;}
  string text() const {if (tptr) return tptr->node.Text(); else return std::string();}
  Array array() const {if (tptr) return tptr->node.GetArray(); else return Array();}
  treeVector& children() const {return tptr->children;}
  tree last() const {if (tptr) return tptr->children.back(); else return tree();}
  tree child(unsigned n) const {if (tptr) return tptr->children.at(n); else return tree();}
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
