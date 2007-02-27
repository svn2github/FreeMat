/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __MainApp_hpp__
#define __MainApp_hpp__

#include <string>
#include <qobject.h>
#include "KeyManager.hpp"
#include "application.hpp"
#include "Interpreter.hpp"

class MainApp : public QObject
{
  Q_OBJECT
  KeyManager* m_keys;
  QObject *m_term;
  bool guimode;
  bool skipGreeting;
  ApplicationWindow *m_win;
  Interpreter *m_eval;
  bool GUIHack;
  void CheckNonClosable();
  Scope *m_global;
  QStringList basePath, userPath;

public:
  MainApp();
  ~MainApp();
  void SetKeyManager(KeyManager* term);
  void SetSkipGreeting(bool skip);
  void SetupGUICase();
  void SetupInteractiveTerminalCase();
  void SetupDumbTerminalCase();
  KeyManager* GetKeyManager();
  void SetGUIMode(bool mode);
  void TerminalReset();
  Context *NewContext();
  int  StartNewInterpreterThread();
  void UpdatePaths();
public slots:
  int Run();
  void HelpWin();
  void PathTool();
  void Editor();
  void ExecuteLine(string txt);
  void UpdateTermWidth(int);
  void DoGraphicsCall(Interpreter*, FuncPtr f, ArrayVector m, int narg);
  void Quit();
  void Crashed();
signals:
  void Shutdown();
  void Initialize();
};

#endif
