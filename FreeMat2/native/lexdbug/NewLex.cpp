/*
 * A lexical analyzer... my apologies for this, but I couldn't get 
 * FLEX to do what I wanted.
 */
#include <stdio.h>
#include "Parser.hpp"
#include "AST.hpp"
#include "Reserved.hpp"

using namespace FreeMat;

#define WS 999

char *buffer = NULL;
char *datap;
int lineNumber;
typedef enum {
  Initial,
  Scanning
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
ASTPtr tokenValue;

reservedWordStruct ts, *p;

void pushBracket(char t) {
  bracketStack[bracketStackSize++] = t;
}

void popBracket(char t) {
  if (bracketStack[--bracketStackSize] != t) {
    printf("mismatched parenthesis\n");
    exit(1);
  }
}

void pushVCState() {
  vcStack[vcStackSize++] = vcFlag;
}

void popVCState() {
  vcFlag = vcStack[--vcStackSize];
}

void setTokenType(int type) {
  tokenType = type;
  tokenActive = 1;
  tokenValue = NULL;
}

int match(char *str) {
  if (strncmp(str,datap,strlen(str)) == 0) {
    datap += strlen(str);
    return 1;
  }
  return 0;
}

int isE(char p) {
  return ((p == 'e') || (p == 'E'));
}

int isWhitespace() {
  return (match(" ") || match("\t"));
}

int isNewline() {
  return (match("\n") || match("\r\n"));
}

int testAlphaChar() {
  return (isalpha(datap[0]));
}

int testAlphaNumChar() {
  return (isalnum(datap[0]) || (datap[0] == '_'));
}

int testDigit() {
  return (isdigit(datap[0]));
}

int testNewline() {
  return ((datap[0] == '\n') || ((datap[0] == '\r') && 
				 (datap[1] == '\n')));
}

int previousChar() {
  if (datap == buffer)
    return 0;
  else
    return datap[-1];
}

int currentChar() {
  return datap[0];
}

int discardChar() {
  datap++;
}


void lexString() {
  char stringval[4096];
  char *strptr;

  strptr = stringval;
  discardChar();
  while ((currentChar() != '\'') && !testNewline()) {
    *strptr++ = currentChar();
    discardChar();
  }
  if (testNewline()) {
    printf("Error - unterminated string...\n");
    exit(1);
  }
  discardChar();
  *strptr++ = '\0';
  setTokenType(STRING);
  tokenValue = new AST(string_const_node,stringval);
  return;
}

void lexIdentifier() {
  int i;
  char ident[256];
  i=0;
  while (testAlphaNumChar()) {
    ident[i++] = currentChar();
    discardChar();
  }
  ident[i] = '\0';
  if (strcmp(ident,"end") == 0) {
    if (bracketStackSize>0) {
      setTokenType(END);
      return;
    } else {
      setTokenType(MAGICEND);
      return;
    }
  } else {
    ts.word = ident;
    p = (reservedWordStruct*)
      bsearch(&ts,reservedWord,RESWORDCOUNT,
	      sizeof(reservedWordStruct),
	      compareReservedWord);
    if (p != NULL) {
      setTokenType(p->token);
      if ((p->token == FOR) || (p->token == WHILE) || (p->token == IF) || (p->token == ELSEIF) || (p->token == CASE))
	vcFlag = 1;
      return;
    } else {
      setTokenType(IDENT);
      tokenValue = new AST(id_node, ident);
    }
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
      } else {
	fprintf(stderr,"malformed float\n");
	exit(1);
      }
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
      } else {
	fprintf(stderr,"malformed float\n");
	exit(1);	
      }
    }
  }
  if ((datap[cp] == 'f') ||
      (datap[cp] == 'F')) {
    cp++;
    vtype = 1;
  } else if (!intonly) {
    vtype = 2;
  } else if ((datap[cp] == 'd') ||
	     (datap[cp] == 'D')) {
    cp++;
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
    tokenValue = new AST(const_float_node,buffer);
    break;
  case 2:
    tokenValue = new AST(const_double_node,buffer);
    break;
  case 3:
    tokenValue = new AST(const_int_node,buffer);
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

  if (match("%")) {
    while (!isNewline())
      discardChar();
    lineNumber++;
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
    lineNumber++;
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
    lineNumber++;
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
  if (testAlphaChar()) {
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
    lineNumber++;
  } else if (isWhitespace()) {
  } else if (match(";")) {
  } else if (match("%")) {
    while (!isNewline())
      discardChar();
    lineNumber++;
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
  }
}


void yylex() {
  static int previousToken = 0;
  tokenActive = 0;
  while (!tokenActive)
    yylexDoLex();
  if ((tokenType == WS) && vcFlag) {
    /* Check for virtual commas... */
    if ((previousToken == ')') || (previousToken == '\'') || (previousToken == NUMERIC) ||
	(previousToken == STRING) || (previousToken == ']') || (previousToken == '}') ||
	(previousToken == IDENT)) {
      /* Test if next character indicates the start of an expression */
      if ((currentChar() == '(') || (currentChar() == '+') || (currentChar() == '-') ||
	  (currentChar() == '~') || (currentChar() == '[') || (currentChar() == '{') ||
	  (currentChar() == '\'') || (isalnum(currentChar()))) {
	/* 
	   OK - now we have to decide if the "+/-" are infix or prefix operators...
	   In fact, this decision alone is the reason for this whole lexer.
	*/
	if ((currentChar() == '+') || (currentChar() == '-')) {
	  /* If we are inside a parenthetical, we never insert virtual commas */
	  if ((bracketStackSize == 0) || (bracketStack[bracketStackSize-1] != '(')) {
	    /*
	      OK - we are not inside a parenthetical.  Insert a virtual comma
	       if the next character is anythong other than a whitespace
	    */
	    if ((datap[1] != ' ') && (datap[1] != '\t'))
	      tokenType = '#';
	  }
	} else
	  tokenType = '#';
      }
    }
  }
  printf("token = %d",tokenType);
  if (tokenType < 255) {
    printf(" char = %c",tokenType);
  }
  if (tokenValue) {
    printf("\nPayload:\n");
    printAST(tokenValue);
  } else 
    printf("\n");
  previousToken = tokenType;
}

void setLexFile(FILE *fp) {
  bracketStackSize = 0;
  lexState = Initial;
  vcStackSize = 0;
  long cpos = ftell(fp);
  fseek(fp,0,SEEK_END);
  cpos -= ftell(fp);
  fseek(fp,0,SEEK_SET);
  cpos = -cpos;
  if (buffer)
    free(buffer);
  buffer = (char*) calloc(cpos+1,sizeof(char));
  datap = buffer;
  fread(buffer,sizeof(char),cpos,fp);
  fclose(fp);
}

int main(int argc, char*argv[]) {
  FILE *fp;
  fp = fopen(argv[1],"r");
  if (fp)
    setLexFile(fp);
  else
    exit(1);
  while (datap[0])
    yylex();
}

