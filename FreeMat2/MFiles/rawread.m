
function x = rawread(fname,size,precision,byteorder)
  try 
    if (isset('byteorder'))
      fp = fopen(fname,'rb',byteorder);
    else
      fp = fopen(fname,'rb');
    end
    x = fread(fp,size,precision);
    fclose(fp);
  catch
    error(['Unable to read file ' fname ' in function rawread']);
  end

