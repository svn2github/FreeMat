#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include "Tree.hpp"
#include "Serialize.hpp"

Tree::~Tree() {
  for (int i=0;i<m_children.size();i++) delete m_children.at(i);
}

Tree* Tree::deepTreeCopy(Tree* t) {
  Tree *p = new Tree(t->m_node);
  for (int i=0;i<t->m_children.size();i++)
    p->addChild(Tree::deepTreeCopy(t->m_children.at(i)));
  p->m_jitstate = t->m_jitstate;
  return p;
}

static int indentlevel = 0;
void Tree::print() const {
  for (int i=0;i<indentlevel;i++)
    cout << " ";
  cout << m_node;
  indentlevel+=3;
  for (int i=0;i<m_children.size();i++)
    m_children[i]->print();
  indentlevel-=3;
}

void Tree::freeze(Serialize *s) const {
  s->putBool(true); // For compatibility with 3.5?
  m_node.freeze(s);
  s->putInt(m_children.size());
  for (int i=0;i<m_children.size();i++)
    m_children.at(i)->freeze(s);
}

Tree::Tree(Serialize *s) {
  bool ValidTree = s->getBool();
  if (!ValidTree) return;
  m_node = Token(s);
  int numchildren = s->getInt();
  for (int i=0;i<numchildren;i++)
    m_children.push_back(new Tree(s));
}
