"""
The parser module provides the :class:`parser`, which parses a program text into an AST.
"""

from fmtoken import Token
from fmscanner import Scanner
from fmtree import Tree
import copy

def HasNestedFunctions(root):
  if (root.node == Token.TOK_NEST_FUNC): return True
  for child in root.children:
    if HasNestedFunctions(child): return True
  return False

def AdjustContextOne(context):
  return (((context & 0xffff) - 1) | (context & 0xffff0000))

class ParseError(Exception) :
  def __init__(self, contextnum, errmsg):
    self.contextnum = contextnum
    self.errmsg = errmsg

class Parser:
  OP_PRECEDENCE = {
    Token.TOK_SOR: 1,
    Token.TOK_SAND: 2,
    '|': 3,
    '&': 4,
    '<': 5,
    '>': 5,
    Token.TOK_LE : 5,
    Token.TOK_GE: 5,
    Token.TOK_EQ: 5,
    Token.TOK_NE: 5,
    ':': 6,
    '+': 7,
    '-': 7,
    '*': 8,
    '/': 8,
    '\\': 8,
    Token.TOK_DOTTIMES: 8,
    Token.TOK_DOTRDIV: 8,
    Token.TOK_DOTLDIV: 8,
    Token.TOK_UNARY_PLUS: 9,
    Token.TOK_UNARY_MINUS: 9,
    '~': 9,
    Token.TOK_INCR_PREFIX: 9,
    Token.TOK_DECR_PREFIX: 9,
    '^': 10,
    Token.TOK_DOTPOWER: 10
    }
  def __init__(self, lex, octaveCompatibility = False):
    self.lex = lex
    self.lastpos = 0
    self.lastErr = ''
    self.octCompat = octaveCompatibility
  def consumeIfFound(self,token):
    if (self.match(token)): self.consume()
  def statementSeperator(self):
    root = Tree()
    if (self.match(';')) :
      root = Tree(Token(Token.TOK_QSTATEMENT,AdjustContextOne(self.lex.contextNum())))
      self.consume()
      self.consumeIfFound('\n')
    elif (self.match('\n')) :
      root = Tree(Token(Token.TOK_STATEMENT,AdjustContextOne(self.lex.contextNum())))
      self.consume()
    elif (self.match(',')) :
      root = Tree(Token(Token.TOK_STATEMENT,AdjustContextOne(self.lex.contextNum())))
      self.consume()
    return root
  def singletonStatement(self) :
    root = Tree(self.next())
    self.consume()
    return root
  def dBStepOrTraceStatement(self) :
    root = Tree(self.next())
    self.consume()
    if (self.matchAny([',',';','\n'])):
      return root
    root.addChild(self.expression())
    return root
  def multiFunctionCall(self):
    root = self.expect('[')
    root.rename(Token.TOK_MULTI)
    lhs = Tree(Token(Token.TOK_BRACKETS,self.lex.contextNum()))
    while (not self.match(']')) :
      lhs.addChild(self.variableDereference())
      self.consumeIfFound(',')
    self.expect(']')
    root.addChild(lhs)
    self.expect('=')
    root.addChild(self.expression())
    return root
  def functionReturnList(self,root):
    if (self.match('[')) :
      self.consume()
      lhs = Tree(Token(Token.TOK_BRACKETS,self.lex.contextNum()))
      while (not self.match(']')) :
        lhs.addChild(self.identifier())
        self.consumeIfFound(',')
      self.expect(']')
      root.addChild(lhs)
      self.expect('=')
      root.addChild(self.identifier())
    else :
      save = self.identifier()
      if (self.match('=')) :
        lhs = Tree(Token(Token.TOK_BRACKETS,self.lex.contextNum()))
        lhs.addChild(save)
        root.addChild(lhs)
        self.expect('=')
        root.addChild(self.identifier())
      else :
        root.addChild(Tree(Token(Token.TOK_BRACKETS,self.lex.contextNum())))
        root.addChild(save)
  def functionArgs(self,root):
    # Process (optional) args
    if (self.match('(')):
      self.consume()
      args = Tree(Token(Token.TOK_PARENS,self.lex.contextNum()))
      while (not self.match(')')):
        ident = Tree()
        if (self.match('&')) :
          ident = self.expect('&')
          ident.addChild(self.identifier())
        else: 
          ident = self.identifier()
          args.addChild(ident)
          if (not self.match(')')): self.expect(',')
      self.expect(')')
      root.addChild(args)
    else :
      root.addChild(Tree(Token(Token.TOK_PARENS,self.lex.contextNum())))
  def functionDefinition(self):
      root = self.expect(Token.TOK_FUNCTION)
      self.functionReturnList(root)
      self.functionArgs(root)
      self.statementSeperator()
      root.addChild(self.statementList())
      return root
  def matchNumber(self):
    return (self.matchAny([Token.TOK_REAL,Token.TOK_IMAG,Token.TOK_REALF,Token.TOK_IMAGF]))
  def specialFunctionCall(self) :
    self.lex.pushWSFlag(False)
    root = Tree(Token(Token.TOK_SPECIAL,self.lex.contextNum()))
    root.addChild(self.identifier())
    # Next must be a whitespace
    if (not self.match(Token.TOK_SPACE)): self.serror("Not special call")
    self.consume()
    t_lex = copy.deepcopy(self.lex)
    if (t_lex.next() in [';','\n','(',',']): self.serror("Not special call")
    if (t_lex.next().isBinaryOperator() or 
        t_lex.next().isUnaryOperator()) : t_lex.self.consume()
    if (t_lex.next() == Token.TOK_SPACE): self.serror("Not special call")
    # If the next thing is a character or a number, we grab "blobs"
    self.lex.setBlobMode(True)
    while (not self.match(';') and not self.match('\n') and not (self.match(','))) :
      root.addChild(Tree(self.next()))
      self.consume()
      self.consumeIfFound(Token.TOK_SPACE)
    self.lex.setBlobMode(False)
    self.lex.popWSFlag()
    return root
  def forIndexExpression(self) :
    if (self.match('(')) :
      self.consume()
      ret = self.forIndexExpression()
      self.expect(')')
      return ret
    ident = self.identifier()
    if (self.match('=')) :
      root = Tree(self.next())
      self.consume()
      expr = self.expression()
      root.addChildren(ident,expr)
      return root
    else :
      return ident
  def forStatement(self) :
    root = self.expect(Token.TOK_FOR)
    index = self.forIndexExpression()
    self.statementSeperator()
    block = self.statementList()
    self.expect(Token.TOK_END,"FOR statement")
    root.addChildren(index,block)
    return root
  def whileStatement(self) :
    root = self.expect(Token.TOK_WHILE)
    warg = self.expression()
    self.statementSeperator()
    block = self.statementList()
    self.expect(Token.TOK_END,"WHILE statement")
    root.addChildren(warg,block)
    return root
  def ifStatement(self) :
    root = self.expect(Token.TOK_IF)
    test = self.expression()
    self.statementSeperator()
    trueblock = self.statementList()
    root.addChildren(test,trueblock)
    while (self.match(Token.TOK_ELSEIF)) :
      elseif = Tree(self.next())
      self.consume()
      test = self.expression()
      block = self.statementList()
      elseif.addChildren(test,block)
      root.addChild(elseif)
    if (self.match(Token.TOK_ELSE)):
      elseblk = Tree(self.next())
      self.consume()
      block = self.statementList()
      elseblk.addChild(block)
      root.addChild(elseblk)
    self.expect(Token.TOK_END," IF block")
    return root
  def identifier(self) :
    if (not self.match(Token.TOK_IDENT)):  self.serror("self.expecting identifier")
    ret = Tree(self.next())
    self.consume()
    return ret
  def declarationStatement(self) :
    root = Tree(self.next())
    self.consume()
    while (self.match(Token.TOK_IDENT)) :
      root.addChild(self.identifier())
    return root
  def tryStatement(self) :
    root = self.expect(Token.TOK_TRY)
    self.statementSeperator()
    block = self.statementList()
    root.addChild(block)
    if (self.match(Token.TOK_CATCH)) :
      catchblock = Tree(self.next())
      self.consume()
      self.statementSeperator()
      block = self.statementList()
      catchblock.addChild(block)
      root.addChild(catchblock)
    self.expect(Token.TOK_END, "TRY block")
    return root
  def keyword(self) :
    root = self.expect('/')
    root.rename(Token.TOK_KEYWORD)
    root.addChild(self.identifier())
    if (self.match('=')):
      self.consume()
      root.addChild(self.expression())
    return root
  def indexingParens(self, root, blankRefOK):
    self.consume()
    if (self.octCompat) : self.lex.pushWSFlag(True)
    sub = Tree(Token(Token.TOK_PARENS,self.lex.contextNum()))
    while (not self.match(')')) :
      if (self.match(':')) :
        sub.addChild(self.expect(':'))
      elif (self.match('/')) :
        sub.addChild(self.keyword())
      else :
        sub.addChild(self.expression())
      if (not self.match(')')) : self.expect(',')
    if ((sub.numChildren() == 0) and (not blankRefOK)) :
      if (self.octCompat): self.lex.popWSFlag()
      self.serror("The expression A() is not allowed.")
    self.expect(')')
    if (self.octCompat): self.lex.popWSFlag()
    root.addChild(sub)
  def indexingBraces(self, root):
    self.consume()
    sub = Tree(Token(Token.TOK_BRACES,self.lex.contextNum()))
    while (not self.match('}')) :
      if (self.match(':')) :
        sub.addChild(self.expect(':'))
      else :
        sub.addChild(self.expression())
      if (not self.match('}')) : self.expect(',')
      if (sub.numChildren() == 0) : self.serror("The expression A{} is not allowed.")
    self.expect('}')
    root.addChild(sub)
  def indexingExpressions(self, root, blankRefOK):
    deref = True
    while (deref) :
      if self.match('(') :
        self.indexingParens(root,blankRefOK)
      elif self.match('{') :
        self.indexingBraces(root)
      elif self.match('.') :
        dynroot = Tree(self.next())
        self.consume()
        if (self.match('(')) :
          self.consume()
          dynroot.rename(Token.TOK_DYN)
          dynroot.addChild(self.expression())
          root.addChild(dynroot)
          self.expect(')')
        else :
          dynroot.addChild(self.identifier())
          root.addChild(dynroot)
      else :
        deref = False
  def variableDereference(self,blankRefOK = True) :
    ident = self.identifier()
    root = Tree(Token(Token.TOK_VARIABLE,self.lex.contextNum()))
    root.addChild(ident)
    self.indexingExpressions(root,blankRefOK)
    return root
  def plusEqStatement(self,ident):
    opr = self.next()
    self.consume()
    opr.value = '='
    root = Tree(opr)
    expr = self.expression()
    opr.value = '+'
    adder = Tree(opr)
    adder.addChildren(ident,expr)
    root.addChildren(ident,adder)
    return root
  def minusEqStatement(self,ident):
    opr = self.next()
    self.consume()
    opr.value = '='
    root = Tree(opr)
    expr = self.expression()
    opr.value = '-'
    adder = Tree(opr)
    adder.addChildren(ident,expr)
    root.addChildren(ident,adder)
    return root
  def assignmentStatement(self) :
    ident = self.variableDereference(False)
    if (not self.octCompat or self.match('=')):
      root = self.expect('=')
      expr = self.expression()
      root.addChildren(ident,expr)
      return root
    if (self.match(Token.TOK_PLUS_EQ)) :
      return self.plusEqStatement(ident)
    if (self.match(Token.TOK_MINUS_EQ)):
      return self.minusEqStatement(ident)
    self.serror("Not an assignment statement")
  def flushSeperators(self) :
    while (True) :
      term = self.statementSeperator()
      if (not term.valid()) : return
  def switchStatement(self) :
    root = self.expect(Token.TOK_SWITCH)
    swexpr = self.expression()
    root.addChild(swexpr)
    self.flushSeperators()
    while (self.match(Token.TOK_CASE)) :
      caseblock = Tree(self.next())
    self.consume()
    csexpr = self.expression()
    self.statementSeperator()
    block = self.statementList()
    caseblock.addChildren(csexpr,block)
    root.addChild(caseblock)
    if (self.match(Token.TOK_OTHERWISE)) :
      otherwise = Tree(self.next())
      self.consume()
      self.statementSeperator()
      block = self.statementList()
      otherwise.addChild(block)
      root.addChild(otherwise)
    self.expect(Token.TOK_END, " SWITCH block")
    return root
  def statementList(self):
    stlist = Tree(Token(Token.TOK_BLOCK,self.lex.contextNum()))
    self.flushSeperators()
    s = self.statement()
    while (s.valid()):
      sep = self.statementSeperator()
      if (not sep.valid()):
        return stlist
      sep.addChild(s)
      stlist.addChild(sep)
      self.flushSeperators()
      s = self.statement()
    return stlist
  def statement(self):
    if (self.match(Token.TOK_EOF)): return Tree()
    if (self.match(Token.TOK_END)): return Tree()
    if (self.match(Token.TOK_FOR)): return self.forStatement()
    if (self.match(Token.TOK_BREAK)): return self.singletonStatement()
    if (self.match(Token.TOK_CONTINUE)): return self.singletonStatement()
    if (self.match(Token.TOK_DBUP) or self.match(Token.TOK_DBDOWN)): return self.singletonStatement()
    if (self.match(Token.TOK_WHILE)): return self.whileStatement()
    if (self.match(Token.TOK_DBSTEP) or self.match(Token.TOK_DBTRACE)): return self.dBStepOrTraceStatement()
    if (self.match(Token.TOK_IF)): return self.ifStatement()
    if (self.match(Token.TOK_SWITCH)): return self.switchStatement()
    if (self.match(Token.TOK_TRY)):  return self.tryStatement()
    if (self.match(Token.TOK_KEYBOARD) or self.match(Token.TOK_RETURN) or 
        self.match(Token.TOK_RETALL) or self.match(Token.TOK_QUIT)): return self.singletonStatement()
    if (self.match(Token.TOK_GLOBAL) or self.match(Token.TOK_PERSISTENT)):
      return self.declarationStatement()
    # Now come the tentative parses
    save = copy.deepcopy(self.lex)
    if (self.match(Token.TOK_IDENT)) :
      try :
        retval = self.assignmentStatement()
        self.lastpos = 0
        return retval
      except ParseError:
        self.lex = save
    if (self.match('[')) :
      try :
        retval = self.multiFunctionCall()
        self.lastpos = 0
        return retval
      except ParseError:
        self.lex = save
    if (self.match(Token.TOK_IDENT)) :
      try :
        retval = self.specialFunctionCall()
        self.lastpos = 0
        return retval
      except ParseError:
        self.lex = save
    if (self.match(Token.TOK_FUNCTION)) :
      try :
        retval = self.functionDefinition()
        retval.rename(Token.TOK_NEST_FUNC)
        self.expect(Token.TOK_END, "FUNCTION definition")
        self.lastpos = 0
        return retval
      except ParseError:
        self.lex = save
    try :
      retval = Tree(Token(Token.TOK_EXPR,self.lex.contextNum()))
      retval.addChild(self.expression())
      self.lastpos = 0
      return retval
    except ParseError:
      self.lex = save
    return Tree()
  def expression(self):
    self.consumeIfFound(Token.TOK_SPACE)
    return self.exp(0)
  def next(self):
    return self.lex.next()
  def serror(self,msg):
    if (self.lex.contextNum() > self.lastpos):
      self.lasterr = msg
      self.lastpos = self.lex.contextNum()
    raise ParseError(self.lex.contextNum(),msg)
  def expect(self, tokenvalue, because = ""):
    ret = self.next()
    if (self.lex.next() != tokenvalue):
      if (tokenvalue != Token.TOK_EOF):
        self.serror("Expecting " +str(Token(tokenvalue)) + " for " + because)
      else:
        self.serror("Unexpected input")
    else:
      self.consume()
    return Tree(ret)
  def match(self, tokenvalue):
    return self.lex.next() == tokenvalue
  def matchAny(self, tokenlist):
    return self.lex.next() in tokenlist
  def consume(self):
    self.lex.consume()
  def exp(self,priority):
    t = self.primaryExpression()
    if (self.octCompat and (self.matchAny(['(','{','.']))):
      sub = Tree(Token(Token.TOK_REINDEX,self.lex.contextNum()))
      sub.addChild(t);
      self.indexingExpressions(sub,True);
      t = sub;
    while (self.next().isBinaryOperator() and (Parser.OP_PRECEDENCE[self.next().value] >= priority)) :
      opr_save = self.next()
      self.consume();
      self.consumeIfFound(Token.TOK_SPACE)
      if (opr_save.isRightAssociative()):
        q = Parser.OP_PRECEDENCE[opr_save.value]
      else:
        q = 1+Parser.OP_PRECEDENCE[opr_save.value]
      t1 = self.exp(q);
      t = Tree(opr_save,t,t1);
    return t
  def unaryExpression(self):
    opr = self.next();
    self.consume();
    self.consumeIfFound(Token.TOK_SPACE)
    if (opr == '+'): opr.value = Token.TOK_UNARY_PLUS
    if (opr == '-'): opr.value = Token.TOK_UNARY_MINUS
    if (opr == Token.TOK_INCR): opr.value = Token.TOK_INCR_PREFIX
    if (opr == Token.TOK_DECR): opr.value = Token.TOK_DECR_PREFIX
    q = Parser.OP_PRECEDENCE[opr.value]
    child = self.exp(q);
    return Tree(opr,child)
  def parenthesisExpression(self):
    self.consume()
    self.lex.pushWSFlag(True)
    t = self.exp(0)
    self.lex.popWSFlag()
    self.expect(')')
    return self.transposeFixup(t)
  def anonymousFunctionExpression(self):
    root = Tree(self.next())
    self.consume()
    if (self.match('(')):
      root.addChild(self.anonymousFunction())
    else:
      root.addChild(self.identifier())
    return self.transposeFixup(root)
  def literalExpression(self):
    t = Tree(self.next())
    self.consume()
    return self.transposeFixup(t)
  def rowDef(self,closebracketvalue):
    rowdef = Tree(Token(Token.TOK_ROWDEF,self.lex.contextNum()))
    while (not self.match(';') and not self.match('\n') and not self.match(closebracketvalue)) :
      rowdef.addChild(self.expression());
      if (self.match(',')):
	self.consume();
	while (self.match(Token.TOK_SPACE)): self.consume()
      elif self.match(Token.TOK_SPACE):
	self.consume();
    if (self.matchAny([';','\n'])) : self.consume()
    self.consumeIfFound(Token.TOK_SPACE)
    return rowdef
  def matDef(self,basetokenvalue,closebracketvalue):
    # Octave compatibility mode requires commas between matrix entries, so white
    # space is ignored inside matrix definitions.
    if (not self.octCompat): self.lex.pushWSFlag(False);
    matdef = Tree(Token(basetokenvalue))
    self.consumeIfFound(Token.TOK_SPACE)
    while (not self.match(closebracketvalue)) :
      rowdef = self.rowDef(closebracketvalue)
      matdef.addChild(rowdef);
    if (not self.octCompat): self.lex.popWSFlag();
    return matdef;
  def matrixLiteralExpression(self):
    self.consume();
    t = self.matDef(Token.TOK_MATDEF,']');
    self.expect(']');
    return self.transposeFixup(t);
  def cellLiteralExpression(self):
    self.consume();
    t = self.matDef(Token.TOK_CELLDEF,'}');
    self.expect('}');
    return self.transposeFixup(t);
  def transposeFixup(self,base):
    while (self.next() in ['\'',Token.TOK_DOTTRANSPOSE]):
      base = Tree(self.next(),base)
      self.consume()
    if (self.match(Token.TOK_SPACE)):
      if (not((self.lex.peek(0,'-') or self.lex.peek(0,'+')) and not self.lex.peek(1,' '))):
        self.consume();
    if (self.octCompat):
      if (self.match(Token.TOK_INCR)):
	  base = Tree(Token.TOK_INCR_POSTFIX,base);
	  self.consume();
      elif (self.match(Token.TOK_DECR)):
	  base = Tree(Token.TOK_DECR_POSTFIX,base);
	  self.consume();
    return base
  def primaryExpression(self):
    if (self.next().isUnaryOperator()): return self.unaryExpression()
    if (self.match('(')): return self.parenthesisExpression()
    if (self.match('@')): return self.anonymousFunctionExpression()
    if (self.matchAny([Token.TOK_REAL,Token.TOK_IMAG,Token.TOK_REALF,Token.TOK_IMAGF,Token.TOK_STRING])): return self.literalExpression()
    if (self.match(Token.TOK_END)): return self.transposeFixup(self.expect(Token.TOK_END,"transpose"))
    if (self.match(Token.TOK_IDENT)): return self.transposeFixup(self.variableDereference())
    if (self.match('[')): return self.matrixLiteralExpression()
    if (self.match('{')): return self.cellLiteralExpression()
    if (self.matchAny([')',']','}'])):
      self.serror("mismatched parenthesis");
    else:
      self.serror("unrecognized token");
  def process(self):
    self.lastpos = 0
    while (self.match('\n')): self.consume()
    try :
      if (self.match(Token.TOK_FUNCTION)) :
        root = Tree(Token(Token.TOK_FUNCTION_DEFS,self.lex.contextNum()))
        while (self.match(Token.TOK_FUNCTION)) :
          child = self.functionDefinition()
          root.addChild(child);
          while (self.match('\n')) : self.consume()
#        print root
        if (HasNestedFunctions(root) or self.match(Token.TOK_END)) :
	  self.expect(Token.TOK_END);
        while (self.match('\n')): self.consume();
        while (self.match(Token.TOK_FUNCTION)) :
          root.addChild(self.functionDefinition());
	if (HasNestedFunctions(root) or self.match(Token.TOK_END)) :
          self.expect(Token.TOK_END);
	while (self.match('\n')): self.consume();
      else :
        root = Tree(Token(Token.TOK_SCRIPT,self.lex.contextNum()))
        root.addChild(self.statementList());
    except ParseError as e:
      raise NameError(self.lastErr + self.lex.context(self.lastpos))
    try :
      self.expect(Token.TOK_EOF);
    except ParseError as e:
#      print root
      raise NameError("Unexpected input ")
    return root
