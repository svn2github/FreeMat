"""
The token module provides the :class:`token`, which captures tokens in the parse tree.
"""



class Token:
    TOK_INVALID = 0; TOK_IDENT = 130; TOK_SPACE = 132;TOK_STRING = 133;TOK_KEYWORD = 134;TOK_BREAK = 135;TOK_CASE = 136;
    TOK_CATCH = 137;TOK_CONTINUE = 138;TOK_DBSTEP = 139;TOK_DBTRACE = 140;TOK_ELSE = 141;TOK_ELSEIF = 142;TOK_END = 143;
    TOK_FOR = 144;TOK_FUNCTION = 145;TOK_GLOBAL = 146;TOK_IF = 147;TOK_KEYBOARD = 148;TOK_OTHERWISE = 149;TOK_PERSISTENT = 150;
    TOK_QUIT = 151;TOK_RETALL = 152;TOK_RETURN = 153;TOK_SWITCH = 154;TOK_TRY = 155;TOK_WHILE = 156;TOK_MULTI = 157;TOK_SPECIAL = 158;
    TOK_VARIABLE = 159;TOK_DYN = 160;TOK_BLOCK = 161;TOK_EOF = 162;TOK_MATDEF = 163;TOK_CELLDEF = 164;TOK_PARENS = 165;TOK_BRACES = 166;
    TOK_BRACKETS = 167;TOK_ROWDEF = 168;TOK_UNARY_MINUS = 169;TOK_UNARY_PLUS = 170;TOK_EXPR = 171;TOK_DOTTIMES = 172;TOK_DOTRDIV = 173;
    TOK_DOTLDIV = 174;TOK_DOTPOWER = 175;TOK_DOTTRANSPOSE = 176;TOK_LE = 177;TOK_GE = 178;TOK_EQ = 179;TOK_NE = 180;TOK_SOR = 181;
    TOK_SAND = 182;TOK_QSTATEMENT = 183;TOK_STATEMENT = 184;TOK_REALF = 185;TOK_IMAGF = 186;TOK_REAL = 187;TOK_IMAG = 188;TOK_FUNCTION_DEFS = 190;
    TOK_SCRIPT = 191;TOK_ANONYMOUS_FUNC = 192;TOK_NEST_FUNC = 193;TOK_TYPE_DECL = 194;TOK_DBUP = 195;TOK_DBDOWN = 196;TOK_REINDEX = 197;
    TOK_INCR = 198;TOK_DECR = 199;TOK_INCR_PREFIX = 200;TOK_DECR_PREFIX = 201;TOK_INCR_POSTFIX = 202;TOK_DECR_POSTFIX = 203;TOK_PLUS_EQ = 204;
    TOK_MINUS_EQ = 205;
    TOK_BINARY_OPERATORS = ['+','-','*','/','\\','^','>','<',':',TOK_LE,TOK_GE,TOK_EQ,TOK_NE,
                            TOK_SOR,TOK_SAND,TOK_DOTTIMES,TOK_DOTRDIV,TOK_DOTLDIV,
                            TOK_DOTPOWER,'|','&']
    
    TOK_UNARY_OPERATORS = ['+','-','~',TOK_UNARY_MINUS,TOK_UNARY_PLUS,TOK_INCR,TOK_DECR]
    TOK_RIGHT_ASSOCIATIVE_OPERATORS = ['^']
    TOK_DISP_DICTIONARY = {TOK_IDENT: "(ident)",TOK_SPACE: "space",TOK_STRING: "(string)",TOK_KEYWORD: "keyword",TOK_BREAK: "break",
                           TOK_CASE: "case",TOK_CATCH: "catch",TOK_CONTINUE: "continue",TOK_DBSTEP: "dbstep",TOK_ELSE: "else",
                           TOK_ELSEIF: "elseif",TOK_END: "end",TOK_FOR: "for",TOK_FUNCTION: "function",TOK_GLOBAL: "global",TOK_IF: "if",
                           TOK_KEYBOARD: "keyboard",TOK_OTHERWISE: "otherwise",TOK_PERSISTENT: "persistent",TOK_QUIT: "quit",
                           TOK_RETALL: "retall",TOK_RETURN: "return",TOK_SWITCH: "switch",TOK_TRY: "try",TOK_WHILE: "while",
                           TOK_MULTI: "multi",TOK_SPECIAL: "special",TOK_VARIABLE: "variable",TOK_DYN: "dyn",TOK_BLOCK: "block",
                           TOK_EOF: "eof",TOK_MATDEF: "matdef",TOK_CELLDEF: "celldef",TOK_PARENS: "()",TOK_BRACES: "{}",TOK_BRACKETS: "[]",
                           TOK_ROWDEF: "row",TOK_UNARY_MINUS: "u-",TOK_UNARY_PLUS: "u+",TOK_EXPR: "expr",TOK_DOTTIMES: ".*",TOK_DOTRDIV: "./",
                           TOK_DOTLDIV: ".\\",TOK_DOTPOWER: ".^",TOK_DOTTRANSPOSE: ".'",TOK_LE: "<=",TOK_GE: ">=",TOK_EQ: "==",TOK_NE: "~=",
                           TOK_SOR: "||",TOK_SAND: "&&",TOK_QSTATEMENT: "qstmnt",TOK_STATEMENT: "stmnt",TOK_REALF: "(real single)",
                           TOK_IMAGF: "(imag single)",TOK_REAL: "(real)",TOK_IMAG: "(imag)",TOK_FUNCTION_DEFS: "functions:",TOK_SCRIPT: "script:",
                           TOK_DBTRACE: "dbtrace",TOK_ANONYMOUS_FUNC: "anon func",TOK_NEST_FUNC: "nest func",TOK_TYPE_DECL: "type decl",
                           TOK_DBUP: "dbup",TOK_DBDOWN: "dbdown",TOK_REINDEX: "reindex",TOK_INCR: "++",TOK_DECR: "--",TOK_INCR_PREFIX: "++(pre)",
                           TOK_DECR_PREFIX: "--(pre)",TOK_INCR_POSTFIX: "++(post)",TOK_DECR_POSTFIX: "--(post)",TOK_PLUS_EQ: "+=",
                           TOK_MINUS_EQ: "-="}
    def __init__(self, tok = 0, pos = 0, text = ''):
        self.value = tok
        self.pos = pos
        self.text = text
    def isBinaryOperator(self):
        return self.value in Token.TOK_BINARY_OPERATORS
    def isUnaryOperator(self):
        return self.value in Token.TOK_UNARY_OPERATORS
    def isRightAssociative(self):
        return self.value in Token.TOK_RIGHT_ASSOCIATIVE_OPERATORS
    def __repr__(self):
        if self.value in Token.TOK_DISP_DICTIONARY:
            return Token.TOK_DISP_DICTIONARY[self.value] + self.text
        if self.value == '\n':
            return "newline"
        return self.value
    def __eq__(self,other):
        return self.value == other
    def __ne__(self,other):
        return self.value != other

        

