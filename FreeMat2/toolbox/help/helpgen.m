function helpgen(source_path)
  rmdir([source_path,'/help/html'],'s');
  rmdir([source_path,'/help/tmp'],'s');
  rmdir([source_path,'/help/latex'],'s');
  rmdir([source_path,'/help/text'],'s');
  rmdir([source_path,'/help/test'],'s');
  rmdir([source_path,'/help/toolbox'],'s');
  
  mkdir([source_path,'/help/html']);
  mkdir([source_path,'/help/tmp']);
  mkdir([source_path,'/help/latex']);
  mkdir([source_path,'/help/text']);
  mkdir([source_path,'/help/test']);
  mkdir([source_path,'/help/toolbox']);

  helpgen_processdir([source_path,'/toolbox']);
  helpgen_processdir([source_path,'/libs']);
  helpgen_processdir([source_path,'/src']);
  
  
  
function helpgen_processfile(filename)
  
