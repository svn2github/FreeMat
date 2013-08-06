import fmscanner as fms
import fmparser as fmp
import fmtranslate as fmt
import os
import sys
import time

f = open(sys.argv[1],'r')
text = f.read()
e = fmt.Emitter(sys.stdout)
t = fmt.Translator(e)
a = fms.Scanner(text)
b = fmp.Parser(a)
c = b.statementList()
print c
print '*'*80
t.block(c)

