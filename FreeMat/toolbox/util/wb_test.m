function retval = htest(text,funcname,dbdir)
  load([dbdir,dirsep,'wbinputs.mat'])
  load([dbdir,dirsep,funcname,'_ref.mat'])
  retval = 0;
  v = regexp(text,'@\$([^#]*)#([^#]*)#?(.*)','tokens');
  if (isempty(v))
    printf(sprintf('bad line: %s',text));
    return;
  end
  % Search for inputs
  ttype = v{1}{1};
  if (~any(strcmp(ttype,{'near','exact','near_permute'})))
    printf(sprintf('bad test type in line: %s',text));
    return;
  end
  expr = v{1}{2};
  exclude = v{1}{3};
  if (isempty(exclude)) exclude = 'false'; end
  sexpr = strrep(expr,'''','''''');
  vars = symvar(expr);
  pnt = regexp(vars,'x.');
  in_count = 0; 
  for i=1:numel(pnt); 
    if (~isempty(pnt{i}))
      in_count = in_count + 1; 
    end; 
  end
  pnt = regexp(vars,'y.');
  out_count = 0;
  for i=1:numel(pnt);
    if (~isempty(pnt{i}))
      out_count = out_count + 1;
    end;
  end
  if (in_count == 0)
    error_flag = 0;
    for k=1:out_count
      eval(sprintf('y%d = [];\n'),k);
    end
    eval(expr,'error_flag=1');
    if (error_flag && ~error_refs); return; end
    for k=1:out_count
      if (~eval(sprintf('wbtest_%s(y%d,y%d_refs{1})',ttype,k,k)))
        return;
      end
    end
  end
  if (in_count == 1)
    error_flag = 0;
    for loopi=1:numel(wbinputs)
      x1 = wbinputs{loopi};
      error_flag = 0;
      for k=1:out_count;
        eval(sprintf('y%d = [];',k));
      end
      eval([expr,';'],'error_flag = 1;');
      if (error_flag && error_refs(loopi))
          continue;
      end
      if (eval(sprintf('error_flag && ~error_refs(loopi) && ~(%s)',exclude)))
         return;
      end
      for k=1:out_count
        if (~eval(sprintf('wbtest_%s(y%d,y%d_refs{loopi})',ttype,k,k)))
          return;
        end
      end
    end
  end
  if (in_count == 2)
      for loopi=1:numel(wbinputs)
          for loopj=1:numel(wbinputs)
              x1 = wbinputs{loopi};
              x2 = wbinputs{loopj};
              error_flag = 0;
              for k=1:out_count;
                  eval(sprintf('y%d = [];',k));
              end
              eval(expr,'error_flag=1');
              if (error_flag && error_refs(loopi))
                  continue;
              end
              if (eval(sprintf('error_flag && ~error_refs(loopi,loopj) && ~(%s)',exclude)))
                  return;
              end
              for k=1:out_count
                  if (~eval(sprintf('wbtest_%s(y%d,y%d_refs{loopi,loopj})',ttype,k,k)))
                      return;
                  end
              end
          end
      end
  end
  retval = 1;
