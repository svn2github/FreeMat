function  num_out = and(num1, num2)
   nbits = round(log(max(num1,num2))/log(2));
   b1 = int2bin(num1, nbits);
   b2 = int2bin(num2, nbits);
   b_out = b1.*b2;
   num_out = bin2int(b_out);