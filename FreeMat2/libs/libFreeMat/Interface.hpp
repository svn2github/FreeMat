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

#ifndef __Interface_hpp__
#define __Interface_hpp__

#include <string>
#include <vector>
#include <QStringList>

namespace FreeMat {
  class Context;

  class Interface {
    Context *m_context;
    QStringList m_basePath;
    QStringList m_userPath;
    void scanDirectory(std::string scdir, bool, std::string prefixo);
    void procFile(std::string fname, std::string fullname, bool);
    std::string app_path;
  public:
    Interface();
    virtual ~Interface();
    std::vector<std::string> GetCompletions(std::string line, int word_end, 
					    std::string &matchString);
    /**
     *  Get/Set the application path 
     */
    virtual std::string getAppPath();
    virtual void setAppPath(std::string);
    /**
     *  The Base Path is the one that contains .m files in the current app bundle
     */
    virtual void setBasePath(QStringList);
    /**
     *  The User Path is the one that the user can tinker with.
     */
    virtual void setUserPath(QStringList);
    /**
     *  Get the current path set for the interface. (user path - legacy interface)
     */
    virtual std::string getPath();
    /**
     *  Set the path for the interface. (user path - legacy interface)
     */
    virtual void setPath(std::string);
    /**
     *  Set the context for the interface.
     */
    virtual void setContext(Context *ctxt);
    /**
     *  Force a rescan of the current path to look for 
     *  new function files.
     */
    virtual void rescanPath();
    /**
     *  Get a line of input from the user with the
     *  given prompt.
     */ 
    virtual char* getLine(std::string prompt) = 0;
    /**
     *  Return the width of the current "terminal" in
     *  characters.
     */
    virtual int getTerminalWidth() = 0;
    /**
     *  Output the following text message.
     */
    virtual void outputMessage(std::string msg) = 0;
    /**
     *  Output the following error message.
     */
    virtual void errorMessage(std::string msg) = 0;
    /**
     *  Output the following warning message.
     */
    virtual void warningMessage(std::string msg) = 0;
  };

  char* TildeExpand(char* path);
}

#endif
