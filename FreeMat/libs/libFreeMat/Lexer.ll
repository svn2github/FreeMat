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

#include "AST.hpp"
#define YYSTYPE FreeMat::ASTPtr	
#include "Parser.h"
extern YYSTYPE yylval;
#include "Reserved.hpp"
#include "Exception.hpp"
#include <stdlib.h>

using namespace FreeMat;

extern int lineNumber;

reservedWordStruct ts, *p;
 
  // These are the possible contexts in which end's can occur. 
  typedef enum {
    context_for,
    context_while,
    context_if,
    context_try,
    context_switch,
    context_index,
    context_matrix,
    context_cell,
    context_error
  } end_context;

  int stackCount;
  int stateStack[1000];
  int contextCount;
  end_context contextStack[1000]; 
  end_context thisContext;
  int rettoken;
  int lineNumber;
  int continuationCount;
  bool firstToken;
  bool minusisneg;
  
  void pushState(int state) {
    stateStack[stackCount] = state;
    stackCount++;
  }
  
  int popState() {
    if (stackCount < 1)
	return INITIAL;
    stackCount--;
    return stateStack[stackCount];
  }
  
  int topState() {
    return stateStack[stackCount-1];
  }
 
  void pushContext(end_context context) { 
    contextStack[contextCount] = context;
    contextCount++;
  }

  end_context popContext() {
    if (contextCount < 1) {
	return context_error;
    }
    contextCount--;
    return contextStack[contextCount];
  }

  end_context topContext() {
    return contextStack[contextCount-1];
  }

%}

Word            [a-zA-Z][a-zA-Z0-9_]*
Digit		[0-9]
Sign		([\+]|[\-])
FltSuffix	([f]|[F])
DblSuffix	([d]|[D])
SignedInt	{Sign}?{Digit}+
Expo		([e]|[E])
ExponentPart	{Expo}{SignedInt}?
Integer		{Digit}+
Float1		{Digit}+[\.]{Digit}+?{ExponentPart}?{FltSuffix}
Float2		[\.]{Digit}+{ExponentPart}?{FltSuffix}
Float3		{Digit}+{ExponentPart}{FltSuffix}
Float4		{Digit}+{FltSuffix}
Float5		{Digit}+[\.]{Digit}+?{ExponentPart}?{DblSuffix}?
Float6		[\.]{Digit}+{ExponentPart}?{DblSuffix}?
Float7		{Digit}+{ExponentPart}{DblSuffix}?
Float8		{Digit}+{DblSuffix}
FloatingPoint	({Float1}|{Float2}|{Float3}|{Float4})
DoubleFloatingPoint	({Float5}|{Float6}|{Float7}|{Float8})
Whitespace      [ \t\xFF]
Commentline     "%".*
Newline         ("\r\n"|\n)
String          [\'][^\'\n\r]*[\']
ArgDot          \.[^*\\/\^\'][^ \t;\n\r]*
ArgFSlash	\/[a-zA-Z\.\///][^ \t;\n\r]*
ArgRSlash	\\[a-zA-Z\.\///][^ \t;\n\r]*
ArgTilde	~[^=][^ \t;\n\r]*
Argument	(-?[a-zA-Z0-9][^ \t;\n\r]*)|{ArgDot}|{ArgFSlash}|{ArgRSlash}|{ArgTilde}
SpecialArgument ({Argument}|{String})

%x IdentDereference
%x TransposeCheck
%x Scanning
%x SpecialSyntax
%x SpecialSyntaxStart
%x SpecialSyntaxArgs
%x str
%%

	char string_buf[4095];
	char *string_buf_ptr;

{Commentline} {
}

{Newline} {
  lineNumber++;
}

{Whitespace}+ {
}

^{Whitespace}*"+" {
 return POS;
}

^{Whitespace}*"-" {
 return NEG;
}

";" {
}

. {
  yyless(0);
  BEGIN(SpecialSyntax);
}

<SpecialSyntax>{Word}({Whitespace}+{SpecialArgument})+";"?{Newline} {
  yyless(0);
  BEGIN(SpecialSyntaxStart);
}

<SpecialSyntax>. {
  yyless(0);
  BEGIN(Scanning);
}

<SpecialSyntaxStart>{Word} {
  /* Check for a reserved word... */
  ts.word = yytext;
  p = (reservedWordStruct*)
    bsearch(&ts,reservedWord,RESWORDCOUNT,
	    sizeof(reservedWordStruct),
	    compareReservedWord);
  if (p!=NULL) {
    yyless(0);
    BEGIN(Scanning);
  } else {
    yylval = new AST(id_node,yytext);
    BEGIN(SpecialSyntaxArgs);  
    return SPECIALCALL;
  }
}

<SpecialSyntaxArgs>{Whitespace}+ {
}

<SpecialSyntaxArgs>{Argument} {
  yylval = new AST(string_const_node,yytext);
  return STRING;
}

<SpecialSyntaxArgs>{String} {
  yytext[yyleng-1] = 0;
  yylval = new AST(string_const_node,yytext+1);
  return STRING;
}

<SpecialSyntaxArgs>";\n"  {
  lineNumber++;
  BEGIN(INITIAL);
  return ENDQSTMNT;
}

<SpecialSyntaxArgs>";"  {
  BEGIN(INITIAL);
  return ENDQSTMNT;
}

<SpecialSyntaxArgs>"\r\n"|\n {
  lineNumber++;
  BEGIN(INITIAL);
  return ENDSTMNT;
}

<Scanning>{Word}"(""-" {
  unput(0xFA);
/*  yyless(1); */
  int i;
  char *yycopy = strdup(yytext);
  for (i=yyleng-2;i>=0;--i)
    unput(yycopy[i]);
  free(yycopy);
}

<Scanning>{Word}"(""+" {
  unput(0xFB);
/*  yyless(1); */
  int i;
  char *yycopy = strdup(yytext);
  for (i=yyleng-2;i>=0;--i)
    unput(yycopy[i]);
  free(yycopy);
}

<Scanning>^[ \t\f]*"-" {
  return NEG;
}

<Scanning>^[ \t\f]*"+" {
  return POS;
}

<Scanning>[({[,+\-*/\\|&<>~^\xFA\xFB;=:][ \f\t]*"-" {
  // Replace the - with a NEG
  int i; 
  char *yycopy = strdup(yytext);
  unput(0xFA);
  for (i=yyleng-2;i>=0;--i)
    unput(yycopy[i]);
  free(yycopy);
}

<Scanning>[ \f\t]+"-"[a-zA-Z0-9"(""[""{""'"] {
  int i;
  char *yycopy = strdup(yytext);
  yycopy[yyleng-2] = 0xFA;
  for (i=yyleng-1;i>=0;--i)
    unput(yycopy[i]);
  free(yycopy);
}

<Scanning>[({[,+\-*/\\|&<>~^\xFA\xFB;=:][ \f\t]*"+" {
  // Replace the - with a NEG
  int i;
  char *yycopy = strdup(yytext);
  unput(0xFB);
  for (i=yyleng-2;i>=0;--i)
    unput(yycopy[i]);
  free(yycopy);
}

<Scanning>[ \f\t]+"+"[a-zA-Z0-9"(""[""{""'"] {
  int i;
  char *yycopy = strdup(yytext);
  yycopy[yyleng-2] = 0xFB;
  for (i=yyleng-1;i>=0;--i)
    unput(yycopy[i]);
  free(yycopy);
}

<Scanning>\xFA {
  return NEG;
}

<Scanning>\xFB {
  return POS;
}

<Scanning>"==" {
  return EQ;
}

<Scanning>".*" {			
  return DOTTIMES;
}

<Scanning>"./" {
  return DOTRDIV;
}

<Scanning>".\\" {
  return DOTLDIV;
}

<Scanning>"<=" {			
  return LE;
}

<Scanning>">=" {
  return GE;
}

<Scanning>"~=" {
  return NE;
}

<Scanning>".^" {
  return DOTPOWER;
}

<Scanning>".\'" {
  return DOTTRANSPOSE;
}

<Scanning>";\n"  {
  lineNumber++;
  firstToken = true;
  BEGIN(INITIAL);
  return ENDQSTMNT;
}

<Scanning>";"  {
  firstToken = true;
  BEGIN(INITIAL);
  return ENDQSTMNT;
}

<Scanning>"\r\n"|\n {
  lineNumber++;
  firstToken = true;
  BEGIN(INITIAL);
  return ENDSTMNT;
}

<Scanning>"(" {
  pushState(Scanning);
  pushContext(context_index);
  return '(';
}

<Scanning>"{" {
  pushState(Scanning);
  pushContext(context_index);
  return '{';
}

<Scanning>")" {
  popContext();
  if (topState() == Scanning) {
    BEGIN(TransposeCheck);
    return ')'; 
  } else {
    BEGIN(popState());
    return REFRPAREN;
  }
}

<Scanning>"}" {
  popContext();
  if (topState() == Scanning) {
    BEGIN(TransposeCheck);
    return '}';
  } else {
    BEGIN(popState());
    return REFRBRACE;
  }
}

<Scanning>"[" {
  pushContext(context_matrix);
  return '[';
}

<Scanning>"]" {
  popContext();
  pushState(Scanning);
  BEGIN(TransposeCheck);
  return ']';
}

<Scanning>[ \f\t] {
  /* skip */
}

<Scanning>"..."{Whitespace}*{Newline} {
  lineNumber++;
  continuationCount++;
  firstToken = false;
  BEGIN(Scanning);
}

<Scanning>\'  string_buf_ptr = string_buf; BEGIN(str);

<str>\' {
	BEGIN(Scanning); 
	*string_buf_ptr = '\0';
	yylval = new AST(string_const_node,string_buf);
	return STRING;
	}

<str>\\n *string_buf_ptr++ = 13;
<str>\\t *string_buf_ptr++ = 19;
<str>\\r *string_buf_ptr++ = 17;
<str>\\b *string_buf_ptr++ = 1;
<str>\\f *string_buf_ptr++ = 5;
<str>\\(.|\n)  *string_buf_ptr++ = yytext[1];

<str>[^\\\n\']+  {
                   char *yptr = yytext;
                   while ( *yptr )
                           *string_buf_ptr++ = *yptr++;
                 }

<Scanning>{Word} {
  /* Search for the identifier in the keyword table */
  ts.word = yytext;
  p = (reservedWordStruct*) 
    bsearch(&ts,reservedWord,RESWORDCOUNT,
	    sizeof(reservedWordStruct),
	    compareReservedWord);
  if (p==NULL) {
    BEGIN(IdentDereference);
    yylval = new AST(id_node,yytext); 
    return IDENT;
  } else {
    // Check for reserved words that trigger end-context changes
    rettoken = p->token;
    switch (p->token) {
    case SWITCH:
      pushContext(context_switch);
      break;
    case FOR:
      pushContext(context_for);
      break;
    case WHILE:
      pushContext(context_while);
      break;
    case TRY:
      pushContext(context_try);
      break;
    case IF:
      pushContext(context_if);
      break;
    case END:
      thisContext = popContext();
      switch (thisContext) {
      case context_for:
	rettoken = ENDFOR;
	break;
      case context_switch:
	rettoken = ENDSWITCH;
	break;
      case context_while:
	rettoken = ENDWHILE;
	break;
      case context_try:
	rettoken = ENDTRY;
	break;
      case context_if:
	rettoken = ENDIF;
	break;
      default:
	pushContext(thisContext);
	rettoken = END;
      }
    }
    yylval = new AST(reserved_node,p->ordinal);
    return rettoken;
  }
}

<IdentDereference>"."{Word} {
  yylval = new AST(id_node,yytext+1);
  return FIELD;
}

<IdentDereference>"(" {
  pushState(IdentDereference);
  pushContext(context_index);
  BEGIN(Scanning);
  return REFLPAREN;
}

<IdentDereference>"{" {
  pushState(IdentDereference);
  pushContext(context_index);
  BEGIN(Scanning);
  return REFLBRACE;
}

<IdentDereference>"'" {
  BEGIN(Scanning);
  return *yytext;
}

<IdentDereference>[^"'"] {
  unput(yytext[0]);
  BEGIN(Scanning);
}

<Scanning>{FloatingPoint} {
  pushState(YY_START);
  BEGIN(TransposeCheck);
  yylval = new AST(const_float_node,yytext);
  return NUMERIC;
}

<Scanning>{DoubleFloatingPoint} {
  pushState(YY_START);
  BEGIN(TransposeCheck);
  yylval = new AST(const_double_node,yytext);
  return NUMERIC;
}

<Scanning>{Integer} {
  pushState(YY_START);
  BEGIN(TransposeCheck);
  yylval = new AST(const_int_node,yytext);
  return NUMERIC;
}

<Scanning>. {
  return *yytext;
}

<TransposeCheck>"'" {
  BEGIN(popState());
  return *yytext;
}

<TransposeCheck>[^"'"] {
  BEGIN(popState());
  unput(yytext[0]);
}

%%

int yywrap() {
  return 1;
}

namespace FreeMat {

  void setLexBuffer(char *buffer) {
    stackCount = 0;
    stateStack[0] = 0;
    contextCount = 0;
    lineNumber = 0;
    continuationCount = 0;;
    YY_FLUSH_BUFFER;
    BEGIN(INITIAL);
    firstToken = true;
    char qbuffer[1000];
    sprintf(qbuffer,"      %s",buffer);
    for (int i=0;i<5;i++) qbuffer[i] = 0xFF;
    yy_scan_string(qbuffer);
  }
  
  void setLexFile(FILE *fp) {
    stackCount = 0;
    stateStack[0] = 0;
    contextCount = 0;
    lineNumber = 0;
    YY_FLUSH_BUFFER;
    firstToken = true;
    BEGIN(INITIAL);
    yyrestart(fp);
  }
  
  int getContinuationCount() {
    return continuationCount;
  }
  bool lexCheckForMoreInput(int ccount) {
    // Scan the command line
    while (yylex() > 0);
    return (((contextCount > 0) && (stackCount == 0)) || (continuationCount > ccount));
  }
  
  void lexSetAdditionalInput(char *buffer) {
    continuationCount = 0;
    firstToken = true;
    yy_scan_string(buffer);
  }    
}
