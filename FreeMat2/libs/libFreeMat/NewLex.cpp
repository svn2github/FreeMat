/*
 * A lexical analyzer... my apologies for this, but I couldn't get 
 * FLEX to do what I wanted.
 */
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

#define WS 999


#include "AST.hpp"

#define YYSTYPE FreeMat::ParseRHS

#include "Reserved.hpp"
#include "Parser.h"
#include "Exception.hpp"

using namespace FreeMat;

extern YYSTYPE yylval;
extern bool interactiveMode;
extern int charcontext;
char *buffer = NULL;
char *datap;
char *linestart;
int lineNumber;
const char *parsing_filename;
int continuationCount;
int inBlock;
typedef enum {
  Initial,
  Scanning,
  SpecScan
} LexingStates;

LexingStates lexState;
int bracketStack[256];
int bracketStackSize;
int vcStack[256];
int vcStackSize;
int vcFlag;

/*
 * These variables capture the token information
 */
int tokenActive;
int tokenType;
ParseRHS tokenValue;

reservedWordStruct ts, *p;

int ContextInt() {
  if (datap==linestart)
    return (1 << 16 | lineNumber);
  else
    return ((datap-linestart+1) << 16) | (lineNumber+1);
}

void NextLine() {
  lineNumber++;
  linestart = datap;
}

void LexerException(const char *msg) {
  char buffer[256];
  if (!interactiveMode && parsing_filename && msg) {
    sprintf(buffer,"Lexical error '%s'\n\tat line %d of file %s",
	    msg,lineNumber+1,parsing_filename);
    throw Exception(buffer);
  } else {
    sprintf(buffer,"Lexical error '%s'",msg);
    throw Exception(buffer);
  }
}

inline void pushBracket(char t) {
  bracketStack[bracketStackSize++] = t;
}

inline void popBracket(char t) {
  if (bracketStackSize <= 0)
    LexerException("mismatched parenthesis");
  if (bracketStack[--bracketStackSize] != t)
    LexerException("mismatched parenthesis");
}

inline void pushVCState() {
  vcStack[vcStackSize++] = vcFlag;
}

inline void popVCState() {
  vcFlag = vcStack[--vcStackSize];
}

inline bool testSpecialFuncs() {
  bool test1, test2;
  char *cp;
  if (!isalpha(datap[0])) return false;
  //FIXME - this should check the current context to see if any of these have been 
  //masked or assigned
  //  test1 = ((strncmp(datap,"cd ",3)==0) || (strncmp(datap,"ls ",3)==0) || (strncmp(datap,"dir ",4)==0));
  test1 = ((strncmp(datap,"cd ",3)==0) || (strncmp(datap,"ls ",3)==0));
  if (test1)
    return test1;
  // Check for non-keyword identifier followed by whitespace followed by alphanum
  char keyword[100];
  cp = datap;
  while (isalnum(*cp)) {
    keyword[cp-datap] = *cp;
    cp++;
  }
  keyword[cp-datap] = 0;
  ts.word = keyword;
  p = (reservedWordStruct*)
    bsearch(&ts,reservedWord,RESWORDCOUNT,
	    sizeof(reservedWordStruct),
	    compareReservedWord);
  if (p != NULL)
    return false;
  while ((*cp == ' ') || (*cp == '\t')) cp++;
  if (isalnum(*cp)) 
    return true;
  return false;
}

inline void setTokenType(int type) {
  tokenType = type;
  tokenActive = 1;
  tokenValue.isToken = true;
  tokenValue.v.p = NULL;
}

inline int match(char *str) {
  if (strncmp(str,datap,strlen(str)) == 0) {
    datap += strlen(str);
    return 1;
  }
  return 0;
}

inline int isE(char p) {
  return ((p == 'e') || (p == 'E'));
}

inline int isWhitespace() {
  return (match(" ") || match("\t"));
}

inline int isNewline() {
  return (match("\n") || match("\r\n"));
}

inline int testAlphaChar() {
  return (isalpha(datap[0]));
}

inline int testAlphaNumChar() {
  return (isalnum(datap[0]) || (datap[0] == '_'));
}

inline int testDigit() {
  return (isdigit(datap[0]));
}

inline int testNewline() {
  return ((datap[0] == '\n') || ((datap[0] == '\r') && 
				 (datap[1] == '\n')) ||
	  (datap[0] == 0));
}

inline int testWhitespace() {
  return ((datap[0] == ' ')  || (datap[0] == '\t'));
}

inline int previousChar() {
  if (datap == buffer)
    return 0;
  else
    return datap[-1];
}

inline int currentChar() {
  return datap[0];
}

inline void discardChar() {
  datap++;
}

void lexUntermString() {
  char stringval[4096];
  char *strptr;

  strptr = stringval;
  while (isWhitespace());
  if (testNewline() || !isalpha(currentChar())) {
    lexState = Scanning;
    return;
  }
  while (!testNewline() && isalnum(currentChar())) {
    *strptr++ = currentChar();
    discardChar();
  }
  *strptr++ = '\0';
  setTokenType(STRING);
  tokenValue.isToken = false;
  tokenValue.v.p = new AST(string_const_node,stringval,ContextInt());
#ifdef LEXDEBUG
  printf("Untermed string %s\r\n",stringval);
#endif
  if ((datap[0] == ';') || (datap[0] == '\r') ||
      (datap[0] == '\n'))
    lexState = Scanning;
}

void lexString() {
  char stringval[4096];
  char *strptr;

  strptr = stringval;
  discardChar();
  while (!currentChar() && (currentChar() != '\'') || ((currentChar() == '\'') && (datap[1] == '\'')) && !testNewline()) {
    if ((currentChar() == '\'') && (datap[1] == '\''))
      discardChar();
    *strptr++ = currentChar();
    discardChar();
  }
  if (testNewline() || !currentChar()) LexerException("unterminated string");
  discardChar();
  *strptr++ = '\0';
  setTokenType(STRING);
  tokenValue.isToken = false;
  tokenValue.v.p = new AST(string_const_node,stringval,ContextInt());
  return;
}

// void lexSpecCall() {
//   char stringval[4096];
//   char *strptr;

//   while ((datap[0] == ' ') || (datap[0] == '\t'))
//     discardChar();
  
//   // First comes the command
//   while (currentChar() != ' ') {
    
//   }

//  if (datap[0] == '\'') 
//    lexString();
//  else {
//    /* Parse this as a string */
//    strptr = stringval;
//    while ((datap[0] != ' ') && (datap[0] != '\t') &&
//	   (datap[0] != '\n') && (datap[0] != '\r') &&
//	   (datap[0] != ';')) {
//      *strptr++ = currentChar();
//      discardChar();
//    }
//    *strptr++ = '\0';
//    setTokenType(STRING);
//    tokenValue.isToken = false;
//    tokenValue.v.p = new AST(string_const_node,stringval,ContextInt());
//  }
//  if ((datap[0] == ';') || (datap[0] == '\r') ||
//      (datap[0] == '\n'))
//    lexState = Scanning;  
//}

//void lexSpecialCall() {
//  char stringval[4096];
//  char *strptr;
//
//  while ((datap[0] == ' ') || (datap[0] == '\t'))
//    discardChar();
//
//  if (match("...")) {
//    while (!isNewline())
//      discardChar();
//    NextLine();
//    continuationCount++;
//    while ((datap[0] == ' ') || (datap[0] == '\t'))
//      discardChar();
//  }
//
//  if (datap[0] == '\'') 
//    lexString();
//  else {
//    /* Parse this as a string */
//    strptr = stringval;
//    while ((datap[0] != ' ') && (datap[0] != '\t') &&
//	   (datap[0] != '\n') && (datap[0] != '\r') &&
//	   (datap[0] != ';')) {
//      *strptr++ = currentChar();
//      discardChar();
//    }
//    *strptr++ = '\0';
//    setTokenType(STRING);
//    tokenValue.isToken = false;
//    tokenValue.v.p = new AST(string_const_node,stringval,ContextInt());
//  }
//  if ((datap[0] == ';') || (datap[0] == '\r') ||
//      (datap[0] == '\n'))
//    lexState = Scanning;
//}
//
//int lexTestSpecialSyntax() {
//  /*
//   * Special syntax detection works by checking for a certain
//   * pattern.  In particular, we look for an identifier followed
//   * by a whitespace.
//   */
//  int i, n;
//  char ident_candidate[2048];
//  if (bracketStackSize) return 0;
//  if (!testAlphaChar()) return 0;
//  i = 0;
//  while (isalnum(datap[i]) || (datap[i] == '_')) {
//    ident_candidate[i] = datap[i];
//    i++;
//  }
//  ident_candidate[i] = '\0';
//  ts.word = ident_candidate;
//    p = (reservedWordStruct*)
//    bsearch(&ts,reservedWord,RESWORDCOUNT,
//	    sizeof(reservedWordStruct),
//	    compareReservedWord);
//  if (p != NULL)
//    return 0;
//  n = i;
//  if ((datap[i] != ' ') && (datap[i] != '\t')) return 0;
//  while ((datap[i] == ' ') || (datap[i] == '\t')) i++;
//  if ((datap[i] == '.') && (datap[i+1] == '*')) return 0;
//  if (datap[i] == '+') return 0;
//  if (datap[i] == '-') return 0;
//  if (datap[i] == '/') return 0;
//  if (datap[i] == '\\') return 0;
//  if ((datap[i] == '.') && (datap[i+1] == '/')) return 0;
//  if ((datap[i] == '.') && (datap[i+1] == '\\')) return 0;
//  if ((datap[i] == '.') && (datap[i+1] == '^')) return 0;
//  if (datap[i] == '^') return 0;
//  if (datap[i] == '>') return 0;
//  if (datap[i] == '=') return 0;
//  if (datap[i] == '<') return 0;
//  if (datap[i] == '~') return 0;
//  if ((datap[i] == '.') && (datap[i+1] == '\'')) return 0;
//  datap += n;
//  lexState = SpecialCall;
//  setTokenType(SPECIALCALL);
//  tokenValue.isToken = false;
//  tokenValue.v.p = new AST(id_node,ident_candidate,ContextInt());
//  return 1;
//}

void lexIdentifier() {
  int i;
  char ident[256];
  i=0;
  while (testAlphaNumChar() || (currentChar() == '_')) {
    ident[i++] = currentChar();
    discardChar();
  }
  ident[i] = '\0';
  ts.word = ident;
  p = (reservedWordStruct*)
    bsearch(&ts,reservedWord,RESWORDCOUNT,
	    sizeof(reservedWordStruct),
	    compareReservedWord);
  if (p != NULL) {
    setTokenType(p->token);
    if (strcmp(ident,"end") == 0) {
      if (bracketStackSize==0) {
	setTokenType(END);
	inBlock--;
      } else {
	setTokenType(MAGICEND);
      }
    }
    // The lexer no longer _has_ to keep track of the "end" keywords
    // to match them up.  But we need this information to determine
    // if more text is needed...
    tokenValue.isToken = false;
    tokenValue.v.p = new AST(reserved_node,p->ordinal,ContextInt());
    if ((p->token == FOR) || (p->token == WHILE) || 
	(p->token == IF) || (p->token == ELSEIF) || 
	(p->token == CASE)) {
      vcFlag = 1;
      inBlock++;
    }
    return;
  } else {
    setTokenType(IDENT);
    tokenValue.isToken = false;
    tokenValue.v.p = new AST(id_node,ident,ContextInt());
  }
}

int lexNumber() {
  int state;
  int cp;
  int i;
  char buffer[256];
  int intonly;
  int vtype;

  // Initialize the state...
  state = 0;
  cp = 0;
  intonly = 1;
  while (state != 7) {
    switch (state) {
    case 0:
      if (datap[cp] == '.') {
	cp++;
	state = 3;
	intonly = 0;
      } else if (isdigit(datap[cp])) {
	while (isdigit(datap[cp]))
	  cp++;
	state = 1;
	break;
      } else 
	return 0;
      break;
    case 1:
      if (datap[cp] == '.') {
	intonly = 0;
	cp++;
	state = 5;
	break;
      } else if (isE(datap[cp])) {
	intonly = 0;
	cp++;
	state = 2;
	break;
      } else 
	state = 7;
      break;
    case 2:
      if ((datap[cp] == '+') || (datap[cp] == '-')) {
	cp++;
	state = 6;
      } else if (isdigit(datap[cp])) {
	state = 6;
      } else 
	LexerException("malformed floating point constant");
      break;
    case 3:
      if (isdigit(datap[cp])) {
	while (isdigit(datap[cp]))
	  cp++;
      } else
	return 0;
      state = 4;
      break;
    case 4:
      if (isE(datap[cp])) {
	intonly = 0;
	cp++;
	state = 2;
	break;
      } else
	state = 7;
      break;
    case 5:
      if (isE(datap[cp])) {
	intonly = 0;
	cp++;
	state = 2;
	break;
      } else if (isdigit(datap[cp])) {
	while (isdigit(datap[cp]))
	  cp++;
	state = 4;
	break;
      } else {
	state = 7;
      }
      break;
    case 6:
      if (isdigit(datap[cp])) {
	while (isdigit(datap[cp]))
	  cp++;
	state = 7;
      } else
	LexerException("malformed floating point constant");
    }
  }
  if ((datap[cp] == 'f') ||
      (datap[cp] == 'F')) {
    cp++;
    vtype = 1;
  } else if ((datap[cp] == 'd') ||
	     (datap[cp] == 'D')) {
    cp++;
    vtype = 2;
  } else if (!intonly) {
    vtype = 2;
  } else 
    vtype = 3;
  for (i=0;i<cp;i++) 
    buffer[i] = datap[i];
  for (i=0;i<cp;i++)
    discardChar();
  buffer[cp] = '\0';
  setTokenType(NUMERIC);
  switch (vtype) {
  case 1:
    tokenValue.isToken = false;
    tokenValue.v.p = new AST(const_float_node,buffer,ContextInt());
    break;
  case 2:
    tokenValue.isToken = false;
    tokenValue.v.p = new AST(const_double_node,buffer,ContextInt());
    break;
  case 3:
    tokenValue.isToken = false;
    tokenValue.v.p = new AST(const_int_node,buffer,ContextInt());
    break;
  }
  return 1;
}

/*
 * String detection is a bit tricky, I suppose....  A quote character
 * immediately following (without whitespace) a bracket or a alphanumeric
 * is a transpose.  Otherwise, a quote character marks the beginning of
 * a string.  This means that we need to look at the _previous_ token.
 */
void lexScanningState() {
  if (match("...")) {
    while (!isNewline())
      discardChar();
    NextLine();
    continuationCount++;
  }
  if (match("%")) {
    while (!isNewline())
      discardChar();
    setTokenType(ENDSTMNT);
    NextLine();
    return;
  }
  if (currentChar() == '\'') 
    if ((previousChar() == ')')  ||
	(previousChar() == ']')  ||
	(previousChar() == '}')  ||
	(isalnum(previousChar()))) {
      /* Not a string... */
      setTokenType((int) '\'');
      discardChar();
      return;
    } else {
      lexString();
      return;
    }
  if (isWhitespace()) {
    while (isWhitespace());
    setTokenType(WS);
    return;
  }
  if (match(";\n") || match(";\r\n")) {
    NextLine();
    setTokenType(ENDQSTMNT);
    lexState = Initial;
    if (bracketStackSize == 0)
      vcFlag = 0;
    return;
  }
  if (match(";")) {
    setTokenType(ENDQSTMNT);
    if (bracketStackSize == 0)
      vcFlag = 0;
    lexState = Initial;
    return;
  }
  if (match("\r\n") || match("\n")) {
    NextLine();
    setTokenType(ENDSTMNT);
    lexState = Initial;
    if (bracketStackSize == 0)
      vcFlag = 0;
    return;
  }
  if (match(".*")) {
    setTokenType(DOTTIMES);
    return;
  } 
  if (match("./")) {
    setTokenType(DOTRDIV);
    return;
  } 
  if (match(".\\")) {
    setTokenType(DOTLDIV);
    return;
  } 
  if (match(".^")) {
    setTokenType(DOTPOWER);
    return;
  } 
  if (match(".'")) {
    setTokenType(DOTTRANSPOSE);
    return;
  } 
  if (match("~=")) {
    setTokenType(NE);
    return;
  }
  if (match("<=")) {
    setTokenType(LE);
    return;
  }
  if (match(">=")) {
    setTokenType(GE);
    return;
  }
  if (match("==")) {
    setTokenType(EQ);
    return;
  }
  if (match("||")) {
    setTokenType(SOR);
    return;
  }
  if (match("&&")) {
    setTokenType(SAND);
    return;
  }
  if (testAlphaChar() || currentChar() == '_') {
    lexIdentifier();
    return;
  }
  if (testDigit() || currentChar() == '.')
    if (lexNumber()) return;
  if ((currentChar() == '[') ||
      (currentChar() == '{')) {
    pushBracket(currentChar());
    pushVCState();
    vcFlag = 1;
  }
  if (currentChar() == '(') {
    pushBracket(currentChar());
    pushVCState();
    vcFlag = 0;
  }
  if (currentChar() == ')') {
    popVCState();
    popBracket('(');
  }
  if (currentChar() == ']') {
    popVCState();
    popBracket('[');
  }
  if (currentChar() == '}') {
    popVCState();
    popBracket('{');
  }


  if (currentChar() == ',') {
    if (bracketStackSize == 0)
      vcFlag = 0;
  }
  setTokenType(currentChar());
  discardChar();
}

void lexInitialState() {
  if (isNewline()) {
    NextLine();
  } else if (isWhitespace()) {
  } else if (match(";")) {
  } else if (match("%")) {
    while (!isNewline())
      discardChar();
    NextLine();
  } else if (testSpecialFuncs()) {
    lexState = SpecScan;
  } else {
    lexState = Scanning;
  }
}

void yylexDoLex() {
  switch (lexState) {
  case Initial:
    lexInitialState();
    break;
  case Scanning:
    lexScanningState();
    break;
  case SpecScan:
  //    lexSpecCall();
    lexUntermString();
    break;
  }
}

int yylexScreen() {
  static int previousToken = 0;
  tokenActive = 0;
  while (!tokenActive)
    yylexDoLex();
  if ((tokenType == WS) && vcFlag) {
    /* Check for virtual commas... */
    if ((previousToken == ')') || (previousToken == '\'') || (previousToken == NUMERIC) ||
	(previousToken == STRING) || (previousToken == ']') || (previousToken == '}') ||
	(previousToken == IDENT) || (previousToken == MAGICEND)) {
      /* Test if next character indicates the start of an expression */
      if ((currentChar() == '(') || (currentChar() == '+') || (currentChar() == '-') ||
	  ((currentChar() == '~')  && (datap[1] != '=')) || (currentChar() == '[') || (currentChar() == '{') ||
	  (currentChar() == '\'') || (isalnum(currentChar())) || ((currentChar() == '.') && (isdigit(datap[1])))) {
	/* 
	   OK - now we have to decide if the "+/-" are infix or prefix operators...
	   In fact, this decision alone is the reason for this whole lexer.
	*/
	if ((currentChar() == '+') || (currentChar() == '-')) {
	  /* If we are inside a parenthetical, we never insert virtual commas */
	  if ((bracketStackSize == 0) || (bracketStack[bracketStackSize-1] != '(')) {
	    /*
	      OK - we are not inside a parenthetical.  Insert a virtual comma
	      if the next character is anything other than a whitespace
	    */
	    if ((datap[1] != ' ') && (datap[1] != '\t'))
	      tokenType = '#';
	  }
	} else
	  tokenType = '#';
      }
    }
  }
  yylval = tokenValue;
  previousToken = tokenType;
  return tokenType;
}

int yylex() {
  int retval;
  yylval.v.i = 0;
  retval = yylexScreen();
  while (retval == WS)
    retval = yylexScreen();
  if (!yylval.v.i) {
    yylval.isToken = true;
    yylval.v.i = ContextInt();
  }
#ifdef LEXDEBUG
  if (tokenType == STRING)
    printf("token string %s\r\n",tokenValue.v.p->text);
  else if (tokenType == IDENT)
    printf("token ident %s\r\n",tokenValue.v.p->text);
  else if (tokenType == NUMERIC)
    printf("token number %s\r\n",tokenValue.v.p->text);
  //  else if (tokenType == SPECIALCALL)
  //    printf("token specialcall %s\r\n",tokenValue.v.p->text);
  else
    printf("token %c %d\r\n",retval,retval);
#endif
  return retval;
}

namespace FreeMat {
  void setLexBuffer(char *buf) {
    continuationCount = 0;
    bracketStackSize = 0;
    inBlock = 0;
    lexState = Initial;
    vcStackSize = 0;
    if (buffer)
      free(buffer);
    buffer = (char*) calloc(strlen(buf)+1,sizeof(char));
    datap = buffer;
    strcpy(buffer,buf);
    linestart = datap;
    lineNumber = 0;
  }

  void setLexFile(FILE *fp) {
    inBlock = 0;
    struct stat st;
    clearerr(fp);
    fstat(fileno(fp),&st);
    bracketStackSize = 0;
    lexState = Initial;
    vcStackSize = 0;
    lineNumber = 0;
    long cpos = st.st_size;
    if (buffer)
      free(buffer);
    // Allocate enough for the text, an extra newline, and null
    buffer = (char*) calloc(cpos+2,sizeof(char));
    datap = buffer;
    int n = fread(buffer,sizeof(char),cpos,fp);
    buffer[n]='\n';
    buffer[n+1]=0;
    linestart = datap;
  }

  bool lexCheckForMoreInput(int ccount) {
    try {
      while (yylex() > 0);
      return ((continuationCount>ccount) || 
	      ((bracketStackSize>0) && 
	       (bracketStack[bracketStackSize-1] == '[')) || 
	      inBlock);
    } catch (Exception &E) {
      continuationCount = 0;
      return false;
    }
  }

  int getContinuationCount() {
    return continuationCount;
  }
}
