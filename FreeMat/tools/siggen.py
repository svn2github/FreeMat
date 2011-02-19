#!/usr/bin/python
import os
import sys

gfuncs = [];

def visit(arg, dirname, names):
    for name in names:
        (root, ext) = os.path.splitext(name)
        if ext == '.cpp':
            fullpath = os.path.join(dirname,name)
            f = open(fullpath,'r')
            line = ' '
            while line != '':
                line = f.readline()
                if (line.startswith("//@@Signature")):
                    fline = f.readline().rstrip()
                    iline = f.readline()
                    oline = f.readline()
                    fsplit = fline.split()
                    function_type = fsplit[0][2:]
                    function_name = fsplit[1]
                    function_internal_name = fsplit[2];
                    if (function_name == '') or (function_internal_name == ''):
                        print 'MISSING Function Name: <%s> <%s> <%s>'%(fline,function_name,function_internal_name)
                    inputs = iline.split()
                    inputs = inputs[1:]
                    outputs = oline.split()
                    outputs = outputs[1:]
                    data = {"type": function_type,
                            "name": function_name,
                            "internal_name": function_internal_name,
                            "inputs": inputs,
                            "outputs": outputs}
                    gfuncs.append(data);
            f.close()
#  print 'Visiting file arg %s dirname %s name %s extension %s t = %s'%(arg,dirname,name,ext,t)

g = open('tst.cpp','w')
g.write("// WARNING - This file is automatically generated by\n");
g.write("// the siggen.py script in tools.  Do not\n");
g.write("// edit it by hand, or changes will be lost!  If you\n");
g.write("// need to add a built in function to FreeMat, add a\n");
g.write("// signature block to its documentation and rerun siggen.\n");
g.write("\n\n// First each function gets prototyped.\n");
g.write('#include "Array.hpp"\n');
g.write('#include "Context.hpp"\n');
g.write("\n\n");

os.path.walk(sys.argv[1],visit,'')

for fnc in gfuncs:
    if (fnc["type"] == "function"):
        g.write('ArrayVector %s(int, const ArrayVector&);\n'%fnc["internal_name"])
    elif (fnc["type"] == "sfunction"):
        g.write('ArrayVector %s(int, const ArrayVector&, Interpreter*);\n'%fnc["internal_name"])
    elif (fnc["type"] == "gfunction"):
        g.write('ArrayVector %s(int, const ArrayVector&);\n'%fnc["internal_name"])
    elif (fnc["type"] == "sgfunction"):
        g.write('ArrayVector %s(int, const ArrayVector&, Interpreter*);\n'%fnc["internal_name"])


funcmapping = {"function":"Function","sfunction":"SpecialFunction","gfunction":"GfxFunction","sgfunction":"GfxSpecialFunction"};

g.write('\n\nvoid LoadBuiltinFunctions(Context *context, bool guiflag) {\n');
for fnc in gfuncs:
  if (fnc["type"] == "gfunction" or fnc["type"] == "sgfunction"):
      g.write('  if (guiflag)')
  g.write('  context->add%s("%s",%s,'%(funcmapping[fnc["type"]],fnc["name"],fnc["internal_name"]))
  if fnc["inputs"][0]=='none':
      input_count = 0
  elif fnc["inputs"][0]=='varargin':
      input_count = -1
  else:
      input_count = len(fnc["inputs"])
  if fnc["outputs"][0]=='none':
      output_count = 0
  elif fnc["outputs"][0]=='varargout':
      output_count = -1
  else:
      output_count = len(fnc["outputs"])
  g.write('%d,%d,'%(input_count,output_count))
  if (input_count > 0):
    for p in fnc["inputs"]:
        g.write('"%s",'%(p))
  g.write('NULL);\n');
g.write('}\n\n');

