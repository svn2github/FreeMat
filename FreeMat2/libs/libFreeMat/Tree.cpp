#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include "Tree.hpp"

tree_node::tree_node() {
  owners = 0;
}

tree_node* tree_node::getCopy() {
  owners++;
  return this;
}

tree::tree(const tree& copy) {
  tptr = NULL;
  if (copy.tptr)
    tptr = copy.tptr->getCopy();
}

tree::tree() {
  tptr = NULL;
}

void FreeTreeNode(tree_node* t) {
  if (!t) return;
  t->owners--;
  if (t->owners<=0) {
    delete t;
    t = NULL;
  }
}

tree::~tree() {
  FreeTreeNode(tptr);
}

void tree_node::Rename(byte a) {
  node.SetValue(a);
}

void tree::operator=(const tree &copy) {
  FreeTreeNode(tptr);
  tptr = NULL;
  if (copy.tptr)
    tptr = copy.tptr->getCopy();
}

void tree::print() {
  if (tptr)
    tptr->print();
}

static int indentlevel = 0;
void tree_node::print() {
  for (int i=0;i<indentlevel;i++)
    cout << " ";
  cout << node;
  indentlevel+=3;
  for (int i=0;i<children.size();i++)
    children[i].print();
  indentlevel-=3;
}

tree mkLeaf(const Token& tok) {
  tree_node *ret = new tree_node;
  ret->node = tok;
  ret->owners = 1;
  return tree(ret);
}

tree mkNode(const Token& tok, tree arg1, tree arg2) {
  tree ret(mkLeaf(tok));
  addChild(ret,arg1,arg2);
  return ret;
}

tree mkNode(const Token& tok, tree arg1) {
  tree ret(mkLeaf(tok));
  addChild(ret,arg1);
  return ret;
}

void addChild(tree &root, tree child) {
  if (!root.ptr()) {
    cout << "Error: cannot add children to null trees\n";
    exit(1);
  }    
  root.ptr()->children.push_back(child);
}

void addChild(tree &root, tree child1, tree child2) {
  addChild(root,child1);
  addChild(root,child2);
}
