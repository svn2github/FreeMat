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
  int yyerror(char *s) {
    char *tokdesc;
    char *tokbuffer = "unprintable";
    char buffer[256];
    if (*yytext < 33) {
	tokdesc = tokbuffer;
	sprintf(buffer,"unprintable char '%d'",*yytext);
	tokdesc = buffer;
    } else
	tokdesc = yytext;
    if (expectString)
      if (!interactiveMode)
        snprintf(msgBuffer,MSGBUFLEN,"Expecting %s at line %d of file %s.  Current token is '%s'",
	expectString,lineNumber,filename,tokdesc);
      else
        snprintf(msgBuffer,MSGBUFLEN,"Expecting %s.  Current token is '%s'",expectString, tokdesc);
    else
      if (!interactiveMode)
        snprintf(msgBuffer,MSGBUFLEN,"Syntax error at line %d of file %s.  Current token is '%s'",
	lineNumber,filename,tokdesc);
      else
        snprintf(msgBuffer,MSGBUFLEN,"Syntax error at input.  Current token is '%s'",tokdesc);
    throw Exception(msgBuffer);
    return 0;
  }
}

using namespace FreeMat;
#define yyerror yyerror

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
   FUNCTION  returnDeclaration IDENT REFLPAREN argumentList REFRPAREN {yyexpect("list of statements");}
   statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = $2->toStringList();
     r->name = $3->text;
     r->arguments = $5->toStringList();
     r->code = $8;
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   } |
   FUNCTION  IDENT REFLPAREN argumentList REFRPAREN {yyexpect("list of statements");} statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = $2->text;
     r->arguments = $4->toStringList();
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
   FUNCTION  returnDeclaration IDENT {yyexpect("list of statements");} statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = $2->toStringList();
     r->name = $3->text;
     r->code = $5;
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   } |
   FUNCTION  IDENT {yyexpect("list of statements");} statementList {
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
   FUNCTION  returnDeclaration IDENT REFLPAREN REFRPAREN {yyexpect("list of statements");} statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = $2->toStringList();
     r->name = $3->text;
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
   FUNCTION  IDENT REFLPAREN REFRPAREN {yyexpect("list of statements");} statementList {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = $2->text;
     r->code = $5;
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   } 
   ;

functionDefList:
  functionDef |
  functionDefList functionDef
  ;

returnDeclaration:
  VARARGOUT '=' {$$ = $1;} |
  IDENT '=' {$$ = $1;} | 
  '[' argumentList ']' '=' {$$ = $2;}
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
	 | QUIT | RETALL
         ;

specialSyntaxStatement:
	SPECIALCALL stringList {$1->addChild($2); $$ = new AST(OP_SCALL,$1);}
	;

stringList:
	STRING {$$ = new AST(OP_PARENS,$1);} |
	stringList STRING {$$ = $1; $$->addChild($2);}
	;

persistentStatement:
	PERSISTENT {yyexpect("list of identifiers (to be tagged as persistent)");}
	identList {$$ = $1; $$->addChild($3); }
 	;

globalStatement:
	GLOBAL {yyexpect("list of identifiers (to be tagged as global)");}
	identList {$$ = $1; $$->addChild($3);}
	;

identList:
	IDENT | 
	identList IDENT {$$ = $1; $$->addChild($2);}
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
	TRY {yyexpect("list of statements");} statementList optionalCatch ENDTRY {
	  $$ = $1; $$->addChild($3); if ($4 != NULL) $$->addChild($4);
	}
        ;

optionalCatch:
	CATCH {yyexpect("list of statements");} statementList {$$ = $3;}
	| {$$ = NULL;}
	;

switchStatement:
	SWITCH {yyexpect("test expression for switch");}
	expr optionalEndStatement caseBlock otherwiseClause ENDSWITCH {
	  $$ = $1; $$->addChild($3); 
	  if ($5 != NULL) $$->addChild($5); 
	  if ($6 != NULL) $$->addChild($6);
	}
        ;

optionalEndStatement:
	ENDSTMNT | ENDQSTMNT |
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
	CASE {yyexpect("case test expression");}
	expr {yyexpect("list of statements");}
	statementList {
	  $$ = $1; $$->addChild($3); $$->addChild($5);
	}
;

otherwiseClause:
	OTHERWISE {yyexpect("list of statements");}
	statementList {
	  $$ = $3;
	} |
	/* empty */ {
	  $$ = NULL;
	}
;

forStatement:
	FOR {yyexpect("for index expression id=expr or (id=expr) or id");} 
	forIndexExpression {yyexpect("list of statements");}
	statementList {yyexpect("matching 'end' statement to 'for'");}
        ENDFOR {
	  $$ = $1; $$->addChild($3); $$->addChild($5);
	}
;

forIndexExpression:
	'(' {yyexpect("identifier (the loop control variable)");}
	IDENT {yyexpect("'=' followed by an expression");}
        '=' {yyexpect("expression");}
	expr {yyexpect("matching ')'");}
        ')' {$$ = $3; $$->addChild($7);} |
	IDENT {yyexpect("'=' followed by an expression");}
	'=' {yyexpect("expression");}
	expr {$$ = $1; $$->addChild($5);} |
	IDENT {$$ = $1; $$->addChild(new AST(OP_RHS, new AST(id_node,$1->text))); }
;

whileStatement:
	WHILE {yyexpect("while test expression");}
	expr {yyexpect("list of statements");}
	statementList {yyexpect("matching 'end' statement to 'while'");}
	ENDWHILE {
	  $$ = $1; $$->addChild($3); $$->addChild($5);
	}
;

ifStatement:
	IF {yyexpect("test condition");} conditionedStatement 
	elseIfBlock elseStatement ENDIF {
	  $$ = $1; $$->addChild($3); if ($4 != NULL) $$->addChild($4); 
	  if ($5 != NULL) $$->addChild($5);
	} 
;

conditionedStatement:
	expr {yyexpect("list of statements");}
	statementList {
	  $$ = new AST(OP_CSTAT,$1,$3);
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
	ELSEIF {yyexpect("test condition");}
	conditionedStatement {
	  $$ = $3;
	} ;

elseStatement:
	ELSE {yyexpect("list of statements");}
	statementList {
	  $$ = $3;
	} |
	/* empty */ {$$ = NULL;}
;

assignmentStatement:
	symbRefList '=' {yyexpect("expression after '='");}
	expr       {$$ = new AST(OP_ASSIGN,$1,$4);}
        ;

multiFunctionCall:
	'[' matrixDef ']' '='  IDENT REFLPAREN  indexList REFRPAREN {
	  $5->addChild(new AST(OP_PARENS,$7));
	  $$ = new AST(OP_MULTICALL,$2,$5);
	} |
	'[' matrixDef ']' '=' IDENT  {
	  $5->addChild(new AST(OP_PARENS,NULL));
	  $$ = new AST(OP_MULTICALL,$2,$5);
	}
        ; 

expr:
	expr ':' {yyexpect("an expression after ':'");} expr {$$ = new AST(OP_COLON,$1,$4);}
	| terminal
	| expr '+' {yyexpect("an expression after '+'");} expr {$$ = new AST(OP_PLUS,$1,$4);}
	| expr '-' {yyexpect("an expression after '-'");} expr {$$ = new AST(OP_SUBTRACT,$1,$4);}
	| expr '*' {yyexpect("an expression after '*'");} expr {$$ = new AST(OP_TIMES,$1,$4);}
	| expr '/' {yyexpect("an expression after '/'");} expr {$$ = new AST(OP_RDIV,$1,$4);}
	| expr '\\' {yyexpect("an expression after '\\'");} expr {$$ = new AST(OP_LDIV,$1,$4);}
	| expr '|' {yyexpect("an expression after '|'");} expr {$$ = new AST(OP_OR,$1,$4);}
	| expr '&' {yyexpect("an expression after '&'");} expr {$$ = new AST(OP_AND,$1,$4);}
	| expr '<' {yyexpect("an expression after '<'");} expr {$$ = new AST(OP_LT,$1,$4);}
	| expr LE  {yyexpect("an expression after '<='");} expr {$$ = new AST(OP_LEQ,$1,$4);}
	| expr '>' {yyexpect("an expression after '>'");} expr {$$ = new AST(OP_GT,$1,$4);}
	| expr GE  {yyexpect("an expression after '>='");} expr {$$ = new AST(OP_GEQ,$1,$4);}
	| expr EQ  {yyexpect("an expression after '=='");} expr {$$ = new AST(OP_EQ,$1,$4);}
	| expr NE  {yyexpect("an expression after '~='");} expr {$$ = new AST(OP_NEQ,$1,$4);}
	| expr DOTTIMES {yyexpect("an expression after '.*'");} expr {$$ = new AST(OP_DOT_TIMES,$1,$4);}
	| expr DOTRDIV {yyexpect("an expression after './'");} expr {$$ = new AST(OP_DOT_RDIV,$1,$4);}
	| expr DOTLDIV {yyexpect("an expression after '.\\'");} expr {$$ = new AST(OP_DOT_LDIV,$1,$4);}
	| NEG {yyexpect("an expression after '-'");} expr %prec NEG {$$ = new AST(OP_NEG,$3);}
	| POS {yyexpect("an expression after '+'");} expr %prec POS {$$ = $3;}
	| '~' {yyexpect("an expression after '~'");} expr %prec NOT {$$ = new AST(OP_NOT,$3);}
	| expr '^' {yyexpect("an expression after '^'");} expr  {$$ = new AST(OP_POWER,$1,$4);}
	| expr DOTPOWER {yyexpect("an expression after '.^'");} expr {$$ = new AST(OP_DOT_POWER,$1,$4);}
	| expr '\''          {$$ = new AST(OP_TRANSPOSE,$1);}
	| expr DOTTRANSPOSE  {$$ = new AST(OP_DOT_TRANSPOSE,$1);}
	| '(' {yyexpect("an expression after '('");} expr {yyexpect("right parenthesis ')'");} ')'  
		{$$ = $3;}
	;

terminal:
        NUMERIC
	| STRING
	| END
	| symbRefList {$$ = new AST(OP_RHS,$1);}
        | '[' matrixDef ']' {$$ = $2;}
        | '[' rowSeperator matrixDef ']' {$$ = $3;}
        | '[' matrixDef rowSeperator ']' {$$ = $2;}
        | '[' rowSeperator matrixDef rowSeperator ']' {$$ = $3;}
	| '[' ']' {$$ = new AST(OP_EMPTY,NULL);}
        | '{' cellDef '}' {$$ = $2;}
        | '{' rowSeperator cellDef '}' {$$ = $3;}
        | '{' cellDef rowSeperator '}' {$$ = $2;}
        | '{' rowSeperator cellDef rowSeperator '}' {$$ = $3;}
	| '{' '}' {$$ = new AST(OP_EMPTY_CELL,NULL);}
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
