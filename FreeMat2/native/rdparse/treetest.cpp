#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Node2 {
public:
  string text;
  Node2();
  ~Node2();
  Node2(string txt);
};

Node2::Node2(string txt) : text(txt) {
  cout << "mknode2 " << txt << "\n";
}

Node2::Node2() {
  cout << "mknode2\n";
}

Node2::~Node2() {
  cout << "dlnode2\n";
}

class ftree;

class treeNode {
public:
  Node2 *Root;
  int owners;
  vector<ftree> children;
  treeNode() {cout << "treeNode make\n";}
  treeNode* getCopy();
  ~treeNode() {delete Root;}
  void print();
};

// Copies of trees must be very fast...
class ftree {
public:
  treeNode *treeptr;
  ftree();
  ftree(const ftree& copy);
  ~ftree();
  void print();
};

static int indentlevel = 0;
void treeNode::print() {
  for (int i=0;i<indentlevel;i++)
    cout << " ";
  cout << Root->text << "(" << owners << ")\n";
  indentlevel+=3;
  for (int i=0;i<children.size();i++)
    children[i].treeptr->print();
  indentlevel-=3;
}

treeNode* MakeTreeNode(Node2 *p) {
  // Instantiate a node2
  treeNode *q = new treeNode;
  q->owners = 1;
  q->Root = p;
  return q;
}

treeNode* treeNode::getCopy() {
  owners++;
  return this;
}

void FreeTreeNode(treeNode* t) {
  if (!t) return;
  t->owners--;
  if (t->owners<=0)
    delete t;
}


void ftree::print() {
  treeptr->print();
}

ftree::ftree(const ftree& copy) {
  cout << "Copy\n";
  treeptr = NULL;
  if (copy.treeptr)
    treeptr = copy.treeptr->getCopy();
}

ftree::ftree() {
  treeptr = NULL;
}

ftree::~ftree() {
  FreeTreeNode(treeptr);
}

void addChild(ftree &q, ftree p) {
  q.treeptr->children.push_back(p);
}

ftree mkLeaf(Node2* q) {
  ftree p;
  p.treeptr = MakeTreeNode(q);
  return p;
}

class Node {
public:
  string text;
  int owners;
  Node();
  ~Node();
  Node* getCopy();
};

Node::Node() {
  cout << "mknode\n";
}

Node::~Node() {
  cout << "dlnode\n";
}

Node* MakeNode(string txt) {
  Node *q = new Node;
  q->text = txt;
  q->owners = 0;
  return q;
}

Node* Node::getCopy() {
  owners++;
  return this;
}

class tree {
public:
  Node *root;
  vector<tree> children;
  void print();
  ~tree();
  tree() {}
  tree(const tree& copy);
};

tree MkLeaf(Node* p) {
  tree node;
  node.root = p->getCopy();
  return node;
}

void AddChild(tree &p, tree q) {
  p.children.push_back(q);
}

void FreePtr(Node *t) {
  t->owners--;
  if (t->owners<=0)
    delete t;
}

tree::~tree() {
  FreePtr(root);
}

tree::tree(const tree& copy) {
  cout << "copy\n";
  root = copy.root->getCopy();
  children = copy.children;
}

void tree::print() {
  for (int i=0;i<indentlevel;i++)
    cout << " ";
  cout << root->text << "\n";
  indentlevel+=3;
  for (int i=0;i<children.size();i++)
    children[i].print();
  indentlevel-=3;
}

tree GenIt() {
  tree sample(MkLeaf(MakeNode("TestGenIt")));
  return sample;
}

void RunTest() {
  tree root(MkLeaf(MakeNode("Root")));
  AddChild(root,MkLeaf(MakeNode("Child1")));
  AddChild(root,MkLeaf(MakeNode("Child2")));
  AddChild(root,MkLeaf(MakeNode("Child3")));
  root.print();
  tree copy(root);
  copy.print();
  tree copy2 = copy;
  copy2.print();
}

void RunTest2() {
  ftree root(mkLeaf(new Node2("Root")));
  addChild(root,mkLeaf(new Node2("Child1")));
  addChild(root,mkLeaf(new Node2("Child2")));
  ftree sub2 = mkLeaf(new Node2("Beta"));
  addChild(root,sub2);
  ftree copy(root);
  ftree copy2(root);
  {
    ftree copy3(root);
  }
  root.treeptr->print();
}

int main(int argc, char *argv[]) {
//   cout << "StartTest\n";
//   RunTest();
//   cout << "StopTest\n";
//   GenIt();
//   cout << "StopTest2\n";
  RunTest2();
  cout << "StopTest3\n";
  return 0;
}
