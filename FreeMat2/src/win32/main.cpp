#include "XWindow.hpp"
#include "WinTerminal.hpp"
#include <stdio.h>
#include "Module.hpp"
#include "LoadCore.hpp"
#include "GraphicsCore.hpp"
#include "System.hpp"
#include "PathSearch.hpp"
#include <stdlib.h>
#include <signal.h>

using namespace FreeMat;

class XWindow2 {
	WindowType m_type;
	HWND m_window;
	int m_width;
	int m_height;
public:
	XWindow2(WindowType wtype)  {m_type = wtype;};
	virtual ~XWindow2() {};
  HWND getWindow() {return m_window;}
  int getWidth() {return m_width;}
  int getHeight() {return m_height;}
  void Raise();
  void Show();
  void Hide();
  void Close();
  void SetImagePseudoColor(unsigned char *data, int width, int height);
  void SetImage(unsigned char *data, int width, int height);
  void OnExpose(int x, int y, int w, int h);
  void OnMouseDown(int x, int y);
  void OnMouseUp(int x, int y);
  void OnDrag(int x, int y);
  void OnResize(int w, int h);
  void PrintMe(std::string filename) {OutputDebugString(filename.c_str());}
  void SetTitle(std::string title);
  void GetClick(int &x, int &y);
  void GetBox(int &x1, int &y1, int &x2, int &y2);
  void EraseRectangle(int cx, int cy, int cwidth, int cheight);
  void SetSize(int w, int h);
  int GetState();
  void Refresh();
  void UpdateContents(unsigned char *data, int width, int height);
  WindowType GetWindowType() {return m_type;}
  void SetTheCursor();
  virtual void OnSize() {};
  virtual void OnClose() {}
  virtual void OnDraw(GraphicsContext &gc) = 0;
};

class MinWin : public XWindow2 {
public:
	MinWin() : XWindow2(VectorWindow) {PrintMe("Google.dat");}
	void OnDraw(GraphicsContext &gc) {};
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {
  SetupWinTerminalClass(hInstance);
  WinTerminal term(hInstance, iCmdShow);

  Context *context = new Context;
  SpecialFunctionDef *sfdef = new SpecialFunctionDef;
  sfdef->retCount = 0;
  sfdef->argCount = 5;
  sfdef->name = "loadFunction";
  sfdef->fptr = LoadLibFunction;
  context->insertFunctionGlobally(sfdef);

  sfdef = new SpecialFunctionDef;
  sfdef->retCount = 0;
  sfdef->argCount = 5;
  sfdef->name = "import";
  sfdef->fptr = ImportFunction;
  context->insertFunctionGlobally(sfdef);

  LoadCoreFunctions(context);
  LoadGraphicsCoreFunctions(context);  
  InitializeXWindowSystem(hInstance);
  InitializePlotSubsystem();
  InitializeImageSubsystem();

  MinWin *mptr = new MinWin;

  const char *envPtr;
  envPtr = getenv("FREEMAT_PATH");
  if (envPtr)
    term.initialize(std::string(envPtr),context);
  else
    term.initialize(std::string(""),context);
  WalkTree *twalk = new WalkTree(context,&term);
  term.SetEvalEngine(twalk);
  term.outputMessage(" Freemat - build ");
  term.outputMessage(__DATE__);
  term.outputMessage("\n");
  term.outputMessage(" Copyright (c) 2002-2004 by Samit Basu\n");
  twalk->evalCLI();
  return 0;
}

