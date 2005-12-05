function x = isnumeric(y)
  x = any(strcmp({'uint8','int8','uint16','int16','uint32','int32','float','double','complex','dcomplex'},class(y)));
