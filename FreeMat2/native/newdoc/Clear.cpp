//**
@Module CLEAR Clear or Delete a Variable
@@Usage
Clears a set of variables from the current context, or alternately, 
delete all variables defined in the current context.  There are
two formats for the function call.  The first is the explicit form
in which a list of variables are provided:
@[
   clear a1 a2 ...
@]
The variables can be persistent or global, and they will be deleted.
The second form
@[
   clear 'all'
@]
clears all variables from the current context.
@@Example
Here is a simple example of using @|clear| to delete a variable.  
First, we create a variable called @|a|.
@<
a=53
@>
Next, we clear @|a| using the @|clear| function, and verify that 
it is deleted.
@<
clear a
a
@>
**//
#include "Array.hpp"
#include "WalkTree.hpp"
#include "Malloc.hpp"
#include <stdio.h>

namespace FreeMat {
  ArrayVector ClearFunction(int nargout, const ArrayVector& arg, WalkTree* eval) {
    int i;
    stringVector names;
    if (arg.size() == 0) 
      throw Exception("clear function expects either a list of variables to clear or the argument 'all' to clear all currently defined variables");
    if (arg.size() == 1) {
      Array singleArg(arg[0]);
      char * singleArgC;
      singleArgC = singleArg.getContentsAsCString();
      if (strcmp(singleArgC,"all") == 0)
	names = eval->getContext()->getCurrentScope()->listAllVariables();
      else
	names.push_back(singleArgC);
    } else {
      for (i=0;i<arg.size();i++) {
	Array varName(arg[i]);
	names.push_back(varName.getContentsAsCString());
      }
    }
    for (i=0;i<names.size();i++) {
      eval->getContext()->deleteVariable(names[i]);
    }
    return ArrayVector();
  }
}
#endif
