%{
// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

    /*************************************************************
     * $Source$
     * $Revision$
     * $Date$
     * $Author$
     *************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "AST.hpp"
#include "WalkTree.hpp"
#include "FunctionDef.hpp"
#include "ParserInterface.hpp"
#include "Exception.hpp"

#define YYSTYPE FreeMat::ASTPtr

#define MSGBUFLEN 2048
static char msgBuffer[MSGBUFLEN];

#include "LexerInterface.hpp"

extern char* yytext;
extern int yylex(void);

extern int lineNumber;
extern int yydebug;

namespace FreeMat {
  static ASTPtr mainAST;
  static MFunctionDef *mainMDef;
  
  bool errorOccured;
  bool interactiveMode;
  static char *expectString = NULL;
  static const char *filename;

  void yyexpect(char *s) {
	expectString = s;
  }

  void yyerror(char *t) {
    t = NULL;
  }

  int yyreport(char *xStr) {
    char *tokdesc;
    char *tokbuffer = "unprintable";
    char buffer[256];
    int addone = 1;
    if (*yytext < 33) {
	tokdesc = tokbuffer;
	sprintf(buffer,"Ran out of input on this line.");
	tokdesc = buffer;
	addone = 0;
    } else {
        sprintf(buffer,"Current token is '%s'",yytext);
	tokdesc = buffer;
    }	
    if (xStr)
      if (!interactiveMode)
        snprintf(msgBuffer,MSGBUFLEN,"Expecting %s at line %d of file %s.  %s",
	xStr,lineNumber+addone,filename,tokdesc);
      else
        snprintf(msgBuffer,MSGBUFLEN,"Expecting %s.  %s",xStr, tokdesc);
    else
      if (!interactiveMode)
        snprintf(msgBuffer,MSGBUFLEN,"Syntax error at line %d of file %s.  %s",
	lineNumber+addone,filename,tokdesc);
      else
        snprintf(msgBuffer,MSGBUFLEN,"Syntax error at input.  %s",tokdesc);
    throw Exception(msgBuffer);
    return 0;
  }

  void yyxpt(char *t) {
   yyreport(t);
  }

}

using namespace FreeMat;

%}

%token IDENT
%token NUMERIC ENDQSTMNT ENDSTMNT
%token LE GE EQ
%token DOTTIMES DOTRDIV DOTLDIV
%token DOTPOWER DOTTRANSPOSE
%token STRING SPECIALCALL
%token END IF FUNCTION FOR BREAK
%token WHILE ELSE ELSEIF
%token SWITCH CASE OTHERWISE CONTINUE
%token TRY CATCH
%token FIELD REFLPAREN REFRPAREN REFLBRACE REFRBRACE 
%token ENDFOR ENDSWITCH ENDWHILE ENDTRY ENDIF PERSISTENT
%token KEYBOARD RETURN VARARGIN VARARGOUT GLOBAL
%token QUIT RETALL

%left ':'
%left '|'
%left '&'
%left '<' LE '>' GE EQ NE
%left '+' '-'
%left '*' '/' '\\' DOTTIMES DOTRDIV DOTLDIV
%right POS NEG NOT
%left '^' DOTPOWER
%nonassoc '\'' DOTTRANSPOSE

%%

program:
  statementList {mainAST = $1;} |
  functionDefList |
;

functionDef:
   FUNCTION  returnDeclaration IDENT REFLPAREN argumentList REFRPAREN statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = $2->toStringList();
     r->name = $3->text;
     r->arguments = $5->toStringList();
     r->code = $7;
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   } |
   FUNCTION  IDENT REFLPAREN argumentList REFRPAREN statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = $2->text;
     r->arguments = $4->toStringList();
     r->code = $6;
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   } |
   FUNCTION  returnDeclaration IDENT statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = $2->toStringList();
     r->name = $3->text;
     r->code = $4;
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   } |
   FUNCTION  IDENT statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = $2->text;
     r->code = $3;
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   } |
   FUNCTION  returnDeclaration IDENT REFLPAREN REFRPAREN statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = $2->toStringList();
     r->name = $3->text;
     r->code = $6;
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   } |
   FUNCTION  IDENT REFLPAREN REFRPAREN statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = $2->text;
     r->code = $4;
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   } |
   FUNCTION  error {yyxpt("function name or return declaration");} |
   FUNCTION  IDENT error {yyxpt("argument list or statement list");} |
   FUNCTION  IDENT REFLPAREN  error {yyxpt("(possibly empty) argument list");}  |
   FUNCTION  IDENT REFLPAREN argumentList REFRPAREN error {yyxpt("statement list");} |
   FUNCTION  returnDeclaration  error {yyxpt("function name");} |
   FUNCTION  returnDeclaration IDENT error {yyxpt("argument list or statement list");} |
   FUNCTION  returnDeclaration IDENT REFLPAREN  error {yyxpt("(possibly empty) argument list");} |
   FUNCTION  returnDeclaration IDENT REFLPAREN argumentList REFRPAREN error {yyxpt("statement list");}
  ;

functionDefList:
  functionDef |
  functionDefList functionDef
  ;

returnDeclaration:
  VARARGOUT '=' {$$ = $1;} |
  IDENT '=' {$$ = $1;} | 
  '[' argumentList ']' '=' {$$ = $2;} |
  error {yyxpt("either 'varargout', a single returned variable, or a list of return variables in return declaration");} |
  VARARGOUT error {yyxpt("an '=' symbol after 'varargout' in return declaration");} |
  IDENT error {yyxpt("an '=' symbol after identifier in return declaration");} |
  '[' error {yyxpt("a valid list of return arguments in return declaration");} | 
  '[' argumentList error {yyxpt("matching ']' in return declaration");} |
  '[' argumentList ']' error {yyxpt("an '=' symbol after return declaration");}
  ;

argumentList:
  argument {$$ = $1;}
  | argumentList ',' argument {$$ = $1; $$->addChild($3);}
  ;

argument:
  IDENT | '&' IDENT {
	$$ = $2;
	char *b = (char*) malloc(strlen($2->text)+2);
	b[0] = '&';
	strcpy(b+1,$2->text);
	$$->text = b;
  } 
  error {yyxpt("either an identifier or an ampersand '&' (indicating pass by reference) followed by an identifier in argument list");}
  ;
  

statementList:
   statement {$$ = new AST(OP_BLOCK,$1);} | 
   statementList statement {$$ = $1; $$->addChild($2);}
   ;

statement:
	 statementType ENDQSTMNT {
  	    $$ = new AST(OP_QSTATEMENT,NULL);
	    if (!interactiveMode) {
	      snprintf(msgBuffer,MSGBUFLEN,"line %d of file %s",lineNumber,filename);
	      $$->down = new AST(context_node, strdup(msgBuffer));
	      $$->down->down = $1;
	    } else
	      $$->down = $1;
	 }
	 | statementType ENDSTMNT {
	    $$ = new AST(OP_RSTATEMENT,NULL);
	    if (!interactiveMode) {
	      snprintf(msgBuffer,MSGBUFLEN,"line %d of file %s",lineNumber,filename);
	      $$->down = new AST(context_node, strdup(msgBuffer));
	      $$->down->down = $1;
	    } else
	      $$->down = $1;
	 }
	 | statementType ',' {
	    $$ = new AST(OP_RSTATEMENT,NULL);
	    if (!interactiveMode) {
	      snprintf(msgBuffer,MSGBUFLEN,"line %d of file %s",lineNumber,filename);
	      $$->down = new AST(context_node, strdup(msgBuffer));
	      $$->down->down = $1;
	    } else
	      $$->down = $1;
	 } 
	 ;

statementType:
	 assignmentStatement
	 | expr
	 | {$$ = new AST(null_node,"");} 
         | multiFunctionCall
         | forStatement
         | breakStatement
	 | continueStatement
         | whileStatement
       	 | ifStatement
	 | switchStatement
	 | tryStatement
	 | keyboardStatement
	 | returnStatement
	 | globalStatement
	 | persistentStatement
	 | specialSyntaxStatement
	 | QUIT | RETALL |
	error {yyxpt("recognizable statement type (assignment, expression, function call, etc...)");}
         ;

specialSyntaxStatement:
	SPECIALCALL stringList {$1->addChild($2); $$ = new AST(OP_SCALL,$1);}
	;

stringList:
	STRING {$$ = new AST(OP_PARENS,$1);} |
	stringList STRING {$$ = $1; $$->addChild($2);}
	;

persistentStatement:
	PERSISTENT identList {$$ = $1; $$->addChild($2); } |
	PERSISTENT error {yyxpt("list of variables to be tagged as persistent");}
 	;

globalStatement:
	GLOBAL identList {$$ = $1; $$->addChild($2);} |
	GLOBAL error {yyxpt("list of variables to be tagged as global");}
	;

identList:
	IDENT | 
	identList IDENT {$$ = $1; $$->addChild($2);} |
	identList error {yyxpt("list of valid identifiers");}
	;

returnStatement:
	RETURN
	;

keyboardStatement:
	KEYBOARD
	;

continueStatement:
	 CONTINUE
	 ;
breakStatement:
	 BREAK
	 ;

tryStatement:
	TRY statementList optionalCatch ENDTRY 
	{ $$ = $1; $$->addChild($2); if ($3 != NULL) $$->addChild($3);} 
        ;

optionalCatch:
	CATCH statementList {$$ = $2;}
	| {$$ = NULL;}
	;

switchStatement:
	SWITCH 	expr optionalEndStatement caseBlock otherwiseClause ENDSWITCH {
	  $$ = $1; $$->addChild($2); 
	  if ($4 != NULL) $$->addChild($4); 
	  if ($5 != NULL) $$->addChild($5);
	}
        ;

optionalEndStatement:
	',' | ENDSTMNT | ENDQSTMNT |
	;

caseBlock:
	/* empty */ {$$ = NULL;} |
	caseList
	;

caseList:
	caseStatement {
	  $$ = new AST(OP_CASEBLOCK,$1);
	} |
	caseList caseStatement {
	  $$ = $1; $$->addChild($2);
	}
;

caseStatement:
	CASE expr statementList {
	  $$ = $1; $$->addChild($2); $$->addChild($3);
	}
;

otherwiseClause:
	OTHERWISE statementList {
	  $$ = $2;
	} |
	/* empty */ {
	  $$ = NULL;
	}
;

forStatement:
	FOR forIndexExpression statementList ENDFOR {
	  $$ = $1; $$->addChild($2); $$->addChild($3);
	}
;

forIndexExpression:
	'(' IDENT '=' expr ')' {$$ = $2; $$->addChild($4);} |
	IDENT '=' expr {$$ = $1; $$->addChild($3);} |
	IDENT {$$ = $1; $$->addChild(new AST(OP_RHS, new AST(id_node,$1->text))); } |
	'(' IDENT '=' expr error {yyxpt("matching right parenthesis");} |
	'(' IDENT '=' error {yyxpt("indexing expression");} |
	'(' IDENT error {yyxpt("equals operator after loop index");} |
	'(' error {yyxpt("identifier that is the loop variable");} |
	IDENT '=' error {yyxpt("indexing expression");} |
	error {yyxpt("identifier or assignment (id = expr) after 'for' ");}
;

whileStatement:
	WHILE expr statementList ENDWHILE {
	  $$ = $1; $$->addChild($2); $$->addChild($3);
	}  |
	WHILE error {yyxpt("test expression after 'while'");} 
;

ifStatement:
	IF conditionedStatement elseIfBlock elseStatement ENDIF {
	  $$ = $1; $$->addChild($2); if ($3 != NULL) $$->addChild($3); 
	  if ($4 != NULL) $$->addChild($4);
	} |
	IF error {yyxpt("condition expression for 'if'");}
;

conditionedStatement:
	expr statementList {
	  $$ = new AST(OP_CSTAT,$1,$2);
	}
;

elseIfBlock:
	/* empty */ {$$ = NULL;} |
	elseIfStatementList
	;

elseIfStatementList:
	elseIfStatement {
	  $$ = new AST(OP_ELSEIFBLOCK,$1);
	} |
	elseIfStatementList elseIfStatement {
	  $$ = $1; $$->addChild($2);
	}
;

elseIfStatement:
	ELSEIF conditionedStatement {
	  $$ = $2;
	} |
	ELSEIF error {yyxpt("test condition for 'elseif' clause");};

elseStatement:
	ELSE statementList {
	  $$ = $2;
	} |
	/* empty */ {$$ = NULL;} |
	ELSE error {yyxpt("statement list for 'else' clause");}
;

assignmentStatement:
	symbRefList '=' expr {$$ = new AST(OP_ASSIGN,$1,$3);} |
	symbRefList '=' error {yyxpt("expression in assignment");}
        ;

multiFunctionCall:
	'[' matrixDef ']' '='  IDENT REFLPAREN  indexList REFRPAREN {
	  $5->addChild(new AST(OP_PARENS,$7));
	  $$ = new AST(OP_MULTICALL,$2,$5);
	} |
	'[' matrixDef ']' '=' IDENT  {
	  $5->addChild(new AST(OP_PARENS,NULL));
	  $$ = new AST(OP_MULTICALL,$2,$5);
	} |
	'[' matrixDef ']' '='  IDENT REFLPAREN  indexList error 
	{yyxpt("matching right parenthesis");} |
	'[' matrixDef ']' '='  IDENT REFLPAREN  error 
	{yyxpt("indexing list");} |
	'[' matrixDef ']' '='  IDENT error 
	{yyxpt("left parenthesis");} |	
	'[' matrixDef ']' '=' error 
	{yyxpt("identifier");}
        ; 

expr:
	expr ':' expr {$$ = new AST(OP_COLON,$1,$3);} 
	| expr ':' error {yyxpt("an expression after ':'");}
	| terminal
	| expr '+' expr {$$ = new AST(OP_PLUS,$1,$3);}
	| expr '+' error {yyxpt("an expression after '+'");}
	| expr '-' expr {$$ = new AST(OP_SUBTRACT,$1,$3);}
	| expr '-' error {yyxpt("an expression after '-'");}
	| expr '*' expr {$$ = new AST(OP_TIMES,$1,$3);}
	| expr '*' error {yyxpt("an expression after '*'");}
	| expr '/' expr {$$ = new AST(OP_RDIV,$1,$3);}
	| expr '/' error {yyxpt("an expression after '/'");}
	| expr '\\' expr {$$ = new AST(OP_LDIV,$1,$3);}
	| expr '\\' error {yyxpt("an expression after '\\'");}
	| expr '|' expr {$$ = new AST(OP_OR,$1,$3);}
	| expr '|' error {yyxpt("an expression after '|'");}
	| expr '&' expr {$$ = new AST(OP_AND,$1,$3);}
	| expr '&' error {yyxpt("an expression after '&'");}
	| expr '<' expr {$$ = new AST(OP_LT,$1,$3);}
	| expr '<' error {yyxpt("an expression after '<'");}
	| expr LE  expr {$$ = new AST(OP_LEQ,$1,$3);}
	| expr LE  error {yyxpt("an expression after '<='");}
	| expr '>' expr {$$ = new AST(OP_GT,$1,$3);}
	| expr '>' error {yyxpt("an expression after '>'");}
	| expr GE  expr {$$ = new AST(OP_GEQ,$1,$3);}
	| expr GE  error {yyxpt("an expression after '>='");}
	| expr EQ  expr {$$ = new AST(OP_EQ,$1,$3);}
	| expr EQ  error {yyxpt("an expression after '=='");}
	| expr NE  expr {$$ = new AST(OP_NEQ,$1,$3);}
	| expr NE  error {yyxpt("an expression after '~='");}
	| expr DOTTIMES expr {$$ = new AST(OP_DOT_TIMES,$1,$3);}
	| expr DOTTIMES error {yyxpt("an expression after '.*'");}
	| expr DOTRDIV  expr {$$ = new AST(OP_DOT_RDIV,$1,$3);}
	| expr DOTRDIV  error {yyxpt("an expression after './'");}
	| expr DOTLDIV expr {$$ = new AST(OP_DOT_LDIV,$1,$3);}
	| expr DOTLDIV error {yyxpt("an expression after '.\\'");}
	| NEG  expr %prec NEG {$$ = new AST(OP_NEG,$2);}
	| NEG  error %prec NEG {yyxpt("an expression after negation");}
	| POS  expr %prec POS {$$ = $2;}
	| POS  error %prec POS {yyxpt("an expression after positive sign");}
	| '~' expr %prec NOT {$$ = new AST(OP_NOT,$2);}
	| '~' error %prec NOT {yyxpt("an expression after logical not");}
	| expr '^' expr  {$$ = new AST(OP_POWER,$1,$3);}
	| expr '^' error  {yyxpt("an expression after '^'");}
	| expr DOTPOWER expr {$$ = new AST(OP_DOT_POWER,$1,$3);}
	| expr DOTPOWER error {yyxpt("an expression after '.^'");}
	| expr '\''          {$$ = new AST(OP_TRANSPOSE,$1);}
	| expr DOTTRANSPOSE  {$$ = new AST(OP_DOT_TRANSPOSE,$1);}
	| '(' expr ')'  {$$ = $2;}
	| '(' expr error  {yyxpt("a right parenthesis after expression");}
	| '(' error  {yyxpt("an expression after left parenthesis");}
	;

terminal:
        NUMERIC
	| STRING
	| END
	| symbRefList {$$ = new AST(OP_RHS,$1);}
        | '[' matrixDef ']' {$$ = $2;}
        | '[' error {yyxpt("a matrix definition followed by a right bracket");}
        | '[' rowSeperator matrixDef ']' {$$ = $3;}
        | '[' matrixDef rowSeperator ']' {$$ = $2;}
        | '[' rowSeperator matrixDef rowSeperator ']' {$$ = $3;}
	| '[' ']' {$$ = new AST(OP_EMPTY,NULL);}
        | '{' cellDef '}' {$$ = $2;}
        | '{' rowSeperator cellDef '}' {$$ = $3;}
        | '{' cellDef rowSeperator '}' {$$ = $2;}
        | '{' rowSeperator cellDef rowSeperator '}' {$$ = $3;}
	| '{' '}' {$$ = new AST(OP_EMPTY_CELL,NULL);}
	| '{' error {yyxpt("a cell-array definition followed by a right brace");}
        ;

symbRefList:
	IDENT 
        | symbRefList symbRef {$$ = $1; $$->addChild($2);}
        ;
	
symbRef:
	REFLPAREN indexList REFRPAREN {$$ = new AST(OP_PARENS,$2); }
        | REFLBRACE indexList REFRBRACE {$$ = new AST(OP_BRACES,$2); }
        | FIELD {$$ = new AST(OP_DOT,$1); }
        ;

indexElement:
	expr
	| ':' {$$ = new AST(OP_ALL,NULL);}
	| '/' IDENT '=' expr {$$ = new AST(OP_KEYWORD,$2,$4);}
	| '/' IDENT {$$ = new AST(OP_KEYWORD,$2);}
        ;

indexList:
	indexElement | 
        indexList ',' indexElement {$$ = $1; $$->addPeer($3);}
        ;

cellDef:
       rowDef {$$ = new AST(OP_BRACES,$1);}|
       cellDef rowSeperator rowDef {$$ = $1; $$->addChild($3);}
       ;

matrixDef:
       rowDef {$$ = new AST(OP_BRACKETS,$1);}|
       matrixDef rowSeperator rowDef {$$ = $1; $$->addChild($3);} 
       ;

rowSeperator:
       ENDSTMNT | ENDQSTMNT
       ;

columnSep:
       ',' |
       ;

rowDef:
       expr {$$ = new AST(OP_SEMICOLON,$1);} |
       rowDef  columnSep expr {$$ = $1; $$->addChild($3);}
       ;

%%

namespace FreeMat {
  
  void resetParser() {
    mainAST = NULL;
    mainMDef = NULL;
    errorOccured = false;
  }
  
  ASTPtr getParsedScriptBlock() {
    return mainAST;
  }
  
  MFunctionDef* getParsedFunctionDef() {
    return mainMDef;
  }
  
  ParserState parseState() {
    if (errorOccured)
      return ParseError;
    if (mainAST != NULL) 
      return ScriptBlock;
    else
      return FuncDef;
  }
  
  ParserState parseString(char *txt) {
/*     yydebug = 1; 	*/
    resetParser();
    interactiveMode = true;
    yyexpect("a valid list of statements");
    setLexBuffer(txt);
    yyparse();
    return parseState();
  }
  
  ParserState parseFile(FILE *fp, const char* fname) {
    resetParser();
    interactiveMode = false;
    filename = fname;
    setLexFile(fp);
    yyexpect("a valid function definition or script");
    yyparse();
    return parseState();
  }
  
}
