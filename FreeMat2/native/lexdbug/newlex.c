/*
 * A lexical analyzer... my apologies for this, but I couldn't get 
 * FLEX to do what I wanted.
 */

char buffer[4096];
char *datap;
int lineNumber;

int match(char *str) {
  if (strncmp(str,datap,strlen(str)) == 0) {
    datap += strlen(str);
    return 1;
  }
  return 0;
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

int currentChar() {
  return datap[0];
}

int discardChar() {
  datap++;
}

void yylex() {
  int i;
  char ident[256];

  if (isNewline()) {
    lineNumber++;
  } else if (isWhitespace()) {
  } else if (match(";")) {
  } else if (match("%")) {
    while (!isNewline())
      discardChar();
    lineNumber++;
  } else {
    if (match(";\n") || match(";\r\n")) {
      lineNumber++;
      printf("Qendstatement\n");
      return;
    }
    if (match(";")) {
      printf("Qendstatement\n");
      return;
    }
    if (match("\r\n") || match("\n")) {
      lineNumber++;
      printf("EndStatement\n");
      return;
    }
    if (testAlphaChar()) {
      i=0;
      while (testAlphaNumChar()) {
	ident[i++] = currentChar();
	discardChar();
      }
      ident[i] = '\0';
      printf("ident %s\n",ident);
      return;
    }
    if (testDigit()) {
      i=0;
      while (testDigit()) {
	ident[i++] = currentChar();
	discardChar();
      }
      ident[i] = '\0';
      printf("number %s\n",ident);
      return;
    }
    printf("token is %c\n",currentChar());
    discardChar();
    return;
  }
}

int main(int argc, char*argv[]) {
  strcpy(buffer,argv[1]);
  datap = buffer;
  while (datap[0])
    yylex();
}
