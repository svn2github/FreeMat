function x = test_newind2
  a(10,40,5).foo = 'green';
  p = a(:,:,5).foo;
  q = p(10,40);
  
