a.foo = 32;
a.goo = 'hello';
a.zoo = {1,2,3};
b.goo = 'hello';
b.foo = 32;
b.zoo = {1,2,3};
for i=1:20
    isequal(a,b)
end
quit
