// Copyright (c) 2002, 2003 Samit Basu
// 
// Permission is hereby granted, free of charge, to any person obtaining a 
// copy of this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the 
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
// DEALINGS IN THE SOFTWARE.

#include "TeclaInterface.hpp"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <signal.h>

#define USETECLA 1

namespace FreeMat {

  static char* messageContext = NULL;
  
  void TeclaInterface::outputMessage(const char* msg) {
    std::cout << msg;
    std::cout.flush();
  }

  void TeclaInterface::errorMessage(const char* msg) {
    std::cout << "Error: " << msg << "\n";
    if (messageContext != NULL)
      std::cout << "   at " << messageContext << "\n";
    std::cout.flush();
  }

  void TeclaInterface::warningMessage(const char* msg) {
    std::cout << "Warning: " << msg << "\n";
    if (messageContext != NULL)
      std::cout << "   at " << messageContext << "\n";
    std::cout.flush();
  }
  
  void TeclaInterface::setMessageContext(const char* context) {
    if (messageContext != NULL)
      free(messageContext);
    if (context != NULL) 
      messageContext = strdup(context);
    else
      messageContext = NULL;
  }

  /*.......................................................................
   * Search backwards for the potential start of a filename. This
   * looks backwards from the specified index in a given string,
   * stopping at the first unescaped space or the start of the line.
   *
   * Input:
   *  string  const char *  The string to search backwards in.
   *  back_from      int    The index of the first character in string[]
   *                        that follows the pathname.
   * Output:
   *  return        char *  The pointer to the first character of
   *                        the potential pathname, or NULL on error.
   */
  static char *start_of_path(const char *string, int back_from)
  {
    int i, j;
    /*
     * Search backwards from the specified index.
     */
    for(i=back_from-1; i>=0; i--) {
      int c = string[i];
      /*
       * Stop on unescaped spaces.
       */
      if(isspace((int)(unsigned char)c)) {
	/*
	 * The space can't be escaped if we are at the start of the line.
	 */
	if(i==0)
	  break;
	/*
	 * Find the extent of the escape characters which precedes the space.
	 */
	for(j=i-1; j>=0 && string[j]=='\\'; j--)
	  ;
	/*
	 * If there isn't an odd number of escape characters before the space,
	 * then the space isn't escaped.
	 */
	if((i - 1 - j) % 2 == 0)
	  break;
      } 
       else if (!isalpha(c) && !isdigit(c) && (c != '_') && (c != '.') && (c != '\\') && (c != '/'))
	 break;
    };
    return (char *)string + i + 1;
  }

  CPL_MATCH_FN(shell_list_fn)
  {
    TeclaInterface *res = (TeclaInterface *) data;
    return(pca_path_completions(cpl, res->ppc, line, word_end));
  }
  
  CPL_MATCH_FN(shell_cpl_fn)
  {
    /*
     * Get the resource object that was passed to gl_customize_completion().
     */
    TeclaInterface *res = (TeclaInterface *) data;
    /*
     * Find the start of the filename prefix to be completed, searching
     * backwards for the first unescaped space, or the start of the line.
     */
    char *start = start_of_path(line, word_end);
    /*
     * Skip spaces preceding the start of the prefix.
     */
//      while(start > line && isspace((int)(unsigned char) start[-1]))
//        start--;
     /*
      * If the preceeding character was not a ' (quote), then
      * do a command expansion, otherwise, do a filename expansion.
      */
     if (start[-1] != '\'') {
       stringVector completions;
       completions = res->context->getCurrentScope()->getCompletions(std::string(start));
       for (int i=0;i<completions.size();i++)
	 cpl_add_completion(cpl, line, start-line, word_end,
			    completions[i].c_str()+strlen(start),"","(");
       completions = res->context->getGlobalScope()->getCompletions(std::string(start));
       for (int i=0;i<completions.size();i++)
	 cpl_add_completion(cpl, line, start-line, word_end,
			    completions[i].c_str()+strlen(start),"","(");
       return 0;
     }
     else {
       cfc_file_start(res->cfc, (int)(start-line));
       int retval;
       retval = cpl_file_completions(cpl, res->cfc, line, word_end);
       return retval;
     }
  }

  CPL_CHECK_FN(cpl_check_dotm)
  {
    int slen;
    struct stat statbuf;
    if (stat(pathname, &statbuf) < 0)
      return 0;
    if (S_ISREG(statbuf.st_mode) == 0) return 0;
    slen = strlen(pathname);
    int endDotM;
    endDotM = ((slen >=3) && pathname[slen-1] == 'm' && 
	       pathname[slen-2] == '.');
    if (endDotM) {
      TeclaInterface *res;
      res = (TeclaInterface *) data;
      res->processFilename(pathname);
    }
    return endDotM;
  }
  
  TeclaInterface::TeclaInterface() {
  }

  void TeclaInterface::initialize(std::string path, Context *ctxt) {
    gl = new_GetLine(500, 60000);
    pc = new_PathCache();
    ppc = new_PcaPathConf(pc);
    cfc = new_CplFileConf();
    pca_set_check_fn(pc, cpl_check_dotm, this);
    gl_customize_completion(gl, this, shell_cpl_fn);
    gl_ignore_signal(gl,SIGINT);
    context = ctxt;
    pathSpec = path;
    rescanPath();
  }

  void TeclaInterface::processFilename(const char *pathname) {
    char buffer[1000];
    strcpy(buffer,pathname);
    // Trim ".m"
    int slen = strlen(buffer);
    buffer[slen-2] = 0;
    // Search for the delimiter
    char *ptr = buffer+slen-2;
    while ((*ptr != '/') && (*ptr != '\\') && (ptr > buffer)) {
      ptr--;
    }
    if ((*ptr == '/') || (*ptr == '\\'))
      ptr++;
    FunctionDef *fdef;
    if (!context->lookupFunctionGlobally(std::string(ptr),fdef)) {
      MFunctionDef *adef;
      adef = new MFunctionDef();
      adef->name = std::string(ptr);
      adef->fileName = std::string(pathname);
      context->insertFunctionGlobally(adef);
      if (transientScan)
	transientFuncs.push_back(std::string(ptr));
    }
  }

  void TeclaInterface::rescanPath() {
    CplMatches *cm;

    transientScan = false;
    // First, scan those files on the base path
    pca_scan_path(pc, pathSpec.c_str());
    cw = new_WordCompletion();
    // Force a rescan...
    cm = cpl_complete_word(cw,"",0,this,shell_list_fn);
    del_WordCompletion(cw);
    // Now, clear the transient functions from the context
    for (int i=0;i<transientFuncs.size();i++)
      context->deleteFunctionGlobally(transientFuncs[i]);
    transientFuncs.clear();
    transientScan = true;
    // Now scan the local directory...
    pca_scan_path(pc, ".");
    cw = new_WordCompletion();
    // Force a rescan...
    cm = cpl_complete_word(cw,"",0,this,shell_list_fn);
    del_WordCompletion(cw);
  }

  TeclaInterface::~TeclaInterface() {
    gl = del_GetLine(gl);
  }

  int TeclaInterface::getTerminalWidth() {
    GlTerminalSize tsize(gl_terminal_size(gl,80,25));
    return tsize.ncolumn;
  }
  
  char* TeclaInterface::getLine(const char* prompt) {
#ifdef USETECLA
    char *line;
    line = gl_get_line(gl, prompt, NULL, -1);
    return line;
#else
    static char buffer[1000];
    printf(prompt);
    fflush(stdout);
    fgets(buffer,sizeof(buffer),stdin);
    return buffer;
#endif
  }
}
