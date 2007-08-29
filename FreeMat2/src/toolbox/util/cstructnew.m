%!
%@Module CSTRUCTNEW Create New Instance of C Structure
%@@Section EXTERNAL
%@@Usage
%The @|cstructnew| function is a convenience function for
%creating a FreeMat structure that corresponds to a C
%structure.  The entire structure is initialized with zeros.
%This has some negative implications, because if the 
%structure definition uses @|cenums|, they may come out
%as @|'unknown'| values if there are no enumerations corresponding
%to zero.  The use of the function is
%@[
%   a = cstructnew('typename')
%@]
%which creates a single structure of C structure type @|'typename'|.
%To create an array of structures, we can provide a second argument
%@[
%   a = cstructnew('typename',count)
%@]
%where @|count| is the number of elements in the structure array.
%!
function a = cstructnew(typename,count)
  if (nargin == 1)
    count = 1;
  end
  if (nargin == 0)
    error('cstructnew requires a typename argument');
  end
  a = cstructthaw(zeros(1,cstructsize(typename,count),'uint8'),typename,count);
