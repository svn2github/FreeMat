function b = parseit(&t,dictionary)
  n = stcmp(dictionary,t);
  b = dictionary{min(find(n))};
  t(1:length(b)) = [];

function c = colorset
c = {'y','m','c','r','g','b','w','k'};

function c = styleset
c = {'--',':','-.','-'};

function b = stcmp(source,pattern)
b = zeros(size(source),'logical');
for i=1:numel(source)
  b(i) = strncmp(source{i},pattern,length(source{i}));
end


