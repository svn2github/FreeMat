/*
 * A lexical analyzer... my apologies for this, but I couldn't get 
 * FLEX to do what I wanted.
 */
#include <stdio.h>

char buffer[4096];
char *datap;
int lineNumber;
typedef enum {
  Initial,
  Scanning
} LexingStates;

LexingStates lexState;
int bracketLevel;

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
  char stringval[256];
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
  printf("string lexed \'%s\'\n",stringval);
  return;
}

void lexNumber() {
  int state;
  int cp;
  int i;
  char buffer[256];
  int intonly;

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
	return;
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
	return;
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
    printf("  <float>");
  } else if (!intonly || (datap[cp] == 'd') ||
	     (datap[cp] == 'D')) {
    cp++;
    printf(" <double>");
  } else 
    printf("  <int>");
  for (i=0;i<cp;i++) 
    buffer[i] = datap[i];
  for (i=0;i<cp;i++)
    discardChar();
  buffer[cp] = '\0';
  printf("token %s\n",buffer);
}

/*
 * String detection is a bit tricky, I suppose....  A quote character
 * immediately following (without whitespace) a bracket or a alphanumeric
 * is a transpose.  Otherwise, a quote character marks the beginning of
 * a string.  This means that we need to look at the _previous_ token.
 */
void lexScanningState() {
  int i;
  char ident[256];

  if (currentChar() == '\'') 
    if ((previousChar() == ')')  ||
	(previousChar() == ']')  ||
	(previousChar() == '}')  ||
	(isalnum(previousChar()))) {
      /* Not a string... */
      printf("transpose\n");
      discardChar();
    } else {
      lexString();
      return;
    }
  if (isWhitespace()) {
    return;
  }
  if (match(";\n") || match(";\r\n")) {
    lineNumber++;
    printf("Qendstatement\n");
    lexState = Initial;
    return;
  }
  if (match(";")) {
    printf("Qendstatement\n");
    lexState = Initial;
    return;
  }
  if (match("\r\n") || match("\n")) {
    lineNumber++;
    printf("EndStatement\n");
    lexState = Initial;
    return;
  }
  if (testAlphaChar()) {
    i=0;
    while (testAlphaNumChar()) {
      ident[i++] = currentChar();
      discardChar();
    }
    ident[i] = '\0';
    if (strcmp(ident,"end") == 0) {
      if (bracketLevel == 0) {
	printf("end\n");
	return;
      } else {
	printf("magicend\n");
	return;
      }
    }
    printf("ident %s\n",ident);
    return;
  }
  if (testDigit() || currentChar() == '.') {
    lexNumber();
    return;
  }
  if ((currentChar() == '(') ||
      (currentChar() == '[') ||
      (currentChar() == '{'))
    bracketLevel++;
  if ((currentChar() == ')') ||
      (currentChar() == ']') ||
      (currentChar() == '}'))
    bracketLevel--;
  printf("token is %c\n",currentChar());
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

void yylex() {
  switch (lexState) {
  case Initial:
    lexInitialState();
    break;
  case Scanning:
    lexScanningState();
    break;
  }
}

int main(int argc, char*argv[]) {
  FILE *fp;
  fp = fopen(argv[1],"r");
  fread(buffer,sizeof(char),sizeof(buffer),fp);
  fclose(fp);
  datap = buffer;
  bracketLevel = 0;
  while (datap[0])
    yylex();
}
