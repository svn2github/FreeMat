function bind(output,mainexe,startcommand,varargin)
% Open up the output stream
fpo = fopen(output,'wb');
% Read the main program in
fpi = fopen(mainexe,'rb');
printf('Reading main executable %s\n',mainexe);
pdata = fread(fpi,[1,inf],'int8');
fclose(fpi);
% Write the main program to the output stream
printf('Copying interpreter to output file %s\n',output);
fwrite(fpo,pdata);
if (~isempty(varargin))
  fwrite(fpo,uint32(length(varargin)));
  for i=1:length(varargin)
    printf('Pcoding support routine %s\n',varargin{i});
    pcode(varargin{i});
    printf('Copying Pcode for %s into output bundle...\n',varargin{i});
    fpi = fopen(sprintf('%s.p',varargin{i}),'rb');
    fwrite(fpo,fread(fpi,[1,inf],'int8'));
    fclose(fpi);
  end
else
  fwrite(fpo,uint32(0));
end
% Write the closing tag
printf('Writing closing tag information...\n');
fwrite(fpo,'fmexe');
fwrite(fpo,uint32(length(pdata)));
fwrite(fpo,uint32(length(startcommand)+1));
fwrite(fpo,startcommand);
fwrite(fpo,int8(zeros(1,1024-5-4-4-length(startcommand))));
fclose(fpo);
printf('Bundle successfully created.\n');

