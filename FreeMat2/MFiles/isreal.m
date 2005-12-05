function x = isreal(y)
  x = any(strcmp({'uint8','int8','uint16','int16','uint32','int32','float','double','string'},class(y)));
