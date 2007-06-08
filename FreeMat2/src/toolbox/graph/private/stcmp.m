function b = stcmp(source,pattern)
b = zeros(size(source),'logical');
for i=1:numel(source)
  b(i) = strncmp(source{i},pattern,length(source{i}));
end
