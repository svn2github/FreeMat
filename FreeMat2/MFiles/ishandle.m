function b = ishandle(handle,type)
  b = logical(0);
  try
   b = strcmp(get(handle,'type'),type);
  catch
  end
