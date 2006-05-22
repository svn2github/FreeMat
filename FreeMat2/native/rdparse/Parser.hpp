#ifndef __Parser_hpp__
#define __Parser_hpp__

class Tree;

class Parser {
public:
  bool Expression();
  bool Block();
  bool WhileStatement();
};

#endif
