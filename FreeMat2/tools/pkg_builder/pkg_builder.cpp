// Test out the feasibility of doing a generic package building tool
// for FreeMat.  In general, downloading of files, and file manipulation
// and macro definitions are straightforward with Qt.  The tricky
// bit appears to be dealing with archives (.zip and .tar.gz).

#include <QtCore>

// Let's define the mini-language of the package builder tool.
// 1. All variables are strings.  
// 2. Tokens are identifiers 
// 3. Lines beginning with a % are comments
// 4. The operators "=","+" are defined.

typedef unsigned char byte;
const byte TOK_IDENT = 130;
const byte TOK_STRING = 132;
const byte TOK_IF = 133;
const byte TOK_ELSE = 134;
const byte TOK_ELSEIF = 135;
const byte TOK_END = 136;
const byte TOK_FUNCTION = 137;
const byte TOK_RETURN = 138;
const byte TOK_WHILE = 139;
const byte TOK_NE = 140;
const byte TOK_EQ = 141;
const byte TOK_EOF = 142;
const byte TOK_SAND = 143;
const byte TOK_SOR = 144;
const byte TOK_FOR = 145;

QMap<QString, byte> reserved;

static bool reservedInitialized = false;
static void InitializeReserved() {
  reserved["if"] = TOK_IF;
  reserved["else"] = TOK_ELSE;
  reserved["elseif"] = TOK_ELSEIF;
  reserved["end"] = TOK_END;
  reserved["function"] = TOK_FUNCTION;
  reserved["return"] = TOK_RETURN;
  reserved["while"] = TOK_WHILE;
  reserved["for"] = TOK_FOR;
  reservedInitialized = true;
}

class Token {
  byte m_tok;
  unsigned m_pos;
  QString m_text;
public:
  Token() : m_tok(0), m_text("") {}
  Token(byte tok, unsigned pos = 0, QString text = QString()) :
    m_tok(tok), m_pos(pos), m_text(text) {}
  bool Is(byte tok) const {return m_tok == tok;}
  byte Value() const {return m_tok;}
  void SetValue(byte a) {m_tok = a;}
  unsigned Position() const {return m_pos;}
  QString Text() const {return m_text;}
  void SetText(QString txt) {m_text = txt;}
};

QString TokenToString(const Token& b) {
  switch (b.Value()) {
  case TOK_IDENT: return "(ident)"+b.Text();
  case TOK_STRING: return "(string)"+b.Text();
  case TOK_IF: return "if";
  case TOK_ELSE: return "else";
  case TOK_ELSEIF: return "elseif";
  case TOK_END: return "end";
  case TOK_FUNCTION: return "function";
  case TOK_RETURN: return "return";
  case TOK_WHILE: return "while";
  case TOK_NE: return "~=";
  case TOK_EQ: return "==";
  case TOK_EOF: return "eof";
  case TOK_SAND: return "&&";
  case TOK_SOR: return "||";
  case TOK_FOR: return "for";
  }
  return QString((QChar) b.Value());
}

bool isdigit(QChar a) {
  return (a.isNumber());
}

bool isalpha(QChar a) {
  return (a.isLetter());
}

bool isalnumus(QChar a) {
  return (a.isLetterOrNumber() || (a=='_'));
}

bool isablank(QChar a) {
  return (a==' ' || a=='\t' || a=='\r');
}

class Scanner {
  QString m_text;
  int m_ptr;
  bool m_tokValid;
  Token m_tok;
  int m_strlen;
  int m_linenumber;
protected:
  void SetToken(byte tok, QString text = QString()) {
    m_tok = Token(tok, m_linenumber, text);
  }
  QChar current() {
    if (m_ptr < m_strlen)
      return (m_text.at(m_ptr));
    else
      return 0;
  }
  QChar ahead(int n) {
    if ((m_ptr+n) >= (int)(m_text.size()))
      return 0;
    else
      return (m_text.at(m_ptr+n));
  }
  void FetchWhitespace() {
    int len = 0;
    while (isablank(ahead(len))) len++;
    m_ptr += len;
  }
  void FetchComment() {
    while ((current() != '\n') && (m_ptr < m_strlen))
      m_ptr++;    
  }
  void FetchIdentifier() {
    int len = 0;
    while (isalnumus(ahead(len))) len++;
    // Collect the identifier into a string
    QString ident(m_text.mid(m_ptr,len));
    if (reserved.contains(ident))
      SetToken(reserved[ident],ident);
    else
      SetToken(TOK_IDENT,ident);
    m_ptr += len;
  }
  void FetchNumber() {
    int len = 0;
    while (isdigit(ahead(len))) len++;
    QString numtext(m_text.mid(m_ptr,len));
    SetToken(TOK_STRING,numtext);
    m_ptr += len;
  }
  void FetchString() {
    int len = 0;
    while (((ahead(len+1) != '\'') ||
	    ((ahead(len+1) == '\'') && (ahead(len+2) == '\''))) &&
	   (ahead(len+1) != '\n')) {
      if ((ahead(len+1) == '\'') &&
	  (ahead(len+2) == '\'')) len+=2;
      else
	len++;
    }
    if (ahead(len+1) == '\n')
      throw QString("unterminated string on line %1").arg(m_linenumber);
    QString ret(m_text.mid(m_ptr+1,len));
    ret.replace("''","'");
    SetToken(TOK_STRING,ret);
    m_ptr += len+2;
  }
  bool TryFetchBinary(const char* op, byte tok) {
    if ((current() == op[0]) && (ahead(1) == op[1])) {
      SetToken(tok);
      m_ptr += 2;
      return true;
    }
    return false;
  }
  void FetchOther() {
    if (TryFetchBinary("==",TOK_EQ)) return;
    if (TryFetchBinary("~=",TOK_NE)) return;
    if (TryFetchBinary("&&",TOK_SAND)) return;
    if (TryFetchBinary("||",TOK_SOR)) return;
    SetToken(current().toAscii());
    m_ptr++;
  }
  void Fetch() {
    if (m_ptr >= m_strlen) 
      SetToken(TOK_EOF);
    else if (current() == '%') 
      FetchComment();
    else if (isalpha(current()))
      FetchIdentifier();
    else if (isdigit(current()))
      FetchNumber();
    else if (isablank(current())) {
      FetchWhitespace();
      return;
    } else if (current() == '\'')
      FetchString();
    else 
      FetchOther();
    m_tokValid = true;
  }
public:
  Scanner(QString buf) : m_text(buf), m_ptr(0), m_tokValid(false), 
			 m_tok(Token()), m_strlen(buf.size()), 
			 m_linenumber(1) {
    if (!reservedInitialized) InitializeReserved();
  }
  const Token& Next() {
    while (!m_tokValid) {
      Fetch();
      if ((m_ptr < m_strlen) && (current() == '\n'))
	m_linenumber++;
    }
    return m_tok;
  }
  void Consume() {
    m_tokValid = false;
  }
};

QString ReadFileToString(const char *fname) {
  QFile file(fname);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return "";
  QTextStream in(&file);
  QString buffer;
  while (!in.atEnd()) {
    QString line = in.readLine();
    buffer += line;
  }
  return buffer;
}

int main(int argc, const char *argv[]) {
  if (argc < 2) return 1;
  Scanner S(ReadFileToString(argv[1]));
  while (!S.Next().Is(TOK_EOF)) {
    qDebug() << TokenToString(S.Next());
    S.Consume();
  }
  return 0;
}
