#ifndef __HandleCommands_hpp__
#define __HandleCommands_hpp__

#include "Context.hpp"
#include "HandleObject.hpp"
#include "HandleFigure.hpp"

namespace FreeMat {
  void LoadHandleGraphicsFunctions(Context* context);

  HandleObject* LookupHandleObject(unsigned handle);
  HandleFigure* LookupHandleFigure(unsigned handle);
  unsigned AssignHandleObject(HandleObject*);
  unsigned AssignHandleFigure(HandleFigure*);
  void FreeHandleObject(unsigned handle);
  void FreeHandleFigure(unsigned handle);
  void ValidateHandle(unsigned handle);
}

#endif
