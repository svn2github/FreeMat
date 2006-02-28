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
#include "System.hpp"
#include "Exception.hpp"
#include <stdlib.h>
#include <QProcess>
#include <QtGui>
#ifdef Q_OS_WIN32
#include <windows.h>
#endif

namespace FreeMat {

  std::vector<std::string> DoSystemCallCaptured(std::string cmd) {
    QProcess toRun;
    std::vector<std::string> ret;
#ifdef Q_OS_WIN32
    char shellCmd[_MAX_PATH];
    if( !GetEnvironmentVariable("ComSpec", shellCmd, _MAX_PATH) )
      throw Exception("Unable to find command shell!");
    cmd = std::string(shellCmd) + " /a /c " + std::string(cmd);
#else
    cmd = std::string("sh -c ") + cmd;
#endif
    if (cmd[cmd.size()-1] == '&') {
      QString cmd2(QString::fromStdString(cmd));
      cmd2.chop(1);
      QProcess::startDetached(cmd2);
      return ret;
    }
    toRun.start(QString::fromStdString(cmd));
    if (!toRun.waitForStarted())
      return ret;
    toRun.closeWriteChannel();
    while (!toRun.waitForFinished(100))
      qApp->processEvents();
    QByteArray result = toRun.readAll();
    QTextStream myStream(&result);
    while (!myStream.atEnd()) {
      QString line(myStream.readLine(0));
      ret.push_back(line.toStdString());
    }
    return ret;
  }
}
