


bool Parser::Expression() {
  if (!Term()) return false;
  if (Keyword("+") && Term()) {
    makeTree(2,1,3);
    return true;
  } else if (Keyword("-") && Term()) {
    makeTree(2,1,3);
    return true;
  }
}

bool Parser::WhileStatement() {
  if (Keyword("while")) {
    if (Expression()) {
      if (Block()) {
	makeTree(3,2,1);
	return true;
      } else error("Missing block");
    } else error("Missing <condition>");
  }
  return false;
}
