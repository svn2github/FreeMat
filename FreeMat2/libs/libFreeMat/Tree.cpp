#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include "Tree.hpp"
#include "Serialize.hpp"

Tree::Tree(const Token& tok) : m_childcount(0), children(NULL), node(tok) {
}

Tree::~Tree() {
  tree p = children;
  while (p) {
    tree q = p;
    delete p;
    p = q;
  }
}

static int indentlevel = 0;
void Tree::print() {
  for (int i=0;i<indentlevel;i++)
    cout << " ";
  cout << node;
  indentlevel+=3;
  for (int i=0;i<children.size();i++)
    children[i].print();
  indentlevel-=3;
}

void Tree::Rename(byte a) {
  node.SetValue(a);
}

void Tree::context() {
  return node.Position();
}

tree Tree::first() {
  return children;
}

tree Tree::second() {
  if (children) return children->children;
  return NULL;
}

bool Tree::is(byte tok) {
  return (node.Value() == tok);
}

void Tree::setBPflag(bool enable) {
  node.SetBPFlag(enable);
}

bool Tree::getBPflag() {
  return node.BPFlag();
}

byte Tree::token() {
  return node.Value();
}

unsigned Tree::numchildren() {
  return m_childcount;
}

string Tree::text() {
  return node.Text();
}

Array Tree::array() {
  return node.GetArray();
}

void Tree::fillArray() {
  node.FillArray();
}

tree Tree::children() {
  return children;
}

tree Tree::last() {
  return lastchild;
}

tree mkLeaf(const Token& tok) {
  return new Tree(tok);
}

tree mkLeafWithLiterals(const Token& tok) {
  tree p = new Tree(tok);
  p->fillArray();
  return p;
}

tree mkLeaf(byte a, unsigned position) {
  Token p(a,position);
  return mkLeaf(p);
}


tree mkNode(const Token& tok, tree arg1, tree arg2) {
  tree ret(mkLeaf(tok));
  ret->addChildren(arg1,arg2);
  return ret;
}

tree mkNode(const Token& tok, tree arg1) {
  tree ret(mkLeaf(tok));
  ret->addChild(ret,arg1);
  return ret;
}

void FreezeTree(tree root, Serialize *s) {
  if (!root.valid()) {
    s->putBool(false);
    return;
  } else {
    s->putBool(true);
  }
  FreezeToken(root.ptr()->node,s);
  s->putInt(root.numchildren());
  tree p = root->children();
  while (p) {
    FreezeTree(p,s);
    p = p->
    
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
