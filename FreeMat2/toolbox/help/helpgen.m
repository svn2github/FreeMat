function helpgen(source_path)
  global sourcepath
%  rmdir([source_path,'/help2/html'],'s');
%  rmdir([source_path,'/help/tmp'],'s');
%  rmdir([source_path,'/help/latex'],'s');
%  rmdir([source_path,'/help/text'],'s');
%  rmdir([source_path,'/help/test'],'s');
%  rmdir([source_path,'/help/toolbox'],'s');
  
  mkdir([source_path,'/help2/html']);
%  mkdir([source_path,'/help/tmp']);
%  mkdir([source_path,'/help/latex']);
%  mkdir([source_path,'/help/text']);
%  mkdir([source_path,'/help/test']);
%  mkdir([source_path,'/help/toolbox']);
  sourcepath = source_path;
  helpgen_processfile([source_path,'/toolbox/array/all.m']);
%  helpgen_processfile([source_path,'/toolbox/general/install.m']);
%  helpgen_processfile([source_path,'/libs/libCore/Misc.cpp']);

%  helpgen_processdir([source_path,'/toolbox']);
%  helpgen_processdir([source_path,'/libs']);
%  helpgen_processdir([source_path,'/src']);
 
%files = helpgen_rdir([source_path,'/toolbox'])
keyboard
 
function file_list = helpgen_rdir(basedir)
  file_list = {};
  avec = dir(basedir);
  for (i=1:numel(avec))
    if (~(strcmp(avec(i).name,'.')  || (strcmp(avec(i).name,'..'))))
      cpath = [basedir dirsep avec(i).name];
      if (avec(i).isdir)
        subdir_list = helpgen_rdir(cpath);
        file_list = [file_list;subdir_list];
      else
        file_list = [file_list;{cpath}];
      end
    end
  end

function helpgen_processfile(filename)
  [path,name,suffix] = fileparts(filename);
  if (strcmp(suffix,'.cpp'))
    comment = '//';
  elseif (strcmp(suffix,'.m'))
    comment = '%';
  else
    return;
  end
  prefix = ['^\s*' comment];
  pset = get_pattern_set(prefix);
  printf('Processing file %s...\n',filename);
  try
    fp = fopen(filename,'r');
  catch
    return;
  end
  while (1)
    line = getline(fp);
    if (feof(fp))
      fclose(fp);
      return;
    end
    if (testmatch(line,pset.docblock))
      line = getline(fp);
      modname = mustmatch(line,pset.modulename);
      moddesc = mustmatch(line,pset.moduledesc);
      line = getline(fp);
      secname = mustmatch(line,pset.sectionname);
      htmlwriter('beginmodule',modname,moddesc,secname);
      line = getline(fp);
      while (~feof(fp) && ~testmatch(line,pset.docblock))
         groupname = mustmatch(line,pset.groupname);
	 htmlwriter('begingroup',groupname);	 
         line = getline(fp);
	 while (~feof(fp) && ~testmatch(line,pset.groupname) ...
	       		  && ~testmatch(line,pset.docblock))
           if (testmatch(line,pset.execin))
	     handle_exec(line,fp,pset);
           elseif (testmatch(line,pset.verbatimin)) 
	     handle_verbatim(line,fp,pset);
           elseif (testmatch(line,pset.figure))
	     handle_figure(line,fp,pset);
           elseif (testmatch(line,pset.eqnin))
	     handle_equation(line,fp,pset);
	   elseif (testmatch(line,pset.fnin))
             handle_filedump(line,fp,pset);
	   elseif (testmatch(line,pset.enumeratein))
	     handle_enumerate(line,fp,pset);
	   elseif (testmatch(line,pset.itemizein))
	     handle_itemize(line,fp,pset);
	   elseif (testmatch(line,pset.ccomment))
	     handle_output(line,fp,pset);
           else
	     error('Unprocessed line:%s',line);
           end
         end
      end;
    end
    htmlwriter('endmodule');
  end

function pset = get_pattern_set(prefix)
  pset.docblock =     [prefix '!'];
  pset.modulename =   [prefix '@Module\s*(\w*)'];
  pset.moduledesc =   [prefix '@Module\s*(.*)'];
  pset.sectionname =  [prefix '@@Section\s*(\w*)'];
  pset.groupname =    [prefix '@@(\w*)'];
  pset.execin =       [prefix '@<(\d*)?'];
  pset.execout =      [prefix '@>'];
  pset.verbatimin =   [prefix '@\['];
  pset.verbatimout =  [prefix '@\]'];
  pset.ccomment =     [prefix '(.*)'];
  pset.figure =       [prefix '@figure\s*(\w+)'];
  pset.eqnin =        [prefix '\\\['];
  pset.eqnout =       [prefix '\\\]'];
  pset.fnin =         [prefix '@{\s*(\S+)'];
  pset.fnout =        [prefix '@}'];
  pset.enumeratein =  [prefix '\\begin{enumerate}'];
  pset.enumerateout = [prefix '\\end{enumerate}'];
  pset.itemizein =    [prefix '\\begin{itemize}'];
  pset.itemizeout =   [prefix '\\end{itemize}'];
  pset.item =         [prefix '\s*\\item(.*)'];

function line = getline(fp)
  persistent version
  if (~exist('version'))
    b = regexp(verstring,'v(.*)','tokens');
    version = b{:}{1};
  end;
  line = fgetline(fp);
  if (~feof(fp))
    line = strrep(line,'<VERSION_NUMBER>',version);
  end

function tok = mustmatch(line,pattern)
  toks = regexpi(line,pattern,'tokens');
  if (isempty(toks)) error(sprintf('Error on line: %s',line)); end;
  tok = toks{1}{1};

function res = testmatch(line,pattern)
  res = ~isempty(regexp(line,pattern));

function handle_filedump(&line,fp,pset)
  fname = mustmatch(line,pset.fnin);
  line = getline(fp);
  fn = '';
  while (~feof(fp) && ~testmatch(line,pset.fnout))
    fn = [fn mustmatch(line,pset.ccomment)];
    line = getline(fp);
  end
  htmlwriter('dofile',fname,fn);
  line = getline(fp);

function handle_equation(&line,fp,pset)
  line = getline(fp);
  eqn = '';
  while (~feof(fp) && ~testmatch(line,pset.eqnout))
     eqn = [eqn,mustmatch(line,pset.ccomment),'\n'];
	line = getline(fp);
  end
  printf('eqn: %s\n',eqn);
  line = getline(fp);

function handle_figure(&line,fp,pset)
  htmlwriter('dofigure',mustmatch(line,pset.figure));
  line = getline(fp);

function handle_verbatim(&line,fp,pset)
  line = getline(fp);
  htmlwriter('beginverbatim');
  while (~feof(fp) && ~testmatch(line,pset.verbatimout))
    htmlwriter('outputtext',mustmatch(line,pset.ccomment));
    line = getline(fp);
  end
  htmlwriter('endverbatim');
  if (feof(fp))
     error('unmatched verbatim block detected!');
  end
  line = getline(fp);

function handle_enumerate(&line,fp,pset)
  line = getline(fp);
  itemlist = {};
  while (~feof(fp) && ~testmatch(line,pset.enumerateout))
    item = mustmatch(line,pset.item);
    line = getline(fp);
    while (~testmatch(line,pset.item) && ...
           ~testmatch(line,pset.enumerateout) && ...
	   ~feof(fp))
      item = [item, mustmatch(line,pset.ccomment)];
      line = getline(fp);
    end
    itemlist = [itemlist,item];
  end
  if (feof(fp)), error('unmatched enumeration block'); end
  line = getline(fp);
  htmlwriter('enumerate',itemlist);
 
function handle_itemize(&line,fp,pset)
  line = getline(fp);
  itemlist = {};
  while (~feof(fp) && ~testmatch(line,pset.itemizeout))
    item = mustmatch(line,pset.item);
    line = getline(fp);
    while (~testmatch(line,pset.item) && ...
           ~testmatch(line,pset.itemizeout) && ...
	   ~feof(fp))
      item = [item, mustmatch(line,pset.ccomment)];
      line = getline(fp);
    end
    itemlist = [itemlist,item];
  end
  if (feof(fp)), error('unmatched enumeration block'); end
  line = getline(fp);
  htmlwriter('itemize',itemlist);

function handle_output(&line,fp,pset)
  line = mustmatch(line,pset.ccomment);
  htmlwriter('outputtext',line);
  line = getline(fp);

function handle_exec(&line,fp,pset)
  line = mustmatch(line,pset.execin);
  errors_expected = str2num(line);
  printf('errors_expected %d\n',errors_expected);
  cmdlist = {};
  line = getline(fp);
  while (~feof(fp) && ~testmatch(line,pset.execout))
    if (testmatch(line,pset.ccomment))
      cmdlist = [cmdlist,[mustmatch(line,pset.ccomment)]];
    end
    line = getline(fp);
  end
  printf('Command\n');
  disp(cmdlist);
  line = getline(fp);

function otext = latin_filter(text)
  otext = strrep(text,'&','&amp;');
  otext = strrep(otext,'<','&lt;');
  otext = strrep(otext,'>','&gt;');

function text = expand_codes(text)
  text = regexprep(text,'\@\|([^\|]*)\|','<code>$1</code>');
  if (strcmp(text,'\n')) 
     text = '<P>\n';
  end

function htmlwriter(cmd,varargin)  
persistent myfile modulename verbatim ignore
global sourcepath
switch (cmd)
  case 'beginmodule'
    modname = varargin{1};
    moddesc = varargin{2};
    secname = varargin{3};
    modulename = lower(modname);
    filename = [sourcepath '/help/html/' secname '_' modulename '.html'];
    myfile = fopen(filename,'w');
    if (myfile < 0) 
      error(sprintf('unable to open %s for output',filename)); 
    end	
    fprintf(myfile,'<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 3.2 Final//EN">\n');
    fprintf(myfile,'\n');
    fprintf(myfile,'<HTML>\n');
    fprintf(myfile,'<HEAD>\n');
    fprintf(myfile,'<TITLE>%s</TITLE>\n',moddesc);
    fprintf(myfile,'</HEAD>\n');
    fprintf(myfile,'<BODY>\n');
    fprintf(myfile,'<H2>%s</H2>\n',moddesc);
%  sectables.insert(secname,QStringList() << modname << moddesc);
    fprintf(myfile,'<P>\n');
    fprintf(myfile,'Section: <A HREF=sec_%s.html> %s </A>\n',lower(secname),'foo');
  case 'begingroup'
    groupname = varargin{1};
    if (strcmp(lower(groupname),'tests'))
      ignore = 1;
    else
      fprintf(myfile,'<H3>%s</H3>\n',groupname);
      ignore = 0;
    end
  case 'beginverbatim'
    fprintf(myfile,'<PRE>\n');
    verbatim = 1;
  case 'endverbatim'
    fprintf(myfile,'</PRE>\n<P>\n');
    verbatim = 0;
  case 'endmodule'
    fprintf(myfile,'</BODY>\n');
    fprintf(myfile,'</HTML>\n');
    fclose(myfile);
  case 'itemize'
    itemlist = varargin{1};
    fprintf(myfile,'<UL>\n');
    for i=1:numel(itemlist)
      fprintf(myfile,'<LI> %s </LI>\n',expand_codes(latin_filter(itemlist{i})));
    end
    fprintf(myfile,'</UL>\n');
  case 'enumerate'
    itemlist = varargin{1};
    fprintf(myfile,'<OL>\n');
    for i=1:numel(itemlist)
      fprintf(myfile,'<LI> %s </LI>\n',expand_codes(latin_filter(itemlist{i})));
    end
    fprintf(myfile,'</OL>\n');
  case 'dofigure'
    name = varargin{1};
    fprintf(myfile,'<P>\n');
    fprintf(myfile,'<DIV ALIGN="CENTER">\n');
    fprintf(myfile,'<IMG SRC="%s.png">\n',name);
    fprintf(myfile,'</DIV>\n');
    fprintf(myfile,'<P>\n');
  case 'dofile'
    name = varargin{1};
    ftxt = varargin{2};
    fprintf(myfile,'<P>\n<PRE>\n');
    fprintf(myfile,'     %s\n',name);
    fprintf(myfile,'%s\n',latin_filter(ftxt));
    fprintf(myfile,'</PRE>\n');
    fprintf(myfile,'<P>\n');
  case 'outputtext'
    if (ignore) return; end;
    text = latin_filter(varargin{1});
    if (verbatim)
      fprintf(myfile,'%s',text);
    else
      fprintf(myfile,'%s',expand_codes(text));
    end
end
