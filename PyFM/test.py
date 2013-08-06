import fmscanner as fm
import fmparser as fm
import os
import sys
import time

f = open(sys.argv[1],'r')
text = f.read()
tic = time.clock()
a = fm.Scanner(text)
b = fm.Parser(a)
c = b.process()
#print c
toc = time.clock()
print "Elapsed time %f" % (toc-tic)
