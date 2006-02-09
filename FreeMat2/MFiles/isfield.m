%!
%@Module ISFIELD Test for Existence of a Structure Field
%@@Section INSPECTION
%@@Usage
%Given a structure array, tests to see if that structure
%array contains a field with the given name.  The syntax
%for its use is
%@[
%  y = isfield(x,field)
%@]
%and returns a logical @|1| if @|x| has a field with the 
%name @|field| and a logical @|0| if not.  It also returns
%a logical @|0| if the argument @|x| is not a structure array.
%@@Example
%Here we define a simple struct, and then test for some 
%fields
%@<
%a.foo = 32
%a.goo = 64
%isfield(a,'goo')
%isfield(a,'got')
%isfield(pi,'round')
%@>
%!

% Copyright (c) 2002-2006 Samit Basu

function y = isfield(x,field)
  if (isa(x,'struct'))
    y = any(strcmp(fieldnames(x),field));
  else
    y = logical(0);
  end
