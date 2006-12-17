#include "Transform.hpp"
#include "Token.hpp"

// 
tree CopyTree(tree root) {
  if (!root.valid()) return tree();
  tree newRoot(mkLeaf(root.ptr()->node));
  for (int i=0;i<root.numchildren();i++)
    addChild(newRoot,CopyTree(root.child(i)));
  return newRoot;
}

tree FindEndReference(tree s) {
  if (s.is(TOK_END))
    return s;
  for (int i=0;i<s.numchildren();i++) {
    tree t(FindEndReference(s.child(i)));
    if (t.valid()) return t;
  }
  return tree();
}

tree TransformEndReferencesStatement(tree s) {
  
}

bool HasEndReference(tree s) {
  if (s.is(TOK_END)) return true;
  for (int i=0;i<s.numchildren();i++)
    if (HasEndReference(s.child(i))) return true;
  return false;
}

bool HasVariableEndRef(tree s) {
  if (s.is(TOK_END)) return true;
  for (int i=0;i<s.numchildren();i++)
    if ((!s.child(i).is(TOK_VARIABLE)) && 
	HasVariableEndRef(s.child(i))) return true;
  return false;
}

tree FindVariableReferences(tree s) {
  if (s.is(TOK_VARIABLE) && HasVariableEndRef(s))
    return s;
  for (int i=0;i<s.numchildren();i++) {
    tree ret(FindVariableReferences(s.child(i)));
    if (ret.valid()) return ret;
  }
  return tree();
}

tree RenameEndReferences(tree s, int tmpnum, bool &replace) {
  tree ret(mkLeaf(s.ptr()->node));
  for (int i=0;i<s.numchildren();i++) {
    if (s.child(i).is(TOK_END) && replace) {
      tree synth(mkLeaf(TOK_VARIABLE));
      addChild(synth,mkLeaf(Token(TOK_IDENT,0,string("_") + tmpnum++)));
      addChild(ret,synth);
      replace = false;
    } else
      addChild(ret,RenameEndReferences(s.child(i),tmpnum,replace));
  }
  return ret;
}

bool RenameEndReferences2(tree s, tree r) {
  tree ret(mkLeaf(s.ptr()->node));
  for (int i=0;i<s.numchildren();i++) {
    if (s.child(i).is(TOK_END)) { 
     s.ptr()->children[i] = r;
     return true;
    } else {
      if (RenameEndReferences2(s.child(i),r))
	return true;
    }
  }
  return false;
}

tree EndFunctionRewrite(tree s, int tmpnum) {
  // Find the first child with an end reference
  tree root(mkLeaf('='));
  tree ret(mkLeaf(s.ptr()->node));
  tree tmp(mkLeaf(TOK_VARIABLE));
  addChild(root,tmp);
  addChild(tmp,mkLeaf(Token(TOK_IDENT,0,string("_") + tmpnum)));
  tree q(mkLeaf(TOK_VARIABLE));
  addChild(root,q);
  addChild(q,mkLeaf(Token(TOK_IDENT,0,string("end"))));
  tree q2(mkLeaf(TOK_PARENS));
  addChild(q,q2);
  addChild(q2,ret);
  int i=0;
  while ((i < s.numchildren()) && (!HasEndReference(s.child(i)))) {
    addChild(ret,s.child(i)); 
    i++;
  }
  const tree &v(s.child(i));
  int j=0;
  while ((j < v.numchildren()) && (!HasEndReference(v.child(j)))) {
    j++;
  }
  addChild(q2,mkLeafWithLiterals(Token(TOK_INTEGER,0,string()+(j+1))));
  addChild(q2,mkLeafWithLiterals(Token(TOK_INTEGER,0,string()+v.numchildren())));
  tree qrt(mkLeaf(TOK_QSTATEMENT));
  addChild(qrt,root);
  return qrt;
}

tree EndFunctionRewrite2(tree s, int tmpnum) {
  // Find the first child with an end reference
  tree ret(mkLeaf(s.ptr()->node));
  tree q(mkLeaf(TOK_VARIABLE));
  addChild(q,mkLeaf(Token(TOK_IDENT,0,string("end"))));
  tree q2(mkLeaf(TOK_PARENS));
  addChild(q,q2);
  addChild(q2,ret);
  int i=0;
  while ((i < s.numchildren()) && (!HasEndReference(s.child(i)))) {
    addChild(ret,s.child(i)); 
    i++;
  }
  const tree &v(s.child(i));
  int j=0;
  while ((j < v.numchildren()) && (!HasEndReference(v.child(j)))) {
    j++;
  }
  addChild(q2,mkLeafWithLiterals(Token(TOK_INTEGER,0,string()+(j+1))));
  addChild(q2,mkLeafWithLiterals(Token(TOK_INTEGER,0,string()+v.numchildren())));
  return q;
}

// Return the parent of the current node
tree Parent(tree root, tree node) {
  if (root == node) 
    throw Exception("cannot find parent of tree node!");
  for (int i=0;i<root.numchildren();i++) 
    if (root.child(i) == node) 
      return root;
  // We are not the parent of node.  Try our children
  for (int i=0;i<root.numchildren();i++) {
    tree s(Parent(root.child(i),node));
    if (s.valid()) return s;
  }
  return tree();
}

// Returns true if the given root tree is an ancestor of 
// the given node
bool IsAncestor(tree root, tree node) {
  if (!node.valid()) return false;
  if (!root.valid()) return false;
  if (root == node) return true;
  for (int i=0;i<root.numchildren();i++)
    if (IsAncestor(root.child(i),node)) return true;
  return false;
}

void TestParent(tree root) {
  tree endRef(FindEndReference(root));
  if (!endRef.valid()) return;
  endRef.print();
  tree varRef(Parent(root,endRef));
  while (varRef.valid() && !varRef.is(TOK_VARIABLE))
    varRef = Parent(root,varRef);
  varRef.print();  
  // Find the child of varRef that has our end Ref
  int childNum = 0;
  bool foundEndRef = false;
  while (!foundEndRef && (childNum < varRef.numchildren())) {
    foundEndRef = IsAncestor(varRef.child(childNum),endRef);
    if (!foundEndRef) childNum++;
  }
  if (!foundEndRef) throw Exception("Internal error!  Corrupted AST!");
  // Copy the tree, but clip the children
  tree outvar = CopyTree(varRef);
  while (outvar.numchildren() > childNum)
    outvar.children().pop_back();
  outvar.print();
  // For the child of interest, how many entries does it have?
  // One problem is that what happens in this case:
  //
  // Foo(a{:},end)
  //
  // ? The end token has to be decided at run time.
  //
  // It can be computed with something like this:
  //
  // foo.a(32){16}.goo(a{:},end,end,3)
  //
  // n = numel([a{:}]);
  // m = numel([a{:},1,4,3]);
  // g = foo.a(32){16}.goo;
  // e = end(g,n,m);
  // 
}

static int tmpnum = 0;
tree RemoveEndReferences(tree s) {
  tree ret(mkLeaf(s.ptr()->node));
  for (int i=0;i<s.numchildren();i++) {
    if ((s.child(i).is(TOK_STATEMENT) || 
	 s.child(i).is(TOK_QSTATEMENT)) && HasEndReference(s.child(i))) {
      tree ref = FindVariableReferences(s.child(i));
      //      ref.print();
      ref = EndFunctionRewrite2(ref,tmpnum);
      //      ref.print();
      //      addChild(ret,ref);
      //      bool replace = true;
      RenameEndReferences2(s.child(i),ref);
      addChild(ret,s.child(i));
    } else {
      addChild(ret,RemoveEndReferences(s.child(i)));
    }
  }
  return(ret);
}

tree TransformEndReferences(tree s) {
  return s;
  TestParent(s);
  while (HasEndReference(s))
    s = RemoveEndReferences(s);
  return s;
}
