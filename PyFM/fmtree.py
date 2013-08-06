"""
The tree module provides the :class:`tree`, which provides the data structure to hold the AST.
"""

from fmtoken import Token

class Tree:
    indent_level = 0
    def __init__(self, token = Token(), child1 = None, child2 = None):
        self.node = token
        self.children = []
        if (child1):
            self.children.append(child1)
        if (child2):
            self.children.append(child2)
    def __repr__(self):
        out = ' ' * Tree.indent_level + str(self.node) + '\n'
        Tree.indent_level += 3
        for child in self.children:
            out += str(child)
        Tree.indent_level -= 3
        return out
    def text(self):
        return self.node.text
    def rename(self,tok):
        self.node.value = tok
    def valid(self):
        return self.node != Token.TOK_INVALID
    def first(self):
        return self.children[0]
    def second(self):
        return self.children[1]
    def __eq__(self,other):
        return self.node == other
    def __ne__(self,other):
        return self.node != other
    def numChildren(self):
        return len(self.children)
    def last(self):
        return self.children[-1]
    def addChild(self, child):
        self.children.append(child)
    def addChildren(self, child1, child2):
        self.children.append(child1)
        self.children.append(child2)
    
    

