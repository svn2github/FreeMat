#ifndef __Tree_hpp__
#define __Tree_hpp__

#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include "Token.hpp"

using namespace std;

class tree;

typedef vector<tree> treeVector;

class tree_node {
public:
  Token node;
  int owners;
  treeVector children;
  void print();
  void Rename(byte newtok);
  tree_node* getCopy();
  tree_node();
};

class tree {
  tree_node* tptr;
public:
  tree();
  tree(tree_node* ptr) : tptr(ptr) {}
  tree(const tree& copy);
  ~tree();
  tree_node* ptr() {return tptr;}
  void print();
  void Rename(byte newtok) {if (tptr) tptr->Rename(newtok);}
  bool valid() {return (tptr != NULL);}
  void operator=(const tree &copy);
  unsigned context();
  tree first();
  tree second();
  bool is(byte tok);
  byte token() {if (tptr) return tptr->node.Value(); else return 0;}
  unsigned numchildren() {if (tptr) return tptr->children.size(); else return 0;}
  bool haschildren() {return numchildren() > 0;}
  string text() {if (tptr) return tptr->node.Text(); else return std::string();}
  treeVector children() {if (tptr) return tptr->children; else return treeVector();}
  tree last() {if (tptr) return tptr->children.back(); else return tree();}
  tree child(unsigned n) {if (tptr) return tptr->children[n]; else return tree();}
};

tree mkLeaf(const Token& tok);
tree mkLeaf(byte a);
tree mkNode(const Token& tok, tree arg1, tree arg2);
tree mkNode(const Token& tok, tree arg1);
tree first(tree root);
tree second(tree root); 
void addChild(tree &root, tree child);
void addChild(tree &root, tree child1, tree child2);

#endif
