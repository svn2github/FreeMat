/* A Bison parser, made by GNU Bison 1.875c.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT = 258,
     NUMERIC = 259,
     ENDQSTMNT = 260,
     ENDSTMNT = 261,
     LE = 262,
     GE = 263,
     EQ = 264,
     DOTTIMES = 265,
     DOTRDIV = 266,
     DOTLDIV = 267,
     DOTPOWER = 268,
     DOTTRANSPOSE = 269,
     STRING = 270,
     SPECIALCALL = 271,
     END = 272,
     IF = 273,
     FUNCTION = 274,
     FOR = 275,
     BREAK = 276,
     WHILE = 277,
     ELSE = 278,
     ELSEIF = 279,
     SWITCH = 280,
     CASE = 281,
     OTHERWISE = 282,
     CONTINUE = 283,
     TRY = 284,
     CATCH = 285,
     FIELD = 286,
     REFLPAREN = 287,
     REFRPAREN = 288,
     REFLBRACE = 289,
     REFRBRACE = 290,
     ENDFOR = 291,
     ENDSWITCH = 292,
     ENDWHILE = 293,
     ENDTRY = 294,
     ENDIF = 295,
     PERSISTENT = 296,
     KEYBOARD = 297,
     RETURN = 298,
     VARARGIN = 299,
     VARARGOUT = 300,
     GLOBAL = 301,
     QUIT = 302,
     RETALL = 303,
     NE = 304,
     NOT = 305,
     NEG = 306,
     POS = 307
   };
#endif
#define IDENT 258
#define NUMERIC 259
#define ENDQSTMNT 260
#define ENDSTMNT 261
#define LE 262
#define GE 263
#define EQ 264
#define DOTTIMES 265
#define DOTRDIV 266
#define DOTLDIV 267
#define DOTPOWER 268
#define DOTTRANSPOSE 269
#define STRING 270
#define SPECIALCALL 271
#define END 272
#define IF 273
#define FUNCTION 274
#define FOR 275
#define BREAK 276
#define WHILE 277
#define ELSE 278
#define ELSEIF 279
#define SWITCH 280
#define CASE 281
#define OTHERWISE 282
#define CONTINUE 283
#define TRY 284
#define CATCH 285
#define FIELD 286
#define REFLPAREN 287
#define REFRPAREN 288
#define REFLBRACE 289
#define REFRBRACE 290
#define ENDFOR 291
#define ENDSWITCH 292
#define ENDWHILE 293
#define ENDTRY 294
#define ENDIF 295
#define PERSISTENT 296
#define KEYBOARD 297
#define RETURN 298
#define VARARGIN 299
#define VARARGOUT 300
#define GLOBAL 301
#define QUIT 302
#define RETALL 303
#define NE 304
#define NOT 305
#define NEG 306
#define POS 307




/* Copy the first part of user declarations.  */
#line 1 "../../../libs/libFreeMat/Parser.yxx"

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

    /*************************************************************
     * $Source$
     * $Revision$
     * $Date$
     * $Author$
     *************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "AST.hpp"
#include "WalkTree.hpp"
#include "FunctionDef.hpp"
#include "ParserInterface.hpp"
#include "Exception.hpp"

#define YYSTYPE FreeMat::ASTPtr

#define MSGBUFLEN 2048
static char msgBuffer[MSGBUFLEN];

#include "LexerInterface.hpp"

extern char* yytext;
extern int yylex(void);

extern int lineNumber;
extern int yydebug;

#ifdef WIN32
#define snprintf _snprintf
#endif

namespace FreeMat {
  static ASTPtr mainAST;
  static MFunctionDef *mainMDef;
  
  bool errorOccured;
  bool interactiveMode;
  static char *expectString = NULL;
  static const char *filename;

  void yyexpect(char *s) {
	expectString = s;
  }

  void yyerror(char *t) {
    t = NULL;
  }

  int yyreport(char *xStr) {
    char *tokdesc;
    char *tokbuffer = "unprintable";
    char buffer[256];
    int addone = 1;
    if (*yytext < 33) {
	tokdesc = tokbuffer;
	sprintf(buffer,"Ran out of input on this line.");
	tokdesc = buffer;
	addone = 0;
    } else {
        sprintf(buffer,"Current token is '%s'",yytext);
	tokdesc = buffer;
    }	
    if (xStr)
      if (!interactiveMode)
        snprintf(msgBuffer,MSGBUFLEN,"Expecting %s at line %d of file %s.  %s",
	xStr,lineNumber+addone,filename,tokdesc);
      else
        snprintf(msgBuffer,MSGBUFLEN,"Expecting %s.  %s",xStr, tokdesc);
    else
      if (!interactiveMode)
        snprintf(msgBuffer,MSGBUFLEN,"Syntax error at line %d of file %s.  %s",
	lineNumber+addone,filename,tokdesc);
      else
        snprintf(msgBuffer,MSGBUFLEN,"Syntax error at input.  %s",tokdesc);
    throw Exception(msgBuffer);
    return 0;
  }

  void yyxpt(char *t) {
   yyreport(t);
  }

}

using namespace FreeMat;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 302 "../../../libs/libFreeMat/Parser.cxx"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  94
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2620

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  49
/* YYNRULES -- Number of rules. */
#define YYNRULES  207
/* YYNRULES -- Number of states. */
#define YYNSTATES  307

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    51,    64,
      70,    71,    57,    55,    68,    56,     2,    58,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    49,    69,
      52,    65,    53,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    66,    59,    67,    63,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    73,    50,    74,    72,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    54,    60,    61,    62
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     7,     8,    16,    23,    28,    32,
      39,    45,    48,    52,    57,    64,    68,    73,    79,    87,
      89,    92,    95,    98,   103,   105,   108,   111,   114,   118,
     123,   125,   129,   131,   134,   136,   139,   142,   145,   148,
     150,   152,   153,   155,   157,   159,   161,   163,   165,   167,
     169,   171,   173,   175,   177,   179,   181,   183,   185,   188,
     190,   193,   196,   199,   202,   205,   207,   210,   213,   215,
     217,   219,   221,   226,   229,   230,   237,   239,   241,   243,
     245,   246,   248,   250,   253,   258,   261,   262,   268,   274,
     278,   280,   286,   291,   295,   298,   302,   304,   310,   313,
     319,   322,   326,   327,   329,   331,   334,   337,   340,   343,
     344,   347,   351,   355,   364,   370,   379,   387,   394,   400,
     404,   408,   410,   414,   418,   422,   426,   430,   434,   438,
     442,   446,   450,   454,   458,   462,   466,   470,   474,   478,
     482,   486,   490,   494,   498,   502,   506,   510,   514,   518,
     522,   526,   530,   534,   538,   541,   544,   547,   550,   554,
     558,   562,   566,   569,   572,   576,   580,   583,   585,   587,
     589,   591,   595,   598,   603,   608,   614,   617,   621,   626,
     631,   637,   640,   643,   645,   648,   652,   656,   660,   664,
     666,   668,   670,   675,   680,   683,   686,   688,   692,   694,
     698,   700,   704,   706,   708,   710,   711,   713
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      76,     0,    -1,    82,    -1,    78,    -1,    -1,    19,    79,
       3,    32,    80,    33,    82,    -1,    19,     3,    32,    80,
      33,    82,    -1,    19,    79,     3,    82,    -1,    19,     3,
      82,    -1,    19,    79,     3,    32,    33,    82,    -1,    19,
       3,    32,    33,    82,    -1,    19,     1,    -1,    19,     3,
       1,    -1,    19,     3,    32,     1,    -1,    19,     3,    32,
      80,    33,     1,    -1,    19,    79,     1,    -1,    19,    79,
       3,     1,    -1,    19,    79,     3,    32,     1,    -1,    19,
      79,     3,    32,    80,    33,     1,    -1,    77,    -1,    78,
      77,    -1,    45,    65,    -1,     3,    65,    -1,    66,    80,
      67,    65,    -1,     1,    -1,    45,     1,    -1,     3,     1,
      -1,    66,     1,    -1,    66,    80,     1,    -1,    66,    80,
      67,     1,    -1,    81,    -1,    80,    68,    81,    -1,     3,
      -1,    51,     3,    -1,    83,    -1,    82,    83,    -1,    84,
       5,    -1,    84,     6,    -1,    84,    68,    -1,   111,    -1,
     113,    -1,    -1,   112,    -1,   102,    -1,    93,    -1,    92,
      -1,   104,    -1,   105,    -1,    96,    -1,    94,    -1,    91,
      -1,    90,    -1,    88,    -1,    87,    -1,    85,    -1,    47,
      -1,    48,    -1,     1,    -1,    16,    86,    -1,    15,    -1,
      86,    15,    -1,    41,    89,    -1,    41,     1,    -1,    46,
      89,    -1,    46,     1,    -1,     3,    -1,    89,     3,    -1,
      89,     1,    -1,    43,    -1,    42,    -1,    28,    -1,    21,
      -1,    29,    82,    95,    39,    -1,    30,    82,    -1,    -1,
      25,   113,    97,    98,   101,    37,    -1,    68,    -1,     6,
      -1,     5,    -1,    69,    -1,    -1,    99,    -1,   100,    -1,
      99,   100,    -1,    26,   113,    97,    82,    -1,    27,    82,
      -1,    -1,    20,   103,    97,    82,    36,    -1,    70,     3,
      65,   113,    71,    -1,     3,    65,   113,    -1,     3,    -1,
      70,     3,    65,   113,     1,    -1,    70,     3,    65,     1,
      -1,    70,     3,     1,    -1,    70,     1,    -1,     3,    65,
       1,    -1,     1,    -1,    22,   113,    97,    82,    38,    -1,
      22,     1,    -1,    18,   106,   107,   110,    40,    -1,    18,
       1,    -1,   113,    97,    82,    -1,    -1,   108,    -1,   109,
      -1,   108,   109,    -1,    24,   106,    -1,    24,     1,    -1,
      23,    82,    -1,    -1,    23,     1,    -1,   115,    65,   113,
      -1,   115,    65,     1,    -1,    66,   120,    67,    65,     3,
      32,   118,    33,    -1,    66,   120,    67,    65,     3,    -1,
      66,   120,    67,    65,     3,    32,   118,     1,    -1,    66,
     120,    67,    65,     3,    32,     1,    -1,    66,   120,    67,
      65,     3,     1,    -1,    66,   120,    67,    65,     1,    -1,
     113,    49,   113,    -1,   113,    49,     1,    -1,   114,    -1,
     113,    55,   113,    -1,   113,    55,     1,    -1,   113,    56,
     113,    -1,   113,    56,     1,    -1,   113,    57,   113,    -1,
     113,    57,     1,    -1,   113,    58,   113,    -1,   113,    58,
       1,    -1,   113,    59,   113,    -1,   113,    59,     1,    -1,
     113,    50,   113,    -1,   113,    50,     1,    -1,   113,    51,
     113,    -1,   113,    51,     1,    -1,   113,    52,   113,    -1,
     113,    52,     1,    -1,   113,     7,   113,    -1,   113,     7,
       1,    -1,   113,    53,   113,    -1,   113,    53,     1,    -1,
     113,     8,   113,    -1,   113,     8,     1,    -1,   113,     9,
     113,    -1,   113,     9,     1,    -1,   113,    54,   113,    -1,
     113,    54,     1,    -1,   113,    10,   113,    -1,   113,    10,
       1,    -1,   113,    11,   113,    -1,   113,    11,     1,    -1,
     113,    12,   113,    -1,   113,    12,     1,    -1,    56,   113,
      -1,    55,   113,    -1,    72,   113,    -1,    72,     1,    -1,
     113,    63,   113,    -1,   113,    63,     1,    -1,   113,    13,
     113,    -1,   113,    13,     1,    -1,   113,    64,    -1,   113,
      14,    -1,    70,   113,    71,    -1,    70,   113,     1,    -1,
      70,     1,    -1,     4,    -1,    15,    -1,    17,    -1,   115,
      -1,    66,   120,    67,    -1,    66,     1,    -1,    66,   121,
     120,    67,    -1,    66,   120,   121,    67,    -1,    66,   121,
     120,   121,    67,    -1,    66,    67,    -1,    73,   119,    74,
      -1,    73,   121,   119,    74,    -1,    73,   119,   121,    74,
      -1,    73,   121,   119,   121,    74,    -1,    73,    74,    -1,
      73,     1,    -1,     3,    -1,   115,   116,    -1,    32,   118,
      33,    -1,    32,   118,     1,    -1,    34,   118,    35,    -1,
      34,   118,     1,    -1,    31,    -1,   113,    -1,    49,    -1,
      58,     3,    65,   113,    -1,    58,     3,    65,     1,    -1,
      58,     3,    -1,    58,     1,    -1,   117,    -1,   118,    68,
     117,    -1,   123,    -1,   119,   121,   123,    -1,   123,    -1,
     120,   121,   123,    -1,     6,    -1,     5,    -1,    68,    -1,
      -1,   113,    -1,   123,   122,   113,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   140,   140,   141,   141,   145,   161,   176,   191,   205,
     220,   234,   235,   236,   237,   238,   239,   240,   241,   245,
     246,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     262,   263,   267,   267,   278,   279,   283,   292,   301,   313,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   329,   330,   334,   338,
     339,   343,   344,   348,   349,   353,   354,   355,   359,   363,
     367,   370,   374,   379,   380,   384,   392,   392,   392,   392,
     396,   397,   401,   404,   410,   416,   419,   425,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   443,   446,   450,
     454,   458,   464,   465,   469,   472,   478,   481,   484,   487,
     488,   492,   493,   497,   501,   505,   507,   509,   511,   516,
     517,   518,   519,   520,   521,   522,   523,   524,   525,   526,
     527,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   538,   539,   540,   541,   542,   543,   544,   545,   546,
     547,   548,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,   559,   560,   561,   562,   563,   567,   568,   569,
     570,   571,   572,   573,   574,   575,   576,   577,   578,   579,
     580,   581,   582,   586,   587,   591,   592,   593,   594,   595,
     599,   600,   601,   602,   603,   604,   608,   609,   613,   614,
     618,   619,   623,   623,   627,   627,   631,   632
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENT", "NUMERIC", "ENDQSTMNT",
  "ENDSTMNT", "LE", "GE", "EQ", "DOTTIMES", "DOTRDIV", "DOTLDIV",
  "DOTPOWER", "DOTTRANSPOSE", "STRING", "SPECIALCALL", "END", "IF",
  "FUNCTION", "FOR", "BREAK", "WHILE", "ELSE", "ELSEIF", "SWITCH", "CASE",
  "OTHERWISE", "CONTINUE", "TRY", "CATCH", "FIELD", "REFLPAREN",
  "REFRPAREN", "REFLBRACE", "REFRBRACE", "ENDFOR", "ENDSWITCH", "ENDWHILE",
  "ENDTRY", "ENDIF", "PERSISTENT", "KEYBOARD", "RETURN", "VARARGIN",
  "VARARGOUT", "GLOBAL", "QUIT", "RETALL", "':'", "'|'", "'&'", "'<'",
  "'>'", "NE", "'+'", "'-'", "'*'", "'/'", "'\\\\'", "NOT", "NEG", "POS",
  "'^'", "'''", "'='", "'['", "']'", "','", "';'", "'('", "')'", "'~'",
  "'{'", "'}'", "$accept", "program", "functionDef", "functionDefList",
  "returnDeclaration", "argumentList", "argument", "statementList",
  "statement", "statementType", "specialSyntaxStatement", "stringList",
  "persistentStatement", "globalStatement", "identList", "returnStatement",
  "keyboardStatement", "continueStatement", "breakStatement",
  "tryStatement", "optionalCatch", "switchStatement",
  "optionalEndStatement", "caseBlock", "caseList", "caseStatement",
  "otherwiseClause", "forStatement", "forIndexExpression",
  "whileStatement", "ifStatement", "conditionedStatement", "elseIfBlock",
  "elseIfStatementList", "elseIfStatement", "elseStatement",
  "assignmentStatement", "multiFunctionCall", "expr", "terminal",
  "symbRefList", "symbRef", "indexElement", "indexList", "cellDef",
  "matrixDef", "rowSeperator", "columnSep", "rowDef", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,    58,
     124,    38,    60,    62,   304,    43,    45,    42,    47,    92,
     305,   306,   307,    94,    39,    61,    91,    93,    44,    59,
      40,    41,   126,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    75,    76,    76,    76,    77,    77,    77,    77,    77,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    78,
      78,    79,    79,    79,    79,    79,    79,    79,    79,    79,
      80,    80,    81,    81,    82,    82,    83,    83,    83,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    85,    86,
      86,    87,    87,    88,    88,    89,    89,    89,    90,    91,
      92,    93,    94,    95,    95,    96,    97,    97,    97,    97,
      98,    98,    99,    99,   100,   101,   101,   102,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   104,   104,   105,
     105,   106,   107,   107,   108,   108,   109,   109,   110,   110,
     110,   111,   111,   112,   112,   112,   112,   112,   112,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   115,   115,   116,   116,   116,   116,   116,
     117,   117,   117,   117,   117,   117,   118,   118,   119,   119,
     120,   120,   121,   121,   122,   122,   123,   123
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     0,     7,     6,     4,     3,     6,
       5,     2,     3,     4,     6,     3,     4,     5,     7,     1,
       2,     2,     2,     4,     1,     2,     2,     2,     3,     4,
       1,     3,     1,     2,     1,     2,     2,     2,     2,     1,
       1,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     2,     2,     2,     2,     1,     2,     2,     1,     1,
       1,     1,     4,     2,     0,     6,     1,     1,     1,     1,
       0,     1,     1,     2,     4,     2,     0,     5,     5,     3,
       1,     5,     4,     3,     2,     3,     1,     5,     2,     5,
       2,     3,     0,     1,     1,     2,     2,     2,     2,     0,
       2,     3,     3,     8,     5,     8,     7,     6,     5,     3,
       3,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     2,     2,     2,     3,     3,
       3,     3,     2,     2,     3,     3,     2,     1,     1,     1,
       1,     3,     2,     4,     4,     5,     2,     3,     4,     4,
       5,     2,     2,     1,     2,     3,     3,     3,     3,     1,
       1,     1,     4,     4,     2,     2,     1,     3,     1,     3,
       1,     3,     1,     1,     1,     0,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    57,   183,   167,   168,     0,   169,     0,     0,     0,
      71,     0,     0,    70,     0,     0,    69,    68,     0,    55,
      56,     0,     0,     0,     0,     0,     0,     0,    19,     3,
       0,    34,     0,    54,    53,    52,    51,    50,    45,    44,
      49,    48,    43,    46,    47,    39,    42,    40,   121,   170,
      59,    58,   100,     0,   102,     0,   170,    11,     0,     0,
       0,     0,    96,    90,     0,     0,    98,     0,     0,     0,
      62,    65,     0,    64,     0,   155,   154,   172,   203,   202,
     176,   206,     0,     0,   205,   166,     0,   157,   156,   182,
     181,     0,     0,   205,     1,    20,    35,    36,    37,    38,
       0,     0,     0,     0,     0,     0,     0,   163,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     162,   189,     0,     0,     0,   184,    60,     0,     0,   109,
     103,   104,    78,    77,    76,    79,     0,    57,     0,    22,
       0,    25,    21,    27,    32,     0,     0,    30,    15,     0,
       0,    94,     0,     0,     0,    80,     0,     0,    67,    66,
     171,     0,     0,   204,     0,   165,   164,   177,     0,     0,
     139,   138,   143,   142,   145,   144,   149,   148,   151,   150,
     153,   152,   161,   160,   120,   119,   133,   132,   135,   134,
     137,   136,   141,   140,   147,   146,   123,   122,   125,   124,
     127,   126,   129,   128,   131,   130,   159,   158,   191,     0,
     190,   196,     0,     0,   112,   111,   171,   107,   106,     0,
       0,   105,     0,    13,     0,     0,    33,    28,     0,     0,
      57,     0,     0,    95,    89,    93,     0,     0,     0,     0,
      86,    81,    82,     0,    72,     0,   174,   205,   173,     0,
     207,   179,   205,   178,     0,   195,   194,   186,   185,     0,
     188,   187,    57,     0,    99,     0,     0,    29,    23,    31,
      17,     0,     0,    92,     0,    87,    97,     0,     0,     0,
      83,   118,     0,   175,   180,     0,   197,    57,     0,     0,
       0,    91,    88,     0,     0,    75,   117,     0,   193,   192,
      57,     0,     0,   116,     0,   115,   113
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    27,    28,    29,    61,   146,   147,    30,    31,    32,
      33,    51,    34,    35,    72,    36,    37,    38,    39,    40,
     157,    41,   136,   240,   241,   242,   279,    42,    65,    43,
      44,    54,   129,   130,   131,   220,    45,    46,    47,    48,
      56,   125,   211,   212,    91,    82,    83,   164,    84
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -133
static const short yypact[] =
{
     340,  -133,  -133,  -133,  -133,    22,  -133,   142,    97,    15,
    -133,  1701,  2401,  -133,  1465,    50,  -133,  -133,    64,  -133,
    -133,  2401,  2401,  1676,  1720,  1749,   787,    45,  -133,    30,
     753,  -133,   171,  -133,  -133,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,  -133,  -133,  -133,  -133,  -133,  2468,  -133,   175,
    -133,    39,  -133,  1676,    31,  2435,   210,    88,   979,     9,
     166,   202,  -133,    -5,   220,   167,  -133,  2435,  2435,  1052,
    -133,  -133,    41,  -133,   102,   213,   213,  -133,  -133,  -133,
    -133,  2468,   188,  2401,   180,  -133,   321,  -133,   213,  -133,
    -133,    14,  2401,     3,  -133,  -133,  -133,  -133,  -133,  -133,
    1779,  1784,  1808,  1813,  1843,  1872,  1891,  -133,  1916,  1936,
    1964,  1995,  2000,  2023,  2028,  2059,  2087,  2106,  2132,  2137,
    -133,  -133,  2316,  2316,  2165,  -133,  -133,   190,  2196,    53,
      31,  -133,  -133,  -133,  -133,  -133,  1465,   201,   272,  -133,
     399,  -133,  -133,  -133,  -133,    92,    63,  -133,  -133,  1111,
    2215,  -133,    82,  1465,  1465,    40,  1465,   113,  -133,  -133,
      90,  2358,   194,  -133,  2401,  -133,  -133,  -133,  2269,    18,
    -133,   439,  -133,   439,  -133,   439,  -133,   213,  -133,   213,
    -133,   213,  -133,   213,  -133,  2498,  -133,  2526,  -133,  2556,
    -133,   439,  -133,   439,  -133,   439,  -133,   498,  -133,   498,
    -133,   213,  -133,   213,  -133,   213,  -133,   213,  -133,   228,
    2468,  -133,    71,   100,  -133,  2468,  -133,  -133,  -133,  1524,
     118,  -133,   861,  -133,  1465,   -18,  -133,  -133,    85,     8,
      29,   406,   458,  -133,  2468,  -133,  2224,  1170,  1229,  2401,
     137,    40,  -133,  1288,  -133,   253,  -133,   184,  -133,  2366,
    2468,  -133,     7,  -133,  2274,  -133,   119,  -133,  -133,  2316,
    -133,  -133,   147,  1347,  -133,   517,  1583,  -133,  -133,  -133,
    -133,  1465,    19,  -133,  2344,  -133,  -133,  2435,  1465,   154,
    -133,  -133,   160,  -133,  -133,  2243,  -133,    38,   576,   635,
    1642,  -133,  -133,  1465,  1406,  -133,  -133,    24,  -133,  2468,
      43,   694,   920,  -133,   150,  -133,  -133
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -133,  -133,   181,  -133,  -133,  -132,   -13,    26,    48,  -133,
    -133,  -133,  -133,  -133,   193,  -133,  -133,  -133,  -133,  -133,
    -133,  -133,   -64,  -133,  -133,    -8,  -133,  -133,  -133,  -133,
    -133,   106,  -133,  -133,   123,  -133,  -133,  -133,    10,  -133,
       0,  -133,    -1,  -116,   168,   -27,   -21,  -133,   -24
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -202
static const short yytable[] =
{
      49,   153,    93,   154,   155,    92,   225,   213,  -198,  -198,
     141,   144,  -199,  -199,    49,   266,    62,    55,    63,    78,
      79,    67,    68,    78,    79,   303,   127,     2,     3,   -16,
      49,    75,    76,    81,    86,    88,    81,    50,   -14,     4,
      69,     6,   158,   -18,   159,    94,   -61,   -61,   -16,     8,
     229,    70,   290,    71,   126,   128,   162,   -14,    49,   145,
     150,   161,   -18,    81,   227,    73,   239,    71,    93,    49,
     168,   163,   257,   208,   142,   163,   219,  -198,    96,    21,
      22,  -199,   209,   235,   140,    64,   267,   229,   167,   -24,
      53,   -24,   253,    81,    24,   226,    25,    26,    57,   272,
      58,   260,    81,   158,   258,   159,   161,   -63,   -63,   -61,
     171,   173,   175,   177,   179,   181,   183,    96,   185,   187,
     189,   191,   193,   195,   197,   199,   201,   203,   205,   207,
     228,   229,   210,   210,   215,   261,    49,   247,    55,   259,
      49,   249,    59,    52,   252,     2,     3,   236,   254,    49,
     268,   305,   244,    49,    49,   245,    49,     4,   264,     6,
     234,   296,   222,    60,   278,  -114,  -114,   143,   259,   144,
     -63,    81,   132,   133,   250,   232,    97,    98,    81,   237,
     238,   304,   243,   306,   285,  -200,  -200,  -110,    96,  -201,
    -201,   295,   297,    78,    79,    78,    79,    21,    22,    78,
      79,   -12,   -26,   148,   -26,   149,   121,   122,    53,   123,
      95,    74,    24,   293,    25,    26,   269,   145,   259,    49,
     -12,   151,    49,   152,    49,   247,   106,   107,  -114,   255,
     252,   256,    49,   280,   218,   134,   135,    49,    49,    99,
     124,   121,   122,    49,   123,   263,   274,  -200,   163,   277,
     265,  -201,   163,   221,   281,   160,   282,   216,   286,    81,
     169,   248,     0,    49,    81,    49,    49,     0,     0,   210,
      96,    49,     0,   223,     0,   144,   119,   120,    49,     0,
      96,     0,     0,     0,     0,    96,    96,     0,    49,    49,
      49,    96,   288,    49,    49,   299,     0,   289,     0,     0,
       0,    49,    49,     0,   294,   224,     0,   210,     0,     0,
       0,    96,     0,    96,     0,     0,   301,     0,     0,   302,
       0,     0,   165,   145,     0,     0,     0,     0,   100,   101,
     102,   103,   104,   105,   106,   107,    96,    96,     0,     0,
      -4,     1,    96,     2,     3,   -41,   -41,     0,     0,    96,
      96,     0,     0,     0,     0,     4,     5,     6,     7,     8,
       9,    10,    11,     0,     0,    12,     0,     0,    13,    14,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,    15,    16,    17,   119,   120,    18,    19,    20,     0,
       0,     0,   166,     0,     0,    21,    22,     0,     0,    -8,
       1,     0,     2,     3,   -41,   -41,    23,   270,   -41,   144,
      24,     0,    25,    26,     4,     5,     6,     7,    -8,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   271,
      15,    16,    17,     0,     0,    18,    19,    20,     0,   103,
     104,   105,   106,   107,    21,    22,     0,   145,    -7,     1,
       0,     2,     3,   -41,   -41,    23,     0,   -41,     0,    24,
       0,    25,    26,     4,     5,     6,     7,    -7,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,     0,     0,
       0,     0,     0,     0,   114,   115,   116,   117,   118,    15,
      16,    17,   119,   120,    18,    19,    20,     0,   103,   104,
     105,   106,   107,    21,    22,     0,     0,   -10,     1,     0,
       2,     3,   -41,   -41,    23,     0,   -41,     0,    24,     0,
      25,    26,     4,     5,     6,     7,   -10,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,     0,     0,
       0,     0,     0,     0,     0,   116,   117,   118,    15,    16,
      17,   119,   120,    18,    19,    20,     0,     0,     0,     0,
       0,     0,    21,    22,     0,     0,    -6,     1,     0,     2,
       3,   -41,   -41,    23,     0,   -41,     0,    24,     0,    25,
      26,     4,     5,     6,     7,    -6,     9,    10,    11,     0,
       0,    12,     0,     0,    13,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,    -9,     1,     0,     2,     3,
     -41,   -41,    23,     0,   -41,     0,    24,     0,    25,    26,
       4,     5,     6,     7,    -9,     9,    10,    11,     0,     0,
      12,     0,     0,    13,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,    16,    17,     0,
       0,    18,    19,    20,     0,     0,     0,     0,     0,     0,
      21,    22,     0,     0,    -5,     1,     0,     2,     3,   -41,
     -41,    23,     0,   -41,     0,    24,     0,    25,    26,     4,
       5,     6,     7,    -5,     9,    10,    11,     0,     0,    12,
       0,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,    -2,     1,     0,     2,     3,   -41,   -41,
      23,     0,   -41,     0,    24,     0,    25,    26,     4,     5,
       6,     7,     0,     9,    10,    11,     0,     0,    12,     0,
       0,    13,    14,     0,     0,     0,     0,     0,    89,     0,
       2,     3,    78,    79,    15,    16,    17,     0,     0,    18,
      19,    20,     4,     0,     6,     0,     0,     0,    21,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,   -41,     0,    24,     0,    25,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    21,    22,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    53,     0,     0,     0,    24,     0,    25,
      26,    90,     1,     0,     2,     3,   -41,   -41,     0,     0,
       0,     0,     0,     0,     0,     0,     4,     5,     6,     7,
       0,     9,    10,    11,  -101,  -101,    12,     0,     0,    13,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -101,    15,    16,    17,     0,     0,    18,    19,    20,
       0,     0,     0,     0,     0,     0,    21,    22,     0,     0,
       0,     1,     0,     2,     3,   -41,   -41,    23,     0,   -41,
       0,    24,     0,    25,    26,     4,     5,     6,     7,     0,
       9,    10,    11,     0,     0,    12,   -84,   -84,    13,    14,
       0,     0,     0,     0,     0,     0,     0,   -84,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
     137,     0,     2,     3,   -41,   -41,    23,     0,   -41,     0,
      24,     0,    25,    26,     4,     5,     6,     7,     0,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,   138,     0,     0,     0,     0,     0,     0,     0,     0,
      15,    16,    17,     0,     0,    18,    19,    20,     0,     0,
       0,     0,     0,     0,    21,    22,     0,     0,     0,     0,
       0,     0,     0,     0,   139,    23,     0,   -41,     0,    24,
       0,    25,    26,     1,     0,     2,     3,   -41,   -41,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,     6,
       7,     0,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,   156,     0,     0,     0,     0,     0,     0,     0,
       0,   -74,     0,    15,    16,    17,     0,     0,    18,    19,
      20,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,   230,     0,     2,     3,   -41,   -41,    23,     0,
     -41,     0,    24,     0,    25,    26,     4,     5,     6,     7,
       0,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     0,   231,     0,     0,     0,     0,     0,     0,
       0,     0,    15,    16,    17,     0,     0,    18,    19,    20,
       0,     0,     0,     0,     0,     0,    21,    22,     0,     0,
       0,     1,     0,     2,     3,   -41,   -41,    23,     0,   -41,
       0,    24,     0,    25,    26,     4,     5,     6,     7,     0,
       9,    10,    11,     0,     0,    12,     0,     0,    13,    14,
       0,     0,     0,     0,     0,     0,   275,     0,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
       1,     0,     2,     3,   -41,   -41,    23,     0,   -41,     0,
      24,     0,    25,    26,     4,     5,     6,     7,     0,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,     0,     0,     0,     0,     0,     0,   276,     0,     0,
      15,    16,    17,     0,     0,    18,    19,    20,     0,     0,
       0,     0,     0,     0,    21,    22,     0,     0,     0,     1,
       0,     2,     3,   -41,   -41,    23,     0,   -41,     0,    24,
       0,    25,    26,     4,     5,     6,     7,     0,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   -73,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     1,     0,
       2,     3,   -41,   -41,    23,     0,   -41,     0,    24,     0,
      25,    26,     4,     5,     6,     7,     0,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -108,    15,    16,
      17,     0,     0,    18,    19,    20,     0,     0,     0,     0,
       0,     0,    21,    22,     0,     0,     0,     1,     0,     2,
       3,   -41,   -41,    23,     0,   -41,     0,    24,     0,    25,
      26,     4,     5,     6,     7,     0,     9,    10,    11,     0,
       0,    12,     0,     0,    13,    14,     0,     0,     0,     0,
       0,     0,     0,   -85,     0,     0,     0,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,     1,     0,     2,     3,
     -41,   -41,    23,     0,   -41,     0,    24,     0,    25,    26,
       4,     5,     6,     7,     0,     9,    10,    11,     0,     0,
      12,     0,     0,    13,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,    16,    17,     0,
       0,    18,    19,    20,     0,     0,     0,     0,     0,     0,
      21,    22,     0,     0,     0,   262,     0,     2,     3,   -41,
     -41,    23,     0,   -41,     0,    24,     0,    25,    26,     4,
       5,     6,     7,     0,     9,    10,    11,     0,     0,    12,
       0,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,   287,     0,     2,     3,   -41,   -41,
      23,     0,   -41,     0,    24,     0,    25,    26,     4,     5,
       6,     7,     0,     9,    10,    11,     0,     0,    12,     0,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,    16,    17,     0,     0,    18,
      19,    20,     0,     0,     0,     0,     0,     0,    21,    22,
       0,     0,     0,   300,     0,     2,     3,   -41,   -41,    23,
       0,   -41,     0,    24,     0,    25,    26,     4,     5,     6,
       7,     0,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,     0,     0,     0,     0,     0,    77,     0,     2,
       3,    78,    79,    15,    16,    17,     0,     0,    18,    19,
      20,     4,     0,     6,     0,     0,     0,    21,    22,     0,
       0,     0,    66,     0,     2,     3,     0,     0,    23,     0,
     -41,     0,    24,     0,    25,    26,     4,     0,     6,     0,
       0,    85,     0,     2,     3,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     4,     0,     6,     0,     0,
       0,     0,    53,    80,     0,     0,    24,     0,    25,    26,
      87,     0,     2,     3,     0,     0,    21,    22,     0,     0,
       0,     0,     0,     0,     4,     0,     6,    53,     0,     0,
       0,    24,     0,    25,    26,    21,    22,     0,     0,     0,
     170,     0,     2,     3,     0,   172,    53,     2,     3,     0,
      24,     0,    25,    26,     4,     0,     6,     0,     0,     4,
       0,     6,     0,     0,    21,    22,     0,     0,     0,   174,
       0,     2,     3,     0,   176,    53,     2,     3,     0,    24,
       0,    25,    26,     4,     0,     6,     0,     0,     4,     0,
       6,     0,     0,     0,    21,    22,     0,     0,     0,    21,
      22,     0,     0,     0,   178,    53,     2,     3,     0,    24,
      53,    25,    26,     0,    24,     0,    25,    26,     4,     0,
       6,     0,     0,    21,    22,     0,     0,     0,    21,    22,
       0,     0,     0,   180,    53,     2,     3,     0,    24,    53,
      25,    26,     0,    24,     0,    25,    26,     4,     0,     6,
       0,     0,   182,     0,     2,     3,     0,     0,    21,    22,
       0,     0,     0,     0,     0,     0,     4,     0,     6,    53,
       0,     0,     0,    24,     0,    25,    26,   184,     0,     2,
       3,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     4,     0,     6,     0,     0,     0,   186,    53,     2,
       3,     0,    24,     0,    25,    26,    21,    22,     0,     0,
       0,     4,     0,     6,     0,     0,     0,    53,     0,     0,
       0,    24,     0,    25,    26,   188,     0,     2,     3,     0,
       0,    21,    22,     0,     0,     0,     0,     0,     0,     4,
       0,     6,    53,     0,     0,     0,    24,     0,    25,    26,
       0,    21,    22,     0,     0,     0,   190,     0,     2,     3,
       0,   192,    53,     2,     3,     0,    24,     0,    25,    26,
       4,     0,     6,     0,     0,     4,     0,     6,     0,    21,
      22,     0,     0,     0,   194,     0,     2,     3,     0,   196,
      53,     2,     3,     0,    24,     0,    25,    26,     4,     0,
       6,     0,     0,     4,     0,     6,     0,     0,     0,     0,
      21,    22,     0,     0,     0,    21,    22,     0,     0,     0,
     198,    53,     2,     3,     0,    24,    53,    25,    26,     0,
      24,     0,    25,    26,     4,     0,     6,     0,    21,    22,
       0,     0,     0,    21,    22,     0,     0,     0,   200,    53,
       2,     3,     0,    24,    53,    25,    26,     0,    24,     0,
      25,    26,     4,     0,     6,     0,     0,   202,     0,     2,
       3,     0,     0,     0,    21,    22,     0,     0,     0,     0,
       0,     4,     0,     6,     0,    53,     0,     0,     0,    24,
       0,    25,    26,   204,     0,     2,     3,     0,   206,     0,
       2,     3,    21,    22,     0,     0,     0,     4,     0,     6,
       0,     0,     4,    53,     6,     0,     0,    24,     0,    25,
      26,    21,    22,     0,     0,     0,   214,     0,     2,     3,
       0,     0,    53,     0,     0,     0,    24,     0,    25,    26,
       4,     0,     6,     0,     0,     0,     0,    21,    22,     0,
       0,     0,    21,    22,     0,     0,     0,   217,    53,     2,
       3,     0,    24,    53,    25,    26,     0,    24,     0,    25,
      26,     4,     0,     6,     0,     0,   233,     0,     2,     3,
      21,    22,     0,     0,     0,   273,     0,     2,     3,     0,
       4,    53,     6,     0,     0,    24,     0,    25,    26,     4,
       0,     6,     0,     0,   298,     0,     2,     3,     0,     0,
       0,    21,    22,     0,     0,     0,     0,     0,     4,     0,
       6,     0,    53,     0,     0,     0,    24,     0,    25,    26,
      21,    22,     2,     3,     0,     0,     0,     2,     3,    21,
      22,    53,     0,     0,     4,    24,     6,    25,    26,     4,
      53,     6,     0,     0,    24,     0,    25,    26,    21,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    53,
       0,     0,     0,    24,     0,    25,    26,     0,     0,     2,
       3,     0,     0,     0,    21,    22,     0,     0,     0,    21,
      22,     4,     0,     6,     0,    53,     0,     0,     0,    24,
      53,    25,    26,   251,    24,   291,    25,    26,   284,     0,
       0,   100,   101,   102,   103,   104,   105,   106,   107,     0,
       0,     2,     3,     0,     0,   208,     0,     0,     0,     2,
       3,    21,    22,     4,   209,     6,     0,     0,     0,     0,
       0,     4,    53,     6,     0,     0,    24,     0,    25,    26,
       0,     0,     0,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,     2,     3,     0,   119,   120,     0,
       0,     0,     0,    21,    22,   292,     4,     0,     6,     0,
       0,    21,    22,     0,    53,   246,     0,     0,    24,     0,
      25,    26,    53,   283,     0,     0,    24,     0,    25,    26,
     132,   133,   100,   101,   102,   103,   104,   105,   106,   107,
       0,     0,     0,     0,     0,     0,    21,    22,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    53,     0,     0,
       0,    24,     0,    25,    26,   100,   101,   102,   103,   104,
     105,   106,   107,     0,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,     0,     0,     0,   119,   120,
       0,     0,     0,   134,   135,   100,   101,   102,   103,   104,
     105,   106,   107,     0,     0,     0,     0,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,     0,     0,
       0,   119,   120,   100,   101,   102,   103,   104,   105,   106,
     107,     0,     0,     0,     0,     0,     0,     0,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,     0,     0,
       0,   119,   120,   100,   101,   102,   103,   104,   105,   106,
     107,     0,     0,     0,     0,     0,     0,   110,   111,   112,
     113,   114,   115,   116,   117,   118,     0,     0,     0,   119,
     120,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   111,   112,
     113,   114,   115,   116,   117,   118,     0,     0,     0,   119,
     120
};

static const short yycheck[] =
{
       0,    65,    26,    67,    68,    26,   138,   123,     5,     6,
       1,     3,     5,     6,    14,    33,     1,     7,     3,     5,
       6,    11,    12,     5,     6,     1,    53,     3,     4,     0,
      30,    21,    22,    23,    24,    25,    26,    15,     0,    15,
      14,    17,     1,     0,     3,     0,     5,     6,    19,    19,
      68,     1,    33,     3,    15,    24,    83,    19,    58,    51,
      65,    82,    19,    53,     1,     1,    26,     3,    92,    69,
      91,    68,     1,    49,    65,    68,    23,    74,    30,    55,
      56,    74,    58,     1,    58,    70,     1,    68,    74,     1,
      66,     3,    74,    83,    70,     3,    72,    73,     1,   231,
       3,     1,    92,     1,    33,     3,   127,     5,     6,    68,
     100,   101,   102,   103,   104,   105,   106,    69,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
      67,    68,   122,   123,   124,    35,   136,   161,   128,    68,
     140,   162,    45,     1,   168,     3,     4,    65,   169,   149,
      65,     1,    39,   153,   154,    65,   156,    15,    40,    17,
     150,     1,   136,    66,    27,     5,     6,     1,    68,     3,
      68,   161,     5,     6,   164,   149,     5,     6,   168,   153,
     154,   297,   156,    33,    65,     5,     6,    40,   140,     5,
       6,    37,    32,     5,     6,     5,     6,    55,    56,     5,
       6,     0,     1,     1,     3,     3,    31,    32,    66,    34,
      29,    18,    70,   277,    72,    73,   229,    51,    68,   219,
      19,     1,   222,     3,   224,   249,    13,    14,    68,     1,
     254,     3,   232,   241,   128,    68,    69,   237,   238,    68,
      65,    31,    32,   243,    34,   219,   236,    67,    68,   239,
     224,    67,    68,   130,     1,    67,     3,    67,   259,   249,
      92,    67,    -1,   263,   254,   265,   266,    -1,    -1,   259,
     222,   271,    -1,     1,    -1,     3,    63,    64,   278,    -1,
     232,    -1,    -1,    -1,    -1,   237,   238,    -1,   288,   289,
     290,   243,   266,   293,   294,   285,    -1,   271,    -1,    -1,
      -1,   301,   302,    -1,   278,    33,    -1,   297,    -1,    -1,
      -1,   263,    -1,   265,    -1,    -1,   290,    -1,    -1,   293,
      -1,    -1,     1,    51,    -1,    -1,    -1,    -1,     7,     8,
       9,    10,    11,    12,    13,    14,   288,   289,    -1,    -1,
       0,     1,   294,     3,     4,     5,     6,    -1,    -1,   301,
     302,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    19,
      20,    21,    22,    -1,    -1,    25,    -1,    -1,    28,    29,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    41,    42,    43,    63,    64,    46,    47,    48,    -1,
      -1,    -1,    71,    -1,    -1,    55,    56,    -1,    -1,     0,
       1,    -1,     3,     4,     5,     6,    66,     1,    68,     3,
      70,    -1,    72,    73,    15,    16,    17,    18,    19,    20,
      21,    22,    -1,    -1,    25,    -1,    -1,    28,    29,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      41,    42,    43,    -1,    -1,    46,    47,    48,    -1,    10,
      11,    12,    13,    14,    55,    56,    -1,    51,     0,     1,
      -1,     3,     4,     5,     6,    66,    -1,    68,    -1,    70,
      -1,    72,    73,    15,    16,    17,    18,    19,    20,    21,
      22,    -1,    -1,    25,    -1,    -1,    28,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    56,    57,    58,    59,    41,
      42,    43,    63,    64,    46,    47,    48,    -1,    10,    11,
      12,    13,    14,    55,    56,    -1,    -1,     0,     1,    -1,
       3,     4,     5,     6,    66,    -1,    68,    -1,    70,    -1,
      72,    73,    15,    16,    17,    18,    19,    20,    21,    22,
      -1,    -1,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    57,    58,    59,    41,    42,
      43,    63,    64,    46,    47,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    -1,    -1,     0,     1,    -1,     3,
       4,     5,     6,    66,    -1,    68,    -1,    70,    -1,    72,
      73,    15,    16,    17,    18,    19,    20,    21,    22,    -1,
      -1,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,
      -1,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    56,    -1,    -1,     0,     1,    -1,     3,     4,
       5,     6,    66,    -1,    68,    -1,    70,    -1,    72,    73,
      15,    16,    17,    18,    19,    20,    21,    22,    -1,    -1,
      25,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    -1,
      -1,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    56,    -1,    -1,     0,     1,    -1,     3,     4,     5,
       6,    66,    -1,    68,    -1,    70,    -1,    72,    73,    15,
      16,    17,    18,    19,    20,    21,    22,    -1,    -1,    25,
      -1,    -1,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,
      46,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56,    -1,    -1,     0,     1,    -1,     3,     4,     5,     6,
      66,    -1,    68,    -1,    70,    -1,    72,    73,    15,    16,
      17,    18,    -1,    20,    21,    22,    -1,    -1,    25,    -1,
      -1,    28,    29,    -1,    -1,    -1,    -1,    -1,     1,    -1,
       3,     4,     5,     6,    41,    42,    43,    -1,    -1,    46,
      47,    48,    15,    -1,    17,    -1,    -1,    -1,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      -1,    68,    -1,    70,    -1,    72,    73,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    66,    -1,    -1,    -1,    70,    -1,    72,
      73,    74,     1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,
      -1,    20,    21,    22,    23,    24,    25,    -1,    -1,    28,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    40,    41,    42,    43,    -1,    -1,    46,    47,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,
      -1,     1,    -1,     3,     4,     5,     6,    66,    -1,    68,
      -1,    70,    -1,    72,    73,    15,    16,    17,    18,    -1,
      20,    21,    22,    -1,    -1,    25,    26,    27,    28,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    -1,    -1,
      -1,    41,    42,    43,    -1,    -1,    46,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,
       1,    -1,     3,     4,     5,     6,    66,    -1,    68,    -1,
      70,    -1,    72,    73,    15,    16,    17,    18,    -1,    20,
      21,    22,    -1,    -1,    25,    -1,    -1,    28,    29,    -1,
      -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      41,    42,    43,    -1,    -1,    46,    47,    48,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    66,    -1,    68,    -1,    70,
      -1,    72,    73,     1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,
      18,    -1,    20,    21,    22,    -1,    -1,    25,    -1,    -1,
      28,    29,    30,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    39,    -1,    41,    42,    43,    -1,    -1,    46,    47,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,
      -1,    -1,     1,    -1,     3,     4,     5,     6,    66,    -1,
      68,    -1,    70,    -1,    72,    73,    15,    16,    17,    18,
      -1,    20,    21,    22,    -1,    -1,    25,    -1,    -1,    28,
      29,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    41,    42,    43,    -1,    -1,    46,    47,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,
      -1,     1,    -1,     3,     4,     5,     6,    66,    -1,    68,
      -1,    70,    -1,    72,    73,    15,    16,    17,    18,    -1,
      20,    21,    22,    -1,    -1,    25,    -1,    -1,    28,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      -1,    41,    42,    43,    -1,    -1,    46,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,
       1,    -1,     3,     4,     5,     6,    66,    -1,    68,    -1,
      70,    -1,    72,    73,    15,    16,    17,    18,    -1,    20,
      21,    22,    -1,    -1,    25,    -1,    -1,    28,    29,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,    -1,    -1,
      41,    42,    43,    -1,    -1,    46,    47,    48,    -1,    -1,
      -1,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,     1,
      -1,     3,     4,     5,     6,    66,    -1,    68,    -1,    70,
      -1,    72,    73,    15,    16,    17,    18,    -1,    20,    21,
      22,    -1,    -1,    25,    -1,    -1,    28,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    39,    -1,    41,
      42,    43,    -1,    -1,    46,    47,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    -1,    -1,    -1,     1,    -1,
       3,     4,     5,     6,    66,    -1,    68,    -1,    70,    -1,
      72,    73,    15,    16,    17,    18,    -1,    20,    21,    22,
      -1,    -1,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,
      43,    -1,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,     6,    66,    -1,    68,    -1,    70,    -1,    72,
      73,    15,    16,    17,    18,    -1,    20,    21,    22,    -1,
      -1,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    37,    -1,    -1,    -1,    41,    42,    43,
      -1,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    56,    -1,    -1,    -1,     1,    -1,     3,     4,
       5,     6,    66,    -1,    68,    -1,    70,    -1,    72,    73,
      15,    16,    17,    18,    -1,    20,    21,    22,    -1,    -1,
      25,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    -1,
      -1,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    56,    -1,    -1,    -1,     1,    -1,     3,     4,     5,
       6,    66,    -1,    68,    -1,    70,    -1,    72,    73,    15,
      16,    17,    18,    -1,    20,    21,    22,    -1,    -1,    25,
      -1,    -1,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,
      46,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      56,    -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,
      66,    -1,    68,    -1,    70,    -1,    72,    73,    15,    16,
      17,    18,    -1,    20,    21,    22,    -1,    -1,    25,    -1,
      -1,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,    46,
      47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,
      -1,    -1,    -1,     1,    -1,     3,     4,     5,     6,    66,
      -1,    68,    -1,    70,    -1,    72,    73,    15,    16,    17,
      18,    -1,    20,    21,    22,    -1,    -1,    25,    -1,    -1,
      28,    29,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,
       4,     5,     6,    41,    42,    43,    -1,    -1,    46,    47,
      48,    15,    -1,    17,    -1,    -1,    -1,    55,    56,    -1,
      -1,    -1,     1,    -1,     3,     4,    -1,    -1,    66,    -1,
      68,    -1,    70,    -1,    72,    73,    15,    -1,    17,    -1,
      -1,     1,    -1,     3,     4,    -1,    -1,    -1,    -1,    -1,
      -1,    55,    56,    -1,    -1,    15,    -1,    17,    -1,    -1,
      -1,    -1,    66,    67,    -1,    -1,    70,    -1,    72,    73,
       1,    -1,     3,     4,    -1,    -1,    55,    56,    -1,    -1,
      -1,    -1,    -1,    -1,    15,    -1,    17,    66,    -1,    -1,
      -1,    70,    -1,    72,    73,    55,    56,    -1,    -1,    -1,
       1,    -1,     3,     4,    -1,     1,    66,     3,     4,    -1,
      70,    -1,    72,    73,    15,    -1,    17,    -1,    -1,    15,
      -1,    17,    -1,    -1,    55,    56,    -1,    -1,    -1,     1,
      -1,     3,     4,    -1,     1,    66,     3,     4,    -1,    70,
      -1,    72,    73,    15,    -1,    17,    -1,    -1,    15,    -1,
      17,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,    55,
      56,    -1,    -1,    -1,     1,    66,     3,     4,    -1,    70,
      66,    72,    73,    -1,    70,    -1,    72,    73,    15,    -1,
      17,    -1,    -1,    55,    56,    -1,    -1,    -1,    55,    56,
      -1,    -1,    -1,     1,    66,     3,     4,    -1,    70,    66,
      72,    73,    -1,    70,    -1,    72,    73,    15,    -1,    17,
      -1,    -1,     1,    -1,     3,     4,    -1,    -1,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    17,    66,
      -1,    -1,    -1,    70,    -1,    72,    73,     1,    -1,     3,
       4,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,
      -1,    15,    -1,    17,    -1,    -1,    -1,     1,    66,     3,
       4,    -1,    70,    -1,    72,    73,    55,    56,    -1,    -1,
      -1,    15,    -1,    17,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    70,    -1,    72,    73,     1,    -1,     3,     4,    -1,
      -1,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      -1,    17,    66,    -1,    -1,    -1,    70,    -1,    72,    73,
      -1,    55,    56,    -1,    -1,    -1,     1,    -1,     3,     4,
      -1,     1,    66,     3,     4,    -1,    70,    -1,    72,    73,
      15,    -1,    17,    -1,    -1,    15,    -1,    17,    -1,    55,
      56,    -1,    -1,    -1,     1,    -1,     3,     4,    -1,     1,
      66,     3,     4,    -1,    70,    -1,    72,    73,    15,    -1,
      17,    -1,    -1,    15,    -1,    17,    -1,    -1,    -1,    -1,
      55,    56,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,
       1,    66,     3,     4,    -1,    70,    66,    72,    73,    -1,
      70,    -1,    72,    73,    15,    -1,    17,    -1,    55,    56,
      -1,    -1,    -1,    55,    56,    -1,    -1,    -1,     1,    66,
       3,     4,    -1,    70,    66,    72,    73,    -1,    70,    -1,
      72,    73,    15,    -1,    17,    -1,    -1,     1,    -1,     3,
       4,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,    -1,
      -1,    15,    -1,    17,    -1,    66,    -1,    -1,    -1,    70,
      -1,    72,    73,     1,    -1,     3,     4,    -1,     1,    -1,
       3,     4,    55,    56,    -1,    -1,    -1,    15,    -1,    17,
      -1,    -1,    15,    66,    17,    -1,    -1,    70,    -1,    72,
      73,    55,    56,    -1,    -1,    -1,     1,    -1,     3,     4,
      -1,    -1,    66,    -1,    -1,    -1,    70,    -1,    72,    73,
      15,    -1,    17,    -1,    -1,    -1,    -1,    55,    56,    -1,
      -1,    -1,    55,    56,    -1,    -1,    -1,     1,    66,     3,
       4,    -1,    70,    66,    72,    73,    -1,    70,    -1,    72,
      73,    15,    -1,    17,    -1,    -1,     1,    -1,     3,     4,
      55,    56,    -1,    -1,    -1,     1,    -1,     3,     4,    -1,
      15,    66,    17,    -1,    -1,    70,    -1,    72,    73,    15,
      -1,    17,    -1,    -1,     1,    -1,     3,     4,    -1,    -1,
      -1,    55,    56,    -1,    -1,    -1,    -1,    -1,    15,    -1,
      17,    -1,    66,    -1,    -1,    -1,    70,    -1,    72,    73,
      55,    56,     3,     4,    -1,    -1,    -1,     3,     4,    55,
      56,    66,    -1,    -1,    15,    70,    17,    72,    73,    15,
      66,    17,    -1,    -1,    70,    -1,    72,    73,    55,    56,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,
      -1,    -1,    -1,    70,    -1,    72,    73,    -1,    -1,     3,
       4,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,    55,
      56,    15,    -1,    17,    -1,    66,    -1,    -1,    -1,    70,
      66,    72,    73,    74,    70,     1,    72,    73,    74,    -1,
      -1,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,     3,     4,    -1,    -1,    49,    -1,    -1,    -1,     3,
       4,    55,    56,    15,    58,    17,    -1,    -1,    -1,    -1,
      -1,    15,    66,    17,    -1,    -1,    70,    -1,    72,    73,
      -1,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,     3,     4,    -1,    63,    64,    -1,
      -1,    -1,    -1,    55,    56,    71,    15,    -1,    17,    -1,
      -1,    55,    56,    -1,    66,    67,    -1,    -1,    70,    -1,
      72,    73,    66,    67,    -1,    -1,    70,    -1,    72,    73,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    66,    -1,    -1,
      -1,    70,    -1,    72,    73,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    -1,    63,    64,
      -1,    -1,    -1,    68,    69,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      -1,    63,    64,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      -1,    63,    64,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    -1,    63,
      64,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    -1,    -1,    63,
      64
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,    15,    16,    17,    18,    19,    20,
      21,    22,    25,    28,    29,    41,    42,    43,    46,    47,
      48,    55,    56,    66,    70,    72,    73,    76,    77,    78,
      82,    83,    84,    85,    87,    88,    90,    91,    92,    93,
      94,    96,   102,   104,   105,   111,   112,   113,   114,   115,
      15,    86,     1,    66,   106,   113,   115,     1,     3,    45,
      66,    79,     1,     3,    70,   103,     1,   113,   113,    82,
       1,     3,    89,     1,    89,   113,   113,     1,     5,     6,
      67,   113,   120,   121,   123,     1,   113,     1,   113,     1,
      74,   119,   121,   123,     0,    77,    83,     5,     6,    68,
       7,     8,     9,    10,    11,    12,    13,    14,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    63,
      64,    31,    32,    34,    65,   116,    15,   120,    24,   107,
     108,   109,     5,     6,    68,    69,    97,     1,    32,    65,
      82,     1,    65,     1,     3,    51,    80,    81,     1,     3,
      65,     1,     3,    97,    97,    97,    30,    95,     1,     3,
      67,   121,   120,    68,   122,     1,    71,    74,   121,   119,
       1,   113,     1,   113,     1,   113,     1,   113,     1,   113,
       1,   113,     1,   113,     1,   113,     1,   113,     1,   113,
       1,   113,     1,   113,     1,   113,     1,   113,     1,   113,
       1,   113,     1,   113,     1,   113,     1,   113,    49,    58,
     113,   117,   118,   118,     1,   113,    67,     1,   106,    23,
     110,   109,    82,     1,    33,    80,     3,     1,    67,    68,
       1,    32,    82,     1,   113,     1,    65,    82,    82,    26,
      98,    99,   100,    82,    39,    65,    67,   123,    67,   121,
     113,    74,   123,    74,   121,     1,     3,     1,    33,    68,
       1,    35,     1,    82,    40,    82,    33,     1,    65,    81,
       1,    33,    80,     1,   113,    36,    38,   113,    27,   101,
     100,     1,     3,    67,    74,    65,   117,     1,    82,    82,
      33,     1,    71,    97,    82,    37,     1,    32,     1,   113,
       1,    82,    82,     1,   118,     1,    33
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 140 "../../../libs/libFreeMat/Parser.yxx"
    {mainAST = yyvsp[0];}
    break;

  case 5:
#line 145 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-5]->toStringList();
     r->name = yyvsp[-4]->text;
     r->arguments = yyvsp[-2]->toStringList();
     r->code = yyvsp[0];
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   }
    break;

  case 6:
#line 161 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-4]->text;
     r->arguments = yyvsp[-2]->toStringList();
     r->code = yyvsp[0];
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   }
    break;

  case 7:
#line 176 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-2]->toStringList();
     r->name = yyvsp[-1]->text;
     r->code = yyvsp[0];
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   }
    break;

  case 8:
#line 191 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-1]->text;
     r->code = yyvsp[0];
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   }
    break;

  case 9:
#line 205 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-4]->toStringList();
     r->name = yyvsp[-3]->text;
     r->code = yyvsp[0];
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   }
    break;

  case 10:
#line 220 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-3]->text;
     r->code = yyvsp[-1];
     r->nextFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	mainMDef->nextFunction = r;
     }
   }
    break;

  case 11:
#line 234 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("function name or return declaration");}
    break;

  case 12:
#line 235 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("argument list or statement list");}
    break;

  case 13:
#line 236 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("(possibly empty) argument list");}
    break;

  case 14:
#line 237 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list");}
    break;

  case 15:
#line 238 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("function name");}
    break;

  case 16:
#line 239 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("argument list or statement list");}
    break;

  case 17:
#line 240 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("(possibly empty) argument list");}
    break;

  case 18:
#line 241 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list");}
    break;

  case 21:
#line 250 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 22:
#line 251 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 23:
#line 252 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2];}
    break;

  case 24:
#line 253 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("either 'varargout', a single returned variable, or a list of return variables in return declaration");}
    break;

  case 25:
#line 254 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after 'varargout' in return declaration");}
    break;

  case 26:
#line 255 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after identifier in return declaration");}
    break;

  case 27:
#line 256 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a valid list of return arguments in return declaration");}
    break;

  case 28:
#line 257 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching ']' in return declaration");}
    break;

  case 29:
#line 258 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after return declaration");}
    break;

  case 30:
#line 262 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[0];}
    break;

  case 31:
#line 263 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 33:
#line 267 "../../../libs/libFreeMat/Parser.yxx"
    {
	yyval = yyvsp[0];
	char *b = (char*) malloc(strlen(yyvsp[0]->text)+2);
	b[0] = '&';
	strcpy(b+1,yyvsp[0]->text);
	yyval->text = b;
  }
    break;

  case 34:
#line 278 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_BLOCK,yyvsp[0]);}
    break;

  case 35:
#line 279 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 36:
#line 283 "../../../libs/libFreeMat/Parser.yxx"
    {
  	    yyval = new AST(OP_QSTATEMENT,NULL);
	    if (!interactiveMode) {
	      snprintf(msgBuffer,MSGBUFLEN,"line %d of file %s",lineNumber,filename);
	      yyval->down = new AST(context_node, strdup(msgBuffer));
	      yyval->down->down = yyvsp[-1];
	    } else
	      yyval->down = yyvsp[-1];
	 }
    break;

  case 37:
#line 292 "../../../libs/libFreeMat/Parser.yxx"
    {
	    yyval = new AST(OP_RSTATEMENT,NULL);
	    if (!interactiveMode) {
	      snprintf(msgBuffer,MSGBUFLEN,"line %d of file %s",lineNumber,filename);
	      yyval->down = new AST(context_node, strdup(msgBuffer));
	      yyval->down->down = yyvsp[-1];
	    } else
	      yyval->down = yyvsp[-1];
	 }
    break;

  case 38:
#line 301 "../../../libs/libFreeMat/Parser.yxx"
    {
	    yyval = new AST(OP_RSTATEMENT,NULL);
	    if (!interactiveMode) {
	      snprintf(msgBuffer,MSGBUFLEN,"line %d of file %s",lineNumber,filename);
	      yyval->down = new AST(context_node, strdup(msgBuffer));
	      yyval->down->down = yyvsp[-1];
	    } else
	      yyval->down = yyvsp[-1];
	 }
    break;

  case 41:
#line 315 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(null_node,"");}
    break;

  case 57:
#line 330 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("recognizable statement type (assignment, expression, function call, etc...)");}
    break;

  case 58:
#line 334 "../../../libs/libFreeMat/Parser.yxx"
    {yyvsp[-1]->addChild(yyvsp[0]); yyval = new AST(OP_SCALL,yyvsp[-1]);}
    break;

  case 59:
#line 338 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_PARENS,yyvsp[0]);}
    break;

  case 60:
#line 339 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 61:
#line 343 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]); }
    break;

  case 62:
#line 344 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as persistent");}
    break;

  case 63:
#line 348 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 64:
#line 349 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as global");}
    break;

  case 66:
#line 354 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 67:
#line 355 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of valid identifiers");}
    break;

  case 72:
#line 375 "../../../libs/libFreeMat/Parser.yxx"
    { yyval = yyvsp[-3]; yyval->addChild(yyvsp[-2]); if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);}
    break;

  case 73:
#line 379 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[0];}
    break;

  case 74:
#line 380 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = NULL;}
    break;

  case 75:
#line 384 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-5]; yyval->addChild(yyvsp[-4]); 
	  if (yyvsp[-2] != NULL) yyval->addChild(yyvsp[-2]); 
	  if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);
	}
    break;

  case 80:
#line 396 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = NULL;}
    break;

  case 82:
#line 401 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = new AST(OP_CASEBLOCK,yyvsp[0]);
	}
    break;

  case 83:
#line 404 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);
	}
    break;

  case 84:
#line 410 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-3]; yyval->addChild(yyvsp[-2]); yyval->addChild(yyvsp[0]);
	}
    break;

  case 85:
#line 416 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 86:
#line 419 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = NULL;
	}
    break;

  case 87:
#line 425 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-4]; yyval->addChild(yyvsp[-3]); yyval->addChild(yyvsp[-1]);
	}
    break;

  case 88:
#line 431 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-3]; yyval->addChild(yyvsp[-1]);}
    break;

  case 89:
#line 432 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 90:
#line 433 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[0]; yyval->addChild(new AST(OP_RHS, new AST(id_node,yyvsp[0]->text))); }
    break;

  case 91:
#line 434 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis");}
    break;

  case 92:
#line 435 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression");}
    break;

  case 93:
#line 436 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("equals operator after loop index");}
    break;

  case 94:
#line 437 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier that is the loop variable");}
    break;

  case 95:
#line 438 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression");}
    break;

  case 96:
#line 439 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier or assignment (id = expr) after 'for' ");}
    break;

  case 97:
#line 443 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-4]; yyval->addChild(yyvsp[-3]); yyval->addChild(yyvsp[-1]);
	}
    break;

  case 98:
#line 446 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test expression after 'while'");}
    break;

  case 99:
#line 450 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-4]; yyval->addChild(yyvsp[-3]); if (yyvsp[-2] != NULL) yyval->addChild(yyvsp[-2]); 
	  if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);
	}
    break;

  case 100:
#line 454 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("condition expression for 'if'");}
    break;

  case 101:
#line 458 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = new AST(OP_CSTAT,yyvsp[-2],yyvsp[0]);
	}
    break;

  case 102:
#line 464 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = NULL;}
    break;

  case 104:
#line 469 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = new AST(OP_ELSEIFBLOCK,yyvsp[0]);
	}
    break;

  case 105:
#line 472 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);
	}
    break;

  case 106:
#line 478 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 107:
#line 481 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test condition for 'elseif' clause");}
    break;

  case 108:
#line 484 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 109:
#line 487 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = NULL;}
    break;

  case 110:
#line 488 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list for 'else' clause");}
    break;

  case 111:
#line 492 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_ASSIGN,yyvsp[-2],yyvsp[0]);}
    break;

  case 112:
#line 493 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expression in assignment");}
    break;

  case 113:
#line 497 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[-3]->addChild(new AST(OP_PARENS,yyvsp[-1]));
	  yyval = new AST(OP_MULTICALL,yyvsp[-6],yyvsp[-3]);
	}
    break;

  case 114:
#line 501 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[0]->addChild(new AST(OP_PARENS,NULL));
	  yyval = new AST(OP_MULTICALL,yyvsp[-3],yyvsp[0]);
	}
    break;

  case 115:
#line 506 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis");}
    break;

  case 116:
#line 508 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing list");}
    break;

  case 117:
#line 510 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("left parenthesis");}
    break;

  case 118:
#line 512 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier");}
    break;

  case 119:
#line 516 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_COLON,yyvsp[-2],yyvsp[0]);}
    break;

  case 120:
#line 517 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after ':'");}
    break;

  case 122:
#line 519 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_PLUS,yyvsp[-2],yyvsp[0]);}
    break;

  case 123:
#line 520 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '+'");}
    break;

  case 124:
#line 521 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_SUBTRACT,yyvsp[-2],yyvsp[0]);}
    break;

  case 125:
#line 522 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '-'");}
    break;

  case 126:
#line 523 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_TIMES,yyvsp[-2],yyvsp[0]);}
    break;

  case 127:
#line 524 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '*'");}
    break;

  case 128:
#line 525 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_RDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 129:
#line 526 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '/'");}
    break;

  case 130:
#line 527 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_LDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 131:
#line 528 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '\\'");}
    break;

  case 132:
#line 529 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_OR,yyvsp[-2],yyvsp[0]);}
    break;

  case 133:
#line 530 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '|'");}
    break;

  case 134:
#line 531 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_AND,yyvsp[-2],yyvsp[0]);}
    break;

  case 135:
#line 532 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '&'");}
    break;

  case 136:
#line 533 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_LT,yyvsp[-2],yyvsp[0]);}
    break;

  case 137:
#line 534 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<'");}
    break;

  case 138:
#line 535 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_LEQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 139:
#line 536 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<='");}
    break;

  case 140:
#line 537 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_GT,yyvsp[-2],yyvsp[0]);}
    break;

  case 141:
#line 538 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>'");}
    break;

  case 142:
#line 539 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_GEQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 143:
#line 540 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>='");}
    break;

  case 144:
#line 541 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_EQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 145:
#line 542 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '=='");}
    break;

  case 146:
#line 543 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_NEQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 147:
#line 544 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '~='");}
    break;

  case 148:
#line 545 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT_TIMES,yyvsp[-2],yyvsp[0]);}
    break;

  case 149:
#line 546 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.*'");}
    break;

  case 150:
#line 547 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT_RDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 151:
#line 548 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after './'");}
    break;

  case 152:
#line 549 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT_LDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 153:
#line 550 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.\\'");}
    break;

  case 154:
#line 551 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_NEG,yyvsp[0]);}
    break;

  case 155:
#line 552 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[0];}
    break;

  case 156:
#line 553 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_NOT,yyvsp[0]);}
    break;

  case 157:
#line 554 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after logical not");}
    break;

  case 158:
#line 555 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_POWER,yyvsp[-2],yyvsp[0]);}
    break;

  case 159:
#line 556 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '^'");}
    break;

  case 160:
#line 557 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT_POWER,yyvsp[-2],yyvsp[0]);}
    break;

  case 161:
#line 558 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.^'");}
    break;

  case 162:
#line 559 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_TRANSPOSE,yyvsp[-1]);}
    break;

  case 163:
#line 560 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT_TRANSPOSE,yyvsp[-1]);}
    break;

  case 164:
#line 561 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 165:
#line 562 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a right parenthesis after expression");}
    break;

  case 166:
#line 563 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after left parenthesis");}
    break;

  case 170:
#line 570 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_RHS,yyvsp[0]);}
    break;

  case 171:
#line 571 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 172:
#line 572 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a matrix definition followed by a right bracket");}
    break;

  case 173:
#line 573 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 174:
#line 574 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2];}
    break;

  case 175:
#line 575 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2];}
    break;

  case 176:
#line 576 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_EMPTY,NULL);}
    break;

  case 177:
#line 577 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 178:
#line 578 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 179:
#line 579 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2];}
    break;

  case 180:
#line 580 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2];}
    break;

  case 181:
#line 581 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_EMPTY_CELL,NULL);}
    break;

  case 182:
#line 582 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a cell-array definition followed by a right brace");}
    break;

  case 184:
#line 587 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 185:
#line 591 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_PARENS,yyvsp[-1]); }
    break;

  case 186:
#line 592 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis");}
    break;

  case 187:
#line 593 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_BRACES,yyvsp[-1]); }
    break;

  case 188:
#line 594 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right brace");}
    break;

  case 189:
#line 595 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT,yyvsp[0]); }
    break;

  case 191:
#line 600 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_ALL,NULL);}
    break;

  case 192:
#line 601 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_KEYWORD,yyvsp[-2],yyvsp[0]);}
    break;

  case 193:
#line 602 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting expression after '=' in keyword assignment");}
    break;

  case 194:
#line 603 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_KEYWORD,yyvsp[0]);}
    break;

  case 195:
#line 604 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting keyword identifier after '/' in keyword assignment");}
    break;

  case 197:
#line 609 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addPeer(yyvsp[0]);}
    break;

  case 198:
#line 613 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_BRACES,yyvsp[0]);}
    break;

  case 199:
#line 614 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 200:
#line 618 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_BRACKETS,yyvsp[0]);}
    break;

  case 201:
#line 619 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 206:
#line 631 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_SEMICOLON,yyvsp[0]);}
    break;

  case 207:
#line 632 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2936 "../../../libs/libFreeMat/Parser.cxx"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
	  yychar = YYEMPTY;

	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 635 "../../../libs/libFreeMat/Parser.yxx"


namespace FreeMat {
  
  void resetParser() {
    mainAST = NULL;
    mainMDef = NULL;
    errorOccured = false;
  }
  
  ASTPtr getParsedScriptBlock() {
    return mainAST;
  }
  
  MFunctionDef* getParsedFunctionDef() {
    return mainMDef;
  }
  
  ParserState parseState() {
    if (errorOccured)
      return ParseError;
    if (mainAST != NULL) 
      return ScriptBlock;
    else
      return FuncDef;
  }
  
  ParserState parseString(char *txt) {
/*     yydebug = 1; 	*/
    resetParser();
    interactiveMode = true;
    setLexBuffer(txt);
    yyparse();
    return parseState();
  }
  
  ParserState parseFile(FILE *fp, const char* fname) {
    resetParser();
    interactiveMode = false;
    filename = fname;
    setLexFile(fp);
    yyparse();
    return parseState();
  }
  
}

