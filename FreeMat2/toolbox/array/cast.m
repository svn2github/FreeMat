%!
%@Module CAST Typecast Variable to Specified Type
%@@Section Array
%@@Usage
%The @|cast| function allows you to typecast a variable from one
%type to another.  The syntax for its use is 
%@[
%    y = cast(x,toclass)
%@]
%where @|toclass| is the name of the class to cast @|x| to.  Note
%that the typecast must make sense, and that @|toclass| must be
%one of the builtin types.  The current list of supported types is
%\begin{itemize}
%\item @|'cell'| for cell-arrays
%\item @|'struct'| for structure-arrays
%\item @|'logical'| for logical arrays
%\item @|'uint8'| for unsigned 8-bit integers
%\item @|'int8'| for signed 8-bit integers
%\item @|'uint16'| for unsigned 16-bit integers
%\item @|'int16'| for signed 16-bit integers
%\item @|'uint32'| for unsigned 32-bit integers
%\item @|'int32'| for signed 32-bit integers
%\item @|'uint64'| for unsigned 64-bit integers
%\item @|'int64'| for signed 64-bit integers
%\item @|'float'| for 32-bit floating point numbers
%\item @|'single'| is a synonym for @|'float'|
%\item @|'double'| for 64-bit floating point numbers
%\item @|'complex'| for complex floating point numbers with 32-bits per field
%\item @|'dcomplex'| for complex floating point numbers with 64-bits per field
%\item @|'string'| for string arrays
%\end{itemize}
%@@Example
%Here is an example of a typecast from a float to an 8-bit integer
%@<
%cast(pi,'uint8')
%@>
%and here we cast an array of arbitrary integers to a logical array
%@<
%cast([1 0 3 0],'logical')
%@>
%@@Tests
%@{"y=cast(pi,'uint8')","3","exact"}
%@{"y=cast([1,0,3,0],'logical')","logical([1,0,1,0])","exact"}
%!
function y = cast(a,toclass)
    switch(toclass)
      case 'cell'
        y = cell(a);
      case 'struct'
        y = struct(a);
      case 'logical'
        y = logical(a);
      case 'uint8'
        y = uint8(a);
      case 'int8'
        y = int8(a);
      case 'uint16'
        y = uint16(a);
      case 'int16'
        y = int16(a);
      case 'uint32'
        y = uint32(a);
      case 'int32'
        y = int32(a);
      case 'uint64'
        y = uint64(a);
      case 'int64'
        y = int64(a);
      case {'float','single'}
        y = float(a);
      case 'double'
        y = double(a);
      case 'complex'
        y = complex(a);
      case 'dcomplex'
        y = dcomplex(a);
      case 'string'
        y = string(a);
      otherwise
        error(['Unsupported class ' toclass ' requested in typecast'])
    end
    