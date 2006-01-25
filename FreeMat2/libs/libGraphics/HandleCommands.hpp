#ifndef __HandleCommands_hpp__
#define __HandleCommands_hpp__

#include "Context.hpp"
#include "HandleObject.hpp"
#include "HandleFigure.hpp"
#define MAX_FIGS 100
#define HANDLE_OFFSET_OBJECT 100000
#define HANDLE_OFFSET_FIGURE 1

namespace FreeMat {
  void LoadHandleGraphicsFunctions(Context* context);

  HandleObject* LookupHandleObject(unsigned handle);
  HandleFigure* LookupHandleFigure(unsigned handle);
  unsigned AssignHandleObject(HandleObject*);
  unsigned AssignHandleFigure(HandleFigure*);
  void FreeHandleObject(unsigned handle);
  void FreeHandleFigure(unsigned handle);
  void ValidateHandle(unsigned handle);
  void NotifyFigureClosed(unsigned handle);
  
  void InitializeHandleGraphics();
  void ShutdownHandleGraphics();
}

#endif
