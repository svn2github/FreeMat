/*
 * A lexical analyzer... my apologies for this, but I couldn't get 
 * FLEX to do what I wanted.
 */

int yylex() {
  char c;

  c = getchar();
  if (isNewLine(c)) {
    lineNumber++;
  } else if (isWhitespace(c)) {
  } else if (isSemicolon(c)) {
  } else if (isCommentchar(c)) {
    while (!isNewLine(c))
      c = getNextCharacterFromStream();
    lineNumber++;
  } else {
    if ((c == '=') && (peekchar() == '=')) {
      getchar();
      return EQ;
    }
    if ((c == '.') && (peekchar() == '*')) {
      getchar();
      return DOTTIMES;
    }
    if ((c == '.') && (peekchar() == '/')) {
      getchar();
      return DOTRDIV;
    }
    if ((c == '.') && (peekchar() == '\\')) {
      getchar();
      return DOTLDIV;
    }
    if ((c == '<') && (peekchar() == '=')) {
      getchar();
      return LE;
    }
    if ((c == '>') && (peekchar() == '=')) {
      getchar();
      return GE;
    }
    if ((c == '~') && (peekchar() == '=')) {
      getchar();
      return NE;
    }
    if ((c == '.') && (peekchar() == '^')) {
      getchar();
      return DOTPOWER;
    }
    if ((c == '.') && (peekchar() == '\'')) {
      getchar();
      return DOTTRANSPOSE;
    }
    if ((c == ';') 
    
  }
  
  
}
