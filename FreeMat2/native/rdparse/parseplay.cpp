// spaces work - what about transposes?
//
// Some observations:
// [1 - 2] --> [-1]
// [1 - - 2] --> [3]
// [1 - 2 - 3 -4] --> [-4]
// Conclusion - do not allow spaces after unary operators (unless you have to)
// [a' 1 b'] --> [a',1,b']
// [a ' 1 b'] --> [a,' 1 b']
// [a ' 1 b] --> error
// [a .' 1 b] --> error
// Conclusion - do not allow spaces before transpose operators
// [1 -3, 4] --> [1,-3,4]
// Conclusion - spaces and commas are equivalent!
// [a (3)] --> [a,3], not [a(3)]
// [(a (3))] --> [a(3)]
// Outside
// fprintf('%d\n',a (3)) --> works as a(3)
//
// So inside a bracket, the behavior is different than outside a bracket.
// Outside a bracket, the default behavior appears to be ignore_ws = true.
//

// Issues: ":", transposes

#include <string>
#include <stack>
#include <vector>
#include <iostream>

using namespace std;

class tree_node {
public:
  string node;
  vector<tree_node*> children;
  void print();
};


typedef enum {
  op_sentinel,
  op_and,
  op_plus,
  op_minus,
  op_times,
  op_divide,
  op_unary_plus,
  op_unary_minus
} op;

typedef tree_node* tree;
typedef char tok;

string operatorname(op a) {
  switch(a) {
  case op_sentinel:
    return "S";
  case op_plus:
    return "+";
  case op_minus:
    return "-";
  case op_times:
    return "*";
  case op_divide:
    return "/";
  case op_unary_plus:
    return "++";
  case op_unary_minus:
    return "--";
  }
}

static int indentlevel = 0;
void tree_node::print() {
  for (int i=0;i<indentlevel;i++)
    cout << " ";
  cout << node << "\n";
  indentlevel+=3;
  for (int i=0;i<children.size();i++)
    children[i]->print();
  indentlevel-=3;
}

tree mkLeaf(tok a) {
  tree ret = new tree_node;
  ret->node = a;
  return ret;
}

tree mkLeaf(string txt) {
  tree ret = new tree_node;
  ret->node = txt;
  return ret;
}

tree mkNode(op a, tree arg1, tree arg2) {
  tree ret = new tree_node;
  ret->node = operatorname(a);
  ret->children.push_back(arg1);
  ret->children.push_back(arg2);
  return ret;
}

tree mkNode(op a, tree arg1) {
  tree ret = new tree_node;
  ret->node = operatorname(a);
  ret->children.push_back(arg1);
  return ret;
}

tree mkNode(string txt, tree arg1, tree arg2) {
  tree ret = new tree_node;
  ret->node = txt;
  ret->children.push_back(arg1);
  ret->children.push_back(arg2);
  return ret;  
}

tree mkNode(string txt, tree arg1) {
  tree ret = new tree_node;
  ret->node = txt;
  ret->children.push_back(arg1);
  return ret;  
}

tree addChild(tree root, tree child) {
  root->children.push_back(child);
}

class ParserState {
public:
  tok *text;
  int textlen;
  int ptr;
  stack<op> operators;
  stack<tree> operands;
  stack<bool> ignore_ws;  
};

class Parser {
  ParserState state;
  stack<ParserState> checkpoints;
private:
  tok next();
  tok ahead(int a);
  bool match(char a);
  bool match(const char *str);
  bool match(string str);
  void consumenext();
  void consume(int count = 1);
  void flushws();
  void serror(string);
  void popOperator();
  void pushOperator(op);
  void PartialExpression();
  void Expression();
  void expect(tok a);
  void expect(string a);
  void CheckpointParser();
  void DeleteCheckpoint();
  void RestoreCheckpoint();
public:
  Parser(tok *text);
  tree ParseForStatement();
  tree ParseSingletonStatement(string keyword);
  tree ParseWhileStatement();
  tree ParseIfStatement();
  tree ParseSwitchStatement();
  tree ParseTryStatement();
  tree ParseExpression();
  tree ParseStatement();
  tree ParseDeclarationStatement(string keyword);
  tree ParseStatementSeperator();
  tree ParseStatementList();
  tree ParseIdentifier();
  tree ParseAssignmentStatement();
  tree ParseVariableDereference();
  tree ParseMultiFunctionCall();
  tree ParseSpecialFunctionCall();
  tree ParseIndexList();
  void FlushWhitespace();
  void CheckWSFlush();
};

void Parser::CheckpointParser() {
  checkpoints.push(state);
}

void Parser::DeleteCheckpoint() {
  checkpoints.pop();
}

void Parser::RestoreCheckpoint() {
  state = checkpoints.top();
  checkpoints.pop();
}

bool Parser::match(char a) {
  return (next() == a);
}

bool Parser::match(const char *str) {
  int n = strlen(str);
  return ((strncmp(str,state.text+state.ptr,n)==0) &&
	  (!isalnum(state.text[state.ptr+n])));
}

bool Parser::match(string str) {
  return (match(str.c_str()));
}

void Parser::FlushWhitespace() {
  while (next() == ' ')
    consumenext();
}

tree Parser::ParseStatementSeperator() {
  flushws();
  if (match(';'))
    consumenext();
  else if (match('\n'))
    consumenext();
  else if (match(','))
    consumenext();
  return NULL;
}

tree Parser::ParseSingletonStatement(string keyword) {
  expect(keyword);
  flushws();
  tree term = ParseStatementSeperator();
  return mkLeaf(keyword);
}

tree Parser::ParseForStatement() {
  expect("for");
  flushws();
  //  tree index = ParseForIndexExpression();
  tree term = ParseStatementSeperator();
  tree block = ParseStatementList();
  expect("end");
  return mkNode("for",term,block);
}

tree Parser::ParseWhileStatement() {
  expect("while");
  flushws();
  tree warg = ParseExpression();
  flushws();
  tree term = ParseStatementSeperator();
  flushws();
  tree block = ParseStatementList();
  expect("end");
  return mkNode("while",warg,block);
}

tree Parser::ParseIfStatement() {
  expect("if");
  flushws();
  tree test = ParseExpression();
  
}

tree Parser::ParseIdentifier() {
  if (!isalpha(next()))
    return NULL;
  char v = next();
  tree ret = mkLeaf(v);
  consumenext();
  return ret;
}

tree Parser::ParseDeclarationStatement(string keyword) {
  expect(keyword);
  flushws();
  tree root = mkLeaf(keyword);
  while (isalpha(next())) {
    root->children.push_back(ParseIdentifier());
    flushws();
  }
  return root;
}

tree Parser::ParseTryStatement() {
  expect("try");
  flushws();
  ParseStatementSeperator();
  tree block = ParseStatementList();
  tree root = mkNode("try",block);
  if (match("catch")) {
    expect("catch");
    flushws();
    tree block = ParseStatementList();
    addChild(root,mkNode("catch",block));
  }
  expect("end");
  flushws();
  return root;
}


// Keeps extracting expressions until 
tree Parser::ParseIndexList() {
  char openbracket = next();
  char closebracket;
  string rootstring;
  if (openbracket == '(') {
    rootstring = "()";
    closebracket = ')';
  } else {
    rootstring = "{}";
    closebracket = '}';
  }
  consume();
  flushws();
  tree root = mkLeaf(rootstring);
  while (!match(closebracket)) {
    addChild(root,ParseExpression());
    if (!match(closebracket))
      expect(',');
  }
  consume();
  return root;
}

// Parse A(foo).goo{1:3}... etc
tree Parser::ParseVariableDereference() {
  tree ident = ParseIdentifier();
  flushws();
  tree root = mkNode("var",ident);
  bool deref = true;
  while (deref) {
    if (match('(') || match('{'))
      addChild(root,ParseIndexList());
    else if (match('.')) {
      consume();
      flushws();
      if (match('(')) {
	expect('(');
	flushws();
	addChild(root,mkNode(".()",ParseIdentifier()));
	flushws();
	expect(')');
      } else 
	addChild(root,mkNode(".",ParseIdentifier()));
    } else
      deref = false;
  }
  return root;
}

tree Parser::ParseAssignmentStatement() {
  tree ident = ParseVariableDereference();
  flushws();
  expect('=');
  flushws();
  tree expr = ParseExpression();
  return mkNode("assign",ident,expr);
}

tree Parser::ParseSwitchStatement() {
  expect("switch");
  flushws();
  tree swexpr = ParseExpression();
  tree root = mkNode("switch",swexpr);
  ParseStatementSeperator();
  while (match("case")) {
    expect("case");
    flushws();
    tree csexpr = ParseExpression();
    ParseStatementSeperator();
    tree block = ParseStatementList();
    tree casetree = mkNode("case",csexpr,block);
    root->children.push_back(casetree);
  }
  if (match("otherwise")) {
    expect("otherwise");
    flushws();
    ParseStatementSeperator();
    tree block = ParseStatementList();
    tree othertree = mkNode("otherwise",block);
    root->children.push_back(othertree);
  }
  expect("end");
  flushws();
  return root;
}

tree Parser::ParseStatement() {
  if (match("for"))
    return ParseForStatement();
  if (match("break"))
    return ParseSingletonStatement("break");
  if (match("continue"))
    return ParseSingletonStatement("continue");
  if (match("while"))
    return ParseWhileStatement();
  if (match("if"))
    return ParseIfStatement();
  if (match("switch"))
    return ParseSwitchStatement();
  if (match("try"))
    return ParseTryStatement();
  if (match("keyboard"))
    return ParseSingletonStatement("keyboard");
  if (match("return"))
    return ParseSingletonStatement("return");
  if (match("retall"))
    return ParseSingletonStatement("retall");
  if (match("quit"))
    return ParseSingletonStatement("quit");
  if (match("global"))
    return ParseDeclarationStatement("global");
  if (match("persistent"))
    return ParseDeclarationStatement("persistent");
  // Now come the tentative parses
  try {
    CheckpointParser();
    tree retval = ParseAssignmentStatement();
    DeleteCheckpoint();
    return retval;
  } catch (string e) {
    RestoreCheckpoint();
  }
  //   try {
  //     CheckpointParser();
  //     tree retval = ParseMultiFunctionCall();
  //     DeleteCheckpoint();
  //     return retval;
  //   } catch (string e) {
  //     RestoreCheckpoint();
  //   }
  //   try {
  //     CheckpointParser();
  //     tree retval = ParseSpecialFunctionCall();
  //     DeleteCheckpoint();
  //     return retval;
  //   } catch (string e) {
  //     RestoreCheckpoint();
  //   }
  try {
    CheckpointParser();
    tree retval = ParseExpression();
    DeleteCheckpoint();
    return retval;
  } catch (string e) {
    RestoreCheckpoint();
  }
  return NULL;
}

tree Parser::ParseStatementList() {
  tree stlist = mkLeaf("block");
  tree statement = ParseStatement();
  while (statement) {
    stlist->children.push_back(statement);
    ParseStatementSeperator();
    statement = ParseStatement();
  }
  return stlist;
}

tree Parser::ParseExpression() {
  state.operators.push(op_sentinel);
  Expression();
  return state.operands.top();
}

Parser::Parser(tok *txt) {
  state.ignore_ws.push(true);
  state.text = strdup(txt);
  state.textlen = strlen(state.text);
  state.ptr = 0;
}

tok Parser::next() {
  return state.text[state.ptr];
}

tok Parser::ahead(int a) {
  return state.text[state.ptr+a];
}

void Parser::consumenext() {
  if (state.ptr < state.textlen)
    state.ptr++;
}

void Parser::consume(int count) {
  for (int i=0;i<count;i++)
    consumenext();
  if (state.ignore_ws.top())
    while (next() == ' ')
      consumenext();
}

void Parser::flushws() {
  while (next() == ' ')
    consume();
}

void Parser::serror(string errmsg) {
  //   cerr << "Error has occured:" << errmsg << "\n";
  //   cerr << "Current token is '" << next() << "'\n";
  //   cerr << "somewhere here:\n";
  //   state.text[state.ptr+1];
  //   cerr << state.text << "\n";
  //  exit(1);
  throw string(errmsg);
}

void Parser::expect(string a) {
  if (match(a))
    consume(a.size());
  else
    serror(string("expecting ") + a);
}

void Parser::expect(tok a) {
  if (next() == a)
    consume();
  else
    serror(string("expecting token ") + a + " but found " + next());
}

bool is_binary_operator(op a) {
  return ((a>=op_plus) && (a < op_unary_plus));
}

bool is_binary_operator(tok a) {
  return ((a == '+') ||
	  (a == '-') ||
	  (a == '*') ||
	  (a == '/'));
}

bool is_unary_operator(tok a) {
  return ((a == '+') ||
	  (a == '-'));
}

bool is_value(tok a) {
  return ((a>='0') && (a<='9'));
}

void Parser::popOperator() {
  if (is_binary_operator(state.operators.top())) {
    tree t1 = state.operands.top(); state.operands.pop();
    tree t0 = state.operands.top(); state.operands.pop();
    state.operands.push(mkNode(state.operators.top(),t0,t1));
    state.operators.pop();
  } else {
    tree t0 = state.operands.top(); state.operands.pop();
    state.operands.push(mkNode(state.operators.top(),t0));
    state.operators.pop();
  }
}

void Parser::pushOperator(op opr) {
  while (state.operators.top() > opr)
    popOperator();
  state.operators.push(opr);
}

op binary(tok a) {
  if (a == '+') return op_plus;
  if (a == '-') return op_minus;
  if (a == '*') return op_times;
  if (a == '/') return op_divide;
}

op unary(tok a) {
  if (a == '+') return op_unary_plus;
  if (a == '-') return op_unary_minus;
}

void Parser::PartialExpression() {
  if (is_value(next())) {
    state.operands.push(mkLeaf(next()));
    consume();
  } else if (next() == '[') {
    consume();
    state.ignore_ws.push(false);
    tree root = mkLeaf("[]");
    while (!match(']')) {
      addChild(root,ParseExpression());
      flushws();
    }
    expect(']');
    state.operands.push(root);
    state.ignore_ws.pop();
  } else if (next() == '(') {
    consume();
    state.ignore_ws.push(true);
    state.operators.push(op_sentinel);
    Expression();
    state.ignore_ws.pop();
    expect(')');
    state.operators.pop();
  } else if (is_unary_operator(next())) {
    pushOperator(unary(next()));
    consume();
    flushws();
    PartialExpression();
  } else
    serror("unrecognized token");
}

void Parser::CheckWSFlush() {
  if (next() == ' ') {
    int p = 0;
    while (ahead(p) == ' ') p++;
    if (is_binary_operator(ahead(p)) &&
	!is_unary_operator(ahead(p)))
      flushws();
  }
}

void Parser::Expression() {
  PartialExpression();
  CheckWSFlush();
  while (is_binary_operator(next())) {
    pushOperator(binary(next()));
    consume();
    flushws();
    PartialExpression();
  }
  while (state.operators.top() != op_sentinel)
    popOperator();
  state.operators.pop();
}

int main(int argc, char *argv[]) {
  Parser G(argv[1]);
  try {
    tree a = G.ParseStatementList();
    a->print();
    cout << "**********************************************************************\n";
  } catch(string s) {
    cout << "Error: " << s << "\n";
  }
  return 0;
}
