function inline_res = inline_evaluate(expr,assignexpr,args)
  eval(assignexpr);
  inline_res = eval(expr,'error evaluating inline expression');
  
