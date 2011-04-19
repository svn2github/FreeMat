#!/usr/bin/python
import os
import sys
import re

version = '4.1'
verstring = 'FreeMat v'+version

class ExpressionSet:
    docblock = ''
    modulename = ''
    moduledesc = ''
    sectionname = ''
    groupname = ''
    execin = ''
    execout = ''
    verbatimin = ''
    verbatimout = ''
    ccomment = ''
    figure = ''
    eqnin = ''
    eqnout = ''
    fnin = ''
    fnout = ''
    enumeratein = ''
    enumerateout = ''
    itemizein = ''
    itemizeout = ''
    item = ''
    def __init__(self,prefix):
        self.docblock =     '^' + prefix+'!'
        self.modulename =   prefix+'@Module\s*(\w*)'
        self.moduledesc =   prefix+'@Module\s*(.*)'
        self.sectionname =  prefix+'@@Section\s*(\w*)'
        self.groupname =    prefix+'@@([ \w]*)'
        self.execin =       prefix+'@<(\d+)?'
        self.execout =      prefix+'@>'
        self.verbatimin =   prefix+'@\['
        self.verbatimout =  prefix+'@\]'
        self.ccomment =     prefix+'(.*)'
        self.figure =       prefix+'@figure\s*(\w+)'
        self.eqnin =        prefix+'\\\\\['
        self.eqnout =       prefix+'\\\\\]';
        self.fnin =         prefix+'@{\s*(\S+)'
        self.fnout =        prefix+'@}'
        self.enumeratein =  prefix+'\\begin{enumerate}'
        self.enumerateout = prefix+'\\end{enumerate}'
        self.itemizein =    prefix+'\\\\begin{itemize}'
        self.itemizeout =   prefix+'\\\\end{itemize}'
        self.item =         prefix+'\s*\\\\item(.*)'
    
class Writer:
    """The base class for writers..."""
    def begingroup(self,groupname):
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname,section_descriptors):
        return
    def beginverbatim(self):
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
        return
    def doenumerate(self,enums):
        return
    def doequation(self,eqn):
        return
    def dofigure(self,figname):
        return
    def dofile(self,filename,text):
        return
    def doitemize(self,enums):
        return
    def endmodule(self):
        return
    def endverbatim(self):
        return
    def outputtext(self,text):
        return
    def writeindex(self):
        return

class WriterGroup(Writer):
    writers = []
    def addwriter(self,writer):
        self.writers.append(writer)
        return
    """The base class for writers..."""
    def begingroup(self,groupname):
        for writer in self.writers:
            writer.begingroup(groupname)
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname,section_descriptors):
        for writer in self.writers:
            writer.beginmodule(sourcepath,modname,moddesc,secname,section_descriptors)
        return
    def beginverbatim(self):
        for writer in self.writers:
            writer.beginverbatim()
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
        for writer in self.writers:
            writer.docomputeblock(cmds,errorsexpected,filename)
        return
    def doenumerate(self,enums):
        for writer in self.writers:
            writer.doenumerate(enums)
        return
    def doequation(self,eqn):
        for writer in self.writers:
            writer.doequation(eqn)
        return
    def dofigure(self,figname):
        for writer in self.writers:
            writer.dofigure(figname)
        return
    def dofile(self,filename,text):
        for writer in self.writers:
            writer.dofile(filename,text)
        return
    def doitemize(self,enums):
        for writer in self.writers:
            writer.doitemize(enums)
        return
    def endmodule(self):
        for writer in self.writers:
            writer.endmodule()
        return
    def endverbatim(self):
        for writer in self.writers:
            writer.endverbatim()
        return
    def outputtext(self,text):
        for writer in self.writers:
            writer.outputtext(text)
        return
    def writeindex(self):
        for writer in self.writers:
            writer.writeindex()
        return

def latin_filter(text):
    text = text.replace('&','&amp;')
    text = text.replace('<','&lt;')
    text = text.replace('>','&gt;')
    return text

class HTMLWriter(Writer):
    myfile = []
    sectables = {}
    eqnlist = []
    verbatim = False
    modulename = ''
    groupname = ''
    ignore = False
    section_descriptors = {}
    sourcepath = ''
    fp = -1
    def expand_codes(self,text):
        text = re.sub(r'\@\|([^\|]*)\|','<code>'r'\1''</code>',text)
        if (text == '\n'):
            text = '<P>\n'
        return text
    def begingroup(self,groupname):
        self.groupname = groupname
        if (groupname.lower() == 'tests'):
            self.ignore = True
        else:
            self.fp.write('<H3>%s</H3>\n'%(groupname))
            self.ignore = False
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname,section_descriptors):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.section_descriptors = section_descriptors
        self.sourcepath = sourcepath
        self.eqnlist = []
        filename = sourcepath + '/help/html/' + self.secname + '_' + self.modulename + '.html'
        self.fp = open(filename,'w')
        self.fp.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n')
        self.fp.write('\n')
        self.fp.write('<HTML>\n')
        self.fp.write('<HEAD>\n')
        self.fp.write('<TITLE>%s</TITLE>\n'%(moddesc))
        self.fp.write('</HEAD>\n')
        self.fp.write('<BODY>\n')
        self.fp.write('<H2>%s</H2>\n'%(moddesc))
        if (not self.secname in self.sectables):
            self.sectables[self.secname] = {}
        self.sectables[self.secname][self.modulename] = self.moddesc;
        self.fp.write('<P>\n')
        self.fp.write('Section: <A HREF=sec_%s.html> %s </A>\n'%(secname.lower(),section_descriptors[secname.lower()]))
        self.verbatim = False
        self.ignore = False
        return
    def beginverbatim(self):
        self.fp.write('<PRE>\n')
        self.verbatim = True
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
        self.fp.write('<PRE>\n')
        self.fp.write('<<<< ' + filename + ' >>>>\n')
        self.fp.write('</PRE>\n')
        return
    def doenumerate(self,enums):
        self.fp.write('<OL>\n')
        for item in enums:
            self.fp.write('<LI> %s </LI>\n'%(self.expand_codes(latin_filter(item))))
        self.fp.write('</OL>\n')
        return
    def doequation(self,eqn):
        self.eqnlist.append(eqn)
        self.fp.write('<P>\n')
        self.fp.write('<DIV ALIGN="CENTER">\n')
        self.fp.write('<IMG SRC="%s_eqn%d.png">\n'%(self.modulename,len(self.eqnlist)))
        self.fp.write('</DIV>\n')
        self.fp.write('<P>\n')
        return
    def dofigure(self,figname):
        self.fp.write('<P>\n');
        self.fp.write('<DIV ALIGN="CENTER">\n');
        self.fp.write('<IMG SRC="%s.png">\n'%(figname))
        self.fp.write('</DIV>\n');
        self.fp.write('<P>\n');
        return
    def dofile(self,filename,text):
        if (self.ignore):
            return
        self.fp.write('<P>\n<PRE>\n');
        self.fp.write('     %s\n'%(filename))
        self.fp.write('%s\n'%(latin_filter(text)))
        self.fp.write('</PRE>\n')
        self.fp.write('<P>\n')
        return
    def doitemize(self,enums):
        self.fp.write('<UL>\n')
        for item in enums:
            self.fp.write('<LI> %s </LI>\n'%(self.expand_codes(latin_filter(item))))
        self.fp.write('</UL>\n')
        return
        return
    def endmodule(self):
        self.fp.write('</BODY>\n')
        self.fp.write('</HTML>\n')
        self.fp.close()
        self.fp = -1
        if (len(self.eqnlist) > 0):
            f = open(self.sourcepath + '/help/tmp/%s_eqn.tex'%(self.modulename),'w')
            f.write('\\documentclass{article}\n')
            f.write('\\usepackage{amsmath}\n');
            f.write('\\pagestyle{empty}\n');
            f.write('\\begin{document}\n');
            for eqn in self.eqnlist:
                f.write('\\[\n');
                f.write(eqn.rstrip() + '\n');
                f.write('\\]\n');
                f.write('\\pagebreak\n');
            f.write('\\end{document}\n');
            f.close()
#    cdir = pwd;
#    cd('../tmp');
#    a = system(sprintf('latex %s_eqn.tex',p.modulename));
#    for i=1:numel(a)
#      if (~isempty(regexp(a{i},'Emergency stop')))
#        printf('Warning: equations for %s failed\n',p.modulename);
#      end
#    end
#    cd(cdir);
#    system(sprintf('dvipng -T tight ../tmp/%s_eqn.dvi',p.modulename));
#  end
        return
    def endverbatim(self):
        self.fp.write('</PRE>\n<P>\n')
        self.verbatim = False
        return
    def outputtext(self,text):
        if (self.ignore):
            return
        text = latin_filter(text)
        if (self.verbatim):
            self.fp.write(text)
        else:
            self.fp.write(self.expand_codes(text))
        return
    def writeindex(self):
        for secname in self.section_descriptors:
            if secname in self.sectables:
                self.writesectiontable(secname,self.sectables[secname])
        f = open(self.sourcepath + '/help/html/index.html','w')
        f.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n')
        f.write('\n')
        f.write('<HTML>\n')
        f.write('<HEAD>\n')
        f.write('<TITLE>%s Documentation</TITLE>\n'%(verstring))
        f.write('</HEAD>\n')
        f.write('<BODY>\n')
        f.write('<H1>%s Documentation</H1>\n'%(verstring))
        f.write('<P>\n')
        f.write('<H2> Documentation Sections </H2>\n')
        f.write('<UL>\n')
        for secname in self.section_descriptors:
            f.write('<LI> <A HREF=sec_%s.html> %s </A> </LI>\n'%(secname,self.section_descriptors[secname]))
        f.write('</UL>\n')
        f.write('</BODY>\n')
        f.write('</HTML>\n')
        f.close()
        f = open(self.sourcepath + '/help/html/modules.txt','w')
        modulenames = []
        for section in self.section_descriptors:
            if (section in self.sectables):
                modules = self.sectables[section]
                for module in modules:
                    modulenames.append(module + ' (' + section + ')')
        for mod in modulenames:
            f.write(mod + '\n')
        f.close()
        f = open(self.sourcepath + '/help/html/sectable.txt','w')
        for section in self.section_descriptors:
            f.write(self.section_descriptors[section] + '\n')
            if (section in self.sectables):
                for module in self.sectables[section]:
                    f.write('+(%s) %s\n'%(section,module))
        f.close()
        return
    def writesectiontable(self,secname,modules):
        f = open('%s/help/html/sec_%s.html'%(self.sourcepath,secname),'w')
        f.write('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n')
        f.write('\n')
        f.write('<HTML>\n')
        f.write('<HEAD>\n')
        f.write('<TITLE>%s</TITLE>\n'%(self.section_descriptors[secname]))
        f.write('</HEAD>\n')
        f.write('<BODY>\n')
        f.write('<H2>%s</H2>\n'%(self.section_descriptors[secname]))
        f.write('<P>\n')
        f.write('<A HREF=index.html> Main Index </A>\n')
        f.write('<P>\n')
        f.write('<UL>\n')
        for module in modules:
            f.write('<LI> <A HREF=%s_%s.html> %s </A> %s </LI>\n'%(secname,module,module,self.sectables[secname][module]))
        f.write('</UL>\n')
        f.write('</BODY>\n')
        f.write('</HTML>\n')
        f.close()
        return

class LaTeXWriter(Writer):
    myfile = []
    sectables = {}
    eqnlist = []
    verbatim = False
    modulename = ''
    groupname = ''
    ignore = False
    section_descriptors = {}
    sourcepath = ''
    fp = -1
    def expand_codes(self,text):
        text = re.sub(r'\@\|([^\|]*)\|','\\\\verb|'r'\1''|',text)
        return text
    def begingroup(self,groupname):
        self.groupname = groupname
        if (groupname.lower() == 'tests'):
            self.ignore = True
        else:
            self.fp.write('\\subsection{%s}\n\n'%(groupname))
            self.ignore = False
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname,section_descriptors):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.section_descriptors = section_descriptors
        self.sourcepath = sourcepath
        self.eqnlist = []
        filename = sourcepath + '/help/latex/' + self.secname + '_' + self.modulename + '.tex'
        self.fp = open(filename,'w')
        self.fp.write('\\section{%s}\n\n'%(moddesc.rstrip()))
        if (not self.secname in self.sectables):
            self.sectables[self.secname] = {}
        self.sectables[self.secname][self.modulename] = self.moddesc;
        self.verbatim = False
        self.ignore = False
        return
    def beginverbatim(self):
        self.fp.write('\\begin{verbatim}\n')
        self.verbatim = True
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
        self.fp.write('\\begin{verbatim}\n')
        self.fp.write('<<<< ' + filename + ' >>>>\n')
        self.fp.write('\\end{verbatim}\n')
        return
    def doenumerate(self,enums):
        self.fp.write('\\begin{enumerate}\n')
        for item in enums:
            self.fp.write('\\item %s\n'%(self.expand_codes(item)))
        self.fp.write('\\end{enumerate}\n')
        return
    def doequation(self,eqn):
        self.fp.write('\\[\n%s\\]\n'%(eqn))
        return
    def dofigure(self,figname):
        self.fp.write('\n\n');
        self.fp.write('\\centerline{\\includegraphics[width=8cm]{%s}}\n\n'%(figname))
        return
    def dofile(self,filename,text):
        if (self.ignore):
            return
        self.fp.write('\\begin{verbatim}\n')
        self.fp.write('    %s\n'%(filename))
        self.fp.write('%s'%(text))
        self.fp.write('\\end{verbatim}\n')
        return
    def doitemize(self,enums):
        self.fp.write('\\begin{itemize}\n')
        for item in enums:
            self.fp.write('\\item %s\n'%(self.expand_codes(item)))
        self.fp.write('\\end{itemize}\n')
        return
    def endmodule(self):
        self.fp.close()
        self.fp = -1
        return
    def endverbatim(self):
        self.fp.write('\\end{verbatim}\n')
        self.verbatim = False
        return
    def outputtext(self,text):
        if (self.ignore):
            return
        if (self.verbatim):
            self.fp.write(text)
        else:
            self.fp.write(self.expand_codes(text))
        return
    def writeindex(self):
        f = open(self.sourcepath + '/help/latex/FreeMat-'+version+'.tex','w')
        f.write('\\documentclass{book}\n')
        f.write('\\usepackage{graphicx}\n')
        f.write('\\usepackage{amsmath}\n')
        f.write('\\renewcommand{\\topfraction}{0.9}\n')
        f.write('\\renewcommand{\\floatpagefraction}{0.9}\n')
        f.write('\\oddsidemargin 0.0in\n')
        f.write('\\evensidemargin 1.0in\n')
        f.write('\\textwidth 6.0in\n')
        f.write('\\title{%s Documentation}\n'%(verstring))
        f.write('\\author{Samit Basu}\n')
        f.write('\\begin{document}\n')
        f.write('\\maketitle\n')
        f.write('\\tableofcontents\n')
        for section in self.section_descriptors:
            if section in self.sectables:
                f.write('\\chapter{%s}\n'%(self.section_descriptors[section]))
                for mod in self.sectables[section]:
                    f.write('\\input{%s_%s}\n'%(section,mod.lower()))
        f.write('\\end{document}\n')
        f.close()
        return

class BBTestWriter(Writer):
    moddesc = ''
    empty = True
    modulename = ''
    blacklist = {'retall','keyboard','return','where'}
    secname = ''
    section_descriptors = {}
    sourcepath = ''
    filename = ''
    fp = []
    def docomputeblock(self,cmds,errorsexpected,filename):
        if (self.modulename not in self.blacklist):
            self.fp.write('NumErrors = 0\n')
            for cmd in cmds:
                self.fp.write('try\n')
                self.fp.write('  ' + cmd + '\n')
                self.fp.write('catch\n')
                self.fp.write('  NumErrors = NumErrors + 1\n')
                self.fp.write('end\n')
            self.fp.write('if (NumErrors ~= %d) bbtest_success = 0 return end\n'%(errorsexpected))
        self.empty = False
    def beginmodule(self,sourcepath,modname,moddesc,secname,section_descriptors):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.section_descriptors = section_descriptors
        self.sourcepath = sourcepath
        self.filename = sourcepath + '/toolbox/test/bbtest_' + self.secname + '_' + self.modulename + '.m'
        self.fp = open(self.filename,'w+')
        self.fp.write('%% Regression test function (black box) for %s\n'%(self.modulename))
        self.fp.write('%% This function is autogenerated by helpgen.py\n')
        self.fp.write('function bbtest_success = bbtest_%s_%s\n'%(self.secname,self.modulename))
        self.fp.write('  bbtest_success = 1\n')
        self.empty = True
        return
    def endmodule(self):
        self.fp.close()
## TODO... delete empty tests
##        if (self.empty):
            


class TextWriter(Writer):
    myfile = []
    sectables = {}
    eqnlist = []
    verbatim = False
    modulename = ''
    groupname = ''
    ignore = False
    section_descriptors = {}
    sourcepath = ''
    def expand_codes(self,text):
        return re.sub(r'\@\|([^\|]*)\|',r'\1',text)
    def begingroup(self,groupname):
        self.groupname = groupname
        if (groupname.lower() != 'usage'):
            self.ignore = True
        else:
            self.myfile.write('Usage\n\n')
            self.ignore = False
        return
    def beginmodule(self,sourcepath,modname,moddesc,secname,section_descriptors):
        self.moddesc = moddesc
        self.secname = secname.lower()
        self.modulename = modname.lower()
        self.section_descriptors = section_descriptors
        self.sourcepath = sourcepath
        filename = sourcepath + '/help/text/' + self.modulename + '.mdc'
        self.myfile = open(filename,'w+')
        self.myfile.write('%s\n'%(moddesc))
        self.verbatim = False
        self.ignore = True
        if (not self.secname in self.sectables):
            self.sectables[self.secname] = {}
        self.sectables[self.secname][self.modulename] = self.moddesc
        return
    def beginverbatim(self):
        if (self.ignore):
            return
        self.verbatim = True
        self.myfile.write('\n')
        return
    def docomputeblock(self,cmds,errorsexpected,filename):
#        self.myfile.write('<<<< ' + filename + ' >>>>\n')
        return
    def doenumerate(self,enums):
        if (self.ignore):
            return
        for i,item in enumerate(enums):
            self.myfile.write('  %d. %s\n'%(i,self.expand_codes(item)))
        return
    def doitemize(self,enums):
        if (self.ignore):
            return
        for item in enums:
            self.myfile.write('  - %s\n'%(item))
        return
    def endmodule(self):
        self.myfile.close()
        return
    def endverbatim(self):
        if (self.ignore):
            return
        self.myfile.write('\n')
        self.verbatim = False
        return
    def outputtext(self,text):
        if (self.ignore):
            return
        self.myfile.write('%s'%(self.expand_codes(text)))
        return

class HelpGen:
    pline = ''
    fp = []
    pset = []
    writers = []
    section_descriptors = {}
    sourcepath = ''
    finished = False 
    genfiles = []
    fragment_counter = 0
    modname = ''
    secname = ''
    def __init__(self,sourcepath):
        self.writers = WriterGroup()
        self.writers.addwriter(TextWriter())
        self.writers.addwriter(BBTestWriter())
        self.writers.addwriter(HTMLWriter())
        self.writers.addwriter(LaTeXWriter())
        self.sourcepath = sourcepath
        fp = open(sourcepath + '/src/toolbox/help/section_descriptors.txt','r')
        for line in fp:
            m = re.search(r'(\w*)\s*([^\n]*)',line)
            self.section_descriptors[m.group(1)] = m.group(2)
        fp.close()
        return
    def testmatch(self,rtext):
        return re.search(rtext,self.pline,re.DOTALL)
        return
    def mustmatch(self,rtext):
        match = re.search(rtext,self.pline,re.DOTALL)
        if (match):
            return match.group(1)
        else:
            exit('Error: match failed to find ' + rtext + ' expression in ' + self.pline)
        return
    def getline(self):
        return self.fp.readline()
    def nextline(self):
        self.pline = self.getline()
        return
    def handle_output(self):
        self.pline = self.mustmatch(self.pset.ccomment)
        self.writers.outputtext(self.pline)
        self.nextline()
        return
    def handle_verbatim(self):
        self.nextline()
        self.writers.beginverbatim()
        while not self.testmatch(self.pset.verbatimout):
            self.writers.outputtext(self.mustmatch(self.pset.ccomment))
            self.nextline()
        self.writers.endverbatim()
        self.nextline()
        return
    def handle_figure(self):
        self.writers.dofigure(self.mustmatch(self.pset.figure))
        self.nextline()
    def handle_equation(self):
        self.nextline()
        eqn = ''
        while not self.testmatch(self.pset.eqnout):
            eqn += self.mustmatch(self.pset.ccomment)
            self.nextline()
        self.writers.doequation(eqn)
        self.nextline()
        return
    def handle_filedump(self):
        fname = self.mustmatch(self.pset.fnin)
        self.nextline()
        fn = ''
        while not self.testmatch(self.pset.fnout):
            fn += self.mustmatch(self.pset.ccomment)
            self.nextline()
        self.genfiles.append(fname)
        zp = open(self.sourcepath + '/help/tmp/' + fname,'w+')
        zp.write(fn)
        zp.close()
        if (not re.search('test_\w+',fname)):
            self.writers.dofile(fname,fn)
        self.nextline()
    def handle_itemize(self):
        self.nextline()
        itemlist = []
        while not self.testmatch(self.pset.itemizeout):
            item = self.mustmatch(self.pset.item)
            self.nextline()
            while not self.testmatch(self.pset.item) and not self.testmatch(self.pset.itemizeout):
                item  += self.mustmatch(self.pset.ccomment)
                self.nextline()
            itemlist.append(item)
        ###if (feof(fp)), error('unmatched enumeration block') end
        self.nextline()
        self.writers.doitemize(itemlist)
        return
    def handle_exec(self):
        match = self.mustmatch(self.pset.execin)
        if (isinstance(match,str)):
            errors_expected = int(self.mustmatch(self.pset.execin))
        else:
            errors_expected = 0
        self.nextline()
        cmdlist = []
        while not self.testmatch(self.pset.execout):
            cmdlist.append(self.mustmatch(self.pset.ccomment))
            self.nextline()
        fragnum = '{:03d}'.format(self.fragment_counter)
        fragname = 'frag_%s_%s_%s'%(self.secname,self.modname,fragnum)
        fragfile = self.sourcepath + '/help/tmp/' + fragname + '.m'
        fp = open(fragfile,'w')
        fp.write(str(errors_expected) + '\n')
        for cmd in cmdlist:
            fp.write(cmd)
        fp.close()
        self.fragment_counter = self.fragment_counter + 1
        self.writers.docomputeblock(cmdlist,errors_expected,fragfile)
        self.nextline()
        return
    def writeindex(self):
        self.writers.writeindex()
        return
    def process_dir(self,dirname):
        if (re.search('\.svn',dirname)):
            return
        print('Processing dir %s'%(dirname))
        names = os.listdir(dirname)
        blacklist = {'dependencies','private'}
        for name in names:
            fname = os.path.join(dirname,name)
            if os.path.isdir(fname) and not name in blacklist:
                self.process_dir(fname)
            else:
                (root, ext) = os.path.splitext(fname)
                if ext == '.cpp':
                    self.pset = ExpressionSet('//')
                    self.process_file(fname)
                elif ext == '.m':
                    self.pset = ExpressionSet('%')
                    self.process_file(fname)
    def process_file(self,filename):
        print('Processing file %s'%(filename))
        self.fp = open(filename,'r')
        self.finished = False
        while not self.finished:
            self.pline = self.fp.readline()
            self.finished = self.pline == ''
            if (self.testmatch(self.pset.docblock)):
                self.nextline()
                self.modname = self.mustmatch(self.pset.modulename).lower()
                print('    Module %s...'%(self.modname.lower()))
                self.fragment_counter = 0
                moddesc = self.mustmatch(self.pset.moduledesc)
                self.nextline()
                self.secname = self.mustmatch(self.pset.sectionname).lower()
                self.writers.beginmodule(self.sourcepath,self.modname,moddesc,self.secname,self.section_descriptors)
                self.nextline()
                while not self.testmatch(self.pset.docblock):
                    groupname = self.mustmatch(self.pset.groupname)
                    if (re.search('Signature',groupname)):
                        self.nextline()
                        while (not self.testmatch(self.pset.groupname) and not self.testmatch(self.pset.docblock)):
                            self.nextline()
                    else:
                        self.writers.begingroup(groupname)
                        self.nextline()
                        while (not self.testmatch(self.pset.groupname) and not self.testmatch(self.pset.docblock)):
                            if (self.testmatch(self.pset.execin)):
                                self.handle_exec()
                            elif (self.testmatch(self.pset.verbatimin)):
                                self.handle_verbatim()
                            elif (self.testmatch(self.pset.figure)):
                                self.handle_figure()
                            elif (self.testmatch(self.pset.eqnin)):
                                self.handle_equation()
                            elif (self.testmatch(self.pset.fnin)):
                                self.handle_filedump()
                            elif (self.testmatch(self.pset.enumeratein)):
                                self.handle_enumerate()
                            elif (self.testmatch(self.pset.itemizein)):
                                self.handle_itemize()
                            elif (self.testmatch(self.pset.ccomment)):
                                self.handle_output()
                            else:
                                exit('Unprocessed line:' + self.pline)
                self.writers.endmodule()
        self.fp.close()    

dirname = sys.argv[1]
h = HelpGen(dirname)
h.process_dir(dirname)
h.writeindex()
