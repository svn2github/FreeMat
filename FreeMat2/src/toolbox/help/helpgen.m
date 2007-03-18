function helpgen(source_path)
  global sourcepath section_descriptors genfiles
  genfiles = {};
  rmdir([source_path,'/help'],'s');
  rmdir([source_path,'/toolbox'],'s');
  
  mkdir([source_path,'/help/html']);
  mkdir([source_path,'/help/tmp']);
  mkdir([source_path,'/help/latex']);
  mkdir([source_path,'/help/text']);
  mkdir([source_path,'/toolbox/test']);
  mkdir([source_path,'/toolbox']);

  sourcepath = source_path;
  read_section_descriptors;
  p = groupwriter({htmlwriter,latexwriter,bbtestwriter,textwriter, ...
                   testwriter});
  file_list = {};
  file_list = [file_list;helpgen_rdir([source_path,'/libs'])];
  file_list = [file_list;helpgen_rdir([source_path,'/src'])];
  for i=1:numel(file_list)
    [path,name,suffix] = fileparts(file_list{i});
    if (~strcmp(name,'MPIWrap'))
      helpgen_processfile(file_list{i},p);
    end
  end

  file_list = helpgen_rdir([source_path,'/src/toolbox']);
  for i=1:numel(file_list)
    merge_mfile(file_list{i});
  end
  writeindex(p);
  copyfile([source_path,'/help/tmp/*.jpg'],[source_path,'/help/latex'])
  copyfile([source_path,'/help/tmp/*.png'],[source_path,'/help/html'])
  copyfile([source_path,'/src/toolbox/help/match_close.m'],[source_path,'/toolbox/test'])
  copyfile([source_path,'/src/toolbox/help/match_exact.m'],[source_path,'/toolbox/test'])
  copyfile([source_path,'/src/toolbox/help/run_tests.m'],[source_path,'/toolbox/test'])
  delete([source_path,'/toolbox/help/match_close.m']);
  delete([source_path,'/toolbox/help/match_exact.m']);
  delete([source_path,'/toolbox/help/run_tests.m']);
  for i=1:numel(genfiles)
    copyfile([source_path,'/help/tmp/',genfiles{i}],[source_path,'/toolbox/test']);
  end
  printf('Latexing...\n');
  cd([source_path,'/help/latex']);
  printf('Pass 1\n');
  system('pdflatex main.tex');
  printf('Pass 2\n');
  system('pdflatex main.tex');
  printf('Pass 3\n');
  system('pdflatex main.tex');
  rmdir([source_path,'/help/tmp'],'s');
  
function merge_mfile(filename)
  global sourcepath
  [path,name,suffix] = fileparts(filename);
  if (~strcmp(suffix,'.m')) return; end
  printf('Merging comments with file %s\n',filename);
  headertext = [];
  try 
    fp = fopen([sourcepath,'/help/text/',name,'.mdc'],'r');
    text = getline(fp);
    while (~feof(fp))
      headertext = [headertext,'% ',text];
      text = getline(fp);
    end
    fclose(fp);
  catch
  end
  h = fopen(filename,'r');
  newname = strrep(filename,'src/toolbox','toolbox');
  [path,name,suffix] = fileparts(newname);
  mkdir(path);
  g = fopen(newname,'w');
  text = getline(h);
  while (~feof(h))
    if (~isempty(regexp(text,'^\s*%!')))
      text = getline(h);
      while (isempty(regexp(text,'^\s*%!')))
        text = getline(h);
      end
      fprintf(g,'%s',strrep(headertext,'\','\\'));
    else
      fprintf(g,'%s',strrep(text,'\','\\'));
    end
    text = getline(h);
  end
  fclose(h);
  fclose(g);
  
function read_section_descriptors
  global sourcepath section_descriptors
  fp = fopen([sourcepath,'/src/toolbox/help/section_descriptors.txt'],'r');
  line = fgetline(fp);
  while (~feof(fp))
    p = regexp(line,'(\w*)\s*([^\n]*)','tokens');
    section_descriptors.(p{1}{1}) = p{1}{2};
    line = fgetline(fp);
  end
  fclose(fp);
  
function file_list = helpgen_rdir(basedir)
  file_list = {};
  avec = dir(basedir);
  for (i=1:numel(avec))
    if (~(strcmp(avec(i).name,'.')  || (strcmp(avec(i).name,'..'))))
      cpath = [basedir dirsep avec(i).name];
      if (avec(i).isdir)
        if (~strcmp(avec(i).name,'.svn'))
          subdir_list = helpgen_rdir(cpath);
          file_list = [file_list;subdir_list];
        end
      else
        file_list = [file_list;{cpath}];
      end
    end
  end

function helpgen_processfile(filename,&writers)
  global sourcepath section_descriptors
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
      printf('    Module %s...\n',lower(modname));
      moddesc = mustmatch(line,pset.moduledesc);
      line = getline(fp);
      secname = mustmatch(line,pset.sectionname);
      beginmodule(writers,sourcepath,modname,moddesc,secname,section_descriptors);
      line = getline(fp);
      exec_id = threadnew;
      while (~feof(fp) && ~testmatch(line,pset.docblock))
         groupname = mustmatch(line,pset.groupname);
	 begingroup(writers,groupname);	 
         line = getline(fp);
	 while (~feof(fp) && ~testmatch(line,pset.groupname) ...
	       		  && ~testmatch(line,pset.docblock))
           if (testmatch(line,pset.execin))
	     handle_exec(line,fp,pset,writers,exec_id);
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
      threadfree(exec_id);
      close all;
      endmodule(writers);
    end
  end

function pset = get_pattern_set(prefix)
  pset.docblock =     [prefix '!'];
  pset.modulename =   [prefix '@Module\s*(\w*)'];
  pset.moduledesc =   [prefix '@Module\s*(.*)'];
  pset.sectionname =  [prefix '@@Section\s*(\w*)'];
  pset.groupname =    [prefix '@@([ \w]*)'];
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
  global genfiles
  fname = mustmatch(line,pset.fnin);
  line = getline(fp);
  fn = '';
  while (~feof(fp) && ~testmatch(line,pset.fnout))
    fn = [fn mustmatch(line,pset.ccomment)];
    line = getline(fp);
  end
  fn = strrep(fn,'\','\\');
  genfiles = [genfiles,{fname}];
  zp = fopen(fname,'w');
  fprintf(zp,'%s',fn);
  fclose(zp);
  if (isempty(regexp(fname,'test_\w+')))
    dofile(writers,fname,fn);
  end
  line = getline(fp);

function handle_equation(&line,fp,pset,&writers)
  line = getline(fp);
  eqn = '';
  while (~feof(fp) && ~testmatch(line,pset.eqnout))
     eqn = [eqn,mustmatch(line,pset.ccomment)];
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
    itemlist = [itemlist,{item}];
  end
  if (feof(fp)), error('unmatched enumeration block'); end
  line = getline(fp);
  doenumerate(writers,itemlist);
 
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
    itemlist = [itemlist,{item}];
  end
  if (feof(fp)), error('unmatched enumeration block'); end
  line = getline(fp);
  doitemize(writers,itemlist);

function handle_output(&line,fp,pset,&writers)
  line = mustmatch(line,pset.ccomment);
  outputtext(writers,line);
  line = getline(fp);

function handle_exec(&line,fp,pset,&writers,exec_id)
  global sourcepath
  line = mustmatch(line,pset.execin);
  errors_expected = str2num(line);
  cmdlist = {};
  line = getline(fp);
  while (~feof(fp) && ~testmatch(line,pset.execout))
    if (testmatch(line,pset.ccomment))
      cmdlist = [cmdlist,{mustmatch(line,pset.ccomment)}];
    end
    line = getline(fp);
  end
%  cmdlist = strrep(cmdlist,'\r','\\r');
%  cmdlist = strrep(cmdlist,'\n','\\n');
  docomputeblock(writers,cmdlist,errors_expected);
  cd([sourcepath,'/help/tmp']);
  beginverbatim(writers);
  etext = threadcall(exec_id,100000,'simkeys',cmdlist);
  etext = regexprep(etext,'(--> mprint[^\n]*\n)','');
  etext = strrep(etext,'--> quit','');
  outputtext(writers,etext);
  endverbatim(writers);
  if (threadcall(exec_id,100000,'errorcount') ~= errors_expected)
    printf('Error count mismatch on block\n');
    for (i=1:numel(cmdlist))
      printf('%s\n',cmdlist{i});
    end
    error('Failed!');
  end
  cd([sourcepath,'/help/tmp']);
  line = getline(fp);

