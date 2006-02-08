#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <glob.h>
#include <unistd.h>
#include <pwd.h>
#endif

#include "Interface.hpp"
#include "Context.hpp"
#include "File.hpp"
#include <algorithm>

#include <qglobal.h>

#ifdef WIN32
#define DELIM "\\"
#define S_ISREG(x) (x & _S_IFREG)
#include <direct.h>
#define PATH_DELIM ";"
#else
#define DELIM "/"
#define PATH_DELIM ":"
#endif

namespace FreeMat {

  char* TildeExpand(char* path) {
#ifdef WIN32
    return path;
#else
    char *newpath = path;
    if (path[0] == '~' && (path[1] == '/') || (path[1] == 0)) {
      char *home;
      home = getenv("HOME");
      if (home) {
	newpath = (char*) malloc(strlen(path) + strlen(home));
	strcpy(newpath,home);
	strcat(newpath,path+1);
      }
    } else if (path[0] == '~' && isalpha(path[1])) {
      char username[4096];
      char *cp, *dp;
      // Extract the user name
      cp = username;
      dp = path+1;
      while (*dp != '/')
	*cp++ = *dp++;
      *cp = 0;
      // Call getpwnam...
      struct passwd *dat = getpwnam(cp);
      if (dat) {
	newpath = (char*) malloc(strlen(path) + strlen(dat->pw_dir));
	strcpy(newpath,dat->pw_dir);
	strcat(newpath,dp);
      }
    }
    return newpath;
#endif
  }
  
  Interface::Interface() {
    m_context = NULL;
  }

  Interface::~Interface() {
  }

  void Interface::setContext(Context *ctxt) {
    if (m_context) delete m_context;
    m_context = ctxt;
  }

  void Interface::setAppPath(std::string path) {
    app_path = path;
  }

  std::string Interface::getAppPath() {
    return app_path;
  }

  void Interface::setPath(std::string path) {
    char* pathdata = strdup(path.c_str());
    // Search through the path
    char *saveptr = (char*) malloc(sizeof(char)*1024);
    char* token;
    token = strtok(pathdata,PATH_DELIM);
    m_userPath.clear();
    while (token != NULL) {
      if (strcmp(token,".") != 0)
	m_userPath << QString(TildeExpand(token));
      token = strtok(NULL,PATH_DELIM);
    }
    rescanPath();
  }

  std::string Interface::getPath() {
    std::string retpath;
    for (int i=0;i<m_userPath.size()-1;i++) 
      retpath = retpath + m_userPath[i].toStdString() + PATH_DELIM;
    if (m_userPath.size() > 0) 
      retpath = retpath + m_userPath[m_userPath.size()-1].toStdString();
    return retpath;
  }
  
  void Interface::rescanPath() {
    if (!m_context) return;
    m_context->flushTemporaryGlobalFunctions();
    for (int i=0;i<m_basePath.size();i++)
      scanDirectory(m_basePath[i].toStdString(),false,"");
    for (int i=0;i<m_userPath.size();i++)
      scanDirectory(m_userPath[i].toStdString(),false,"");
    // Scan the current working directory.
    char cwd[1024];
    getcwd(cwd,1024);
    scanDirectory(std::string(cwd),true,"");
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

  std::vector<std::string> Interface::GetCompletions(std::string line, 
						     int word_end, 
						     std::string &matchString) {
    std::vector<std::string> completions;
    /*
     * Find the start of the filename prefix to be completed, searching
     * backwards for the first unescaped space, or the start of the line.
     */
    char *start = start_of_path(line.c_str(), word_end);
    char *tmp;
    int mtchlen;
    mtchlen = word_end - (start-line.c_str());
    tmp = (char*) malloc(mtchlen+1);
    memcpy(tmp,start,mtchlen);
    tmp[mtchlen] = 0;
    matchString = std::string(tmp);
    
    /*
     *  the preceeding character was not a ' (quote), then
     * do a command expansion, otherwise, do a filename expansion.
     */
    if (start[-1] != '\'') {
      std::vector<std::string> local_completions;
      std::vector<std::string> global_completions;
      int i;
      local_completions = m_context->getCurrentScope()->getCompletions(std::string(start));
      global_completions = m_context->getGlobalScope()->getCompletions(std::string(start));
      for (i=0;i<local_completions.size();i++)
	completions.push_back(local_completions[i]);
      for (i=0;i<global_completions.size();i++)
	completions.push_back(global_completions[i]);
      std::sort(completions.begin(),completions.end());
      return completions;
    } else {
#ifdef WIN32
      HANDLE hSearch;
      WIN32_FIND_DATA FileData;
      std::string pattern(tmp);
      pattern.append("*");
      hSearch = FindFirstFile(pattern.c_str(),&FileData);
      if (hSearch != INVALID_HANDLE_VALUE) {
	// Windows does not return any part of the path in the completion,
	// So we need to find the base part of the pattern.
	int lastslash;
	std::string prefix;
	lastslash = pattern.find_last_of("/");
	if (lastslash == -1) {
	  lastslash = pattern.find_last_of("\\");
	}
	if (lastslash != -1)
	  prefix = pattern.substr(0,lastslash+1);
	completions.push_back(prefix + FileData.cFileName);
	while (FindNextFile(hSearch, &FileData))
	  completions.push_back(prefix + FileData.cFileName);
      }
      FindClose(hSearch);
      return completions;
#else
      glob_t names;
      std::string pattern(tmp);
      pattern.append("*");
      glob(pattern.c_str(), GLOB_MARK, NULL, &names);
      int i;
      for (i=0;i<names.gl_pathc;i++) 
	completions.push_back(names.gl_pathv[i]);
      globfree(&names);
      free(tmp);
      return completions;
#endif
    }
  }

  void Interface::setBasePath(QStringList pth) {
    m_basePath = pth;
  }

  void Interface::setUserPath(QStringList pth) {
    m_userPath = pth;
  }

  void Interface::scanDirectory(std::string scdir, bool tempfunc,
				std::string prefix) {
#ifdef WIN32
    HANDLE hSearch;
    WIN32_FIND_DATA FileData;
    std::string searchpat(scdir + "\\*.m");
    hSearch = FindFirstFile(searchpat.c_str(), &FileData);
    if (hSearch != INVALID_HANDLE_VALUE) {
      if (prefix.empty())
	procFile(std::string(FileData.cFileName),
		 scdir + "\\" + std::string(FileData.cFileName),tempfunc);
      else
	procFile(prefix + ":" + std::string(FileData.cFileName),
		 scdir + "\\" + std::string(FileData.cFileName),tempfunc);
      while (FindNextFile(hSearch, &FileData)) {
	if (prefix.empty())
	  procFile(std::string(FileData.cFileName),
		   scdir + "\\" + std::string(FileData.cFileName),tempfunc);
	else
	  procFile(prefix + ":" + std::string(FileData.cFileName),
		   scdir + "\\" + std::string(FileData.cFileName),tempfunc);
      }
      FindClose(hSearch);
    }    
    searchpat = std::string(scdir+"\\@*");
    hSearch = FindFirstFile(searchpat.c_str(), &FileData);
    if (hSearch != INVALID_HANDLE_VALUE) {
      scanDirectory(scdir + "\\" + std::string(FileData.cFileName),tempfunc,
		    std::string(FileData.cFileName));
      while (FindNextFile(hSearch, &FileData)) {
	scanDirectory(scdir + "\\" + std::string(FileData.cFileName),tempfunc,
		      std::string(FileData.cFileName));
      }
      FindClose(hSearch);
    }
    searchpat = std::string(scdir+"\\private");
    hSearch = FindFirstFile(searchpat.c_str(), &FileData);
    if (hSearch != INVALID_HANDLE_VALUE) {
      scanDirectory(scdir + "\\" + std::string(FileData.cFileName),tempfunc,
		    scdir + "\\" + std::string(FileData.cFileName));
      FindClose(hSearch);
    }
#else
    // Open the directory
    DIR *dir;
  
    dir = opendir(scdir.c_str());
    if (dir == NULL) return;
    // Scan through the directory..
    struct dirent *fspec;
    char *fname;
    std::string fullname;
    while (fspec = readdir(dir)) {
      // Get the name of the entry
      fname = fspec->d_name;
      // Check for '.' and '..'
      if ((strcmp(fname,".") == 0) || (strcmp(fname,"..") == 0)) 
	continue;
      // Stat the file...
      fullname = std::string(scdir + std::string(DELIM) + fname);
      if (fname[0] == '@')
	scanDirectory(fullname,tempfunc,fname);
      if (strcmp(fname,"private")==0)
	scanDirectory(fullname,tempfunc,fullname);
      if (prefix.empty())
	procFile(fname,fullname,tempfunc);
      else
	// Class name mangling here...
	procFile(prefix + ":" + fname,fullname,tempfunc);
    }
    closedir(dir);
#endif
  }

  void Interface::procFile(std::string fname, std::string fullname, bool tempfunc) {
#ifdef WIN32
    struct stat filestat;
    char buffer[1024];
    char *fnamec;

    fnamec = strdup(fname.c_str());
    stat(fullname.c_str(),&filestat);
    if (S_ISREG(filestat.st_mode)) {
      int namelen;
      namelen = strlen(fnamec);
      if (fnamec[namelen-2] == '.' && 
	  (fnamec[namelen-1] == 'm' ||
	   fnamec[namelen-1] == 'M')) {
	fnamec[namelen-2] = 0;
	MFunctionDef *adef;
	adef = new MFunctionDef();
	adef->name = std::string(fnamec);
	adef->fileName = fullname;
	m_context->insertFunctionGlobally(adef, tempfunc);
      } else if (fnamec[namelen-2] == '.' && 
		 (fnamec[namelen-1] == 'p' ||
		  fnamec[namelen-1] == 'P')) {
	fnamec[namelen-2] = 0;
	MFunctionDef *adef;
	// Open the file
	try {
	  File *f = new File(fullname.c_str(),"rb");
	  Serialize *s = new Serialize(f);
	  s->handshakeClient();
	  s->checkSignature('p',1);
	  adef = ThawMFunction(s);
	  adef->pcodeFunction = true;
	  delete f;
	  m_context->insertFunctionGlobally(adef, tempfunc);
	} catch (Exception &e) {
	}
      }
    }
    free(fnamec);
#else
    struct stat filestat;
    char buffer[1024];
    char fnamec[1024];
    strcpy(fnamec,fname.c_str());
    stat(fullname.c_str(),&filestat);
    if (S_ISREG(filestat.st_mode)) {
      int namelen;
      namelen = strlen(fnamec);
      if ((namelen > 2) && 
	  fnamec[namelen-2] == '.' && 
	  (fnamec[namelen-1] == 'm' ||
	   fnamec[namelen-1] == 'M')) {
	fnamec[namelen-2] = 0;
	MFunctionDef *adef;
	adef = new MFunctionDef();
	adef->name = std::string(fnamec);
	adef->fileName = fullname;
	m_context->insertFunctionGlobally(adef, tempfunc);
      } else if (fnamec[namelen-2] == '.' && 
		 (fnamec[namelen-1] == 'p' ||
		  fnamec[namelen-1] == 'P')) {
	fnamec[namelen-2] = 0;
	MFunctionDef *adef;
	try {
	  // Open the file
	  File *f = new File(fullname.c_str(),"rb");
	  Serialize *s = new Serialize(f);
	  s->handshakeClient();
	  s->checkSignature('p',1);
	  adef = ThawMFunction(s);
	  adef->pcodeFunction = true;
	  delete f;
	  m_context->insertFunctionGlobally(adef, tempfunc);
	} catch (Exception &e) {
	}
      }
    } else if (S_ISLNK(filestat.st_mode)) {
      int lncnt = readlink(fullname.c_str(),buffer,1024);
      buffer[lncnt] = 0;
      procFile(fnamec, std::string(buffer),tempfunc);
    }
#endif
  }
}
