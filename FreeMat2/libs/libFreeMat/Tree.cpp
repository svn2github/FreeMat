#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include "Tree.hpp"
#include "Serialize.hpp"

tree_node::tree_node() {
}

tree::tree() : tptr(new tree_node()) {
}

tree::tree(const Token& tok) {
  tree_node* dp = new tree_node;
  dp->node = tok;
  tptr = dp;
}

void tree_node::Rename(byte a) {
  node.SetValue(a);
}

bool tree::operator==(const tree &copy) {
  return (tptr == copy.tptr);
}

//void tree::operator=(const tree &copy) {
//  FreeTreeNode(tptr);
//  tptr = NULL;
//  if (copy.tptr)
//    tptr = copy.tptr->getCopy();
//}

void tree::print() const {
  if (valid())
    tptr->print();
  cout.flush();
}

static int indentlevel = 0;
void tree_node::print() const {
  for (int i=0;i<indentlevel;i++)
    cout << " ";
  cout << node;
  indentlevel+=3;
  for (int i=0;i<children.size();i++)
    children[i].print();
  indentlevel-=3;
}

tree mkLeaf(const Token& tok) {
  return tree(tok);
}

tree mkLeafWithLiterals(const Token& tok) {
  tree ret(tok);
  ret.node().FillArray();
  return tree(ret);
}

tree mkLeaf(byte a, unsigned position) {
  Token p(a,position);
  return mkLeaf(p);
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
  if (!root.valid()) {
    cout << "Error: cannot add children to null trees\n";
    exit(1);
  }    
  root.addChild(child);
}

void addChild(tree &root, tree child1, tree child2) {
  addChild(root,child1);
  addChild(root,child2);
}

void FreezeTree(tree root, Serialize *s) {
  if (!root.valid()) {
    s->putBool(false);
    return;
  } else {
    s->putBool(true);
  }
  FreezeToken(root.node(),s);
  s->putInt(root.numchildren());
  for (int i=0;i<root.numchildren();i++)
    FreezeTree(root.child(i),s);
}

tree ThawTree(Serialize *s) {
  bool ValidTree = s->getBool();
  if (!ValidTree) return tree();
  Token tok(ThawToken(s));
  tree root(mkLeaf(tok));
  int numchildren = s->getInt();
  for (int i=0;i<numchildren;i++)
    addChild(root,ThawTree(s));
  return root;
}
