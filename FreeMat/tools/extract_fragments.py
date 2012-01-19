#!/usr/bin/python
# This script extracts the fragments from doc files and
# writes them to fragment directories.  
import os
import sys
import re
import shutil
import glob

basedir = ''

def visit(arg, dirname, names):
    for name in names:
        (root, ext) = os.path.splitext(name)
        if ext=='.doc':
            print('Processing file %s'%(name))
            f = open(dirname + '/' + name, 'rU')
            line = f.readline()
            while line:
                if (line.find('if FRAGMENT') != -1):
                    fragfilename = f.readline()[:-1]
                    print('Fragment %s'%(fragfilename))
                    line = f.readline()
                    while (line.find(r'\endif') == -1):
                        print('  >%s'%(line[:-1]))
                        line = f.readline()
                line = f.readline()

basedir = sys.argv[1]
os.path.walk(basedir, visit, '')


