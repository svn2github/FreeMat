/* A Bison parser, made by GNU Bison 1.875a.  */

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
#line 1 "../../../libs/libFreeMat/Parser.yy"

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
#line 298 "../../../libs/libFreeMat/Parser.cc"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
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
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

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
#  if 1 < __GNUC__
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
#define YYFINAL  96
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   3054

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  50
/* YYNRULES -- Number of rules. */
#define YYNRULES  210
/* YYNRULES -- Number of states. */
#define YYNSTATES  306

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
      69,    70,    57,    55,    68,    56,     2,    58,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    49,     2,
      52,    65,    53,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    66,    59,    67,    63,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    72,    50,    73,    71,     2,     2,     2,
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
     123,   125,   129,   131,   132,   137,   139,   142,   145,   148,
     151,   153,   155,   156,   158,   160,   162,   164,   166,   168,
     170,   172,   174,   176,   178,   180,   182,   184,   186,   188,
     191,   193,   196,   199,   202,   205,   208,   210,   213,   216,
     218,   220,   222,   224,   229,   232,   233,   240,   242,   244,
     246,   247,   248,   250,   252,   255,   259,   262,   263,   268,
     274,   278,   280,   286,   291,   295,   298,   302,   304,   309,
     312,   318,   321,   324,   325,   327,   329,   332,   335,   338,
     341,   342,   345,   349,   353,   362,   368,   377,   385,   392,
     398,   402,   406,   408,   412,   416,   420,   424,   428,   432,
     436,   440,   444,   448,   452,   456,   460,   464,   468,   472,
     476,   480,   484,   488,   492,   496,   500,   504,   508,   512,
     516,   520,   524,   528,   532,   536,   539,   542,   545,   548,
     551,   554,   558,   562,   566,   570,   573,   576,   580,   584,
     587,   589,   591,   593,   595,   599,   602,   607,   612,   618,
     621,   625,   630,   635,   641,   644,   647,   649,   652,   656,
     660,   664,   668,   670,   672,   674,   679,   684,   687,   690,
     692,   696,   698,   702,   704,   708,   710,   712,   714,   715,
     717
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      75,     0,    -1,    82,    -1,    77,    -1,    -1,    19,    78,
       3,    32,    79,    33,    82,    -1,    19,     3,    32,    79,
      33,    82,    -1,    19,    78,     3,    82,    -1,    19,     3,
      82,    -1,    19,    78,     3,    32,    33,    82,    -1,    19,
       3,    32,    33,    82,    -1,    19,     1,    -1,    19,     3,
       1,    -1,    19,     3,    32,     1,    -1,    19,     3,    32,
      79,    33,     1,    -1,    19,    78,     1,    -1,    19,    78,
       3,     1,    -1,    19,    78,     3,    32,     1,    -1,    19,
      78,     3,    32,    79,    33,     1,    -1,    76,    -1,    77,
      76,    -1,    45,    65,    -1,     3,    65,    -1,    66,    79,
      67,    65,    -1,     1,    -1,    45,     1,    -1,     3,     1,
      -1,    66,     1,    -1,    66,    79,     1,    -1,    66,    79,
      67,     1,    -1,    80,    -1,    79,    68,    80,    -1,     3,
      -1,    -1,    51,     3,    81,     1,    -1,    83,    -1,    82,
      83,    -1,    84,     5,    -1,    84,     6,    -1,    84,    68,
      -1,   111,    -1,   113,    -1,    -1,   112,    -1,   102,    -1,
      93,    -1,    92,    -1,   104,    -1,   105,    -1,    96,    -1,
      94,    -1,    91,    -1,    90,    -1,    88,    -1,    87,    -1,
      85,    -1,    47,    -1,    48,    -1,     1,    -1,    16,    86,
      -1,    15,    -1,    86,    15,    -1,    41,    89,    -1,    41,
       1,    -1,    46,    89,    -1,    46,     1,    -1,     3,    -1,
      89,     3,    -1,    89,     1,    -1,    43,    -1,    42,    -1,
      28,    -1,    21,    -1,    29,    82,    95,    39,    -1,    30,
      82,    -1,    -1,    25,   113,    97,    98,   101,    37,    -1,
      68,    -1,     6,    -1,     5,    -1,    -1,    -1,    99,    -1,
     100,    -1,    99,   100,    -1,    26,   113,    82,    -1,    27,
      82,    -1,    -1,    20,   103,    82,    36,    -1,    69,     3,
      65,   113,    70,    -1,     3,    65,   113,    -1,     3,    -1,
      69,     3,    65,   113,     1,    -1,    69,     3,    65,     1,
      -1,    69,     3,     1,    -1,    69,     1,    -1,     3,    65,
       1,    -1,     1,    -1,    22,   113,    82,    38,    -1,    22,
       1,    -1,    18,   106,   107,   110,    40,    -1,    18,     1,
      -1,   113,    82,    -1,    -1,   108,    -1,   109,    -1,   108,
     109,    -1,    24,   106,    -1,    24,     1,    -1,    23,    82,
      -1,    -1,    23,     1,    -1,   115,    65,   113,    -1,   115,
      65,     1,    -1,    66,   120,    67,    65,     3,    32,   118,
      33,    -1,    66,   120,    67,    65,     3,    -1,    66,   120,
      67,    65,     3,    32,   118,     1,    -1,    66,   120,    67,
      65,     3,    32,     1,    -1,    66,   120,    67,    65,     3,
       1,    -1,    66,   120,    67,    65,     1,    -1,   113,    49,
     113,    -1,   113,    49,     1,    -1,   114,    -1,   113,    55,
     113,    -1,   113,    55,     1,    -1,   113,    56,   113,    -1,
     113,    56,     1,    -1,   113,    57,   113,    -1,   113,    57,
       1,    -1,   113,    58,   113,    -1,   113,    58,     1,    -1,
     113,    59,   113,    -1,   113,    59,     1,    -1,   113,    50,
     113,    -1,   113,    50,     1,    -1,   113,    51,   113,    -1,
     113,    51,     1,    -1,   113,    52,   113,    -1,   113,    52,
       1,    -1,   113,     7,   113,    -1,   113,     7,     1,    -1,
     113,    53,   113,    -1,   113,    53,     1,    -1,   113,     8,
     113,    -1,   113,     8,     1,    -1,   113,     9,   113,    -1,
     113,     9,     1,    -1,   113,    54,   113,    -1,   113,    54,
       1,    -1,   113,    10,   113,    -1,   113,    10,     1,    -1,
     113,    11,   113,    -1,   113,    11,     1,    -1,   113,    12,
     113,    -1,   113,    12,     1,    -1,    61,   113,    -1,    61,
       1,    -1,    62,   113,    -1,    62,     1,    -1,    71,   113,
      -1,    71,     1,    -1,   113,    63,   113,    -1,   113,    63,
       1,    -1,   113,    13,   113,    -1,   113,    13,     1,    -1,
     113,    64,    -1,   113,    14,    -1,    69,   113,    70,    -1,
      69,   113,     1,    -1,    69,     1,    -1,     4,    -1,    15,
      -1,    17,    -1,   115,    -1,    66,   120,    67,    -1,    66,
       1,    -1,    66,   121,   120,    67,    -1,    66,   120,   121,
      67,    -1,    66,   121,   120,   121,    67,    -1,    66,    67,
      -1,    72,   119,    73,    -1,    72,   121,   119,    73,    -1,
      72,   119,   121,    73,    -1,    72,   121,   119,   121,    73,
      -1,    72,    73,    -1,    72,     1,    -1,     3,    -1,   115,
     116,    -1,    32,   118,    33,    -1,    32,   118,     1,    -1,
      34,   118,    35,    -1,    34,   118,     1,    -1,    31,    -1,
     113,    -1,    49,    -1,    58,     3,    65,   113,    -1,    58,
       3,    65,     1,    -1,    58,     3,    -1,    58,     1,    -1,
     117,    -1,   118,    68,   117,    -1,   123,    -1,   119,   121,
     123,    -1,   123,    -1,   120,   121,   123,    -1,     6,    -1,
       5,    -1,    68,    -1,    -1,   113,    -1,   123,   122,   113,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   136,   136,   137,   137,   141,   157,   172,   187,   201,
     216,   230,   231,   232,   233,   234,   235,   236,   237,   241,
     242,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     258,   259,   263,   263,   263,   275,   276,   280,   289,   298,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   326,   327,   331,
     335,   336,   340,   341,   345,   346,   350,   351,   352,   356,
     360,   364,   367,   371,   376,   377,   381,   389,   389,   389,
     389,   393,   394,   398,   401,   407,   413,   416,   422,   428,
     429,   430,   431,   432,   433,   434,   435,   436,   440,   443,
     447,   451,   455,   461,   462,   466,   469,   475,   478,   481,
     484,   485,   489,   490,   494,   498,   502,   504,   506,   508,
     513,   514,   515,   516,   517,   518,   519,   520,   521,   522,
     523,   524,   525,   526,   527,   528,   529,   530,   531,   532,
     533,   534,   535,   536,   537,   538,   539,   540,   541,   542,
     543,   544,   545,   546,   547,   548,   549,   550,   551,   552,
     553,   554,   555,   556,   557,   558,   559,   560,   561,   562,
     566,   567,   568,   569,   570,   571,   572,   573,   574,   575,
     576,   577,   578,   579,   580,   581,   585,   586,   590,   591,
     592,   593,   594,   598,   599,   600,   601,   602,   603,   607,
     608,   612,   613,   617,   618,   622,   622,   626,   626,   630,
     631
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
  "REFRPAREN", "REFLBRACE", "REFRBRACE", "ENDFOR", "ENDSWITCH", 
  "ENDWHILE", "ENDTRY", "ENDIF", "PERSISTENT", "KEYBOARD", "RETURN", 
  "VARARGIN", "VARARGOUT", "GLOBAL", "QUIT", "RETALL", "':'", "'|'", 
  "'&'", "'<'", "'>'", "NE", "'+'", "'-'", "'*'", "'/'", "'\\\\'", "NOT", 
  "NEG", "POS", "'^'", "'''", "'='", "'['", "']'", "','", "'('", "')'", 
  "'~'", "'{'", "'}'", "$accept", "program", "functionDef", 
  "functionDefList", "returnDeclaration", "argumentList", "argument", 
  "@1", "statementList", "statement", "statementType", 
  "specialSyntaxStatement", "stringList", "persistentStatement", 
  "globalStatement", "identList", "returnStatement", "keyboardStatement", 
  "continueStatement", "breakStatement", "tryStatement", "optionalCatch", 
  "switchStatement", "optionalEndStatement", "caseBlock", "caseList", 
  "caseStatement", "otherwiseClause", "forStatement", 
  "forIndexExpression", "whileStatement", "ifStatement", 
  "conditionedStatement", "elseIfBlock", "elseIfStatementList", 
  "elseIfStatement", "elseStatement", "assignmentStatement", 
  "multiFunctionCall", "expr", "terminal", "symbRefList", "symbRef", 
  "indexElement", "indexList", "cellDef", "matrixDef", "rowSeperator", 
  "columnSep", "rowDef", 0
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
     305,   306,   307,    94,    39,    61,    91,    93,    44,    40,
      41,   126,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    74,    75,    75,    75,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    77,
      77,    78,    78,    78,    78,    78,    78,    78,    78,    78,
      79,    79,    80,    81,    80,    82,    82,    83,    83,    83,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    85,
      86,    86,    87,    87,    88,    88,    89,    89,    89,    90,
      91,    92,    93,    94,    95,    95,    96,    97,    97,    97,
      97,    98,    98,    99,    99,   100,   101,   101,   102,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   104,   104,
     105,   105,   106,   107,   107,   108,   108,   109,   109,   110,
     110,   110,   111,   111,   112,   112,   112,   112,   112,   112,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   115,   115,   116,   116,
     116,   116,   116,   117,   117,   117,   117,   117,   117,   118,
     118,   119,   119,   120,   120,   121,   121,   122,   122,   123,
     123
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     0,     7,     6,     4,     3,     6,
       5,     2,     3,     4,     6,     3,     4,     5,     7,     1,
       2,     2,     2,     4,     1,     2,     2,     2,     3,     4,
       1,     3,     1,     0,     4,     1,     2,     2,     2,     2,
       1,     1,     0,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     2,     2,     2,     2,     2,     1,     2,     2,     1,
       1,     1,     1,     4,     2,     0,     6,     1,     1,     1,
       0,     0,     1,     1,     2,     3,     2,     0,     4,     5,
       3,     1,     5,     4,     3,     2,     3,     1,     4,     2,
       5,     2,     2,     0,     1,     1,     2,     2,     2,     2,
       0,     2,     3,     3,     8,     5,     8,     7,     6,     5,
       3,     3,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     2,     2,
       2,     3,     3,     3,     3,     2,     2,     3,     3,     2,
       1,     1,     1,     1,     3,     2,     4,     4,     5,     2,
       3,     4,     4,     5,     2,     2,     1,     2,     3,     3,
       3,     3,     1,     1,     1,     4,     4,     2,     2,     1,
       3,     1,     3,     1,     3,     1,     1,     1,     0,     1,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    58,   186,   170,   171,     0,   172,     0,     0,     0,
      72,     0,     0,    71,     0,     0,    70,    69,     0,    56,
      57,     0,     0,     0,     0,     0,     0,     0,    19,     3,
       0,    35,     0,    55,    54,    53,    52,    51,    46,    45,
      50,    49,    44,    47,    48,    40,    43,    41,   122,   173,
      60,    59,   101,     0,   103,     0,   173,    11,     0,     0,
       0,     0,    97,    91,     0,     0,    99,     0,    80,     0,
      63,    66,     0,    65,     0,   156,   155,   158,   157,   175,
     206,   205,   179,   209,     0,     0,   208,   169,     0,   160,
     159,   185,   184,     0,     0,   208,     1,    20,    36,    37,
      38,    39,     0,     0,     0,     0,     0,     0,     0,   166,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   165,   192,     0,     0,     0,   187,    61,     0,
       0,   110,   104,   105,     0,    58,     0,    22,     0,    25,
      21,    27,    32,     0,     0,    30,    15,     0,     0,    95,
       0,     0,     0,    79,    78,    77,    81,     0,     0,    68,
      67,   174,     0,     0,   207,     0,   168,   167,   180,     0,
       0,   140,   139,   144,   143,   146,   145,   150,   149,   152,
     151,   154,   153,   164,   163,   121,   120,   134,   133,   136,
     135,   138,   137,   142,   141,   148,   147,   124,   123,   126,
     125,   128,   127,   130,   129,   132,   131,   162,   161,   194,
       0,   193,   199,     0,     0,   113,   112,   174,   108,   107,
       0,     0,   106,    13,     0,     0,    33,    28,     0,     0,
      58,     0,     0,    96,    90,    94,     0,    88,    98,     0,
      87,    82,    83,     0,    73,     0,   177,   208,   176,     0,
     210,   182,   208,   181,     0,   198,   197,   189,   188,     0,
     191,   190,    58,     0,   100,     0,     0,     0,    29,    23,
      31,    17,     0,     0,    93,     0,     0,     0,     0,    84,
     119,     0,   178,   183,     0,   200,    58,     0,    34,     0,
       0,    92,    89,     0,     0,    76,   118,     0,   196,   195,
      58,     0,   117,     0,   116,   114
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    27,    28,    29,    61,   144,   145,   267,    30,    31,
      32,    33,    51,    34,    35,    72,    36,    37,    38,    39,
      40,   158,    41,   156,   240,   241,   242,   278,    42,    65,
      43,    44,    54,   131,   132,   133,   221,    45,    46,    47,
      48,    56,   127,   212,   213,    93,    84,    85,   165,    86
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -136
static const short yypact[] =
{
     355,  -136,  -136,  -136,  -136,   -12,  -136,  2018,    87,    25,
    -136,  2033,  2817,  -136,  1730,    43,  -136,  -136,   250,  -136,
    -136,  2052,  2071,  1980,  2105,  2124,   198,    34,  -136,    22,
     866,  -136,   181,  -136,  -136,  -136,  -136,  -136,  -136,  -136,
    -136,  -136,  -136,  -136,  -136,  -136,  -136,  2905,  -136,   157,
    -136,    35,  -136,  1980,     1,   938,    11,   277,  1154,    75,
      36,   278,  -136,   -16,   282,  1730,  -136,   938,  2885,  1226,
    -136,  -136,    30,  -136,    67,  -136,   241,  -136,   241,  -136,
    -136,  -136,  -136,  2905,   131,  2817,   159,  -136,  2708,  -136,
     241,  -136,  -136,    47,  2817,     6,  -136,  -136,  -136,  -136,
    -136,  -136,  2143,  2158,  2177,  2196,  2230,  2249,  2268,  -136,
    2283,  2302,  2321,  2355,  2374,  2393,  2408,  2427,  2446,  2480,
    2499,  2518,  -136,  -136,  2787,  2787,  2533,  -136,  -136,   139,
    2552,    72,     1,  -136,  1010,   192,     5,  -136,   428,  -136,
    -136,  -136,  -136,    51,    62,  -136,  -136,  1298,  2571,  -136,
      77,  1370,  1442,  -136,  -136,  -136,   100,  1730,    18,  -136,
    -136,    68,  2684,   175,  -136,  2817,  -136,  -136,  -136,   320,
      54,  -136,  2669,  -136,  2669,  -136,  2669,  -136,   241,  -136,
     241,  -136,   241,  -136,    13,  -136,  2963,  -136,  2977,  -136,
    2990,  -136,  2669,  -136,  2669,  -136,  2669,  -136,   401,  -136,
     401,  -136,   241,  -136,   241,  -136,   241,  -136,    13,  -136,
     283,  2905,  -136,    14,    31,  -136,  2905,  -136,  -136,  -136,
    1802,    99,  -136,  -136,  1730,    -4,  -136,  -136,    96,    10,
     177,   211,   501,  -136,  2905,  -136,  2605,  -136,  -136,  2817,
     122,   100,  -136,  1514,  -136,   305,  -136,   173,  -136,  2802,
    2905,  -136,    12,  -136,  2641,  -136,    90,  -136,  -136,  2787,
    -136,  -136,   129,  1586,  -136,   574,  1874,   161,  -136,  -136,
    -136,  -136,  1730,    98,  -136,  2730,   938,  1730,   135,  -136,
    -136,   153,  -136,  -136,  2624,  -136,   200,   647,  -136,   720,
    1946,  -136,  -136,  1082,  1658,  -136,  -136,   167,  -136,  2905,
     256,   793,  -136,    88,  -136,  -136
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -136,  -136,   146,  -136,  -136,  -135,   -53,  -136,    26,    56,
    -136,  -136,  -136,  -136,  -136,   179,  -136,  -136,  -136,  -136,
    -136,  -136,  -136,  -136,  -136,  -136,   -36,  -136,  -136,  -136,
    -136,  -136,    79,  -136,  -136,    78,  -136,  -136,  -136,    -2,
    -136,     0,  -136,   -42,  -123,   124,   -37,   -22,  -136,   -19
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -205
static const short yytable[] =
{
      49,   225,   214,    50,    94,    55,   223,    95,   142,    67,
      68,  -201,  -201,   142,    49,   257,   129,  -202,  -202,    76,
      78,    83,    88,    90,    83,   130,    62,   109,    63,   266,
      49,   159,   260,   160,    96,   -62,   -62,   141,   224,   142,
      69,     8,   123,   124,    70,   125,    71,   258,   163,   148,
     128,    83,    80,    81,   226,    49,   143,   244,    49,    80,
      81,   143,   162,   227,   229,    49,   261,    49,   159,    49,
     160,   169,   -64,   -64,   164,    95,   139,   122,   235,  -201,
     164,   134,   259,    83,   138,  -202,    98,   143,    57,   304,
      58,   151,    83,   152,    64,   220,   273,   268,   -62,   259,
     172,   174,   176,   178,   180,   182,   184,   162,   186,   188,
     190,   192,   194,   196,   198,   200,   202,   204,   206,   208,
     168,   305,   211,   211,   216,    98,   239,   253,    55,   228,
     229,   290,    59,   245,    49,   -64,    80,    81,    49,   264,
     140,   249,   236,   247,    80,    81,   234,    49,   254,   277,
     252,    49,    49,    60,   296,   284,   259,    49,  -115,  -115,
      83,   269,   288,   250,  -203,  -203,   229,    83,   302,  -111,
       2,     3,   295,   232,   303,    97,   270,   -16,  -204,  -204,
      80,    81,     4,   243,     6,   297,    99,   100,   123,   124,
      98,   125,   -12,   -26,    98,   -26,   -16,    74,   161,    91,
     -14,     2,     3,    80,    81,   279,   217,    98,    98,   219,
     222,   -12,   271,     4,   142,     6,   209,   285,   170,   -14,
      49,  -115,   126,     0,    49,   210,  -203,   164,    21,    22,
     247,     0,    49,    53,   275,   252,    24,   276,    25,    26,
    -204,   164,   248,    49,   272,     0,   263,    83,     0,   101,
     265,    73,    83,    71,   108,   109,   -18,   211,     0,    21,
      22,     0,   143,    49,    53,    49,    49,    24,     0,    25,
      26,    92,    49,     0,     0,   -18,    49,    49,   -24,   146,
     -24,   147,   299,   149,   255,   150,   256,    49,    98,    49,
      49,     0,   287,    49,    49,   211,     0,     0,   289,    98,
       0,    49,   293,   294,   121,   122,   280,     0,   281,     0,
       0,     0,     0,     0,     0,     0,   301,     0,     0,    98,
       0,    98,     0,     2,     3,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     0,     6,     0,     0,
       0,     0,     0,    98,     0,    98,     0,     0,     0,    98,
      98,     0,     0,     0,     0,    -4,     1,    98,     2,     3,
     -42,   -42,     0,     0,     0,     0,     0,     0,     0,     0,
       4,     5,     6,     7,     8,     9,    10,    11,     0,     0,
      12,    21,    22,    13,    14,     0,    53,     0,     0,    24,
       0,    25,    26,   251,     0,     0,    15,    16,    17,     0,
       0,    18,    19,    20,     0,     0,     0,     0,     0,     0,
       0,   105,   106,   107,   108,   109,    21,    22,     0,     0,
       0,    23,     0,   -42,    24,     0,    25,    26,    -8,     1,
       0,     2,     3,   -42,   -42,     0,     0,     0,     0,     0,
       0,     0,     0,     4,     5,     6,     7,    -8,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,   118,   119,
     120,     0,     0,     0,   121,   122,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,    23,     0,   -42,    24,     0,    25,
      26,    -7,     1,     0,     2,     3,   -42,   -42,     0,     0,
       0,     0,     0,     0,     0,     0,     4,     5,     6,     7,
      -7,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    15,    16,    17,     0,     0,    18,    19,    20,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    21,    22,     0,     0,     0,    23,     0,   -42,
      24,     0,    25,    26,   -10,     1,     0,     2,     3,   -42,
     -42,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       5,     6,     7,   -10,     9,    10,    11,     0,     0,    12,
       0,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
      23,     0,   -42,    24,     0,    25,    26,    -6,     1,     0,
       2,     3,   -42,   -42,     0,     0,     0,     0,     0,     0,
       0,     0,     4,     5,     6,     7,    -6,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    15,    16,
      17,     0,     0,    18,    19,    20,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    21,    22,
       0,     0,     0,    23,     0,   -42,    24,     0,    25,    26,
      -9,     1,     0,     2,     3,   -42,   -42,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     5,     6,     7,    -9,
       9,    10,    11,     0,     0,    12,     0,     0,    13,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,    23,     0,   -42,    24,
       0,    25,    26,    -5,     1,     0,     2,     3,   -42,   -42,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     5,
       6,     7,    -5,     9,    10,    11,     0,     0,    12,     0,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,    16,    17,     0,     0,    18,
      19,    20,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    21,    22,     0,     0,     0,    23,
       0,   -42,    24,     0,    25,    26,    -2,     1,     0,     2,
       3,   -42,   -42,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     5,     6,     7,     0,     9,    10,    11,     0,
       0,    12,     0,     0,    13,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,    23,     0,   -42,    24,     0,    25,    26,     1,
       0,     2,     3,   -42,   -42,   102,   103,   104,   105,   106,
     107,   108,   109,     4,     5,     6,     7,     0,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,     0,    21,
      22,   121,   122,     0,    23,     0,   -42,    24,     0,    25,
      26,     1,     0,     2,     3,   -42,   -42,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     5,     6,     7,     0,
       9,    10,    11,  -102,  -102,    12,     0,     0,    13,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -102,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,    23,     0,   -42,    24,
       0,    25,    26,     1,     0,     2,     3,   -42,   -42,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,     6,
       7,     0,     9,    10,    11,     0,     0,    12,   -85,   -85,
      13,    14,     0,     0,     0,     0,     0,     0,     0,   -85,
       0,     0,     0,    15,    16,    17,     0,     0,    18,    19,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,    23,     0,
     -42,    24,     0,    25,    26,   135,     0,     2,     3,   -42,
     -42,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       5,     6,     7,     0,     9,    10,    11,     0,     0,    12,
       0,     0,    13,    14,     0,     0,   136,     0,     0,     0,
       0,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,   137,
      23,     0,   -42,    24,     0,    25,    26,     1,     0,     2,
       3,   -42,   -42,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     5,     6,     7,     0,     9,    10,    11,     0,
       0,    12,     0,     0,    13,    14,   157,     0,     0,     0,
       0,     0,     0,     0,     0,   -75,     0,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,    23,     0,   -42,    24,     0,    25,    26,   230,
       0,     2,     3,   -42,   -42,     0,     0,     0,     0,     0,
       0,     0,     0,     4,     5,     6,     7,     0,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,     0,     0,
     231,     0,     0,     0,     0,     0,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,    23,     0,   -42,    24,     0,    25,
      26,     1,     0,     2,     3,   -42,   -42,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     5,     6,     7,     0,
       9,    10,    11,     0,     0,    12,     0,     0,    13,    14,
       0,     0,     0,     0,     0,     0,   237,     0,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,    23,     0,   -42,    24,
       0,    25,    26,     1,     0,     2,     3,   -42,   -42,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,     6,
       7,     0,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,     0,     0,     0,     0,     0,     0,     0,     0,
     238,     0,     0,    15,    16,    17,     0,     0,    18,    19,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,    23,     0,
     -42,    24,     0,    25,    26,     1,     0,     2,     3,   -42,
     -42,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       5,     6,     7,     0,     9,    10,    11,     0,     0,    12,
       0,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   -74,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
      23,     0,   -42,    24,     0,    25,    26,     1,     0,     2,
       3,   -42,   -42,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     5,     6,     7,     0,     9,    10,    11,     0,
       0,    12,     0,     0,    13,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,  -109,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,    23,     0,   -42,    24,     0,    25,    26,     1,
       0,     2,     3,   -42,   -42,     0,     0,     0,     0,     0,
       0,     0,     0,     4,     5,     6,     7,     0,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,   -86,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,    23,     0,   -42,    24,     0,    25,
      26,     1,     0,     2,     3,   -42,   -42,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     5,     6,     7,     0,
       9,    10,    11,     0,     0,    12,     0,     0,    13,    14,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,    23,     0,   -42,    24,
       0,    25,    26,   262,     0,     2,     3,   -42,   -42,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,     6,
       7,     0,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,    16,    17,     0,     0,    18,    19,
      20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,    23,     0,
     -42,    24,     0,    25,    26,   286,     0,     2,     3,   -42,
     -42,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       5,     6,     7,     0,     9,    10,    11,     0,     0,    12,
       0,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
      23,     0,   -42,    24,     0,    25,    26,   300,     0,     2,
       3,   -42,   -42,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     5,     6,     7,     0,     9,    10,    11,     0,
       0,    12,     0,     0,    13,    14,     0,     0,     0,     0,
       0,    79,     0,     2,     3,    80,    81,    15,    16,    17,
       0,     0,    18,    19,    20,     4,     0,     6,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,    23,     0,   -42,    24,     0,    25,    26,    52,
       0,     2,     3,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     4,    66,     6,     2,     3,     0,     0,
       0,    21,    22,     0,     0,     0,    53,    82,     4,    24,
       6,    25,    26,    75,     0,     2,     3,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     0,     6,
       0,     0,    77,     0,     2,     3,     0,     0,     0,    21,
      22,     0,     0,     0,    53,     0,     4,    24,     6,    25,
      26,     0,     0,     0,    21,    22,     0,     0,     0,    53,
       0,     0,    24,     0,    25,    26,    87,     0,     2,     3,
       0,     0,     0,    21,    22,     0,     0,     0,    53,     0,
       4,    24,     6,    25,    26,    89,     0,     2,     3,     0,
       0,     0,    21,    22,     0,     0,     0,    53,     0,     4,
      24,     6,    25,    26,   171,     0,     2,     3,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,   173,
       6,     2,     3,     0,     0,     0,    21,    22,     0,     0,
       0,    53,     0,     4,    24,     6,    25,    26,   175,     0,
       2,     3,     0,     0,     0,    21,    22,     0,     0,     0,
      53,     0,     4,    24,     6,    25,    26,   177,     0,     2,
       3,     0,     0,     0,    21,    22,     0,     0,     0,    53,
       0,     4,    24,     6,    25,    26,     0,     0,     0,    21,
      22,     0,     0,     0,    53,     0,     0,    24,     0,    25,
      26,   179,     0,     2,     3,     0,     0,     0,    21,    22,
       0,     0,     0,    53,     0,     4,    24,     6,    25,    26,
     181,     0,     2,     3,     0,     0,     0,    21,    22,     0,
       0,     0,    53,     0,     4,    24,     6,    25,    26,   183,
       0,     2,     3,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     4,   185,     6,     2,     3,     0,     0,
       0,    21,    22,     0,     0,     0,    53,     0,     4,    24,
       6,    25,    26,   187,     0,     2,     3,     0,     0,     0,
      21,    22,     0,     0,     0,    53,     0,     4,    24,     6,
      25,    26,   189,     0,     2,     3,     0,     0,     0,    21,
      22,     0,     0,     0,    53,     0,     4,    24,     6,    25,
      26,     0,     0,     0,    21,    22,     0,     0,     0,    53,
       0,     0,    24,     0,    25,    26,   191,     0,     2,     3,
       0,     0,     0,    21,    22,     0,     0,     0,    53,     0,
       4,    24,     6,    25,    26,   193,     0,     2,     3,     0,
       0,     0,    21,    22,     0,     0,     0,    53,     0,     4,
      24,     6,    25,    26,   195,     0,     2,     3,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,   197,
       6,     2,     3,     0,     0,     0,    21,    22,     0,     0,
       0,    53,     0,     4,    24,     6,    25,    26,   199,     0,
       2,     3,     0,     0,     0,    21,    22,     0,     0,     0,
      53,     0,     4,    24,     6,    25,    26,   201,     0,     2,
       3,     0,     0,     0,    21,    22,     0,     0,     0,    53,
       0,     4,    24,     6,    25,    26,     0,     0,     0,    21,
      22,     0,     0,     0,    53,     0,     0,    24,     0,    25,
      26,   203,     0,     2,     3,     0,     0,     0,    21,    22,
       0,     0,     0,    53,     0,     4,    24,     6,    25,    26,
     205,     0,     2,     3,     0,     0,     0,    21,    22,     0,
       0,     0,    53,     0,     4,    24,     6,    25,    26,   207,
       0,     2,     3,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     4,   215,     6,     2,     3,     0,     0,
       0,    21,    22,     0,     0,     0,    53,     0,     4,    24,
       6,    25,    26,   218,     0,     2,     3,     0,     0,     0,
      21,    22,     0,     0,     0,    53,     0,     4,    24,     6,
      25,    26,   233,     0,     2,     3,     0,     0,     0,    21,
      22,     0,     0,     0,    53,     0,     4,    24,     6,    25,
      26,     0,     0,     0,    21,    22,     0,     0,     0,    53,
       0,     0,    24,     0,    25,    26,   274,     0,     2,     3,
       0,     0,     0,    21,    22,     0,     0,     0,    53,     0,
       4,    24,     6,    25,    26,   298,     0,     2,     3,     0,
       0,     0,    21,    22,     0,     0,     0,    53,     0,     4,
      24,     6,    25,    26,     2,     3,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     4,     0,     6,     0,
       0,     0,     0,     0,     0,     0,    21,    22,     0,     0,
       0,    53,     0,     0,    24,     0,    25,    26,     0,   105,
     106,   107,   108,   109,     0,    21,    22,     2,     3,     0,
      53,     0,     0,    24,     0,    25,    26,     0,     0,     4,
       0,     6,    21,    22,     0,     0,     0,    53,     0,   166,
      24,     0,    25,    26,   283,   102,   103,   104,   105,   106,
     107,   108,   109,     0,   116,   117,   118,   119,   120,     0,
       0,   291,   121,   122,     0,     0,     0,   102,   103,   104,
     105,   106,   107,   108,   109,    21,    22,     0,     0,     0,
      53,   246,     0,    24,     0,    25,    26,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,     0,     0,
       0,   121,   122,     0,     0,     0,     0,     0,   167,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
       2,     3,     0,   121,   122,     0,     0,     0,     0,     0,
     292,     0,     4,     0,     6,     2,     3,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     0,     6,
       2,     3,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     4,     0,     6,     0,   209,     0,     0,     0,
       0,     0,     0,     0,     0,   210,     0,     0,    21,    22,
       0,     0,     0,    53,     0,     0,    24,     0,    25,    26,
       0,     0,     0,    21,    22,     0,     0,     0,    53,   282,
       0,    24,     0,    25,    26,     0,     0,     0,    21,    22,
       0,     0,     0,    53,     0,     0,    24,     0,    25,    26,
     153,   154,   102,   103,   104,   105,   106,   107,   108,   109,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   102,   103,   104,   105,   106,   107,   108,   109,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,     0,     0,     0,   121,   122,
       0,     0,     0,   155,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,     0,     0,     0,   121,   122,
     102,   103,   104,   105,   106,   107,   108,   109,     0,     0,
       0,     0,     0,     0,   102,   103,   104,   105,   106,   107,
     108,   109,     0,     0,     0,     0,     0,   102,   103,   104,
     105,   106,   107,   108,   109,     0,     0,     0,     0,     0,
       0,     0,     0,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,     0,     0,     0,   121,   122,   112,   113,
     114,   115,   116,   117,   118,   119,   120,     0,     0,     0,
     121,   122,   113,   114,   115,   116,   117,   118,   119,   120,
       0,     0,     0,   121,   122
};

static const short yycheck[] =
{
       0,   136,   125,    15,    26,     7,     1,    26,     3,    11,
      12,     5,     6,     3,    14,     1,    53,     5,     6,    21,
      22,    23,    24,    25,    26,    24,     1,    14,     3,    33,
      30,     1,     1,     3,     0,     5,     6,     1,    33,     3,
      14,    19,    31,    32,     1,    34,     3,    33,    85,    65,
      15,    53,     5,     6,     3,    55,    51,    39,    58,     5,
       6,    51,    84,     1,    68,    65,    35,    67,     1,    69,
       3,    93,     5,     6,    68,    94,     1,    64,     1,    73,
      68,    55,    68,    85,    58,    73,    30,    51,     1,     1,
       3,    65,    94,    67,    69,    23,   231,     1,    68,    68,
     102,   103,   104,   105,   106,   107,   108,   129,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
      73,    33,   124,   125,   126,    69,    26,    73,   130,    67,
      68,    33,    45,    65,   134,    68,     5,     6,   138,    40,
      65,   163,    65,   162,     5,     6,   148,   147,   170,    27,
     169,   151,   152,    66,     1,    65,    68,   157,     5,     6,
     162,    65,     1,   165,     5,     6,    68,   169,     1,    40,
       3,     4,    37,   147,   297,    29,   229,     0,     5,     6,
       5,     6,    15,   157,    17,    32,     5,     6,    31,    32,
     134,    34,     0,     1,   138,     3,    19,    18,    67,     1,
       0,     3,     4,     5,     6,   241,    67,   151,   152,   130,
     132,    19,     1,    15,     3,    17,    49,   259,    94,    19,
     220,    68,    65,    -1,   224,    58,    67,    68,    61,    62,
     249,    -1,   232,    66,   236,   254,    69,   239,    71,    72,
      67,    68,    67,   243,    33,    -1,   220,   249,    -1,    68,
     224,     1,   254,     3,    13,    14,     0,   259,    -1,    61,
      62,    -1,    51,   263,    66,   265,   266,    69,    -1,    71,
      72,    73,   272,    -1,    -1,    19,   276,   277,     1,     1,
       3,     3,   284,     1,     1,     3,     3,   287,   232,   289,
     290,    -1,   266,   293,   294,   297,    -1,    -1,   272,   243,
      -1,   301,   276,   277,    63,    64,     1,    -1,     3,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   290,    -1,    -1,   263,
      -1,   265,    -1,     3,     4,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    17,    -1,    -1,
      -1,    -1,    -1,   287,    -1,   289,    -1,    -1,    -1,   293,
     294,    -1,    -1,    -1,    -1,     0,     1,   301,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      15,    16,    17,    18,    19,    20,    21,    22,    -1,    -1,
      25,    61,    62,    28,    29,    -1,    66,    -1,    -1,    69,
      -1,    71,    72,    73,    -1,    -1,    41,    42,    43,    -1,
      -1,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    10,    11,    12,    13,    14,    61,    62,    -1,    -1,
      -1,    66,    -1,    68,    69,    -1,    71,    72,     0,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,    16,    17,    18,    19,    20,    21,
      22,    -1,    -1,    25,    -1,    -1,    28,    29,    57,    58,
      59,    -1,    -1,    -1,    63,    64,    -1,    -1,    -1,    41,
      42,    43,    -1,    -1,    46,    47,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      62,    -1,    -1,    -1,    66,    -1,    68,    69,    -1,    71,
      72,     0,     1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,
      19,    20,    21,    22,    -1,    -1,    25,    -1,    -1,    28,
      29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    41,    42,    43,    -1,    -1,    46,    47,    48,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    61,    62,    -1,    -1,    -1,    66,    -1,    68,
      69,    -1,    71,    72,     0,     1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      16,    17,    18,    19,    20,    21,    22,    -1,    -1,    25,
      -1,    -1,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,
      46,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,
      66,    -1,    68,    69,    -1,    71,    72,     0,     1,    -1,
       3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    15,    16,    17,    18,    19,    20,    21,    22,
      -1,    -1,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,
      43,    -1,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,
      -1,    -1,    -1,    66,    -1,    68,    69,    -1,    71,    72,
       0,     1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    19,
      20,    21,    22,    -1,    -1,    25,    -1,    -1,    28,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    42,    43,    -1,    -1,    46,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    62,    -1,    -1,    -1,    66,    -1,    68,    69,
      -1,    71,    72,     0,     1,    -1,     3,     4,     5,     6,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    16,
      17,    18,    19,    20,    21,    22,    -1,    -1,    25,    -1,
      -1,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,    46,
      47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,
      -1,    68,    69,    -1,    71,    72,     0,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    17,    18,    -1,    20,    21,    22,    -1,
      -1,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,
      -1,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,    -1,
      -1,    -1,    66,    -1,    68,    69,    -1,    71,    72,     1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    -1,    20,    21,
      22,    -1,    -1,    25,    -1,    -1,    28,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,
      42,    43,    -1,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    61,
      62,    63,    64,    -1,    66,    -1,    68,    69,    -1,    71,
      72,     1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    -1,
      20,    21,    22,    23,    24,    25,    -1,    -1,    28,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      40,    41,    42,    43,    -1,    -1,    46,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    62,    -1,    -1,    -1,    66,    -1,    68,    69,
      -1,    71,    72,     1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,
      18,    -1,    20,    21,    22,    -1,    -1,    25,    26,    27,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,
      -1,    -1,    -1,    41,    42,    43,    -1,    -1,    46,    47,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,    -1,
      68,    69,    -1,    71,    72,     1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      16,    17,    18,    -1,    20,    21,    22,    -1,    -1,    25,
      -1,    -1,    28,    29,    -1,    -1,    32,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,
      46,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    62,    -1,    -1,    65,
      66,    -1,    68,    69,    -1,    71,    72,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    17,    18,    -1,    20,    21,    22,    -1,
      -1,    25,    -1,    -1,    28,    29,    30,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    39,    -1,    41,    42,    43,
      -1,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,    -1,
      -1,    -1,    66,    -1,    68,    69,    -1,    71,    72,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,    16,    17,    18,    -1,    20,    21,
      22,    -1,    -1,    25,    -1,    -1,    28,    29,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,
      42,    43,    -1,    -1,    46,    47,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      62,    -1,    -1,    -1,    66,    -1,    68,    69,    -1,    71,
      72,     1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    -1,
      20,    21,    22,    -1,    -1,    25,    -1,    -1,    28,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      -1,    41,    42,    43,    -1,    -1,    46,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    62,    -1,    -1,    -1,    66,    -1,    68,    69,
      -1,    71,    72,     1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,
      18,    -1,    20,    21,    22,    -1,    -1,    25,    -1,    -1,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    -1,    41,    42,    43,    -1,    -1,    46,    47,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,    -1,
      68,    69,    -1,    71,    72,     1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      16,    17,    18,    -1,    20,    21,    22,    -1,    -1,    25,
      -1,    -1,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    39,    -1,    41,    42,    43,    -1,    -1,
      46,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,
      66,    -1,    68,    69,    -1,    71,    72,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    17,    18,    -1,    20,    21,    22,    -1,
      -1,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    40,    41,    42,    43,
      -1,    -1,    46,    47,    48,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,    -1,
      -1,    -1,    66,    -1,    68,    69,    -1,    71,    72,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,    16,    17,    18,    -1,    20,    21,
      22,    -1,    -1,    25,    -1,    -1,    28,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    37,    -1,    -1,    -1,    41,
      42,    43,    -1,    -1,    46,    47,    48,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      62,    -1,    -1,    -1,    66,    -1,    68,    69,    -1,    71,
      72,     1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    -1,
      20,    21,    22,    -1,    -1,    25,    -1,    -1,    28,    29,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    41,    42,    43,    -1,    -1,    46,    47,    48,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    62,    -1,    -1,    -1,    66,    -1,    68,    69,
      -1,    71,    72,     1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,
      18,    -1,    20,    21,    22,    -1,    -1,    25,    -1,    -1,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    41,    42,    43,    -1,    -1,    46,    47,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,    -1,
      68,    69,    -1,    71,    72,     1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      16,    17,    18,    -1,    20,    21,    22,    -1,    -1,    25,
      -1,    -1,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,
      46,    47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,
      66,    -1,    68,    69,    -1,    71,    72,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    17,    18,    -1,    20,    21,    22,    -1,
      -1,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,    -1,
      -1,     1,    -1,     3,     4,     5,     6,    41,    42,    43,
      -1,    -1,    46,    47,    48,    15,    -1,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    62,    -1,
      -1,    -1,    66,    -1,    68,    69,    -1,    71,    72,     1,
      -1,     3,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,     1,    17,     3,     4,    -1,    -1,
      -1,    61,    62,    -1,    -1,    -1,    66,    67,    15,    69,
      17,    71,    72,     1,    -1,     3,     4,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    17,
      -1,    -1,     1,    -1,     3,     4,    -1,    -1,    -1,    61,
      62,    -1,    -1,    -1,    66,    -1,    15,    69,    17,    71,
      72,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,
      -1,    -1,    69,    -1,    71,    72,     1,    -1,     3,     4,
      -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,    -1,
      15,    69,    17,    71,    72,     1,    -1,     3,     4,    -1,
      -1,    -1,    61,    62,    -1,    -1,    -1,    66,    -1,    15,
      69,    17,    71,    72,     1,    -1,     3,     4,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,     1,
      17,     3,     4,    -1,    -1,    -1,    61,    62,    -1,    -1,
      -1,    66,    -1,    15,    69,    17,    71,    72,     1,    -1,
       3,     4,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,
      66,    -1,    15,    69,    17,    71,    72,     1,    -1,     3,
       4,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,
      -1,    15,    69,    17,    71,    72,    -1,    -1,    -1,    61,
      62,    -1,    -1,    -1,    66,    -1,    -1,    69,    -1,    71,
      72,     1,    -1,     3,     4,    -1,    -1,    -1,    61,    62,
      -1,    -1,    -1,    66,    -1,    15,    69,    17,    71,    72,
       1,    -1,     3,     4,    -1,    -1,    -1,    61,    62,    -1,
      -1,    -1,    66,    -1,    15,    69,    17,    71,    72,     1,
      -1,     3,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,     1,    17,     3,     4,    -1,    -1,
      -1,    61,    62,    -1,    -1,    -1,    66,    -1,    15,    69,
      17,    71,    72,     1,    -1,     3,     4,    -1,    -1,    -1,
      61,    62,    -1,    -1,    -1,    66,    -1,    15,    69,    17,
      71,    72,     1,    -1,     3,     4,    -1,    -1,    -1,    61,
      62,    -1,    -1,    -1,    66,    -1,    15,    69,    17,    71,
      72,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,
      -1,    -1,    69,    -1,    71,    72,     1,    -1,     3,     4,
      -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,    -1,
      15,    69,    17,    71,    72,     1,    -1,     3,     4,    -1,
      -1,    -1,    61,    62,    -1,    -1,    -1,    66,    -1,    15,
      69,    17,    71,    72,     1,    -1,     3,     4,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,     1,
      17,     3,     4,    -1,    -1,    -1,    61,    62,    -1,    -1,
      -1,    66,    -1,    15,    69,    17,    71,    72,     1,    -1,
       3,     4,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,
      66,    -1,    15,    69,    17,    71,    72,     1,    -1,     3,
       4,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,
      -1,    15,    69,    17,    71,    72,    -1,    -1,    -1,    61,
      62,    -1,    -1,    -1,    66,    -1,    -1,    69,    -1,    71,
      72,     1,    -1,     3,     4,    -1,    -1,    -1,    61,    62,
      -1,    -1,    -1,    66,    -1,    15,    69,    17,    71,    72,
       1,    -1,     3,     4,    -1,    -1,    -1,    61,    62,    -1,
      -1,    -1,    66,    -1,    15,    69,    17,    71,    72,     1,
      -1,     3,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,     1,    17,     3,     4,    -1,    -1,
      -1,    61,    62,    -1,    -1,    -1,    66,    -1,    15,    69,
      17,    71,    72,     1,    -1,     3,     4,    -1,    -1,    -1,
      61,    62,    -1,    -1,    -1,    66,    -1,    15,    69,    17,
      71,    72,     1,    -1,     3,     4,    -1,    -1,    -1,    61,
      62,    -1,    -1,    -1,    66,    -1,    15,    69,    17,    71,
      72,    -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,
      -1,    -1,    69,    -1,    71,    72,     1,    -1,     3,     4,
      -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,    -1,
      15,    69,    17,    71,    72,     1,    -1,     3,     4,    -1,
      -1,    -1,    61,    62,    -1,    -1,    -1,    66,    -1,    15,
      69,    17,    71,    72,     3,     4,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    62,    -1,    -1,
      -1,    66,    -1,    -1,    69,    -1,    71,    72,    -1,    10,
      11,    12,    13,    14,    -1,    61,    62,     3,     4,    -1,
      66,    -1,    -1,    69,    -1,    71,    72,    -1,    -1,    15,
      -1,    17,    61,    62,    -1,    -1,    -1,    66,    -1,     1,
      69,    -1,    71,    72,    73,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    55,    56,    57,    58,    59,    -1,
      -1,     1,    63,    64,    -1,    -1,    -1,     7,     8,     9,
      10,    11,    12,    13,    14,    61,    62,    -1,    -1,    -1,
      66,    67,    -1,    69,    -1,    71,    72,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
      -1,    63,    64,    -1,    -1,    -1,    -1,    -1,    70,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
       3,     4,    -1,    63,    64,    -1,    -1,    -1,    -1,    -1,
      70,    -1,    15,    -1,    17,     3,     4,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    17,
       3,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    15,    -1,    17,    -1,    49,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    58,    -1,    -1,    61,    62,
      -1,    -1,    -1,    66,    -1,    -1,    69,    -1,    71,    72,
      -1,    -1,    -1,    61,    62,    -1,    -1,    -1,    66,    67,
      -1,    69,    -1,    71,    72,    -1,    -1,    -1,    61,    62,
      -1,    -1,    -1,    66,    -1,    -1,    69,    -1,    71,    72,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    -1,    63,    64,
      -1,    -1,    -1,    68,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    -1,    -1,    63,    64,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    -1,    -1,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    -1,    -1,    63,    64,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    -1,    -1,    -1,
      63,    64,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    -1,    63,    64
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,    15,    16,    17,    18,    19,    20,
      21,    22,    25,    28,    29,    41,    42,    43,    46,    47,
      48,    61,    62,    66,    69,    71,    72,    75,    76,    77,
      82,    83,    84,    85,    87,    88,    90,    91,    92,    93,
      94,    96,   102,   104,   105,   111,   112,   113,   114,   115,
      15,    86,     1,    66,   106,   113,   115,     1,     3,    45,
      66,    78,     1,     3,    69,   103,     1,   113,   113,    82,
       1,     3,    89,     1,    89,     1,   113,     1,   113,     1,
       5,     6,    67,   113,   120,   121,   123,     1,   113,     1,
     113,     1,    73,   119,   121,   123,     0,    76,    83,     5,
       6,    68,     7,     8,     9,    10,    11,    12,    13,    14,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    63,    64,    31,    32,    34,    65,   116,    15,   120,
      24,   107,   108,   109,    82,     1,    32,    65,    82,     1,
      65,     1,     3,    51,    79,    80,     1,     3,    65,     1,
       3,    82,    82,     5,     6,    68,    97,    30,    95,     1,
       3,    67,   121,   120,    68,   122,     1,    70,    73,   121,
     119,     1,   113,     1,   113,     1,   113,     1,   113,     1,
     113,     1,   113,     1,   113,     1,   113,     1,   113,     1,
     113,     1,   113,     1,   113,     1,   113,     1,   113,     1,
     113,     1,   113,     1,   113,     1,   113,     1,   113,    49,
      58,   113,   117,   118,   118,     1,   113,    67,     1,   106,
      23,   110,   109,     1,    33,    79,     3,     1,    67,    68,
       1,    32,    82,     1,   113,     1,    65,    36,    38,    26,
      98,    99,   100,    82,    39,    65,    67,   123,    67,   121,
     113,    73,   123,    73,   121,     1,     3,     1,    33,    68,
       1,    35,     1,    82,    40,    82,    33,    81,     1,    65,
      80,     1,    33,    79,     1,   113,   113,    27,   101,   100,
       1,     3,    67,    73,    65,   117,     1,    82,     1,    82,
      33,     1,    70,    82,    82,    37,     1,    32,     1,   113,
       1,    82,     1,   118,     1,    33
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
#define YYERROR		goto yyerrlab1


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
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
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
| TOP (cinluded).                                                   |
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
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
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

#if YYMAXDEPTH == 0
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
#line 136 "../../../libs/libFreeMat/Parser.yy"
    {mainAST = yyvsp[0];}
    break;

  case 5:
#line 141 "../../../libs/libFreeMat/Parser.yy"
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
#line 157 "../../../libs/libFreeMat/Parser.yy"
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
#line 172 "../../../libs/libFreeMat/Parser.yy"
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
#line 187 "../../../libs/libFreeMat/Parser.yy"
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
#line 201 "../../../libs/libFreeMat/Parser.yy"
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
#line 216 "../../../libs/libFreeMat/Parser.yy"
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
#line 230 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("function name or return declaration");}
    break;

  case 12:
#line 231 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("argument list or statement list");}
    break;

  case 13:
#line 232 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("(possibly empty) argument list");}
    break;

  case 14:
#line 233 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("statement list");}
    break;

  case 15:
#line 234 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("function name");}
    break;

  case 16:
#line 235 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("argument list or statement list");}
    break;

  case 17:
#line 236 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("(possibly empty) argument list");}
    break;

  case 18:
#line 237 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("statement list");}
    break;

  case 21:
#line 246 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 22:
#line 247 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 23:
#line 248 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2];}
    break;

  case 24:
#line 249 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("either 'varargout', a single returned variable, or a list of return variables in return declaration");}
    break;

  case 25:
#line 250 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an '=' symbol after 'varargout' in return declaration");}
    break;

  case 26:
#line 251 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an '=' symbol after identifier in return declaration");}
    break;

  case 27:
#line 252 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("a valid list of return arguments in return declaration");}
    break;

  case 28:
#line 253 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("matching ']' in return declaration");}
    break;

  case 29:
#line 254 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an '=' symbol after return declaration");}
    break;

  case 30:
#line 258 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[0];}
    break;

  case 31:
#line 259 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 33:
#line 263 "../../../libs/libFreeMat/Parser.yy"
    {
	yyval = yyvsp[0];
	char *b = (char*) malloc(strlen(yyvsp[0]->text)+2);
	b[0] = '&';
	strcpy(b+1,yyvsp[0]->text);
	yyval->text = b;
  }
    break;

  case 34:
#line 270 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("either an identifier or an ampersand '&' (indicating pass by reference) followed by an identifier in argument list");}
    break;

  case 35:
#line 275 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_BLOCK,yyvsp[0]);}
    break;

  case 36:
#line 276 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 37:
#line 280 "../../../libs/libFreeMat/Parser.yy"
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

  case 38:
#line 289 "../../../libs/libFreeMat/Parser.yy"
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

  case 39:
#line 298 "../../../libs/libFreeMat/Parser.yy"
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

  case 42:
#line 312 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(null_node,"");}
    break;

  case 58:
#line 327 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("recognizable statement type (assignment, expression, function call, etc...)");}
    break;

  case 59:
#line 331 "../../../libs/libFreeMat/Parser.yy"
    {yyvsp[-1]->addChild(yyvsp[0]); yyval = new AST(OP_SCALL,yyvsp[-1]);}
    break;

  case 60:
#line 335 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_PARENS,yyvsp[0]);}
    break;

  case 61:
#line 336 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 62:
#line 340 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]); }
    break;

  case 63:
#line 341 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("list of variables to be tagged as persistent");}
    break;

  case 64:
#line 345 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 65:
#line 346 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("list of variables to be tagged as global");}
    break;

  case 67:
#line 351 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 68:
#line 352 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("list of valid identifiers");}
    break;

  case 73:
#line 372 "../../../libs/libFreeMat/Parser.yy"
    { yyval = yyvsp[-3]; yyval->addChild(yyvsp[-2]); if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);}
    break;

  case 74:
#line 376 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[0];}
    break;

  case 75:
#line 377 "../../../libs/libFreeMat/Parser.yy"
    {yyval = NULL;}
    break;

  case 76:
#line 381 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-5]; yyval->addChild(yyvsp[-4]); 
	  if (yyvsp[-2] != NULL) yyval->addChild(yyvsp[-2]); 
	  if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);
	}
    break;

  case 81:
#line 393 "../../../libs/libFreeMat/Parser.yy"
    {yyval = NULL;}
    break;

  case 83:
#line 398 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = new AST(OP_CASEBLOCK,yyvsp[0]);
	}
    break;

  case 84:
#line 401 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);
	}
    break;

  case 85:
#line 407 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-2]; yyval->addChild(yyvsp[-1]); yyval->addChild(yyvsp[0]);
	}
    break;

  case 86:
#line 413 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 87:
#line 416 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = NULL;
	}
    break;

  case 88:
#line 422 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-3]; yyval->addChild(yyvsp[-2]); yyval->addChild(yyvsp[-1]);
	}
    break;

  case 89:
#line 428 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-3]; yyval->addChild(yyvsp[-1]);}
    break;

  case 90:
#line 429 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 91:
#line 430 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[0]; yyval->addChild(new AST(OP_RHS, new AST(id_node,yyvsp[0]->text))); }
    break;

  case 92:
#line 431 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("matching right parenthesis");}
    break;

  case 93:
#line 432 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("indexing expression");}
    break;

  case 94:
#line 433 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("equals operator after loop index");}
    break;

  case 95:
#line 434 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("identifier that is the loop variable");}
    break;

  case 96:
#line 435 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("indexing expression");}
    break;

  case 97:
#line 436 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("identifier or assignment (id = expr) after 'for' ");}
    break;

  case 98:
#line 440 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-3]; yyval->addChild(yyvsp[-2]); yyval->addChild(yyvsp[-1]);
	}
    break;

  case 99:
#line 443 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("test expression after 'while'");}
    break;

  case 100:
#line 447 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-4]; yyval->addChild(yyvsp[-3]); if (yyvsp[-2] != NULL) yyval->addChild(yyvsp[-2]); 
	  if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);
	}
    break;

  case 101:
#line 451 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("condition expression for 'if'");}
    break;

  case 102:
#line 455 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = new AST(OP_CSTAT,yyvsp[-1],yyvsp[0]);
	}
    break;

  case 103:
#line 461 "../../../libs/libFreeMat/Parser.yy"
    {yyval = NULL;}
    break;

  case 105:
#line 466 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = new AST(OP_ELSEIFBLOCK,yyvsp[0]);
	}
    break;

  case 106:
#line 469 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);
	}
    break;

  case 107:
#line 475 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 108:
#line 478 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("test condition for 'elseif' clause");}
    break;

  case 109:
#line 481 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 110:
#line 484 "../../../libs/libFreeMat/Parser.yy"
    {yyval = NULL;}
    break;

  case 111:
#line 485 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("statement list for 'else' clause");}
    break;

  case 112:
#line 489 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_ASSIGN,yyvsp[-2],yyvsp[0]);}
    break;

  case 113:
#line 490 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("expression in assignment");}
    break;

  case 114:
#line 494 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyvsp[-3]->addChild(new AST(OP_PARENS,yyvsp[-1]));
	  yyval = new AST(OP_MULTICALL,yyvsp[-6],yyvsp[-3]);
	}
    break;

  case 115:
#line 498 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyvsp[0]->addChild(new AST(OP_PARENS,NULL));
	  yyval = new AST(OP_MULTICALL,yyvsp[-3],yyvsp[0]);
	}
    break;

  case 116:
#line 503 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("matching right parenthesis");}
    break;

  case 117:
#line 505 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("indexing list");}
    break;

  case 118:
#line 507 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("left parenthesis");}
    break;

  case 119:
#line 509 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("identifier");}
    break;

  case 120:
#line 513 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_COLON,yyvsp[-2],yyvsp[0]);}
    break;

  case 121:
#line 514 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after ':'");}
    break;

  case 123:
#line 516 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_PLUS,yyvsp[-2],yyvsp[0]);}
    break;

  case 124:
#line 517 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '+'");}
    break;

  case 125:
#line 518 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_SUBTRACT,yyvsp[-2],yyvsp[0]);}
    break;

  case 126:
#line 519 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '-'");}
    break;

  case 127:
#line 520 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_TIMES,yyvsp[-2],yyvsp[0]);}
    break;

  case 128:
#line 521 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '*'");}
    break;

  case 129:
#line 522 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_RDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 130:
#line 523 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '/'");}
    break;

  case 131:
#line 524 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_LDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 132:
#line 525 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '\\'");}
    break;

  case 133:
#line 526 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_OR,yyvsp[-2],yyvsp[0]);}
    break;

  case 134:
#line 527 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '|'");}
    break;

  case 135:
#line 528 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_AND,yyvsp[-2],yyvsp[0]);}
    break;

  case 136:
#line 529 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '&'");}
    break;

  case 137:
#line 530 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_LT,yyvsp[-2],yyvsp[0]);}
    break;

  case 138:
#line 531 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '<'");}
    break;

  case 139:
#line 532 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_LEQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 140:
#line 533 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '<='");}
    break;

  case 141:
#line 534 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_GT,yyvsp[-2],yyvsp[0]);}
    break;

  case 142:
#line 535 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '>'");}
    break;

  case 143:
#line 536 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_GEQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 144:
#line 537 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '>='");}
    break;

  case 145:
#line 538 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_EQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 146:
#line 539 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '=='");}
    break;

  case 147:
#line 540 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_NEQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 148:
#line 541 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '~='");}
    break;

  case 149:
#line 542 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT_TIMES,yyvsp[-2],yyvsp[0]);}
    break;

  case 150:
#line 543 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '.*'");}
    break;

  case 151:
#line 544 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT_RDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 152:
#line 545 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after './'");}
    break;

  case 153:
#line 546 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT_LDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 154:
#line 547 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '.\\'");}
    break;

  case 155:
#line 548 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_NEG,yyvsp[0]);}
    break;

  case 156:
#line 549 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after negation");}
    break;

  case 157:
#line 550 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[0];}
    break;

  case 158:
#line 551 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after positive sign");}
    break;

  case 159:
#line 552 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_NOT,yyvsp[0]);}
    break;

  case 160:
#line 553 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after logical not");}
    break;

  case 161:
#line 554 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_POWER,yyvsp[-2],yyvsp[0]);}
    break;

  case 162:
#line 555 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '^'");}
    break;

  case 163:
#line 556 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT_POWER,yyvsp[-2],yyvsp[0]);}
    break;

  case 164:
#line 557 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '.^'");}
    break;

  case 165:
#line 558 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_TRANSPOSE,yyvsp[-1]);}
    break;

  case 166:
#line 559 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT_TRANSPOSE,yyvsp[-1]);}
    break;

  case 167:
#line 560 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 168:
#line 561 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("a right parenthesis after expression");}
    break;

  case 169:
#line 562 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after left parenthesis");}
    break;

  case 173:
#line 569 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_RHS,yyvsp[0]);}
    break;

  case 174:
#line 570 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 175:
#line 571 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("a matrix definition followed by a right bracket");}
    break;

  case 176:
#line 572 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 177:
#line 573 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2];}
    break;

  case 178:
#line 574 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2];}
    break;

  case 179:
#line 575 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_EMPTY,NULL);}
    break;

  case 180:
#line 576 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 181:
#line 577 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 182:
#line 578 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2];}
    break;

  case 183:
#line 579 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2];}
    break;

  case 184:
#line 580 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_EMPTY_CELL,NULL);}
    break;

  case 185:
#line 581 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("a cell-array definition followed by a right brace");}
    break;

  case 187:
#line 586 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 188:
#line 590 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_PARENS,yyvsp[-1]); }
    break;

  case 189:
#line 591 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("matching right parenthesis");}
    break;

  case 190:
#line 592 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_BRACES,yyvsp[-1]); }
    break;

  case 191:
#line 593 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("matching right brace");}
    break;

  case 192:
#line 594 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT,yyvsp[0]); }
    break;

  case 194:
#line 599 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_ALL,NULL);}
    break;

  case 195:
#line 600 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_KEYWORD,yyvsp[-2],yyvsp[0]);}
    break;

  case 196:
#line 601 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("expecting expression after '=' in keyword assignment");}
    break;

  case 197:
#line 602 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_KEYWORD,yyvsp[0]);}
    break;

  case 198:
#line 603 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("expecting keyword identifier after '/' in keyword assignment");}
    break;

  case 200:
#line 608 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addPeer(yyvsp[0]);}
    break;

  case 201:
#line 612 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_BRACES,yyvsp[0]);}
    break;

  case 202:
#line 613 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 203:
#line 617 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_BRACKETS,yyvsp[0]);}
    break;

  case 204:
#line 618 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 209:
#line 630 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_SEMICOLON,yyvsp[0]);}
    break;

  case 210:
#line 631 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;


    }

/* Line 999 of yacc.c.  */
#line 3032 "../../../libs/libFreeMat/Parser.cc"

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
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
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

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
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
      yyvsp--;
      yystate = *--yyssp;

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


#line 634 "../../../libs/libFreeMat/Parser.yy"


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

