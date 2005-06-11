#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "GUITerminal.hpp"
#include "MainApp.hpp"
#include <qapplication.h>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv, TRUE);
  MainApp m_app;
  QTimer *m_start;
  
  GUITerminal *iterm = new GUITerminal;
  iterm->show();
  iterm->resizeTextSurface();
  m_app.SetTerminal(iterm);
  m_start = new QTimer;
  QObject::connect(m_start,SIGNAL(timeout()),&m_app,SLOT(Run()));
  m_start->start(10,TRUE);
  return app.exec();
}
