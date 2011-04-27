import os
import sys
import re
import shutil
import glob

def visit(arg, dirname, names):
    for name in names:
        (root, ext) = os.path.splitext(name)
        if (ext == '.m'):
            print('merging file %s'%(name))
            headertext = ''
            if os.path.exists('../help/text/%s.mdc'%(root)):
                h = open('../help/text/%s.mdc'%(root),'r')
                for line in h:
                    headertext = headertext + '% ' + line
                h.close()
            h = open('%s/%s'%(dirname,name),'r')
            g = open('../%s/%s'%(dirname,name),'w')
            skip = False
            for line in h:
                if (line.rstrip() == '%!'):
                    skip = not skip
                if (not skip):
                    if (line.rstrip() != '%!'):
                        g.write(line)
                    else:
                        g.write(headertext)
            g.close()
            h.close()
            

basedir = sys.argv[1]
os.chdir(basedir)
# Merge in the fragment text
os.path.walk('.', visit, '')
