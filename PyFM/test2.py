import os
import sys
import time
import numpy

def testfunc():
    tic = time.clock()
    N = 1000000
    a = numpy.zeros(N)
    for i in numpy.arange(0,N):
        a[i] = i + 1
    toc = time.clock()
    print "Elapsed time %f" % (toc-tic)

def testfunc1():
    tic = time.clock()
    N = 1000000
    a = numpy.zeros(N)
    for i in numpy.arange(0,N):
        a[i] = i + 1
    toc = time.clock()
    print "Elapsed time %f" % (toc-tic)


def testfunc2(rmin,rmax,N):
    tic = time.clock()
    a = numpy.zeros(N)
    if (rmin == int(rmin) and rmax == int(rmax)):
        for_range = range(int(rmin),int(rmax))
    else:
        for_range = numpy.arange(rmin,rmax)
    for i in for_range:
        a[i] = i + 1
    toc = time.clock()
    print "Elapsed time %f" % (toc-tic)

def testfunc2b(rmin,rmax,N):
    tic = time.clock();
    a = numpy.zeros(N)
    for_range = numpy.arange(rmin,rmax)
    for i in range(0,for_range.size):
        a[for_range[i]] = for_range[i] + 1
    toc = time.clock()
    print "Elapsed time %f" % (toc-tic)
    

def add(a,b):
    return a + b

def testfunc3():
    tic = time.clock()
    N = 1000000
    a = numpy.zeros(N)
    for i in range(0,N):
        a[i] = add(i,1)
    toc = time.clock()
    print "Elapsed time %f" % (toc-tic)

def dot(a,b):
    return a.dot(b)

def testfunc4():
    tic = time.clock()
    N = 1000000
    a = numpy.zeros(N)
    for i in range(0,N):
        a[i] = add(i,1)
    toc = time.clock()
    print "Elapsed time %f" % (toc-tic)

def testfunc5():
    tic = time.clock()
    N = 1000000
    a = numpy.zeros(N)
    p1 = numpy.array([1,2])
    p2 = numpy.array([3,4])
    for i in range(0,N):
        a[i] = i + dot(p1,p2)
    toc = time.clock()
    print "Elapsed time %f" % (toc-tic)

class fmarray :
    def __init__(self,*N):
        self.a = numpy.zeros(N)
    def __call__(self,*ndx):
        return self.a[ndx]
    def __setitem__(self,arg,val):
        self.a.__setitem__(arg,val)

def testfunc6():
    tic = time.clock()
    N = 1000000
    a = numpy.zeros(N)
    accum = 0
    for i in range(0,N):
        accum = accum + a[i]
    toc = time.clock()
    print "Elapsed time %f" % (toc-tic)

def testfunc7():
    tic = time.clock()
    N = 1000000
    a = fmarray(N)
    accum = 0
    for i in range(0,N):
        accum = accum + a(i)
    toc = time.clock()
    print "Elapsed time %f" % (toc-tic)

def testfunc8():
    tic = time.clock()
    N = 1000000
    a = fmarray(N,3)
    accum = 0
    for i in range(0,N):
        accum = accum + a(i,2)
    toc = time.clock()
    print "Elapsed time %f" % (toc-tic)

def testfunc9():
    a = fmarray(4,4)
    a[2:3,2:3] = 1
    print a.a
