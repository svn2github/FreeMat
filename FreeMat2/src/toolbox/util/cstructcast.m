%!
%@Module CSTRUCTCAST Cast FreeMat Structure to C Structure
%@@Section EXTERNAL
%@@Usage
%The @|cstructcast| function is a convenience function for ensuring that
%a FreeMat structure fits the definition of a C struct (as defined
%via @|cstructdefine|.  It does so by encoding the structure
%to a byte array using @|cstructfreeze| and then recovering it using
%the @|cstructthaw| function.  The usage is simply
%@[
%   s = cstructcast(s,typename)
%@]
%where @|s| is the structure and @|typename| is the name of the
%C structure that describes the desired layout and types for elements
%of @|s|.  This function is equivalent to calling @|cstructfreeze|
%and @|cstructthaw| in succession on a FreeMat structure.
%!
function s = cstructcast(s,typename)
  if (nargin ~= 2) error('cstructcast requires a struct and a typename'); end
  s = cstructthaw(cstructfreeze(s,typename),typename);
