// spaces work - what about transposes?

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

class Parser {
  tok *text;
  int textlen;
  int ptr;
  stack<op> operators;
  stack<tree> operands;
  stack<bool> ignore_ws;
private:
  tok next();
  tok ahead(int a);
  bool match(const char *str);
  void consumenext();
  void consume(int count = 1);
  void flushws();
  void error(string);
  void popOperator();
  void pushOperator(op);
  void P();
  void E();
  void expect(tok a);
public:
  Parser(tok *text);
  tree ParseExpression();
  tree ParseStatement();
  void FlushWhitespace();
  void CheckWSFlush();
};

bool Parser::match(const char *str) {
  return (strncmp(str,text+ptr,strlen(str))==0);
}

void Parser::FlushWhitespace() {
  while (next() == ' ')
    consumenext();
}

tree Parser::ParseStatement() {
  if (match("for"))
    return ParseForStatement();
  if (match("break"))
    return ParseBreakStatement();
  if (match("continue"))
    return ParseContinueStatement();
  if (match("while"))
    return ParseWhileStatement();
  if (match("if"))
    return ParseIfStatement();
  if (match("switch"))
    return ParseSwitchStatement();
  if (match("try"))
    return ParseTryStatement();
  if (match("keyboard"))
    return ParseKeyboardStatement();
}

tree Parser::ParseExpression() {
  while (!operators.empty())
    operators.pop();
  while (!operands.empty())
    operands.pop();
  operators.push(op_sentinel);
  E();
  return operands.top();
}

Parser::Parser(tok *txt) {
  ignore_ws.push(false);
  text = strdup(txt);
  textlen = strlen(text);
  ptr = 0;
}

tok Parser::next() {
  return text[ptr];
}

tok Parser::ahead(int a) {
  return text[ptr+a];
}

void Parser::consumenext() {
  if (ptr < textlen)
    ptr++;
}

void Parser::consume(int count) {
  for (int i=0;i<count;i++)
    consumenext();
  if (ignore_ws.top())
    while (next() == ' ')
      consumenext();
}

void Parser::flushws() {
  while (next() == ' ')
    consume();
}

void Parser::error(string errmsg) {
  cerr << "Error has occured:" << errmsg << "\n";
  cerr << "Current token is '" << next() << "'\n";
  cerr << "somewhere here:\n";
  text[ptr+1] = 0;
  cerr << text << "\n";
  exit(1);
}

void Parser::expect(tok a) {
  if (next() == a)
    consume();
  else
    error(string("expecting token ") + a + " but found " + next());
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
  if (is_binary_operator(operators.top())) {
    tree t1 = operands.top(); operands.pop();
    tree t0 = operands.top(); operands.pop();
    operands.push(mkNode(operators.top(),t0,t1));
    operators.pop();
  } else {
    tree t0 = operands.top(); operands.pop();
    operands.push(mkNode(operators.top(),t0));
    operators.pop();
  }
}

void Parser::pushOperator(op opr) {
  while (operators.top() > opr)
    popOperator();
  operators.push(opr);
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

void Parser::P() {
  if (is_value(next())) {
    operands.push(mkLeaf(next()));
    consume();
  } else if (next() == '(') {
    consume();
    ignore_ws.push(true);
    operators.push(op_sentinel);
    E();
    ignore_ws.pop();
    expect(')');
    operators.pop();
  } else if (is_unary_operator(next())) {
    pushOperator(unary(next()));
    consume();
    flushws();
    P();
  } else
    error("unrecognized token");
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

void Parser::E() {
  P();
  CheckWSFlush();
  while (is_binary_operator(next())) {
    pushOperator(binary(next()));
    consume();
    flushws();
    P();
  }
  while (operators.top() != op_sentinel)
    popOperator();
}

tree Eparser(tok *txt) {
  Parser G(txt);
  return G.ParseExpression();
}

int main(int argc, char *argv[]) {
  Parser G(argv[1]);
  tree a = G.ParseExpression();
  a->print();
  cout << "**********************************************************************\n";
  G.FlushWhitespace();
  tree b = G.ParseExpression();
  b->print();
  return 0;
}
