%!
%@Module ISHANDLE Test for Graphics Handle
%@@Section INSPECTION
%@@Usage
%Given a constant, this routine will test to see if the
%constant is a valid graphics handle or not.  The syntax
%for its use is
%@[
%  y = ishandle(h,type)
%@]
%and returns a logical @|1| if @|x| is a handle of type @|type|
%and a logical @|0| if not.  
%!

% Copyright (c) 2002-2006 Samit Basu

function b = ishandle(handle,type)
    if (strcmp(type,'figure') && (handle<=100))
        b = logical(1);
        return;
    end
    if ((handle<=100000) || (handle ~= int32(handle)))
        b = logical(0);
        return;
    end
    b = logical(0);
    try
        b = strcmp(get(handle,'type'),type);
    catch
    end
