from fmtree import Tree
from fmtoken import Token
a = Tree(Token('a'))
b = Tree(Token('b'))
c = Tree(Token('+'),a,b)
print c
