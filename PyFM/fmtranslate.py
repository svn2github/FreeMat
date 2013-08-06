"""
The translate modules provides the :class"`translator`, which outputs Python for the given AST.
"""

from fmtree import Tree
from fmtoken import Token

class Emitter:
    def __init__(self,file):
        self.file = file
    def write(self,text):
        self.file.write(text)
    def quote(self,text):
        self.file.write('"')
        self.file.write(text)
        self.file.write('"')
    def fcallstart(self,func):
        self.file.write(func+"(")
    def comma(self):
        self.file.write(",")
    def fcallend(self):
        self.file.write(")")
    def mcallstart(self,func):
        self.file.write("."+func+"(")
    def mcallend(self):
        self.file.write(")")
    def liststart(self):
        self.file.write('[')
    def listend(self):
        self.file.write(']')
    def tuplestart(self):
        self.file.write('(')
    def tupleend(self):
        self.file.write(')')

class Translator:
    BINARY_OPNAMES = {'+':'add',
                      '-':'minus',
                      '*':'times',
                      '/':'rdivide',
                      '\\':'ldivide'}
    UNARY_OPNAMES = {Token.TOK_UNARY_MINUS:'uminus',
                     Token.TOK_UNARY_PLUS:'uplus',
                     '~':'not'}
    def __init__(self,emitter):
        self.emitter = emitter
    def literal(self,tree):
        self.emitter.write(tree.text())
    def unary(self,tree):
        self.emitter.fcallstart(self.UNARY_OPNAMES[tree.node.value])
        self.expression(tree.first())
        self.emitter.fcallend()
    def binary(self,tree):
        self.emitter.fcallstart(self.BINARY_OPNAMES[tree.node.value])
        self.expression(tree.first())
        self.emitter.comma()
        self.expression(tree.second())
        self.emitter.fcallend()
    def expression(self, tree):
        if (tree == Token.TOK_VARIABLE): return self.rhs(tree)
        if (tree in [Token.TOK_REAL,Token.TOK_IMAG,Token.TOK_REALF,Token.TOK_IMAGF,Token.TOK_STRING]):
            return self.literal(tree)
        if (tree in ['+','-','*','/','\\']): return self.binary(tree)
        if (tree in [Token.TOK_UNARY_MINUS,Token.TOK_UNARY_PLUS,'~']): return self.unary(tree)
        return
    def indexType(self,tree):
        self.emitter.tuplestart()
        if tree == Token.TOK_PARENS:
            self.emitter.quote('()')
            self.emitter.comma()
            self.expression(tree.first())
        if tree == '.':
            self.emitter.quote('.')
            self.emitter.comma()
            self.emitter.quote(tree.first().text())
        if tree == Token.TOK_BRACES:
            self.emitter.quote('{}')
            self.emitter.comma()
            self.expression(tree.first())
        self.emitter.tupleend()
    def assignment(self,tree):
        self.emitter.write(tree.first().first().text())
        self.emitter.mcallstart('assign')
        self.emitter.liststart()
        for child in tree.first().children[1:]:
            self.indexType(child)
            self.emitter.comma()
        self.emitter.listend()
        self.emitter.comma()
        self.expression(tree.second())
        self.emitter.mcallend()
## Collect into a call:
#  varname.assign([types],[indices],value)
## Worry about performance improvements later.
    def statementType(self,tree,quiet):
        if (tree == '='): return self.assignment(tree)
    def statement(self,tree):
        if (tree == Token.TOK_QSTATEMENT):
            self.statementType(tree.first(),False)
        else:
            self.statementType(tree.first(),True)
        self.emitter.write('\n')
    def block(self,tree):
        if (tree != Token.TOK_BLOCK):
            raise NameError("Wha?")
        for statement in tree.children:
            self.statement(statement)

