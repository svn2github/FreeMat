function helpgen(source_path)
  global sourcepath
%  rmdir([source_path,'/help2/html'],'s');
%  rmdir([source_path,'/help/tmp'],'s');
%  rmdir([source_path,'/help/latex'],'s');
%  rmdir([source_path,'/help/text'],'s');
%  rmdir([source_path,'/help/test'],'s');
%  rmdir([source_path,'/help/toolbox'],'s');
  
  mkdir([source_path,'/help2/html']);
  mkdir([source_path,'/help2/tmp']);
%  mkdir([source_path,'/help/latex']);
%  mkdir([source_path,'/help/text']);
%  mkdir([source_path,'/help/test']);
%  mkdir([source_path,'/help/toolbox']);
  sourcepath = source_path;
  h = htmlwriter;
  p = groupwriter({h});
  helpgen_processfile([source_path,'/toolbox/array/all.m'],p);
%  helpgen_processfile([source_path,'/toolbox/general/install.m']);
%  helpgen_processfile([source_path,'/libs/libCore/Misc.cpp'],p);

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

function helpgen_processfile(filename,writers)
  global sourcepath
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
      beginmodule(writers,sourcepath,modname,moddesc,secname);
      line = getline(fp);
      while (~feof(fp) && ~testmatch(line,pset.docblock))
         groupname = mustmatch(line,pset.groupname);
	 begingroup(writers,groupname);	 
         line = getline(fp);
	 while (~feof(fp) && ~testmatch(line,pset.groupname) ...
	       		  && ~testmatch(line,pset.docblock))
           if (testmatch(line,pset.execin))
	     handle_exec(line,fp,pset,writers);
           elseif (testmatch(line,pset.verbatimin)) 
	     handle_verbatim(line,fp,pset,writers);
           elseif (testmatch(line,pset.figure))
	     handle_figure(line,fp,pset,writers);
           elseif (testmatch(line,pset.eqnin))
	     handle_equation(line,fp,pset,writers);
	   elseif (testmatch(line,pset.fnin))
             handle_filedump(line,fp,pset,writers);
	   elseif (testmatch(line,pset.enumeratein))
	     handle_enumerate(line,fp,pset,writers);
	   elseif (testmatch(line,pset.itemizein))
	     handle_itemize(line,fp,pset,writers);
	   elseif (testmatch(line,pset.ccomment))
	     handle_output(line,fp,pset,writers);
           else
	     error('Unprocessed line:%s',line);
           end
         end
      end
      endmodule(writers);
    end
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

function handle_filedump(&line,fp,pset,&writers)
  fname = mustmatch(line,pset.fnin);
  line = getline(fp);
  fn = '';
  while (~feof(fp) && ~testmatch(line,pset.fnout))
    fn = [fn mustmatch(line,pset.ccomment)];
    line = getline(fp);
  end
  dofile(writers,fname,fn);
  line = getline(fp);

function handle_equation(&line,fp,pset,&writers)
  line = getline(fp);
  eqn = '';
  while (~feof(fp) && ~testmatch(line,pset.eqnout))
     eqn = [eqn,mustmatch(line,pset.ccomment),'\n'];
	line = getline(fp);
  end
  doequation(writers,eqn);
  line = getline(fp);

function handle_figure(&line,fp,pset,&writers)
  dofigure(writers,mustmatch(line,pset.figure));
  line = getline(fp);

function handle_verbatim(&line,fp,pset,&writers)
  line = getline(fp);
  beginverbatim(writers);
  while (~feof(fp) && ~testmatch(line,pset.verbatimout))
    outputtext(writers,mustmatch(line,pset.ccomment));
    line = getline(fp);
  end
  endverbatim(writers);
  if (feof(fp))
     error('unmatched verbatim block detected!');
  end
  line = getline(fp);

function handle_enumerate(&line,fp,pset,&writers)
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
  enumerate(writers,itemlist);
 
function handle_itemize(&line,fp,pset,&writers)
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
  itemize(writers,itemlist);

function handle_output(&line,fp,pset,&writers)
  line = mustmatch(line,pset.ccomment);
  outputtext(writers,line);
  line = getline(fp);

function handle_exec(&line,fp,pset,&writers)
  global sourcepath
  line = mustmatch(line,pset.execin);
  errors_expected = str2num(line);
  printf('errors_expected %d\n',errors_expected);
  cmdlist = '';
  line = getline(fp);
  while (~feof(fp) && ~testmatch(line,pset.execout))
    if (testmatch(line,pset.ccomment))
      cmdlist = [cmdlist,[mustmatch(line,pset.ccomment)],13];
    end
    line = getline(fp);
  end
  cd([sourcepath,'/help2/tmp']);
  save helpgen.dat
  delete diary.txt
  diary([sourcepath,'/help2/tmp/diary.txt'])
  simkeys(cmdlist);
  diary off
  load helpgen.dat
  qp = fopen([sourcepath,'/help2/tmp/diary.txt'],'r');
  line = fgetline(qp);
  beginverbatim(writers);
  while (~feof(qp))
    outputtext(writers,line);
    line = fgetline(qp);
  end
  fclose(qp);
  line = getline(fp);
