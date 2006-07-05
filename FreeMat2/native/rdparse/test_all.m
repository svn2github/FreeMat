g = a{:};
h = [];
h(2).foo = [];
c = [];
[h.foo c] = svd(rand(3))

a = 1:32
a(9) = 10
b = rand(3)
b(2,2) = 1
b(3,2:3) = 9
c = {a,b}
d = c{1}(9)
f = 3/4
g = a.*a
k = [1 2 -3 +4]
l = [k,k-1;k+2,k-3]
l+-l
m = {3 4 2}
n = {3,2,4;2,3,5}
g = 3
j = 0;
for i=1:20
    if (i==15), break;  end
    j = j + i;
end
j
k = 1:20
jj = 0;
for k
    if (k==10)
        continue;
    end
    jj = jj + k;
end
jj
printf('foobar\n');
if ((size(g,2) > 2) && (g(2) == 2)), printf('This should not print\n'); end
g = [1,2 4]
if ((size(g,2) > 2) && (g(2) == 2)), printf('This should print\n'); end
if (g(2) == 5)
    printf('five\n');
elseif (g(2) == 3)
    printf('three\n');
elseif (g(2) == 2)
    printf('two\n');
else
    printf('other\n');
end
k = 10;
t = 0;
while (k>0)
    t = t + k;
    k = k-1;
    if (k==4)
        %        keyboard
    end
end
t
a = {1:4 5:8}
a{:}
a{1:2}
% Need to survive this: b = {1:4

%This doesn't work right... p(6).foo should be a 1x2 cell array with a 
%string in the second cell
p(6).foo{2} = 'hello'
k = p(end).foo{end}

a = cell(1,3);
[a{1:3}] = svd(rand(3))
g = [];
g(3).foo = [];
[g.foo] = svd(rand(3))
h = [];
h(2).foo = [];
c = [];
[h.foo c] = svd(rand(3))

try 
    gooby
catch
    printf('gooby error caught\n');
end

try 
    gooby
end

x = 3;

switch x
  case 1
    printf('x is one\n');
  case 2
    printf('x is two\n');
  case 3
    printf('x is three\n');
  case 4
    printf('x is four\n');
end

a = 'red';
switch(a)
  case {'lima beans','root beer'}
    c = 'food';
  case {'red','green','blue'}
    c = 'color';
  otherwise
    c = 'not sure';
end

disp(c)

a = 'black';
switch(a)
  case {'lima beans','root beer'}
    c = 'food';
  case {'red','green','blue'}
    c = 'color';
  otherwise
    c = 'not sure';
end

disp(c)

a = 3 + 3
a = 3 - 6
a = 3 * 6
a = 3 / 6
a = 3 \ 6
a = 1 | 2
a = 1 || 2
a = 1 & 2
a = 1 && 2
a = 1 < 2
a = 1 <= 2
a = 1 > 2
a = 1 >= 2
a = 1 == 2
a = 1 ~= 2
a = 3 .* 4
a = 3 ./ 4
a = 3 .\ 4
a = -4
a = +5
a = ~a
a = 3 ^ 2
a = 3 .^ 2
b = a'
c = a.'
d = @sin
d(pi/4)

