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

#include "Core.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include "Malloc.hpp"
#include "MemPtr.hpp"
#if HAVE_PCRE
#include <pcre.h>
#endif


ArrayVector StrCmpFunction(int nargout, const ArrayVector& arg) {
  Array retval, arg1, arg2;
  if (arg.size() != 2)
    throw Exception("strcomp function requires two arguments");
  arg1 = arg[0];
  arg2 = arg[1];
  if (!(arg1.isString()))
    return singleArrayVector(Array::logicalConstructor(false));
  if (!(arg2.isString()))
    return singleArrayVector(Array::logicalConstructor(false));
  if (!(arg1.dimensions().equals(arg2.dimensions())))
    retval = Array::logicalConstructor(false);
  else {
    string s1 = arg1.getContentsAsString();
    string s2 = arg2.getContentsAsString();
    retval = Array::logicalConstructor(s1 == s2);
  }
  ArrayVector o;
  o.push_back(retval);
  return o;
}

//!
//@Module STRSTR String Search Function
//@@Section STRING
//@@Usage
//Searches for the first occurance of one string inside another.
//The general syntax for its use is
//@[
//   p = strstr(x,y)
//@]
//where @|x| and @|y| are two strings.  The returned integer @|p|
//indicates the index into the string @|x| where the substring @|y|
//occurs.  If no instance of @|y| is found, then @|p| is set to
//zero.
//@@Example
//Some examples of @|strstr| in action
//@<
//strstr('hello','lo')
//strstr('quick brown fox','own')
//strstr('free stuff','lunch')
//@>
//!
ArrayVector StrStrFunction(int nargout, const ArrayVector& arg) {
  Array retval, arg1, arg2;
  if (arg.size() != 2)
    throw Exception("strstr function requires two string arguments");
  arg1 = arg[0];
  arg2 = arg[1];
  if (!(arg1.isString()))
    throw Exception("strstr function requires two string arguments");
  if (!(arg2.isString()))
    throw Exception("strstr function requires two string arguments");
  string s1 = arg1.getContentsAsString();
  string s2 = arg2.getContentsAsString();
  size_t pos = s1.find(s2);
  int retndx;
  if (pos == string::npos)
    retndx = 0;
  else
    retndx = pos+1;
  return singleArrayVector(Array::int32Constructor(retndx));
}

char* strrep(const char* source, const char* pattern, const char* replace) {
  // Count how many instances of 'pattern' occur
  int instances = 0;
  const char *cp = source;
  while (cp) {
    cp = strstr(cp,pattern);
    if (cp) {
      cp += strlen(pattern);
      instances++;
    }
  }
  // The output array should be large enough...
  int outlen = strlen(source) + instances*(strlen(replace) - strlen(pattern)) + 1;
  char *op = (char*) malloc(sizeof(char)*outlen);
  char *opt = op;
  // Retrace through the source array
  cp = source;
  const char *lastp = source;
  while (cp) {
    cp = strstr(cp,pattern);
    if (cp) {
      memcpy(opt,lastp,(cp-lastp));
      opt += (cp-lastp);
      memcpy(opt,replace,strlen(replace));
      opt += strlen(replace);
      cp += strlen(pattern);
      lastp = cp;
      instances++;
    } else
      memcpy(opt,lastp,strlen(source)-(lastp-source)+1);
  }
  return op;
}

ArrayVector StrRepStringFunction(int nargout, const ArrayVector& arg) {
  Array arg1, arg2, arg3;
  if (arg.size() != 3)
    throw Exception("strrep_string function requires three string arguments");
  arg1 = arg[0];
  arg2 = arg[1];
  arg3 = arg[2];
  if (!(arg1.isString()))
    throw Exception("strrep_string function requires three string arguments");
  if (!(arg2.isString()))
    throw Exception("strrep_string function requires three string arguments");
  if (!(arg3.isString()))
    throw Exception("strrep_string function requires three string arguments");
  string s1 = arg1.getContentsAsString();
  string s2 = arg2.getContentsAsString();
  string s3 = arg3.getContentsAsString();
  char *cp = strrep(s1.c_str(),s2.c_str(),s3.c_str());
  ArrayVector retval(singleArrayVector(Array::stringConstructor(cp)));
  free(cp);
  return retval;
}

//!
//@Module REGEXP Regular Expression Matching Function
//@@Section STRING
//@@Usage
//Matches regular expressions in the provided string.  This function is
//complicated, and compatibility with MATLABs syntax is not perfect.  The
//syntax for its use is
//@[
//  regexp('str','expr')
//@]
//which returns a row vector containing the starting index of each substring
//of @|str| that matches the regular expression described by @|expr|.  The
//second form of @|regexp| returns six outputs in the following order:
//@[
//  [start stop tokenExtents match tokens names] = regexp('str','expr')
//@]
//where the meaning of each of the outputs is defined below.
//\begin{itemize}
//\item @|start| is a row vector containing the starting index of each 
//substring that matches the regular expression.
//\item @|stop| is a row vector containing the ending index of each 
//substring that matches the regular expression.
//\item @|tokenExtents| is a cell array containing the starting and ending
//indices of each substring that matches the @|tokens| in the regular
//expression.  A token is a captured part of the regular expression.
//If the @|'once'| mode is used, then this output is a @|double| array.
//\item @|match| is a cell array containing the text for each substring
//that matches the regular expression.  In @|'once'| mode, this is a 
//string.
//\item @|tokens| is a cell array of cell arrays of strings that correspond
//to the tokens in the regular expression.  In @|'once'| mode, this is a
//cell array of strings.
//\item @|named| is a structure array containing the named tokens captured
//in a regular expression. Each named token is assigned a field in the resulting
//structure array, and each element of the array corresponds to a different
//match.
//\end{itemize}
//If you want only some of the the outputs,  you can use the 
//following variant of @|regexp|:
//@[
//  [o1 o2 ...] = regexp('str','expr', 'p1', 'p2', ...)
//@]
//where @|p1| etc. are the names of the outputs (and the order we want
//the outputs in).  As a final variant, you can supply some mode 
//flags to @|regexp|
//@[
//  [o1 o2 ...] = regexp('str','expr', p1, p2, ..., 'mode1', 'mode2')
//@]
//where acceptable @|mode| flags are:
//\begin{itemize}
//\item @|'once'| - only the first match is returned.
//\item @|'matchcase'| - letter case must match (selected by default for @|regexp|)
//\item @|'ignorecase'| - letter case is ignored (selected by default for @|regexpi|)
//\item @|'dotall'| - the @|'.'| operator matches any character (default)
//\item @|'dotexceptnewline'| - the @|'.'| operator does not match the newline character
//\item @|'stringanchors'| - the @|^| and @|$| operators match at the beginning and 
//end (respectively) of a string.
//\item @|'lineanchors'| - the @|^| and @|$| operators match at the beginning and
//end (respectively) of a line.
//\item @|'literalspacing'| - the space characters and comment characters @|#| are matched
//as literals, just like any other ordinary character (default).
//\item @|'freespacing'| - all spaces and comments are ignored in the regular expression.
//You must use '\ ' and '\#' to match spaces and comment characters, respectively.
//\end{itemize}
//Note the following behavior differences between MATLABs regexp and FreeMats:
//\begin{itemize}
//\item If you have an old version of @|pcre| installed, then named tokens must use the
//older @|<?P<name>| syntax, instead of the new @|<?<name>| syntax.  
//\item The @|pcre| library is pickier about named tokens and their appearance in 
//expressions.  So, for example, the regexp from the MATLAB 
//manual
//@[
//   '(?<first>\w+)\s+(?<last>\w+)|(?<last>\w+),\s+(?<first>\w+)'
//@]
//does not work correctly (as of this writing) because the same named 
//tokens appear multiple
//times.  The workaround is to assign different names to each token, and then collapse
//the results later.
//\end{itemize}
//@@Example
//Some examples of using the @|regexp| function
//@<
//[start,stop,tokenExtents,match,tokens,named] = regexp('quick down town zoo','(.)own')
//@>
//!
static bool isSlotSpec(string t) {
  return ((t == "start") ||
	  (t == "end") ||
	  (t == "tokenExtents") ||
	  (t == "match") ||
	  (t == "tokens") ||
	  (t == "names"));
}

ArrayVector RegExpCoreFunction(rvstring stringed_args, bool defaultMatchCase) {
#if HAVE_PCRE
  // These are the default options
  bool globalMatch = true;
  bool matchCase = defaultMatchCase;
  bool greedyDot = true;
  bool stringAnchors = true;
  bool literalSpacing = true;
  
  int start_slot = 0;
  int end_slot = 1;
  int tokenExtents_slot = 2;
  int match_slot = 3;
  int tokens_slot = 4;
  int names_slot = 5;
  int count_slots = 6;

  // This is a cut-paste-hack of the pcredemo program that comes with pcre
  QList<uint32> startList;
  QList<uint32> stopList;
  QList<uint32> tokenStartList;
  QList<uint32> tokenStopList;
  QList<QList<uint32> > tokenExtentsList;
  QList<QStringList> tokenList;
  QStringList matchList;
  rvstring namedTokenNames;
  QList<QStringList> namedTokenValues;

  pcre *re;
  const char *error;
  const char *pattern;
  const char *subject;
  unsigned char *name_table;
  int erroffset;
  int namecount;
  int name_entry_size;
  int subject_length;
  int rc, i;
  ArrayVector retvec;

  if (stringed_args.size() < 2)
    throw Exception("regexpcore requires at least two arguments - both strings, the data string and pattern string");

  // Check for output specifiers
  bool slot_specs_present = false;
  for (int i=2;i<stringed_args.size();i++) 
    if (isSlotSpec(stringed_args[i])) {
      slot_specs_present = true;
      break;
    }
  if (slot_specs_present) {
    // output specifiers are active... eliminate all outputs by default
    start_slot = end_slot = tokenExtents_slot = match_slot = tokens_slot = names_slot = -1;
    count_slots = 0;
    // Process inputs
    for (int i=2;i<stringed_args.size();i++) {
      string t = stringed_args[i];
      if (t == "start")  start_slot = count_slots++;
      if (t == "end") end_slot = count_slots++;
      if (t == "tokenExtents") tokenExtents_slot = count_slots++;
      if (t == "match") match_slot = count_slots++;
      if (t == "tokens") tokens_slot = count_slots++;
      if (t == "names") names_slot = count_slots++;
    }
  }
  
  for (int i=0;i<count_slots;i++)
    retvec << Array::emptyConstructor();
  
  for (int j=2;j<stringed_args.size();j++) {
    if (stringed_args[j]=="once")
      globalMatch = false;
    else if (stringed_args[j]=="matchcase")
      matchCase = true;
    else if (stringed_args[j]=="ignorecase")
      matchCase = false;
    else if (stringed_args[j]=="dotall")
      greedyDot = true;
    else if (stringed_args[j]=="dotexceptnewline")
      greedyDot = false;
    else if (stringed_args[j]=="stringanchors")
      stringAnchors = true;
    else if (stringed_args[j]=="lineanchors")
      stringAnchors = false;
    else if (stringed_args[j]=="literalspacing")
      literalSpacing = true;
    else if (stringed_args[j]=="freespacing")
      literalSpacing = false;
  }

  pattern = stringed_args[1].c_str();
  subject = stringed_args[0].c_str();
  subject_length = (int)strlen(subject);
  QString qsubject(subject);

  int options = 0;

  if (!matchCase) options |= PCRE_CASELESS;
  if (greedyDot) options |= PCRE_DOTALL;
  if (!stringAnchors) options |= PCRE_MULTILINE;
  if (!literalSpacing) options |= PCRE_EXTENDED;

  /*************************************************************************
   * Now we are going to compile the regular expression pattern, and handle *
   * and errors that are detected.                                          *
   *************************************************************************/
  
  re = pcre_compile(
		    pattern,              /* the pattern */
		    options,              /* default options */
		    &error,               /* for error message */
		    &erroffset,           /* for error offset */
		    NULL);                /* use default character tables */
  
  /* Compilation failed: print the error message and exit */
  
  if (re == NULL) 
    throw Exception(string("regular expression compilation failed at offset ") + 
		    erroffset + ": " + error);
  
  /* Determine how many capture expressions there are */
  int captureCount;
  rc = pcre_fullinfo(re,NULL,PCRE_INFO_CAPTURECOUNT,&captureCount);
  
  /* Allocate the output vector */
  int ovectorSize = (captureCount+1)*3;
  MemBlock<int> ovectorData(ovectorSize);
  int *ovector(&ovectorData);
  
  /*************************************************************************
   * If the compilation succeeded, we call PCRE again, in order to do a     *
   * pattern match against the subject string. This does just ONE match. If *
   * further matching is needed, it will be done below.                     *
   *************************************************************************/
  
  rc = pcre_exec(
		 re,                   /* the compiled pattern */
		 NULL,                 /* no extra data - we didn't study the pattern */
		 subject,              /* the subject string */
		 subject_length,       /* the length of the subject */
		 0,                    /* start at offset 0 in the subject */
		 0,                    /* default options */
		 ovector,              /* output vector for substring information */
		 ovectorSize);         /* number of elements in the output vector */

  /* Matching failed: handle error cases */
  
  if (rc < 0) {
    pcre_free(re);     /* Release memory used for the compiled pattern */
    return retvec;
  }
  
  /* Match succeded */

  startList << ovector[0]+1;
  stopList << ovector[1];
  matchList << qsubject.mid(ovector[0],ovector[1]-ovector[0]);

  /*************************************************************************
   * We have found the first match within the subject string. If the output *
   * vector wasn't big enough, set its size to the maximum. Then output any *
   * substrings that were captured.                                         *
   *************************************************************************/

  /* The output vector wasn't big enough */

  if (rc == 0) 
    throw Exception("internal error - more outputs than expected?");

  /* Show substrings stored in the output vector by number. Obviously, in a real
     application you might want to do things other than print them. */

  QList<uint32> tEList;
  QStringList   tList;
  for (i = 1; i < rc; i++)
    {
      tEList << ovector[2*i]+1;
      tEList << ovector[2*i+1];
      tList << qsubject.mid(ovector[2*i],ovector[2*i+1]-ovector[2*i]);
    }

  tokenExtentsList << tEList;
  tokenList << tList;


  /* See if there are any named substrings, and if so, show them by name. First
     we have to extract the count of named parentheses from the pattern. */
  
  (void)pcre_fullinfo(
		      re,                   /* the compiled pattern */
		      NULL,                 /* no extra data - we didn't study the pattern */
		      PCRE_INFO_NAMECOUNT,  /* number of named substrings */
		      &namecount);          /* where to put the answer */


  if (namecount > 0)  {
    unsigned char *tabptr;
    
    /* Before we can access the substrings, we must extract the table for
       translating names to numbers, and the size of each entry in the table. */
    
    (void)pcre_fullinfo(
			re,                       /* the compiled pattern */
			NULL,                     /* no extra data - we didn't study the pattern */
			PCRE_INFO_NAMETABLE,      /* address of the table */
			&name_table);             /* where to put the answer */
    
    (void)pcre_fullinfo(
			re,                       /* the compiled pattern */
			NULL,                     /* no extra data - we didn't study the pattern */
			PCRE_INFO_NAMEENTRYSIZE,  /* size of each entry in the table */
			&name_entry_size);        /* where to put the answer */
    
    /* Now we can scan the table and, for each entry, print the number, the name,
       and the substring itself. */
    
    tabptr = name_table;
    for (i = 0; i < namecount; i++)
      {
	namedTokenValues << QStringList();
	int n = (tabptr[0] << 8) | tabptr[1];
	namedTokenNames << QString((char*)(tabptr+2)).left(name_entry_size-3).toStdString();
	namedTokenValues[i] << qsubject.mid(ovector[2*n],ovector[2*n+1]-ovector[2*n]);
	tabptr += name_entry_size;
      }
  }


  /*************************************************************************
   * If the "-g" option was given on the command line, we want to continue  *
   * to search for additional matches in the subject string, in a similar   *
   * way to the /g option in Perl. This turns out to be trickier than you   *
   * might think because of the possibility of matching an empty string.    *
   * What happens is as follows:                                            *
   *                                                                        *
   * If the previous match was NOT for an empty string, we can just start   *
   * the next match at the end of the previous one.                         *
   *                                                                        *
   * If the previous match WAS for an empty string, we can't do that, as it *
   * would lead to an infinite loop. Instead, a special call of pcre_exec() *
   * is made with the PCRE_NOTEMPTY and PCRE_ANCHORED flags set. The first  *
   * of these tells PCRE that an empty string is not a valid match; other   *
   * possibilities must be tried. The second flag restricts PCRE to one     *
   * match attempt at the initial string position. If this match succeeds,  *
   * an alternative to the empty string match has been found, and we can    *
   * proceed round the loop.                                                *
   *************************************************************************/


  /* Loop for second and subsequent matches */
  
  if (globalMatch && (ovector[1] < subject_length)) {
    for (;;)
      {
	int options = 0;                 /* Normally no options */
	int start_offset = ovector[1];   /* Start at end of previous match */
	
	/* If the previous match was for an empty string, we are finished if we are
	   at the end of the subject. Otherwise, arrange to run another match at the
	   same point to see if a non-empty match can be found. */
	
	if (ovector[0] == ovector[1])
	  {
	    if (ovector[0] == subject_length) break;
	    options = PCRE_NOTEMPTY | PCRE_ANCHORED;
	  }
	
	/* Run the next matching operation */
	
	rc = pcre_exec(
		       re,                   /* the compiled pattern */
		       NULL,                 /* no extra data - we didn't study the pattern */
		       subject,              /* the subject string */
		       subject_length,       /* the length of the subject */
		       start_offset,         /* starting offset in the subject */
		       options,              /* options */
		       ovector,              /* output vector for substring information */
		       ovectorSize);           /* number of elements in the output vector */
	
	/* This time, a result of NOMATCH isn't an error. If the value in "options"
	   is zero, it just means we have found all possible matches, so the loop ends.
	   Otherwise, it means we have failed to find a non-empty-string match at a
	   point where there was a previous empty-string match. In this case, we do what
	   Perl does: advance the matching position by one, and continue. We do this by
	   setting the "end of previous match" offset, because that is picked up at the
	   top of the loop as the point at which to start again. */
	
	if (rc == PCRE_ERROR_NOMATCH)
	  {
	    if (options == 0) break;
	    ovector[1] = start_offset + 1;
	    continue;    /* Go round the loop again */
	  }
	
	/* Other matching errors are not recoverable. */
	
	if (rc < 0)
	  {
	    break;
	  }
	
	/* Match succeded */
	
	/* The match succeeded, but the output vector wasn't big enough. */
	
	if (rc == 0)
	  throw Exception("internal error - more outputs than expected?");
	
	startList << ovector[0]+1;
	stopList << ovector[1];
	matchList << qsubject.mid(ovector[0],ovector[1]-ovector[0]);

	QList<uint32> tEList;
	QStringList   tList;
	for (i = 1; i < rc; i++)
	  {
	    tEList << ovector[2*i]+1;
	    tEList << ovector[2*i+1];
	    tList << qsubject.mid(ovector[2*i],ovector[2*i+1]-ovector[2*i]);
	  }
	
	tokenExtentsList << tEList;
	tokenList << tList;

	if (namecount > 0) {
	  unsigned char *tabptr = name_table;
	  tabptr = name_table;
	  for (i = 0; i < namecount; i++)
	    {
	      int n = (tabptr[0] << 8) | tabptr[1];
	      namedTokenValues[i] << qsubject.mid(ovector[2*n],ovector[2*n+1]-ovector[2*n]);
	      tabptr += name_entry_size;
	    }
	}
      }      /* End of loop to find second and subsequent matches */
  }

  // Return this data to the user
  Array start(Uint32VectorFromQList(startList));
  Array end(Uint32VectorFromQList(stopList));
  Array matches(CellArrayFromQStringList(matchList));
  // Now build the tokens array
  ArrayVector tokensArrayContents;
  for (int i=0;i<tokenList.size();i++) 
    tokensArrayContents << CellArrayFromQStringList(tokenList[i]);
  Array tokens(Array::cellConstructor(tokensArrayContents));
  // Finally the token extents array
  ArrayVector tokensExtentsContents;
  for (int i=0;i<tokenExtentsList.size();i++)
    tokensExtentsContents << Uint32VectorFromQList(tokenExtentsList[i]);
  Array tokenExtents(Array::cellConstructor(tokensExtentsContents));
  // The named token data has to be resliced
  ArrayVector namedTokenValueContents;
  for (int i=0;i<namedTokenValues.size();i++)
    namedTokenValueContents << CellArrayFromQStringList(namedTokenValues[i]);
  Array namedTokens(Array::structConstructor(namedTokenNames,namedTokenValueContents));
  // Stuff it all into a return vector
  pcre_free(re);
  if (start_slot >= 0)
    retvec[start_slot] = start;
  if (end_slot >= 0)
    retvec[end_slot] = end;
  if (tokenExtents_slot >= 0)
    retvec[tokenExtents_slot] = tokenExtents;
  if (match_slot >= 0)
    retvec[match_slot] = matches;
  if (tokens_slot >= 0)
    retvec[tokens_slot] = tokens;
  if (names_slot >= 0)
    retvec[names_slot] = namedTokens;
  return retvec;
#else
  throw Exception("regexp support not compiled in (requires PCRE library)");
#endif  

}

static bool IsCellStrings(Array t) {
  if (t.dataClass() != FM_CELL_ARRAY) return false;
  const Array *dp = (const Array *) t.getDataPointer();
  for (int j=0;j<t.getLength();j++)
    if (!dp[j].isString()) return false;
  return true;
}

// res is organized like this:
//  <o1 o2 o3 o4>
//  <o1 o2 o3 o4>
// ...
// We want to perform a transpose
static ArrayVector CellifyRegexpResults(QList<ArrayVector> res, Dimensions dims) {
  ArrayVector retVec;
  if (res.size() == 0) return ArrayVector();
  for (int i=0;i<res[0].size();i++) {
    ArrayVector slice;
    for (int j=0;j<res.size();j++) 
      slice << res[j][i];
    Array sliceArray(Array::cellConstructor(slice));
    sliceArray.reshape(dims);
    retVec << sliceArray;
  }
  return retVec;
}

ArrayVector RegExpWrapperFunction(int nargout, const ArrayVector& arg, 
				  bool caseMatch) {
  if (arg.size() < 2) throw Exception("regexp requires at least two arguments to function");
  for (int i=2;i<arg.size();i++) 
    if (!arg[i].isString())
      throw Exception("all arguments to regexp must be strings");
  if (arg[0].isString() && arg[1].isString()) {
    // If both arguments are scalar strings...
    rvstring stringed_args;
    // Convert the argument array to strings
    for (int i=0;i<arg.size();i++) 
      stringed_args << ArrayToString(arg[i]);
    return RegExpCoreFunction(stringed_args,caseMatch);
  } else if (IsCellStrings(arg[0]) && arg[1].isString()) {
    const Array *dp = (const Array *) arg[0].getDataPointer();
    QList<ArrayVector> results;
    for (int j=0;j<arg[0].getLength();j++) {
      rvstring stringed_args;
      stringed_args << ArrayToString(dp[j]);
      for (int i=1;i<arg.size();i++) 
	stringed_args << ArrayToString(arg[i]);
      results << RegExpCoreFunction(stringed_args,caseMatch);
    }
    return CellifyRegexpResults(results,arg[0].dimensions());
  } else if (arg[0].isString() && IsCellStrings(arg[1])) {
    const Array *dp = (const Array *) arg[1].getDataPointer();
    QList<ArrayVector> results;
    for (int j=0;j<arg[1].getLength();j++) {
      rvstring stringed_args;
      stringed_args << ArrayToString(arg[0]);
      stringed_args << ArrayToString(dp[j]);
      for (int i=2;i<arg.size();i++) 
	stringed_args << ArrayToString(arg[i]);
      results << RegExpCoreFunction(stringed_args,caseMatch);
    }
    return CellifyRegexpResults(results,arg[1].dimensions());
  } else if (IsCellStrings(arg[0]) && IsCellStrings(arg[1])) {
    if (arg[0].getLength() != arg[1].getLength())
      throw Exception("cell-arrays of strings as the first two arguments to regexp must be the same size");
    const Array *dp = (const Array *) arg[0].getDataPointer();
    const Array *sp = (const Array *) arg[1].getDataPointer();
    QList<ArrayVector> results;
    for (int j=0;j<arg[0].getLength();j++) {
      rvstring stringed_args;
      stringed_args << ArrayToString(dp[j]);
      stringed_args << ArrayToString(sp[j]);
      for (int i=2;i<arg.size();i++) 
	stringed_args << ArrayToString(arg[i]);
      results << RegExpCoreFunction(stringed_args,caseMatch);
    }
    return CellifyRegexpResults(results,arg[0].dimensions());
  } else {
    throw Exception("first two arguments to regexp must be strings of cell-arrays of strings");
  }
  return ArrayVector();
}

ArrayVector RegExpFunction(int nargout, const ArrayVector&arg) {
  return RegExpWrapperFunction(nargout, arg, true);
}

ArrayVector RegExpIFunction(int nargout, const ArrayVector& arg) {
  return RegExpWrapperFunction(nargout, arg, false);
}

// Perform a replace 
string RegExpRepCoreFunction(string subject,
			     string pattern,
			     rvstring modes,
			     rvstring replacements) {
#if HAVE_PCRE
  // These are the default options
  bool globalMatch = true;
  bool matchCase = true;
  bool greedyDot = true;
  bool stringAnchors = true;
  bool literalSpacing = true;
  
  pcre *re;
  const char *error;
  int erroffset;
  int namecount;
  int name_entry_size;
  int rc, i;

  for (int j=0;j<modes.size();j++) {
    if (modes[j]=="once")
      globalMatch = false;
    else if (modes[j]=="matchcase")
      matchCase = true;
    else if (modes[j]=="ignorecase")
      matchCase = false;
    else if (modes[j]=="dotall")
      greedyDot = true;
    else if (modes[j]=="dotexceptnewline")
      greedyDot = false;
    else if (modes[j]=="stringanchors")
      stringAnchors = true;
    else if (modes[j]=="lineanchors")
      stringAnchors = false;
    else if (modes[j]=="literalspacing")
      literalSpacing = true;
    else if (modes[j]=="freespacing")
      literalSpacing = false;
  }

  int options = 0;

  if (!matchCase) options |= PCRE_CASELESS;
  if (greedyDot) options |= PCRE_DOTALL;
  if (!stringAnchors) options |= PCRE_MULTILINE;
  if (!literalSpacing) options |= PCRE_EXTENDED;

  /*************************************************************************
   * Now we are going to compile the regular expression pattern, and handle *
   * and errors that are detected.                                          *
   *************************************************************************/
  
  re = pcre_compile(
		    pattern.c_str(),      /* the pattern */
		    options,              /* default options */
		    &error,               /* for error message */
		    &erroffset,           /* for error offset */
		    NULL);                /* use default character tables */
  
  /* Compilation failed: print the error message and exit */
  
  if (re == NULL) 
    throw Exception(string("regular expression compilation failed at offset ") + 
		    erroffset + ": " + error);

  /* Determine how many capture expressions there are */
  int captureCount;
  rc = pcre_fullinfo(re,NULL,PCRE_INFO_CAPTURECOUNT,&captureCount);
  
  /* Allocate the output vector */
  int ovectorSize = (captureCount+1)*3;
  MemBlock<int> ovectorData(ovectorSize);
  int *ovector(&ovectorData);
  
  /*************************************************************************
   * If the compilation succeeded, we call PCRE again, in order to do a     *
   * pattern match against the subject string. This does just ONE match. If *
   * further matching is needed, it will be done below.                     *
   *************************************************************************/
  
  rc = pcre_exec(
		 re,                   /* the compiled pattern */
		 NULL,                 /* no extra data - we didn't study the pattern */
		 subject.c_str(),      /* the subject string */
		 subject.size(),       /* the length of the subject */
		 0,                    /* start at offset 0 in the subject */
		 0,                    /* default options */
		 ovector,              /* output vector for substring information */
		 ovectorSize);         /* number of elements in the output vector */

  /* Matching failed: handle error cases */
  
  if (rc < 0) {
    pcre_free(re);     /* Release memory used for the compiled pattern */
    return subject;
  }
  
  string outputString;
  string tokenSelect;
  int nextReplacement = 0;
  int inputPointer = 0;
  int outputPtr = 0;
  int digitFinder = 0;
  int replacementLength = replacements[nextReplacement].size();
  int tokenNumber;
  /* Match succeeded... start building up the output string */
  while (inputPointer < ovector[0]) outputString += subject[inputPointer++];
  /* Now insert the replacement string */
  while (outputPtr < replacementLength) {
    if (replacements[nextReplacement][outputPtr] != '$')
      outputString += replacements[nextReplacement][outputPtr++];
    else
      if ((outputPtr < (replacementLength-1)) &&
	  isdigit(replacements[nextReplacement][outputPtr+1])) {
	// Try to collect a token name
	digitFinder = outputPtr+1;
	while ((digitFinder < replacementLength) && 
	       isdigit(replacements[nextReplacement][digitFinder])) {
	  // Add this digit to the token name
	  tokenSelect += replacements[nextReplacement][digitFinder];
	  digitFinder++;
	}
	// try to map this to a token number
	tokenNumber = atoi(tokenSelect.c_str());
	// Is this a valid tokenNumber?
	if (tokenNumber <= captureCount) {
	  // Yes - skip
	  outputPtr = digitFinder;
	  // Push the capturecount...
	  for (int p=ovector[2*tokenNumber];p<ovector[2*tokenNumber+1];p++)
	    outputString += subject[p];
	} else {
	  // No - just push the '$' to the output, and bump outputPtr
	  outputString += '$';
	  outputPtr++;
	}
      }
  }
  /* advance the input pointer */
  inputPointer = ovector[1];
  
  if (replacements.size() > 1)
    nextReplacement++;
  
  if (globalMatch && (ovector[1] < subject.size())) {
    for (;;)
      {
	int options = 0;                 /* Normally no options */
	int start_offset = ovector[1];   /* Start at end of previous match */
	
	/* If the previous match was for an empty string, we are finished if we are
	   at the end of the subject. Otherwise, arrange to run another match at the
	   same point to see if a non-empty match can be found. */
	
	if (ovector[0] == ovector[1])
	  {
	    if (ovector[0] == subject.size()) break;
	    options = PCRE_NOTEMPTY | PCRE_ANCHORED;
	  }
	
	/* Run the next matching operation */
	
	rc = pcre_exec(
		       re,                   /* the compiled pattern */
		       NULL,                 /* no extra data - we didn't study the pattern */
		       subject.c_str(),      /* the subject string */
		       subject.size(),       /* the length of the subject */
		       start_offset,         /* starting offset in the subject */
		       options,              /* options */
		       ovector,              /* output vector for substring information */
		       ovectorSize);           /* number of elements in the output vector */
	
	/* This time, a result of NOMATCH isn't an error. If the value in "options"
	   is zero, it just means we have found all possible matches, so the loop ends.
	   Otherwise, it means we have failed to find a non-empty-string match at a
	   point where there was a previous empty-string match. In this case, we do what
	   Perl does: advance the matching position by one, and continue. We do this by
	   setting the "end of previous match" offset, because that is picked up at the
	   top of the loop as the point at which to start again. */
	
	if (rc == PCRE_ERROR_NOMATCH)
	  {
	    if (options == 0) break;
	    ovector[1] = start_offset + 1;
	    continue;    /* Go round the loop again */
	  }
	
	/* Other matching errors are not recoverable. */
	
	if (rc < 0)
	  {
	    break;
	  }
	
	/* Match succeded */
	
	/* The match succeeded, but the output vector wasn't big enough. */
	
	if (rc == 0)
	  throw Exception("internal error - more outputs than expected?");
	
	/* output characters from the subject string until we reach the next match */
	while (inputPointer < ovector[0]) outputString += subject[inputPointer++];
	inputPointer = ovector[1];

	/* output the replacement string */
	replacementLength = replacements[nextReplacement].size();
	outputPtr = 0;
	tokenSelect = "";
	/* Now insert the replacement string */
	while (outputPtr < replacementLength) {
	  if (replacements[nextReplacement][outputPtr] != '$')
	    outputString += replacements[nextReplacement][outputPtr++];
	  else
	    if ((outputPtr < (replacementLength-1)) &&
		isdigit(replacements[nextReplacement][outputPtr+1])) {
	      // Try to collect a token name
	      digitFinder = outputPtr+1;
	      while ((digitFinder < replacementLength) && 
		     isdigit(replacements[nextReplacement][digitFinder])) {
		// Add this digit to the token name
		tokenSelect += replacements[nextReplacement][digitFinder];
		digitFinder++;
	      }
	      // try to map this to a token number
	      tokenNumber = atoi(tokenSelect.c_str());
	      // Is this a valid tokenNumber?
	      if (tokenNumber <= captureCount) {
		// Yes - skip
		outputPtr = digitFinder;
		// Push the capturecount...
		for (int p=ovector[2*tokenNumber];p<ovector[2*tokenNumber+1];p++)
		  outputString += subject[p];
	      } else {
		// No - just push the '$' to the output, and bump outputPtr
		outputString += '$';
		outputPtr++;
	      }
	    }
	}
	if (replacements.size() > 1) {
	  nextReplacement++;
	  if (nextReplacement >= replacements.size())
	    break; // No more replacments to make
	}
      }      /* End of loop to find second and subsequent matches */
  }

  while (inputPointer < subject.size()) 
    outputString += subject[inputPointer++];
  
  pcre_free(re);
  return outputString;
#else
  throw Exception("regexprep support not compiled in (requires PCRE library)");
#endif
}

ArrayVector RegExpRepDriverFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3) throw Exception("regexprep requires at least three arguments to function");
  for (int i=3;i<arg.size();i++) 
    if (!arg[i].isString())
      throw Exception("all arguments to regexprep must be strings");
  rvstring modes;
  for (int i=3;i<arg.size();i++)
    modes << ArrayToString(arg[i]);
  string subject = ArrayToString(arg[0]);
  string pattern = ArrayToString(arg[1]);
  rvstring replist;
  if (arg[2].isString())
    replist << ArrayToString(arg[2]);
  else if (IsCellStrings(arg[2])) {
    const Array *dp = (const Array *) arg[2].getDataPointer();
    for (int i=0;i<arg[2].getLength();i++)
      replist << ArrayToString(dp[i]);
  }
  return ArrayVector() << Array::stringConstructor(RegExpRepCoreFunction(subject,
									 pattern,
									 modes,
									 replist));
}				  
