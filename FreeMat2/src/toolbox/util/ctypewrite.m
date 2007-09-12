%!
%@Module CTYPEWRITE Write a C Typedef To File
%@@Section EXTERNAL
%@@Usage 
%The @|ctypewrite| function is a convenience function for
%writing a C typedef to a file.  This is generally a very
%bad idea, as writing of C typedefs to files is notoriously
%unportable.  Consider yourself warned.  The syntax for this
%function is 
%@[
%  ctypewrite(fid,a,'typename')
%@]
%where @|a| is the FreeMat typedef to write, @|'typename'| is
%a string containing the name of the C typedef to use when
%writing the typedef to the file (previously defined using
%@|ctypedefine|), and @|fid| is the file handle returned
%by @|fopen|.
%!
function ctypewrite(fid,a,typename)
  if (nargin < 3), error('ctypewrite requires 3 args, the fid, variable, and typename'); end;
  fwrite(fid,ctypefreeze(a,typename));

