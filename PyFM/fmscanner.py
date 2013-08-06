"""
The scanner module provides the :class:`scanner`, which performs lexical analysis on an input text.
"""

from fmtoken import Token

FREEMAT_RESERVED = {"break": Token.TOK_BREAK,
                    "case": Token.TOK_CASE,
                    "catch": Token.TOK_CATCH,
                    "continue": Token.TOK_CONTINUE,
                    "dbstep": Token.TOK_DBSTEP,
                    "dbtrace": Token.TOK_DBTRACE,
                    "dbdown": Token.TOK_DBDOWN,
                    "dbup": Token.TOK_DBUP,
                    "else": Token.TOK_ELSE,
                    "elseif": Token.TOK_ELSEIF,
                    "end": Token.TOK_END,
                    "for": Token.TOK_FOR,
                    "function": Token.TOK_FUNCTION,
                    "global": Token.TOK_GLOBAL,
                    "if": Token.TOK_IF,
                    "keyboard": Token.TOK_KEYBOARD,
                    "otherwise": Token.TOK_OTHERWISE,
                    "persistent": Token.TOK_PERSISTENT,
                    "quit": Token.TOK_QUIT,
                    "retall": Token.TOK_RETALL,
                    "return": Token.TOK_RETURN,
                    "switch": Token.TOK_SWITCH,
                    "try": Token.TOK_TRY,
                    "while": Token.TOK_WHILE}

def isalnumus(a):
    return a.isalnum() or a == '_'

def isablank(a):
    return a == ' ' or a == '\t' or a == '\r'


class Scanner:
    def __init__(self, buffer = '', filename = ''):
        self.text = buffer
        self.strlen = len(buffer)
        self.filename = filename
        self.ptr = 0
        self.linenumber = 1
        self.tokValid = False
        self.inContinuationState = False
        self.bracketDepth = 0
        self.ignorews = [True]
        self.debug = False
        self.blob = False
        self.tok = Token()
    def setBlobMode(self,flag):
        self.blob = flag
    def pushWSFlag(self,flag):
        self.ignorews.append(flag)
    def popWSFlag(self):
        self.ignorews.pop()
    def contextNum(self):
        return self.ptr << 16 | self.linenumber
    def context(self,pos):
        return "Some stuff"
    def setToken(self, tok, text = ''):
        self.tok = Token(tok,self.ptr << 16 | self.linenumber ,text)
    def done(self):
        return self.ptr > self.strlen
    def consume(self):
#        print "Consumed " + str(self.tok)
        self.tokValid = False
    def peek(self,chars,tok):
        return self.ahead(chars) == tok
    def fetchIdentifier(self):
        numlen = 0
        while (isalnumus(self.ahead(numlen))): 
            numlen += 1
        ident = self.text[self.ptr:(self.ptr+numlen)]
        if ident in FREEMAT_RESERVED:
            self.setToken(FREEMAT_RESERVED[ident])
        else:
            self.setToken(Token.TOK_IDENT,ident)
        self.ptr += numlen
        return
    def fetchWhitespace(self):
        spclen = 0
        while (isablank(self.ahead(spclen))):
            spclen += 1
        self.setToken(Token.TOK_SPACE)
        self.ptr += spclen
        return
    def fetchContinuation(self):
        self.ptr += 3
        while ((self.current() != '\n') and (self.ptr < self.strlen)):
            self.ptr += 1
        if (self.current() == '\n'):
            self.linenumber += 1
            self.ptr += 1
        self.inContinuationState = True
#        print "continuation found - in continuation state"
        return
    def fetchOther(self):
        if (self.current() == '.') :
            if (self.tryFetchBinary(".*",Token.TOK_DOTTIMES)): return
            if (self.tryFetchBinary("./",Token.TOK_DOTRDIV)): return
            if (self.tryFetchBinary(".\\",Token.TOK_DOTLDIV)): return
            if (self.tryFetchBinary(".^",Token.TOK_DOTPOWER)): return
            if (self.tryFetchBinary(".'",Token.TOK_DOTTRANSPOSE)): return
        if (self.tryFetchBinary("<=",Token.TOK_LE)): return
        if (self.tryFetchBinary(">=",Token.TOK_GE)): return
        if (self.tryFetchBinary("==",Token.TOK_EQ)): return
        if (self.tryFetchBinary("~=",Token.TOK_NE)): return
        if (self.tryFetchBinary("&&",Token.TOK_SAND)): return
        if (self.tryFetchBinary("||",Token.TOK_SOR)): return
        self.setToken(self.text[self.ptr])
        if (self.text[self.ptr] == '['):
            self.bracketDepth += 1
        if (self.text[self.ptr] == ']'):
            self.bracketDepth = min(0,self.bracketDepth-1)
        if (self.text[self.ptr] == '{'):
            self.bracketDepth += 1
        if (self.text[self.ptr] == '}'):
            self.bracketDepth = min(0,self.bracketDepth-1)
        self.ptr += 1
    def fetchNumber(self):
        numlen = 0
        lookahead = 0
        imagnumber = False
        singleprecision = False
        while (self.ahead(numlen).isdigit()):
            numlen += 1
        lookahead = numlen;
        if (self.ahead(lookahead) == '.'):
            lookahead += 1
            numlen = 0
            while (self.ahead(numlen+lookahead).isdigit()):
                numlen += 1
            lookahead += numlen
        if ((self.ahead(lookahead) == 'E') or
            (self.ahead(lookahead) == 'e')) :
            lookahead += 1
            if ((self.ahead(lookahead) == '+') or
                (self.ahead(lookahead) == '-')) :
                lookahead += 1
            numlen = 0
            while (self.ahead(numlen+lookahead).isdigit()) : numlen += 1
            lookahead+=numlen;
        if ((self.ahead(lookahead) == 'f') or (self.ahead(lookahead) == 'F')):
            singleprecision = True;
            lookahead += 1
        if ((self.ahead(lookahead) == 'd') or (self.ahead(lookahead) == 'D')):
            lookahead += 1;
        # Recognize the complex constants, but strip the "i" off
        if ((self.ahead(lookahead) == 'i') or
            (self.ahead(lookahead) == 'I') or
            (self.ahead(lookahead) == 'j') or
            (self.ahead(lookahead) == 'J')):
            imagnumber = True;
        # Back off if we aggregated a "." from "..." into the number
        if (((self.ahead(lookahead-1) == '.') and
             (self.ahead(lookahead) == '.') and
             (self.ahead(lookahead+1) == '.') and
             (self.ahead(lookahead+2) != '.')) or
            ((self.ahead(lookahead-1) == '.') and
             ((self.ahead(lookahead) == '*') or
              (self.ahead(lookahead) == '/') or
              (self.ahead(lookahead) == '\\') or
              (self.ahead(lookahead) == '^') or
              (self.ahead(lookahead) == '\'')))): lookahead -= 1
        numtext = self.text[self.ptr:(self.ptr + lookahead)]
#        print "Number parse text:" +numtext
        self.ptr += lookahead
        if (imagnumber):
            self.ptr += 1;
        if (not imagnumber):
            if (singleprecision):
                self.setToken(Token.TOK_REALF,numtext)
            else:
                self.setToken(Token.TOK_REAL,numtext)
        else:
            if (singleprecision):
                self.setToken(Token.TOK_IMAGF,numtext)
            else:
                self.setToken(Token.TOK_IMAG,numtext)
        return
    def tryFetchBinary(self,op, value):
        if (self.current() == op[0]) and (self.ahead(1) == op[1]):
            self.setToken(value)
            self.ptr += 2
            return True
        return False
    def fetchString(self):
        strlen = 0
        while (((self.ahead(strlen+1) != '\'') or
                ((self.ahead(strlen+1) == '\'') and (self.ahead(strlen+2) == '\''))) and
            (self.ahead(strlen+1) != '\n')):
            if ((self.ahead(strlen+1) == '\'') and
                (self.ahead(strlen+2) == '\'')): 
                strlen+=2
            else:
                strlen+=1
        if (self.ahead(strlen+1) == '\n'):
            raise NameError("unterminated string")
        txt = self.text[(self.ptr+1):(self.ptr+1+strlen)]
        txt = txt.replace("''","'")
        self.setToken(Token.TOK_STRING,txt);
        self.ptr += strlen+2
    def fetchBlob(self):
        if (self.current() == '\''):
            self.fetchString()
            self.tokValid = True
        else:
            bloblen = 0
            while ((self.ahead(bloblen) != '\n') and (not isablank(self.ahead(bloblen))) and
                   (self.ahead(bloblen) != '%') and (self.ahead(bloblen) != ',') and
                   (self.ahead(bloblen) != ';')): bloblen += 1
            if (bloblen > 0):
                self.setToken(Token.TOK_STRING,self.text[self.ptr:(self.ptr+bloblen)])
                self.ptr += bloblen
                self.tokValid = True
    def fetchComment(self):
        while (self.current() != '\n') and (self.ptr < self.strlen):
            self.ptr += 1
        return
    def current(self):
        if (self.ptr < self.strlen):
            return self.text[self.ptr]
        return ''
    def previous(self):
        if (self.ptr > 0):
            return self.text[self.ptr-1]
        return ''
    def ahead(self,n):
        if (self.ptr+n >= self.strlen):
            return ''
        return self.text[self.ptr+n]
    def next(self):
        while (not self.tokValid):
            self.fetch()
            if (self.ptr < self.strlen) and (self.current() == '\n'):
                self.linenumber += 1
        if (self.inContinuationState and self.tokValid and not self.tok == Token.TOK_EOF):
            self.inContinuationState = False
        return self.tok
    def fetch(self):
        if (self.ptr >= self.strlen):
            self.setToken(Token.TOK_EOF)
            self.ptr += 1
        elif (self.current() == '%'):
            self.fetchComment()
            return
        elif (self.current() == '.' and self.ahead(1) == '.' and self.ahead(2) == '.'):
            self.fetchContinuation()
            return
        elif (self.blob and not isablank(self.current()) and 
              (self.current() != '\n') and (self.current() != ';') and
              (self.current() != ',') and (self.current() != '\'') and
              (self.current() != '%')):
            self.fetchBlob()
        elif (self.current().isalpha()):
            self.fetchIdentifier()
        elif (self.current().isdigit() or ((self.current() == '.') and (self.ahead(1).isdigit()))):
            self.fetchNumber()
        elif (isablank(self.current())):
            self.fetchWhitespace()
            if (self.ignorews[-1]):
                return
        elif (self.current() == '\'') and not ((self.previous() == '\'') or
                                               (self.previous() == ')') or
                                               (self.previous() == ']') or
                                               (self.previous() == '}') or
                                               (isalnumus(self.previous()))):
            self.fetchString()
        else:
            self.fetchOther()
        self.tokValid = True
        
    
        
    
        
