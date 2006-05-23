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
// Comment in ASU - "unary if following parenthesis, =, comma, operator"
// Not quitetrue, as 
// [1 -2] would yield the wrong answer.

// Issues: ":", transposes



//
//%token IDENT
//%token NUMERIC ENDQSTMNT ENDSTMNT
//%token LE GE EQ
//%token DOTTIMES DOTRDIV DOTLDIV
//%token DOTPOWER DOTTRANSPOSE
//%token STRING SPECIALCALL
//%token END IF FUNCTION FOR BREAK
//%token MAGICEND
//%token WHILE ELSE ELSEIF
//%token SWITCH CASE OTHERWISE CONTINUE
//%token TRY CATCH
//%token FIELD REFLPAREN REFRPAREN
//%token PERSISTENT
//%token KEYBOARD RETURN VARARGIN VARARGOUT GLOBAL
//%token QUIT RETALL
//
//
//%left SOR
//%left SAND
//%left '|'
//%left '&'
//%left '<' LE '>' GE EQ NE
//%left ':'
//%left '+' '-'
//%left '*' '/' '\\' DOTTIMES DOTRDIV DOTLDIV
//%left POS NEG NOT
//%right '^' DOTPOWER
//%nonassoc '\'' DOTTRANSPOSE
//
//%%
//
//program:
//  statementList {mainAST = $1.v.p;} |
//  functionDefList |
//  | error {yyxpt("statement list or function definition",$1);}
//;
//
//functionDef:
//   FUNCTION  returnDeclaration IDENT '(' argumentList ')' newLine statementList {
//     MFunctionDef *r;
//     r = new MFunctionDef();
//     r->returnVals = $2.v.p->toStringList();
//     r->name = $3.v.p->text;
//     r->arguments = $5.v.p->toStringList();
//     r->code = $8.v.p;
//     r->fileName = parsing_filename;
//     chainFunction(r);
//   } |
//   FUNCTION  IDENT '(' argumentList ')' newLine statementList {
//     MFunctionDef *r;
//     r = new MFunctionDef();
//     r->name = $2.v.p->text;
//     r->arguments = $4.v.p->toStringList();
//     r->code = $7.v.p;
//     r->fileName = parsing_filename;
//     chainFunction(r);
//   } |
//   FUNCTION  returnDeclaration IDENT newLine statementList {
//     MFunctionDef *r;
//     r = new MFunctionDef();
//     r->returnVals = $2.v.p->toStringList();
//     r->name = $3.v.p->text;
//     r->code = $5.v.p;
//     r->fileName = parsing_filename;
//     chainFunction(r);
//   } |
//   FUNCTION  IDENT newLine statementList {
//     MFunctionDef *r;
//     r = new MFunctionDef();
//     r->name = $2.v.p->text;
//     r->code = $4.v.p;
//     r->fileName = parsing_filename;
//     chainFunction(r);
//   } |
//   FUNCTION  returnDeclaration IDENT '(' ')' newLine statementList {
//     MFunctionDef *r;
//     r = new MFunctionDef();
//     r->returnVals = $2.v.p->toStringList();
//     r->name = $3.v.p->text;
//     r->code = $7.v.p;
//     r->fileName = parsing_filename;
//     chainFunction(r);
//   } |
//   FUNCTION  IDENT '(' ')' newLine statementList {
//     MFunctionDef *r;
//     r = new MFunctionDef();
//     r->name = $2.v.p->text;
//     r->code = $6.v.p;
//     r->fileName = parsing_filename;
//     chainFunction(r);
//   } |
//   FUNCTION  error {yyxpt("legal function name or return declaration after 'function'",$1);} |
//   FUNCTION  IDENT error {yyxpt(std::string("argument list or statement list after identifier '") + 
//	$2.v.p->text + "'",$2);} |
//   FUNCTION  IDENT '('  error {yyxpt("(possibly empty) argument list after '('",$3);}  |
//   FUNCTION  IDENT '(' argumentList ')' error {yyxpt("statement list after ')'",$5);} |
//   FUNCTION  returnDeclaration  error {yyxpt("function name for function declared",$1);} |
//   FUNCTION  returnDeclaration IDENT error {yyxpt(std::string("argument list or statement list following function name :") + 
//	$3.v.p->text,$3);} |
//   FUNCTION  returnDeclaration IDENT '('  error {yyxpt("(possibly empty) argument list after '('",$4);} |
//   FUNCTION  returnDeclaration IDENT '(' argumentList ')' error {yyxpt("statement list after ')'",$6);} 
//  ;
//
//functionDefList:
//  functionDef |
//  functionDefList functionDef
//  ;
//
//returnDeclaration:
//  VARARGOUT '=' {$$.v.p = $1.v.p;} |
//  IDENT '=' {$$.v.p = $1.v.p;} | 
//  '[' argumentList ']' '=' {$$.v.p = $2.v.p;} |
//  IDENT error {yyxpt("an '=' symbol after identifier in return declaration",$1);} |
//  '[' error {yyxpt("a valid list of return arguments in return declaration",$1);} | 
//  '[' argumentList error {yyxpt("matching ']' in return declaration for '['",$1);} |
//  '[' argumentList ']' error {yyxpt("an '=' symbol after return declaration",$3);}
//  ;
//
//argumentList:
//  argument {$$.v.p = $1.v.p;}
//  | argumentList columnSep argument {$$.v.p = $1.v.p; $$.v.p->addChild($3.v.p);}
//  ;
//
//argument:
//  IDENT | '&' IDENT {
//	$$.v.p = $2.v.p;
//	char *b = (char*) malloc(strlen($2.v.p->text)+2);
//	b[0] = '&';
//	strcpy(b+1,$2.v.p->text);
//	$$.v.p->text = b;
//  }
//  ;
//  
//
//statementList:
//   statement {$$.v.p = new AST(OP_BLOCK,$1.v.p,$1.v.p->context());} | 
//   statementList statement {$$.v.p = $1.v.p; $$.v.p->addChild($2.v.p);} 
//   ;
//
//statement:
//	 statementType ENDQSTMNT {
//  	    $$.v.p = new AST(OP_QSTATEMENT,NULL,$2.v.i);
//	    $$.v.p->down = $1.v.p;
//	 }
//	 | statementType ENDSTMNT {
//	    $$.v.p = new AST(OP_RSTATEMENT,NULL,$2.v.i);
//            $$.v.p->down = $1.v.p;
//	 }
//	 | statementType ',' {
//	    $$.v.p = new AST(OP_RSTATEMENT,NULL,$2.v.i);
//	    $$.v.p->down = $1.v.p;
//	 }
//	 ;
//
//         | multiFunctionCall
//	 | specialCall

//
//specialCall:
//	IDENT NUMERIC {$$.v.p = new AST(OP_SCALL,$1.v.p,$2.v.p,$1.v.p->context());} |
//	STRING STRING {$$.v.p = new AST(OP_SCALL,$1.v.p,$2.v.p,$1.v.p->context());} |
//	IDENT STRING {$$.v.p = new AST(OP_SCALL,$1.v.p,$2.v.p,$1.v.p->context());} |
//	IDENT IDENT {$$.v.p = new AST(OP_SCALL,$1.v.p,$2.v.p,$1.v.p->context()); } |
//	specialCall STRING {$1.v.p->addChild($2.v.p);} |
//	specialCall IDENT {$1.v.p->addChild($2.v.p);} |
//	specialCall NUMERIC {$1.v.p->addChild($2.v.p);}
//        ;
//
////specialSyntaxStatement:
////	SPECIALCALL stringList {$1.v.p->addChild($2.v.p); 
////				$$.v.p = new AST(OP_SCALL,$1.v.p,$1.v.p->context());}
////	;
//
////	STRING STRING {$$.v.p = new AST(OP_PARENS,$1.v.p,$1.v.p->context());} |
////	stringList STRING {$$.v.p = $1.v.p; $$.v.p->addChild($2.v.p);}
////	;
//
//persistentStatement:
//	PERSISTENT identList {$$.v.p = $1.v.p; $$.v.p->addChild($2.v.p); } |
//	PERSISTENT error {yyxpt("list of variables to be tagged as persistent",$1);}
// 	;
//
//globalStatement:
//	GLOBAL identList {$$.v.p = $1.v.p; $$.v.p->addChild($2.v.p);} |
//	GLOBAL error {yyxpt("list of variables to be tagged as global",$1);}
//	;
//
//identList:
//	IDENT | 
//	identList IDENT {$$.v.p = $1.v.p; $$.v.p->addChild($2.v.p);} |
//	identList error {yyxpt("list of valid identifiers",$2);}
//	;
//
//returnStatement:
//	RETURN
//	;
//
//keyboardStatement:
//	KEYBOARD
//	;
//
//continueStatement:
//	 CONTINUE
//	 ;
//breakStatement:
//	 BREAK
//	 ;
//
//tryStatement:
//	TRY  statementList optionalCatch END 
//	{ $$.v.p = $1.v.p; $$.v.p->addChild($2.v.p); if ($3.v.p != NULL) $$.v.p->addChild($3.v.p);} 
//	| TRY statementList optionalCatch error 
//	{yyxpt(std::string("matching 'end' to 'try' clause from line ") + decodeline($1),$4);}
//        ;
//
//optionalCatch:
//	CATCH statementList {$$.v.p = $2.v.p;}
//	| {$$.v.p = NULL;}
//	;
//
//switchStatement:
//	SWITCH 	expr optionalEndStatement caseBlock otherwiseClause END {
//	  $$.v.p = $1.v.p; $$.v.p->addChild($2.v.p); 
//	  if ($4.v.p != NULL) $$.v.p->addChild($4.v.p); 
//	  if ($5.v.p != NULL) $$.v.p->addChild($5.v.p);
//	} |
//	SWITCH 	expr optionalEndStatement caseBlock otherwiseClause error {
//          yyxpt(std::string("matching 'end' to 'switch' clause from line ") + decodeline($1),$6);
//        } 
//        ;
//
//optionalEndStatement:
//	',' | ENDSTMNT | ENDQSTMNT | ';' | '#'
//	;
//
//newLine:
//	ENDSTMNT | ENDQSTMNT
//	;
//
//
//caseBlock:
//	/* empty */ {$$.v.p = NULL;} |
//	caseList
//	;
//
//caseList:
//	caseStatement {
//	  $$.v.p = new AST(OP_CASEBLOCK,$1.v.p,$1.v.p->context());
//	} |
//	caseList caseStatement {
//	  $$.v.p = $1.v.p; $$.v.p->addChild($2.v.p);
//	}
//;
//
//caseStatement:
//	CASE expr optionalEndStatement statementList {
//	  $$.v.p = $1.v.p; $$.v.p->addChild($2.v.p); $$.v.p->addChild($4.v.p);
//	}
//;
//
//otherwiseClause:
//	OTHERWISE statementList {
//	  $$.v.p = $2.v.p;
//	} |
//	/* empty */ {
//	  $$.v.p = NULL;
//	}
//;
//
//forStatement:
//	FOR forIndexExpression optionalEndStatement statementList END {
//	  $$.v.p = $1.v.p; $$.v.p->addChild($2.v.p); $$.v.p->addChild($4.v.p);
//	} |
//	FOR forIndexExpression optionalEndStatement statementList error 
//           {yyxpt(std::string("'end' to match 'for' statement from line ") + decodeline($1),$5);}
//;
//
//forIndexExpression:
//	'(' IDENT '=' expr ')' {$$.v.p = $2.v.p; $$.v.p->addChild($4.v.p);} |
//	IDENT '=' expr {$$.v.p = $1.v.p; $$.v.p->addChild($3.v.p);} |
//	IDENT {$$.v.p = $1.v.p; 
//	      $$.v.p->addChild(new AST(OP_RHS,new AST(id_node,$1.v.p->text,$1.v.p->context()),$1.v.p->context())); } |
//	'(' IDENT '=' expr error {yyxpt("matching right parenthesis",$1);} |
//	'(' IDENT '=' error {yyxpt("indexing expression",$3);} |
//	'(' IDENT error {yyxpt("equals operator after loop index",$2);} |
//	'(' error {yyxpt("identifier that is the loop variable",$1);} |
//	IDENT '=' error {yyxpt("indexing expression",$2);} |
//	error {yyxpt("identifier or assignment (id = expr) after 'for' ",$1);} 
//;
//
//whileStatement:
//	WHILE expr optionalEndStatement statementList END {
//	  $$.v.p = $1.v.p; $$.v.p->addChild($2.v.p); $$.v.p->addChild($4.v.p);
//	}  |
//	WHILE error {yyxpt("test expression after 'while'",$1);} |
//	WHILE expr optionalEndStatement statementList error 
//	{yyxpt(std::string("'end' to match 'while' statement from line ") + decodeline($1),$5);}
//;
//
//ifStatement:
//	IF conditionedStatement elseIfBlock elseStatement END {
//	  $$.v.p = $1.v.p; $$.v.p->addChild($2.v.p); if ($3.v.p != NULL) $$.v.p->addChild($3.v.p); 
//	  if ($4.v.p != NULL) $$.v.p->addChild($4.v.p);
//	} |
//	IF error {yyxpt("condition expression for 'if'",$1);} |
//	IF conditionedStatement elseIfBlock elseStatement error {yyxpt(std::string("'end' to match 'if' statement from line ") + decodeline($1),$5);}
//;
//
//conditionedStatement:
//	expr optionalEndStatement statementList {
//	  $$.v.p = new AST(OP_CSTAT,$1.v.p,$3.v.p,$1.v.p->context());
//	} |
//	expr optionalEndStatement {
//	  $$.v.p = new AST(OP_CSTAT,$1.v.p,$1.v.p->context());
//	} |
//	expr error {yyxpt("valid list of statements after condition",$2);}
//;
//
//elseIfBlock:
//	/* empty */ {$$.v.p = NULL;} |
//	elseIfStatementList
//	;
//
//elseIfStatementList:
//	elseIfStatement {
//	  $$.v.p = new AST(OP_ELSEIFBLOCK,$1.v.p,$1.v.p->context());
//	} |
//	elseIfStatementList elseIfStatement {
//	  $$.v.p = $1.v.p; $$.v.p->addChild($2.v.p);
//	}
//;
//
//elseIfStatement:
//	ELSEIF conditionedStatement {
//	  $$.v.p = $2.v.p;
//	} |
//	ELSEIF error {yyxpt("test condition for 'elseif' clause",$1);};
//
//elseStatement:
//	ELSE statementList {
//	  $$.v.p = $2.v.p;
//	} |
//	/* empty */ {$$.v.p = NULL;} |
//	ELSE error {yyxpt("statement list for 'else' clause",$1);}
//;
//
//assignmentStatement:
//	symbRefList '=' expr {$$.v.p = new AST(OP_ASSIGN,$1.v.p,$3.v.p,$2.v.i);} |
//	symbRefList '=' error {yyxpt("expression in assignment",$2);}
//        ;
//
//multiFunctionCall:
//	'[' matrixDef ']' '='  IDENT '('  indexList ')' {
//	  $5.v.p->addChild(new AST(OP_PARENS,$7.v.p,$6.v.i));
//	  $$.v.p = new AST(OP_MULTICALL,$2.v.p,$5.v.p,$1.v.i);
//	} |
//	'[' matrixDef ']' '=' IDENT  {
//	  $5.v.p->addChild(new AST(OP_PARENS,NULL,-1));
//	  $$.v.p = new AST(OP_MULTICALL,$2.v.p,$5.v.p,$1.v.i);
//	} |
//	'[' matrixDef ']' '='  IDENT '('  indexList error 
//	{yyxpt("matching right parenthesis",$6);} |
//	'[' matrixDef ']' '='  IDENT '('  error 
//	{yyxpt("indexing list",$6);} |
//	'[' matrixDef ']' '='  IDENT error 
//	{yyxpt("left parenthesis",$5);} |	
//	'[' matrixDef ']' '=' error 
//	{yyxpt("identifier",$4);}
//        ; 
//
//expr:
//	expr ':' expr {$$.v.p = new AST(OP_COLON,$1.v.p,$3.v.p,$2.v.i);} 
//	| expr ':' error {yyxpt("an expression after ':'",$2);}
//	| terminal
//	| expr '+' expr {$$.v.p = new AST(OP_PLUS,$1.v.p,$3.v.p,$2.v.i);}
//	| expr '+' error {yyxpt("an expression after '+'",$2);}
//	| expr '-' expr {$$.v.p = new AST(OP_SUBTRACT,$1.v.p,$3.v.p,$2.v.i);}
//	| expr '-' error {yyxpt("an expression after '-'",$2);}
//	| expr '*' expr {$$.v.p = new AST(OP_TIMES,$1.v.p,$3.v.p,$2.v.i);}
//	| expr '*' error {yyxpt("an expression after '*'",$2);}
//	| expr '/' expr {$$.v.p = new AST(OP_RDIV,$1.v.p,$3.v.p,$2.v.i);}
//	| expr '/' error {yyxpt("an expression after '/'",$2);}
//	| expr '\\' expr {$$.v.p = new AST(OP_LDIV,$1.v.p,$3.v.p,$2.v.i);}
//	| expr '\\' error {yyxpt("an expression after '\\'",$2);}
//	| expr '|' expr {$$.v.p = new AST(OP_OR,$1.v.p,$3.v.p,$2.v.i);}
//	| expr '|' error {yyxpt("an expression after '|'",$2);}
//	| expr '&' expr {$$.v.p = new AST(OP_AND,$1.v.p,$3.v.p,$2.v.i);}
//	| expr '&' error {yyxpt("an expression after '&'",$2);}
//	| expr SOR expr {$$.v.p = new AST(OP_SOR,$1.v.p,$3.v.p,$2.v.i);}
//	| expr SOR error {yyxpt("an expression after '||'",$2);}
//	| expr SAND expr {$$.v.p = new AST(OP_SAND,$1.v.p,$3.v.p,$2.v.i);}
//	| expr SAND error {yyxpt("an expression after '&&'",$2);}
//	| expr '<' expr {$$.v.p = new AST(OP_LT,$1.v.p,$3.v.p,$2.v.i);}
//	| expr '<' error {yyxpt("an expression after '<'",$2);}
//	| expr LE  expr {$$.v.p = new AST(OP_LEQ,$1.v.p,$3.v.p,$2.v.i);}
//	| expr LE  error {yyxpt("an expression after '<='",$2);}
//	| expr '>' expr {$$.v.p = new AST(OP_GT,$1.v.p,$3.v.p,$2.v.i);}
//	| expr '>' error {yyxpt("an expression after '>'",$2);}
//	| expr GE  expr {$$.v.p = new AST(OP_GEQ,$1.v.p,$3.v.p,$2.v.i);}
//	| expr GE  error {yyxpt("an expression after '>='",$2);}
//	| expr EQ  expr {$$.v.p = new AST(OP_EQ,$1.v.p,$3.v.p,$2.v.i);}
//	| expr EQ  error {yyxpt("an expression after '=='",$2);}
//	| expr NE  expr {$$.v.p = new AST(OP_NEQ,$1.v.p,$3.v.p,$2.v.i);}
//	| expr NE  error {yyxpt("an expression after '~='",$2);}
//	| expr DOTTIMES expr {$$.v.p = new AST(OP_DOT_TIMES,$1.v.p,$3.v.p,$2.v.i);}
//	| expr DOTTIMES error {yyxpt("an expression after '.*'",$2);}
//	| expr DOTRDIV  expr {$$.v.p = new AST(OP_DOT_RDIV,$1.v.p,$3.v.p,$2.v.i);}
//	| expr DOTRDIV  error {yyxpt("an expression after './'",$2);}
//	| expr DOTLDIV expr {$$.v.p = new AST(OP_DOT_LDIV,$1.v.p,$3.v.p,$2.v.i);}
//	| expr DOTLDIV error {yyxpt("an expression after '.\\'",$2);}
//	| '-'  expr %prec NEG {$$.v.p = new AST(OP_NEG,$2.v.p,$1.v.i);}
//	| '+'  expr %prec POS {$$.v.p = $2.v.p;}
//	| '~' expr %prec NOT {$$.v.p = new AST(OP_NOT,$2.v.p,$1.v.i);}
//	| '~' error %prec NOT {yyxpt("an expression after logical not",$2);}
//	| expr '^' expr  {$$.v.p = new AST(OP_POWER,$1.v.p,$3.v.p,$2.v.i);}
//	| expr '^' error  {yyxpt("an expression after '^'",$2);}
//	| expr DOTPOWER expr {$$.v.p = new AST(OP_DOT_POWER,$1.v.p,$3.v.p,$2.v.i);}
//	| expr DOTPOWER error {yyxpt("an expression after '.^'",$2);}
//	| expr '\''          {$$.v.p = new AST(OP_TRANSPOSE,$1.v.p,$2.v.i);}
//	| expr DOTTRANSPOSE  {$$.v.p = new AST(OP_DOT_TRANSPOSE,$1.v.p,$2.v.i);}
//	| '(' expr ')'  {$$.v.p = $2.v.p;}
//	| '(' expr error  {yyxpt("a right parenthesis after expression to match this one",$1);}
//	| '(' error  {yyxpt("an expression after left parenthesis",$1);}
//	;
//
//terminal:
//        NUMERIC
//	| STRING
//	| MAGICEND
//	| '@' IDENT {$$.v.p = new AST(OP_ADDRESS,$2.v.p,$1.v.i);}
//	| symbRefList {$$.v.p = new AST(OP_RHS,$1.v.p,$1.v.p->context());}
//        | '[' matrixDef ']' {$$.v.p = $2.v.p;}
//        | '[' error {yyxpt("a matrix definition followed by a right bracket",$1);}
//        | '[' rowSeperator matrixDef ']' {$$.v.p = $3.v.p;}
//        | '[' matrixDef rowSeperator ']' {$$.v.p = $2.v.p;}
//        | '[' rowSeperator matrixDef rowSeperator ']' {$$.v.p = $3.v.p;}
//	| '[' ']' {$$.v.p = new AST(OP_EMPTY,NULL,$1.v.i);}
//        | '{' cellDef '}' {$$.v.p = $2.v.p;}
//        | '{' rowSeperator cellDef '}' {$$.v.p = $3.v.p;}
//        | '{' cellDef rowSeperator '}' {$$.v.p = $2.v.p;}
//        | '{' rowSeperator cellDef rowSeperator '}' {$$.v.p = $3.v.p;}
//	| '{' '}' {$$.v.p = new AST(OP_EMPTY_CELL,NULL,$1.v.i);}
//	| '{' error {yyxpt("a cell-array definition followed by a right brace",$1);}
//        ;
//symbRefList:
//	IDENT 
//        | symbRefList symbRef {$$.v.p = $1.v.p; $$.v.p->addChild($2.v.p);}
//        ;
//	
//symbRef:
//	'(' indexList ')' {$$.v.p = new AST(OP_PARENS,$2.v.p,$1.v.i); }
//        | '(' ')' {$$.v.p = new AST(OP_PARENS,NULL,$1.v.i); }
//	| '(' indexList error {yyxpt("matching right parenthesis",$1);}
//        | '{' indexList '}' {$$.v.p = new AST(OP_BRACES,$2.v.p,$1.v.i); }
//	| '{' indexList error {yyxpt("matching right brace",$1);}
//        | '.' IDENT {$$.v.p = new AST(OP_DOT,$2.v.p,$1.v.i); }
//	| '.' '(' expr ')' {$$.v.p = new AST(OP_DOTDYN,$3.v.p,$1.v.i);}
//        ;
//
//indexElement:
//	expr
//	| ':' {$$.v.p = new AST(OP_ALL,NULL,$1.v.i);}
//	| '/' IDENT '=' expr {$$.v.p = new AST(OP_KEYWORD,$2.v.p,$4.v.p,$1.v.i);}
//	| '/' IDENT '=' error {yyxpt("expecting expression after '=' in keyword assignment",$3);}
//	| '/' IDENT {$$.v.p = new AST(OP_KEYWORD,$2.v.p,$1.v.i);}
//	| '/' error {yyxpt("expecting keyword identifier after '/' in keyword assignment",$1);}
//        ;
//
//indexList:
//	indexElement | 
//        indexList ',' indexElement {$$.v.p = $1.v.p; $$.v.p->addPeer($3.v.p);}
//        ;
//
//cellDef:
//       rowDef {$$.v.p = new AST(OP_BRACES,$1.v.p,$1.v.p->context());}|
//       cellDef rowSeperator rowDef {$$.v.p = $1.v.p; $$.v.p->addChild($3.v.p);}
//       ;
//
//matrixDef:
//       rowDef {$$.v.p = new AST(OP_BRACKETS,$1.v.p,$1.v.p->context());}|
//       matrixDef rowSeperator rowDef {$$.v.p = $1.v.p; $$.v.p->addChild($3.v.p);} 
//       ;
//
//rowSeperator:
//       ENDSTMNT | ENDQSTMNT
//       ;
//
//columnSep:
//       ',' | '#'
//       ;
//
//rowDef:
//       expr {$$.v.p = new AST(OP_SEMICOLON,$1.v.p,$1.v.p->context());} |
//       rowDef  columnSep expr {$$.v.p = $1.v.p; $$.v.p->addChild($3.v.p);}
//       ;
//
//%%
//
//namespace FreeMat {
//
//#ifdef YYBYACC  
//  int yyparse();
//#endif
//
//  void resetParser() {
//    mainAST = NULL;
//    mainMDef = NULL;
//  }
//  
//  ASTPtr getParsedScriptBlock() {
//    return mainAST;
//  }
//  
//  MFunctionDef* getParsedFunctionDef() {
//    return mainMDef;
//  }
//  
//  ParserState parseState() {
//    if (mainAST != NULL) 
//      return ScriptBlock;
//    else
//      return FuncDef;
//  }
//  
//  ParserState parseString(char *txt) {
///*     yydebug = 1; 	*/
//    resetParser();
//    interactiveMode = true;
//    setLexBuffer(txt);
//    yyparse();
//    return parseState();
//  }
//  
//  ParserState parseFile(FILE *fp, const char* fname) {
////    yydebug = 1; 
//    resetParser();
//    interactiveMode = false;
//    parsing_filename = fname;
//    setLexFile(fp);
//    yyparse();
//    return parseState();
//  }
//  
//}
//


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
  op_power,
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
  case op_power:
    return "^";
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

typedef int op_prec;

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
  void Dump(); 
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
  tree ParseMatrixDefinition();
  tree ParseRowVectorDefinition();
  tree ParseSpecialFunctionCall();
  tree ParseIndexList();
  tree Exp(op_prec p);
  tree PrimaryExpression();
  void FlushWhitespace();
  void CheckWSFlush();
};

void Parser::Dump() {
  char *txt = strdup(state.text);
  txt[state.ptr] = '*';
  txt[state.ptr+1] = 0;
  cout << txt << "\n";
}

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

tree Parser::ParseMultiFunctionCall() {
  tree root = mkLeaf("multi");
  addChild(root,ParseMatrixDefinition());
  flushws();
  expect("=");
  flushws();
  addChild(root,ParseIdentifier());
  addChild(root,ParseIndexList());
  return root;
}

tree Parser::ParseSpecialFunctionCall() {
  tree root = mkLeaf("special");
  addChild(root,ParseIdentifier());
  addChild(root,ParseIdentifier());
  return root;
}

tree Parser::ParseForStatement() {
  expect("for");
  flushws();
  //  tree index = ParseForIndexExpression();
  tree term = ParseStatementSeperator();
  tree block = ParseStatementList();
  flushws();
  expect("end");
  return mkNode("for",term,block);
}

tree Parser::ParseWhileStatement() {
  expect("while");
  flushws();
  tree warg = ParseExpression();
  cout << "warg:\n";
  warg->print();
  flushws();
  tree term = ParseStatementSeperator();
  flushws();
  tree block = ParseStatementList();
  cout << "block:\n";
  block->print();
  flushws();
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
  try {
    CheckpointParser();
    tree retval = ParseMultiFunctionCall();
    DeleteCheckpoint();
    return retval;
  } catch (string e) {
    RestoreCheckpoint();
  }
  try {
    CheckpointParser();
    tree retval = ParseSpecialFunctionCall();
    DeleteCheckpoint();
    return retval;
  } catch (string e) {
    RestoreCheckpoint();
  }
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
  //  state.operators.push(op_sentinel);
  //  Expression();
  //  return state.operands.top();
  return Exp(0);
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

op_prec prec(op a) {
  switch(a) {
  case op_sentinel: return 0;
  case op_and: return 1;
  case op_plus: return 3;
  case op_minus: return 3;
  case op_times: return 4;
  case op_divide: return 4;
  case op_power: return 5;
  case op_unary_plus: return 6;
  case op_unary_minus: return 6;
  }
  return 0;
}

bool is_right_associative(op a) {
  return (a == op_power);
}

bool is_binary_operator(tok a) {
  return ((a == '+') ||
	  (a == '-') ||
	  (a == '*') ||
	  (a == '/') ||
	  (a == '^'));
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
  if (a == '^') return op_power;
}

op unary(tok a) {
  if (a == '+') return op_unary_plus;
  if (a == '-') return op_unary_minus;
}

tree Parser::PrimaryExpression() {
  if (is_unary_operator(next())) {
    op opr = unary(next());
    consume();
    op_prec q = prec(opr);
    return mkNode(operatorname(opr),Exp(q));
  } else if (next() == '(') {
    consume();
    tree t = Exp(0);
    expect(')');
    return t;
  } else if (is_value(next())) {
    tree t = mkLeaf(next());
    consume();
    return t;
  } else if (match('[')) {
    consume();
    tree root = mkLeaf("[]");
    while (!match(']')) {
      addChild(root,ParseExpression());
      if (!match(']'))
	expect(',');
    }
    expect(']');
    return root;
  } else
    serror("unrecognized token");
}

tree Parser::Exp(op_prec p) {
  tree t = PrimaryExpression();
  while (is_binary_operator(next()) && (prec(binary(next())) >= p)) {
    op opr = binary(next());
    consume();
    op_prec q;
    if (is_right_associative(opr)) 
      q = prec(opr);
    else
      q = 1+prec(opr);
    tree t1 = Exp(q);
    t = mkNode(opr,t,t1);
  }
  return t;
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
    cout << "Somewhere here:\n";
    G.Dump();
  }
  return 0;
}
