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
     MAGICEND = 277,
     WHILE = 278,
     ELSE = 279,
     ELSEIF = 280,
     SWITCH = 281,
     CASE = 282,
     OTHERWISE = 283,
     CONTINUE = 284,
     TRY = 285,
     CATCH = 286,
     FIELD = 287,
     REFLPAREN = 288,
     REFRPAREN = 289,
     PERSISTENT = 290,
     KEYBOARD = 291,
     RETURN = 292,
     VARARGIN = 293,
     VARARGOUT = 294,
     GLOBAL = 295,
     QUIT = 296,
     RETALL = 297,
     NE = 298,
     NOT = 299,
     NEG = 300,
     POS = 301
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
#define MAGICEND 277
#define WHILE 278
#define ELSE 279
#define ELSEIF 280
#define SWITCH 281
#define CASE 282
#define OTHERWISE 283
#define CONTINUE 284
#define TRY 285
#define CATCH 286
#define FIELD 287
#define REFLPAREN 288
#define REFRPAREN 289
#define PERSISTENT 290
#define KEYBOARD 291
#define RETURN 292
#define VARARGIN 293
#define VARARGOUT 294
#define GLOBAL 295
#define QUIT 296
#define RETALL 297
#define NE 298
#define NOT 299
#define NEG 300
#define POS 301




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

//extern char* yytext;
extern int yylex(void);

extern int lineNumber;
extern int yydebug;
extern const char *parsing_filename;

bool interactiveMode;

#ifdef WIN32
#define snprintf _snprintf
#endif

namespace FreeMat {
  static ASTPtr mainAST;
  static MFunctionDef *mainMDef;
  
  bool errorOccured;
  static char *expectString = NULL;

  void yyexpect(char *s) {
	expectString = s;
  }

  void yyerror(char *t) {
    t = NULL;
  }

  int yyreport(char *xStr) {
    char buffer[256];
    int addone = 1;
    if (xStr)
      if (!interactiveMode)
        snprintf(msgBuffer,MSGBUFLEN,
	"Expecting %s\n\tat line %d of file %s",
	xStr,lineNumber+addone,parsing_filename);
      else
        snprintf(msgBuffer,MSGBUFLEN,"Expecting %s",xStr);
    else
      if (!interactiveMode)	
        snprintf(msgBuffer,MSGBUFLEN,
	"Syntax error at line %d of file %s",
	lineNumber+addone,parsing_filename);
      else
        snprintf(msgBuffer,MSGBUFLEN,"Syntax error at input");
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
#line 282 "../../../libs/libFreeMat/Parser.cxx"

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
#define YYLAST   2563

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  71
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  50
/* YYNRULES -- Number of rules. */
#define YYNRULES  211
/* YYNRULES -- Number of states. */
#define YYNSTATES  321

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   301

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    66,     2,     2,    45,    58,
      59,    60,    51,    49,    64,    50,    70,    52,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    43,    65,
      46,    61,    47,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    62,    53,    63,    57,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    68,    44,    69,    67,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    48,    54,
      55,    56
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
{
       0,     0,     3,     5,     7,     8,    17,    25,    31,    36,
      44,    51,    54,    58,    63,    70,    74,    79,    85,    93,
      95,    98,   101,   104,   109,   111,   114,   117,   120,   124,
     129,   131,   135,   137,   140,   142,   145,   148,   151,   154,
     156,   158,   159,   161,   163,   165,   167,   169,   171,   173,
     175,   177,   179,   181,   183,   185,   187,   189,   191,   194,
     196,   199,   202,   205,   208,   211,   213,   216,   219,   221,
     223,   225,   227,   232,   235,   236,   243,   245,   247,   249,
     251,   253,   255,   257,   258,   260,   262,   265,   270,   273,
     274,   280,   286,   290,   292,   298,   303,   307,   310,   314,
     316,   322,   325,   331,   334,   338,   339,   341,   343,   346,
     349,   352,   355,   356,   359,   363,   367,   376,   382,   391,
     399,   406,   412,   416,   420,   422,   426,   430,   434,   438,
     442,   446,   450,   454,   458,   462,   466,   470,   474,   478,
     482,   486,   490,   494,   498,   502,   506,   510,   514,   518,
     522,   526,   530,   534,   538,   542,   546,   550,   553,   556,
     559,   562,   566,   570,   574,   578,   581,   584,   588,   592,
     595,   597,   599,   601,   603,   607,   610,   615,   620,   626,
     629,   633,   638,   643,   649,   652,   655,   657,   660,   664,
     668,   672,   676,   679,   684,   686,   688,   693,   698,   701,
     704,   706,   710,   712,   716,   718,   722,   724,   726,   728,
     730,   732
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      72,     0,    -1,    78,    -1,    74,    -1,    -1,    19,    75,
       3,    59,    76,    60,    94,    78,    -1,    19,     3,    59,
      76,    60,    94,    78,    -1,    19,    75,     3,    94,    78,
      -1,    19,     3,    94,    78,    -1,    19,    75,     3,    59,
      60,    94,    78,    -1,    19,     3,    59,    60,    94,    78,
      -1,    19,     1,    -1,    19,     3,     1,    -1,    19,     3,
      59,     1,    -1,    19,     3,    59,    76,    60,     1,    -1,
      19,    75,     1,    -1,    19,    75,     3,     1,    -1,    19,
      75,     3,    59,     1,    -1,    19,    75,     3,    59,    76,
      60,     1,    -1,    73,    -1,    74,    73,    -1,    39,    61,
      -1,     3,    61,    -1,    62,    76,    63,    61,    -1,     1,
      -1,    39,     1,    -1,     3,     1,    -1,    62,     1,    -1,
      62,    76,     1,    -1,    62,    76,    63,     1,    -1,    77,
      -1,    76,    64,    77,    -1,     3,    -1,    45,     3,    -1,
      79,    -1,    78,    79,    -1,    80,     5,    -1,    80,     6,
      -1,    80,    64,    -1,   108,    -1,   110,    -1,    -1,   109,
      -1,    99,    -1,    89,    -1,    88,    -1,   101,    -1,   102,
      -1,    92,    -1,    90,    -1,    87,    -1,    86,    -1,    84,
      -1,    83,    -1,    81,    -1,    41,    -1,    42,    -1,     1,
      -1,    16,    82,    -1,    15,    -1,    82,    15,    -1,    35,
      85,    -1,    35,     1,    -1,    40,    85,    -1,    40,     1,
      -1,     3,    -1,    85,     3,    -1,    85,     1,    -1,    37,
      -1,    36,    -1,    29,    -1,    21,    -1,    30,    78,    91,
      17,    -1,    31,    78,    -1,    -1,    26,   110,    93,    95,
      98,    17,    -1,    64,    -1,     6,    -1,     5,    -1,    65,
      -1,    66,    -1,     6,    -1,     5,    -1,    -1,    96,    -1,
      97,    -1,    96,    97,    -1,    27,   110,    93,    78,    -1,
      28,    78,    -1,    -1,    20,   100,    93,    78,    17,    -1,
      59,     3,    61,   110,    60,    -1,     3,    61,   110,    -1,
       3,    -1,    59,     3,    61,   110,     1,    -1,    59,     3,
      61,     1,    -1,    59,     3,     1,    -1,    59,     1,    -1,
       3,    61,     1,    -1,     1,    -1,    23,   110,    93,    78,
      17,    -1,    23,     1,    -1,    18,   103,   104,   107,    17,
      -1,    18,     1,    -1,   110,    93,    78,    -1,    -1,   105,
      -1,   106,    -1,   105,   106,    -1,    25,   103,    -1,    25,
       1,    -1,    24,    78,    -1,    -1,    24,     1,    -1,   112,
      61,   110,    -1,   112,    61,     1,    -1,    62,   117,    63,
      61,     3,    59,   115,    60,    -1,    62,   117,    63,    61,
       3,    -1,    62,   117,    63,    61,     3,    59,   115,     1,
      -1,    62,   117,    63,    61,     3,    59,     1,    -1,    62,
     117,    63,    61,     3,     1,    -1,    62,   117,    63,    61,
       1,    -1,   110,    43,   110,    -1,   110,    43,     1,    -1,
     111,    -1,   110,    49,   110,    -1,   110,    49,     1,    -1,
     110,    50,   110,    -1,   110,    50,     1,    -1,   110,    51,
     110,    -1,   110,    51,     1,    -1,   110,    52,   110,    -1,
     110,    52,     1,    -1,   110,    53,   110,    -1,   110,    53,
       1,    -1,   110,    44,   110,    -1,   110,    44,     1,    -1,
     110,    45,   110,    -1,   110,    45,     1,    -1,   110,    46,
     110,    -1,   110,    46,     1,    -1,   110,     7,   110,    -1,
     110,     7,     1,    -1,   110,    47,   110,    -1,   110,    47,
       1,    -1,   110,     8,   110,    -1,   110,     8,     1,    -1,
     110,     9,   110,    -1,   110,     9,     1,    -1,   110,    48,
     110,    -1,   110,    48,     1,    -1,   110,    10,   110,    -1,
     110,    10,     1,    -1,   110,    11,   110,    -1,   110,    11,
       1,    -1,   110,    12,   110,    -1,   110,    12,     1,    -1,
      50,   110,    -1,    49,   110,    -1,    67,   110,    -1,    67,
       1,    -1,   110,    57,   110,    -1,   110,    57,     1,    -1,
     110,    13,   110,    -1,   110,    13,     1,    -1,   110,    58,
      -1,   110,    14,    -1,    59,   110,    60,    -1,    59,   110,
       1,    -1,    59,     1,    -1,     4,    -1,    15,    -1,    22,
      -1,   112,    -1,    62,   117,    63,    -1,    62,     1,    -1,
      62,   118,   117,    63,    -1,    62,   117,   118,    63,    -1,
      62,   118,   117,   118,    63,    -1,    62,    63,    -1,    68,
     116,    69,    -1,    68,   118,   116,    69,    -1,    68,   116,
     118,    69,    -1,    68,   118,   116,   118,    69,    -1,    68,
      69,    -1,    68,     1,    -1,     3,    -1,   112,   113,    -1,
      59,   115,    60,    -1,    59,   115,     1,    -1,    68,   115,
      69,    -1,    68,   115,     1,    -1,    70,     3,    -1,    70,
      59,   110,    60,    -1,   110,    -1,    43,    -1,    52,     3,
      61,   110,    -1,    52,     3,    61,     1,    -1,    52,     3,
      -1,    52,     1,    -1,   114,    -1,   115,    64,   114,    -1,
     120,    -1,   116,   118,   120,    -1,   120,    -1,   117,   118,
     120,    -1,     6,    -1,     5,    -1,    64,    -1,    66,    -1,
     110,    -1,   120,   119,   110,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   133,   133,   134,   134,   138,   154,   169,   184,   198,
     213,   227,   228,   229,   230,   231,   232,   233,   234,   238,
     239,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     255,   256,   260,   260,   271,   272,   276,   280,   284,   291,
     292,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   307,   308,   312,   317,
     318,   322,   323,   327,   328,   332,   333,   334,   338,   342,
     346,   349,   353,   358,   359,   363,   371,   371,   371,   371,
     371,   375,   375,   380,   381,   385,   388,   394,   400,   403,
     409,   415,   416,   417,   419,   420,   421,   422,   423,   424,
     428,   431,   435,   439,   443,   449,   450,   454,   457,   463,
     466,   469,   472,   473,   477,   478,   482,   486,   490,   492,
     494,   496,   501,   502,   503,   504,   505,   506,   507,   508,
     509,   510,   511,   512,   513,   514,   515,   516,   517,   518,
     519,   520,   521,   522,   523,   524,   525,   526,   527,   528,
     529,   530,   531,   532,   533,   534,   535,   536,   537,   538,
     539,   540,   541,   542,   543,   544,   545,   546,   547,   548,
     552,   553,   554,   555,   556,   557,   558,   559,   560,   561,
     562,   563,   564,   565,   566,   567,   570,   571,   575,   576,
     577,   578,   579,   580,   584,   585,   586,   587,   588,   589,
     593,   594,   598,   599,   603,   604,   608,   608,   612,   612,
     616,   617
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
  "FUNCTION", "FOR", "BREAK", "MAGICEND", "WHILE", "ELSE", "ELSEIF",
  "SWITCH", "CASE", "OTHERWISE", "CONTINUE", "TRY", "CATCH", "FIELD",
  "REFLPAREN", "REFRPAREN", "PERSISTENT", "KEYBOARD", "RETURN", "VARARGIN",
  "VARARGOUT", "GLOBAL", "QUIT", "RETALL", "':'", "'|'", "'&'", "'<'",
  "'>'", "NE", "'+'", "'-'", "'*'", "'/'", "'\\\\'", "NOT", "NEG", "POS",
  "'^'", "'''", "'('", "')'", "'='", "'['", "']'", "','", "';'", "'#'",
  "'~'", "'{'", "'}'", "'.'", "$accept", "program", "functionDef",
  "functionDefList", "returnDeclaration", "argumentList", "argument",
  "statementList", "statement", "statementType", "specialSyntaxStatement",
  "stringList", "persistentStatement", "globalStatement", "identList",
  "returnStatement", "keyboardStatement", "continueStatement",
  "breakStatement", "tryStatement", "optionalCatch", "switchStatement",
  "optionalEndStatement", "newLine", "caseBlock", "caseList",
  "caseStatement", "otherwiseClause", "forStatement", "forIndexExpression",
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
     295,   296,   297,    58,   124,    38,    60,    62,   298,    43,
      45,    42,    47,    92,   299,   300,   301,    94,    39,    40,
      41,    61,    91,    93,    44,    59,    35,   126,   123,   125,
      46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    71,    72,    72,    72,    73,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    74,
      74,    75,    75,    75,    75,    75,    75,    75,    75,    75,
      76,    76,    77,    77,    78,    78,    79,    79,    79,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    81,    82,
      82,    83,    83,    84,    84,    85,    85,    85,    86,    87,
      88,    89,    90,    91,    91,    92,    93,    93,    93,    93,
      93,    94,    94,    95,    95,    96,    96,    97,    98,    98,
      99,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     101,   101,   102,   102,   103,   104,   104,   105,   105,   106,
     106,   107,   107,   107,   108,   108,   109,   109,   109,   109,
     109,   109,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   112,   112,   113,   113,
     113,   113,   113,   113,   114,   114,   114,   114,   114,   114,
     115,   115,   116,   116,   117,   117,   118,   118,   119,   119,
     120,   120
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     0,     8,     7,     5,     4,     7,
       6,     2,     3,     4,     6,     3,     4,     5,     7,     1,
       2,     2,     2,     4,     1,     2,     2,     2,     3,     4,
       1,     3,     1,     2,     1,     2,     2,     2,     2,     1,
       1,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     2,     2,     2,     2,     1,     2,     2,     1,     1,
       1,     1,     4,     2,     0,     6,     1,     1,     1,     1,
       1,     1,     1,     0,     1,     1,     2,     4,     2,     0,
       5,     5,     3,     1,     5,     4,     3,     2,     3,     1,
       5,     2,     5,     2,     3,     0,     1,     1,     2,     2,
       2,     2,     0,     2,     3,     3,     8,     5,     8,     7,
       6,     5,     3,     3,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
       2,     3,     3,     3,     3,     2,     2,     3,     3,     2,
       1,     1,     1,     1,     3,     2,     4,     4,     5,     2,
       3,     4,     4,     5,     2,     2,     1,     2,     3,     3,
       3,     3,     2,     4,     1,     1,     4,     4,     2,     2,
       1,     3,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    57,   186,   170,   171,     0,     0,     0,     0,    71,
     172,     0,     0,    70,     0,     0,    69,    68,     0,    55,
      56,     0,     0,     0,     0,     0,     0,     0,    19,     3,
       0,    34,     0,    54,    53,    52,    51,    50,    45,    44,
      49,    48,    43,    46,    47,    39,    42,    40,   124,   173,
      59,    58,   103,     0,   105,     0,   173,    11,     0,     0,
       0,     0,    99,    93,     0,     0,   101,     0,     0,     0,
      62,    65,     0,    64,     0,   158,   157,   169,     0,   175,
     207,   206,   179,   210,     0,     0,   204,   160,   159,   185,
     184,     0,     0,   202,     1,    20,    35,    36,    37,    38,
       0,     0,     0,     0,     0,     0,     0,   166,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     165,     0,     0,     0,     0,   187,    60,     0,     0,   112,
     106,   107,    78,    77,    76,    79,    80,     0,    12,    82,
      81,     0,    22,     0,    25,    21,    27,    32,     0,     0,
      30,    15,     0,     0,    97,     0,     0,     0,    83,     0,
       0,    67,    66,   168,   167,   174,     0,     0,   208,   209,
       0,   180,     0,     0,   142,   141,   146,   145,   148,   147,
     152,   151,   154,   153,   156,   155,   164,   163,   123,   122,
     136,   135,   138,   137,   140,   139,   144,   143,   150,   149,
     126,   125,   128,   127,   130,   129,   132,   131,   134,   133,
     162,   161,   195,     0,   194,   200,     0,   115,   114,     0,
     192,     0,   174,   110,   109,     0,     0,   108,     0,    13,
       0,     0,     0,    33,    28,     0,     0,    16,     0,     0,
      98,    92,    96,     0,     0,     0,     0,    89,    84,    85,
       0,    72,     0,   177,   205,   176,     0,   211,   182,   203,
     181,     0,   199,   198,   189,   188,     0,   191,   190,     0,
      57,     0,   102,     0,     0,    29,    23,    31,    17,     0,
       0,     0,    95,     0,    90,   100,     0,     0,     0,    86,
     121,     0,   178,   183,     0,   201,   193,     0,    14,     0,
       0,     0,    94,    91,     0,     0,    75,   120,     0,   197,
     196,     0,     0,    18,     0,     0,   119,     0,     0,   118,
     116
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    27,    28,    29,    61,   149,   150,    30,    31,    32,
      33,    51,    34,    35,    72,    36,    37,    38,    39,    40,
     160,    41,   137,   143,   247,   248,   249,   288,    42,    65,
      43,    44,    54,   129,   130,   131,   226,    45,    46,    47,
      48,    56,   125,   215,   216,    91,    84,    85,   170,    86
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -150
static const short yypact[] =
{
     341,  -150,  -150,  -150,  -150,    17,   848,   131,    63,  -150,
    -150,  1570,  2201,  -150,  1478,   173,  -150,  -150,   185,  -150,
    -150,  2201,  2201,  1620,  1596,  1646,   874,    37,  -150,    31,
     824,  -150,    18,  -150,  -150,  -150,  -150,  -150,  -150,  -150,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  2415,  -150,   -23,
    -150,   106,  -150,  1596,    15,  2265,   137,   210,    83,    78,
     170,   211,  -150,     2,   220,   144,  -150,  2265,  2265,  1070,
    -150,  -150,    28,  -150,    38,   190,   190,  -150,  2289,  -150,
    -150,  -150,  -150,  2415,   135,  2201,   158,  -150,   190,  -150,
    -150,     7,  2201,   158,  -150,  -150,  -150,  -150,  -150,  -150,
    1650,  1670,  1700,  1720,  1724,  1750,  1774,  -150,  1800,  1804,
    1824,  1854,  1874,  1878,  1904,  1928,  1954,  1958,  1978,  2008,
    -150,  2144,  2028,  2144,    70,  -150,  -150,   157,  2032,    61,
      15,  -150,  -150,  -150,  -150,  -150,  -150,  1478,   230,  -150,
    -150,   182,  -150,  1478,  -150,  -150,  -150,  -150,   152,    14,
    -150,  -150,   196,  2058,  -150,    85,  1478,  1478,   111,  1478,
     143,  -150,  -150,  -150,  -150,   118,  2169,   163,  -150,  -150,
    2201,  -150,   215,    22,  -150,  2505,  -150,  2505,  -150,  2505,
    -150,   190,  -150,   190,  -150,   190,  -150,   190,  -150,  2430,
    -150,  2482,  -150,  2495,  -150,  2505,  -150,  2505,  -150,  2505,
    -150,   915,  -150,   915,  -150,   190,  -150,   190,  -150,   190,
    -150,   190,  -150,   251,  2415,  -150,    64,  -150,  2415,     3,
    -150,  2201,  -150,  -150,  -150,  1546,   172,  -150,   934,  -150,
      54,   -18,   410,  -150,  -150,    92,     6,  -150,   189,  1478,
    -150,  2415,  -150,  2082,  1138,  1206,  2201,   167,   111,  -150,
    1274,  -150,   265,  -150,   158,  -150,  2185,  2415,  -150,   158,
    -150,  2112,  -150,   145,  -150,  -150,  2144,  -150,  -150,  2361,
     200,  1342,  -150,  1478,    48,  -150,  -150,  -150,  -150,    54,
      -8,   479,  -150,  2343,  -150,  -150,  2265,  1478,   219,  -150,
    -150,    56,  -150,  -150,  2108,  -150,  -150,   548,  -150,  1478,
    1478,    69,  -150,  -150,  1478,  1410,  -150,  -150,   132,  -150,
    2415,   617,   686,  -150,  1478,  1002,  -150,    67,   755,  -150,
    -150
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -150,  -150,   214,  -150,  -150,  -119,    23,    21,    25,  -150,
    -150,  -150,  -150,  -150,   240,  -150,  -150,  -150,  -150,  -150,
    -150,  -150,   -57,  -149,  -150,  -150,    19,  -150,  -150,  -150,
    -150,  -150,   134,  -150,  -150,   133,  -150,  -150,  -150,    -5,
    -150,     0,  -150,    10,  -121,   180,   -27,    -1,  -150,   -21
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -118
static const short yytable[] =
{
      49,    55,   219,   239,   267,    93,    67,    68,   156,   147,
     157,   158,    80,    81,    49,   234,    75,    76,    78,    83,
      88,    83,   231,    97,    98,    92,   127,    80,    81,   161,
      49,   162,    50,   -61,   -61,    69,   121,    94,   122,   161,
     128,   162,   274,   -63,   -63,   123,   236,   124,    83,   298,
       7,   148,   301,   139,   140,    96,   236,   307,   167,   139,
     140,  -117,  -117,   153,    62,   264,    63,   266,   319,    49,
     313,    93,   268,   220,   139,   140,   171,   235,   236,   144,
      83,   273,    99,   166,   138,   225,   242,    83,   139,   140,
     172,   260,   -61,   275,    96,   175,   177,   179,   181,   183,
     185,   187,   -63,   189,   191,   193,   195,   197,   199,   201,
     203,   205,   207,   209,   211,   308,   214,   218,   214,   280,
    -117,   126,    64,    55,   265,   299,   166,   320,   266,   221,
     300,   266,    57,   316,    58,     2,     3,    49,   246,   145,
      80,    81,   141,    49,   142,   254,   243,     4,   241,   132,
     133,   259,   314,   276,    10,   233,    49,    49,   228,    49,
     251,    83,    80,    81,   232,   257,   256,    83,    80,    81,
      59,   146,   261,   147,    70,   212,    71,   244,   245,   252,
     250,    21,    22,   229,   213,   147,    73,   317,    71,   272,
     278,    23,   147,    60,    53,   287,   121,   237,   165,    25,
      26,   139,   140,   106,   107,   123,   294,   124,   134,   135,
     136,   -24,   151,   -24,   152,   148,   269,  -113,     2,     3,
     222,   154,   168,   155,   169,    49,   255,   148,    49,   304,
       4,   -26,    49,   -26,   148,   254,   306,    10,   283,    49,
     259,   286,   230,    95,    49,    49,   271,   119,   120,   279,
      49,    83,   262,    96,   263,   238,    83,    96,    74,   277,
     281,   214,   224,   227,    21,    22,   290,   289,   291,    96,
      96,    49,   173,    49,    23,    96,   295,    53,     0,     0,
       0,    49,    25,    26,   258,     0,     0,    49,     0,   310,
       0,     0,     0,     0,   297,     0,    96,    49,     0,    49,
      49,     0,     0,   214,    49,    49,    96,     0,   305,     0,
       0,    49,    49,     0,    49,    49,     0,     0,    49,     0,
     311,   312,    96,     0,     0,   315,     0,     0,     0,     0,
      96,     0,     0,     0,     0,   318,    96,    96,     0,     0,
      96,    -4,     1,    96,     2,     3,   -41,   -41,     0,     0,
       0,     0,     0,     0,     0,     0,     4,     5,     0,     6,
       7,     8,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,     0,     0,     0,     0,    15,    16,    17,     0,
       0,    18,    19,    20,     0,     0,     0,     0,     0,     0,
      21,    22,     0,     0,     0,     0,     0,     0,     0,     0,
      23,     0,     0,    24,     0,   -41,     0,     0,    25,    26,
      -8,     1,     0,     2,     3,   -41,   -41,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     5,     0,     6,    -8,
       8,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,     0,    24,     0,   -41,     0,     0,    25,    26,    -7,
       1,     0,     2,     3,   -41,   -41,     0,     0,     0,     0,
       0,     0,     0,     0,     4,     5,     0,     6,    -7,     8,
       9,    10,    11,     0,     0,    12,     0,     0,    13,    14,
       0,     0,     0,     0,    15,    16,    17,     0,     0,    18,
      19,    20,     0,     0,     0,     0,     0,     0,    21,    22,
       0,     0,     0,     0,     0,     0,     0,     0,    23,     0,
       0,    24,     0,   -41,     0,     0,    25,    26,   -10,     1,
       0,     2,     3,   -41,   -41,     0,     0,     0,     0,     0,
       0,     0,     0,     4,     5,     0,     6,   -10,     8,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,     0,     0,    15,    16,    17,     0,     0,    18,    19,
      20,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,     0,
      24,     0,   -41,     0,     0,    25,    26,    -6,     1,     0,
       2,     3,   -41,   -41,     0,     0,     0,     0,     0,     0,
       0,     0,     4,     5,     0,     6,    -6,     8,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,     0,     0,
       0,     0,    15,    16,    17,     0,     0,    18,    19,    20,
       0,     0,     0,     0,     0,     0,    21,    22,     0,     0,
       0,     0,     0,     0,     0,     0,    23,     0,     0,    24,
       0,   -41,     0,     0,    25,    26,    -9,     1,     0,     2,
       3,   -41,   -41,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     5,     0,     6,    -9,     8,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,     0,    24,     0,
     -41,     0,     0,    25,    26,    -5,     1,     0,     2,     3,
     -41,   -41,     0,     0,     0,     0,     0,     0,     0,     0,
       4,     5,     0,     6,    -5,     8,     9,    10,    11,     0,
       0,    12,     0,     0,    13,    14,     0,     0,     0,     0,
      15,    16,    17,     0,     0,    18,    19,    20,     0,     0,
       0,     0,     0,     0,    21,    22,     0,     0,     0,     0,
       0,     0,     0,     0,    23,     0,     0,    24,     0,   -41,
       0,     0,    25,    26,    -2,     1,     0,     2,     3,   -41,
     -41,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       5,     0,     6,     0,     8,     9,    10,    11,     0,    52,
      12,     2,     3,    13,    14,     0,     0,     0,     0,    15,
      16,    17,     0,     4,    18,    19,    20,     0,     0,     0,
      10,     0,     0,    21,    22,    89,     0,     2,     3,    80,
      81,     0,     0,    23,     0,     0,    24,     0,   -41,     4,
       0,    25,    26,     0,     0,     0,    10,    21,    22,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,     0,
      53,     0,     0,     0,     0,    25,    26,     0,     0,     0,
       0,     0,     0,    21,    22,   103,   104,   105,   106,   107,
       0,     0,     0,    23,     0,     1,    53,     2,     3,   -41,
     -41,    25,    26,    90,     0,     0,     0,     0,     0,     4,
       5,  -104,     6,     0,     8,     9,    10,    11,  -104,  -104,
      12,     0,     0,    13,    14,     0,   116,   117,   118,    15,
      16,    17,   119,   120,    18,    19,    20,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,    24,     0,   -41,     0,
       0,    25,    26,     1,     0,     2,     3,   -41,   -41,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,   -87,
       6,     0,     8,     9,    10,    11,     0,     0,    12,   -87,
     -87,    13,    14,     0,     0,     0,     0,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,    24,     0,   -41,     0,     0,    25,
      26,     1,     0,     2,     3,   -41,   -41,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     5,   -74,     6,     0,
       8,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,   159,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,     0,    24,     0,   -41,     0,     0,    25,    26,     1,
       0,     2,     3,   -41,   -41,     0,     0,     0,     0,     0,
       0,     0,     0,     4,     5,   284,     6,     0,     8,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,     0,     0,    15,    16,    17,     0,     0,    18,    19,
      20,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,     0,
      24,     0,   -41,     0,     0,    25,    26,     1,     0,     2,
       3,   -41,   -41,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     5,   285,     6,     0,     8,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,     0,    24,     0,
     -41,     0,     0,    25,    26,     1,     0,     2,     3,   -41,
     -41,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       5,   -73,     6,     0,     8,     9,    10,    11,     0,     0,
      12,     0,     0,    13,    14,     0,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,    24,     0,   -41,     0,
       0,    25,    26,     1,     0,     2,     3,   -41,   -41,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,  -111,
       6,     0,     8,     9,    10,    11,     0,     0,    12,     0,
       0,    13,    14,     0,     0,     0,     0,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,    24,     0,   -41,     0,     0,    25,
      26,     1,     0,     2,     3,   -41,   -41,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     5,   -88,     6,     0,
       8,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,     0,    24,     0,   -41,     0,     0,    25,    26,     1,
       0,     2,     3,   -41,   -41,     0,     0,     0,     0,     0,
       0,     0,     0,     4,     5,     0,     6,     0,     8,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,     0,     0,    15,    16,    17,     0,     0,    18,    19,
      20,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,     0,
      24,     0,   -41,     0,     0,    25,    26,   270,     0,     2,
       3,   -41,   -41,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     5,     0,     6,     0,     8,     9,    10,    11,
       0,    66,    12,     2,     3,    13,    14,     0,     0,     0,
       0,    15,    16,    17,     0,     4,    18,    19,    20,     0,
       0,     0,    10,     0,     0,    21,    22,    79,     0,     2,
       3,    80,    81,     0,     0,    23,     0,     0,    24,     0,
     -41,     4,     0,    25,    26,     0,     0,     0,    10,    21,
      22,    77,     0,     2,     3,     0,     0,     0,     0,    23,
       0,     0,    53,     0,     0,     4,     0,    25,    26,     0,
       0,     0,    10,     0,     0,    21,    22,    87,     0,     2,
       3,   174,     0,     2,     3,    23,     0,     0,    53,    82,
       0,     4,     0,    25,    26,     4,     0,     0,    10,    21,
      22,   176,    10,     2,     3,     0,     0,     0,     0,    23,
       0,     0,    53,     0,     0,     4,     0,    25,    26,     0,
       0,     0,    10,     0,     0,    21,    22,     0,     0,    21,
      22,   178,     0,     2,     3,    23,     0,     0,    53,    23,
       0,     0,    53,    25,    26,     4,     0,    25,    26,    21,
      22,   180,    10,     2,     3,   182,     0,     2,     3,    23,
       0,     0,    53,     0,     0,     4,     0,    25,    26,     4,
       0,     0,    10,     0,     0,     0,    10,     0,     0,    21,
      22,   184,     0,     2,     3,     0,     0,     0,     0,    23,
       0,     0,    53,     0,     0,     4,     0,    25,    26,    21,
      22,     0,    10,    21,    22,   186,     0,     2,     3,    23,
       0,     0,    53,    23,     0,     0,    53,    25,    26,     4,
       0,    25,    26,     0,     0,     0,    10,     0,     0,    21,
      22,   188,     0,     2,     3,   190,     0,     2,     3,    23,
       0,     0,    53,     0,     0,     4,     0,    25,    26,     4,
       0,     0,    10,    21,    22,   192,    10,     2,     3,     0,
       0,     0,     0,    23,     0,     0,    53,     0,     0,     4,
       0,    25,    26,     0,     0,     0,    10,     0,     0,    21,
      22,     0,     0,    21,    22,   194,     0,     2,     3,    23,
       0,     0,    53,    23,     0,     0,    53,    25,    26,     4,
       0,    25,    26,    21,    22,   196,    10,     2,     3,   198,
       0,     2,     3,    23,     0,     0,    53,     0,     0,     4,
       0,    25,    26,     4,     0,     0,    10,     0,     0,     0,
      10,     0,     0,    21,    22,   200,     0,     2,     3,     0,
       0,     0,     0,    23,     0,     0,    53,     0,     0,     4,
       0,    25,    26,    21,    22,     0,    10,    21,    22,   202,
       0,     2,     3,    23,     0,     0,    53,    23,     0,     0,
      53,    25,    26,     4,     0,    25,    26,     0,     0,     0,
      10,     0,     0,    21,    22,   204,     0,     2,     3,   206,
       0,     2,     3,    23,     0,     0,    53,     0,     0,     4,
       0,    25,    26,     4,     0,     0,    10,    21,    22,   208,
      10,     2,     3,     0,     0,     0,     0,    23,     0,     0,
      53,     0,     0,     4,     0,    25,    26,     0,     0,     0,
      10,     0,     0,    21,    22,     0,     0,    21,    22,   210,
       0,     2,     3,    23,     0,     0,    53,    23,     0,     0,
      53,    25,    26,     4,     0,    25,    26,    21,    22,   217,
      10,     2,     3,   223,     0,     2,     3,    23,     0,     0,
      53,     0,     0,     4,     0,    25,    26,     4,     0,     0,
      10,     0,     0,     0,    10,     0,     0,    21,    22,   240,
       0,     2,     3,     0,     0,     0,     0,    23,     0,     0,
      53,     0,     0,     4,     0,    25,    26,    21,    22,     0,
      10,    21,    22,   282,     0,     2,     3,    23,     0,     0,
      53,    23,     0,     0,    53,    25,    26,     4,     0,    25,
      26,     0,     0,     0,    10,     0,     0,    21,    22,   309,
       0,     2,     3,     0,     0,     2,     3,    23,     0,     0,
      53,     0,     0,     4,     0,    25,    26,     4,     0,     0,
      10,    21,    22,     0,    10,     0,     0,     0,     0,     0,
       0,    23,     0,     0,    53,     0,     0,     2,     3,    25,
      26,     0,     0,     0,     0,     0,     0,    21,    22,     4,
       0,    21,    22,     0,     0,     0,    10,    23,     0,     0,
      53,    23,     2,     3,    53,    25,    26,     0,     0,    25,
      26,   293,     0,     0,     4,     0,     0,   212,     2,     3,
       0,    10,     0,    21,    22,     0,   213,     0,     0,     0,
       4,     0,     0,    23,     2,     3,    53,    10,     0,     0,
       0,    25,    26,     0,     0,     0,     4,     0,    21,    22,
       0,     0,     0,    10,     0,     0,     0,     0,    23,     0,
       0,    53,   253,     0,    21,    22,    25,    26,     0,     0,
       0,     0,     0,     0,    23,     0,     0,    53,   292,     0,
      21,    22,    25,    26,     0,     0,     0,     0,     0,     0,
      23,     0,     0,    53,     0,     0,     0,     0,    25,    26,
     132,   133,   100,   101,   102,   103,   104,   105,   106,   107,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     163,     0,     0,     0,     0,     0,   100,   101,   102,   103,
     104,   105,   106,   107,     0,     0,     0,     0,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,     0,
       0,     0,   119,   120,     0,     0,     0,     0,     0,   134,
     135,   136,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,     0,   302,     0,   119,   120,     0,   164,
     100,   101,   102,   103,   104,   105,   106,   107,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   100,   101,
     102,   103,   104,   105,   106,   107,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,     0,     0,     0,
     119,   120,     0,   303,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,     0,     0,     0,   119,   120,
       0,   296,   100,   101,   102,   103,   104,   105,   106,   107,
       0,     0,     0,     0,     0,     0,     0,   100,   101,   102,
     103,   104,   105,   106,   107,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,     0,
       0,     0,   119,   120,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,     0,     0,     0,   119,   120,   100,
     101,   102,   103,   104,   105,   106,   107,     0,     0,     0,
       0,     0,   100,   101,   102,   103,   104,   105,   106,   107,
       0,     0,     0,     0,     0,   103,   104,   105,   106,   107,
       0,     0,     0,     0,     0,     0,     0,   110,   111,   112,
     113,   114,   115,   116,   117,   118,     0,     0,     0,   119,
     120,   111,   112,   113,   114,   115,   116,   117,   118,     0,
       0,     0,   119,   120,   114,   115,   116,   117,   118,     0,
       0,     0,   119,   120
};

static const short yycheck[] =
{
       0,     6,   123,   152,     1,    26,    11,    12,    65,     3,
      67,    68,     5,     6,    14,     1,    21,    22,    23,    24,
      25,    26,   141,     5,     6,    26,    53,     5,     6,     1,
      30,     3,    15,     5,     6,    14,    59,     0,    61,     1,
      25,     3,    60,     5,     6,    68,    64,    70,    53,     1,
      19,    45,    60,     5,     6,    30,    64,     1,    85,     5,
       6,     5,     6,    61,     1,     1,     3,    64,     1,    69,
       1,    92,    69,     3,     5,     6,    69,    63,    64,     1,
      85,   230,    64,    84,     1,    24,     1,    92,     5,     6,
      91,    69,    64,     1,    69,   100,   101,   102,   103,   104,
     105,   106,    64,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,    59,   121,   122,   123,   238,
      64,    15,    59,   128,    60,   274,   127,    60,    64,    59,
     279,    64,     1,     1,     3,     3,     4,   137,    27,    61,
       5,     6,    59,   143,    61,   166,    61,    15,   153,     5,
       6,   172,   301,    61,    22,     3,   156,   157,   137,   159,
      17,   166,     5,     6,   143,   170,   167,   172,     5,     6,
      39,     1,   173,     3,     1,    43,     3,   156,   157,    61,
     159,    49,    50,     1,    52,     3,     1,   308,     3,    17,
       1,    59,     3,    62,    62,    28,    59,     1,    63,    67,
      68,     5,     6,    13,    14,    68,    61,    70,    64,    65,
      66,     1,     1,     3,     3,    45,   221,    17,     3,     4,
      63,     1,    64,     3,    66,   225,    63,    45,   228,   286,
      15,     1,   232,     3,    45,   256,    17,    22,   243,   239,
     261,   246,    60,    29,   244,   245,   225,    57,    58,    60,
     250,   256,     1,   228,     3,    59,   261,   232,    18,   236,
     239,   266,   128,   130,    49,    50,     1,   248,     3,   244,
     245,   271,    92,   273,    59,   250,   266,    62,    -1,    -1,
      -1,   281,    67,    68,    69,    -1,    -1,   287,    -1,   294,
      -1,    -1,    -1,    -1,   273,    -1,   271,   297,    -1,   299,
     300,    -1,    -1,   308,   304,   305,   281,    -1,   287,    -1,
      -1,   311,   312,    -1,   314,   315,    -1,    -1,   318,    -1,
     299,   300,   297,    -1,    -1,   304,    -1,    -1,    -1,    -1,
     305,    -1,    -1,    -1,    -1,   314,   311,   312,    -1,    -1,
     315,     0,     1,   318,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    -1,    18,
      19,    20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,
      29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,
      -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    -1,    -1,    62,    -1,    64,    -1,    -1,    67,    68,
       0,     1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    -1,    18,    19,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,    -1,    64,    -1,    -1,    67,    68,     0,
       1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    15,    16,    -1,    18,    19,    20,
      21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,
      -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,
      41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,
      -1,    62,    -1,    64,    -1,    -1,    67,    68,     0,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,    16,    -1,    18,    19,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,
      62,    -1,    64,    -1,    -1,    67,    68,     0,     1,    -1,
       3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    15,    16,    -1,    18,    19,    20,    21,    22,
      23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,
      -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,    62,
      -1,    64,    -1,    -1,    67,    68,     0,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    -1,    18,    19,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,
      -1,    35,    36,    37,    -1,    -1,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    -1,
      64,    -1,    -1,    67,    68,     0,     1,    -1,     3,     4,
       5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      15,    16,    -1,    18,    19,    20,    21,    22,    23,    -1,
      -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,
      35,    36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    49,    50,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    -1,    64,
      -1,    -1,    67,    68,     0,     1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      16,    -1,    18,    -1,    20,    21,    22,    23,    -1,     1,
      26,     3,     4,    29,    30,    -1,    -1,    -1,    -1,    35,
      36,    37,    -1,    15,    40,    41,    42,    -1,    -1,    -1,
      22,    -1,    -1,    49,    50,     1,    -1,     3,     4,     5,
       6,    -1,    -1,    59,    -1,    -1,    62,    -1,    64,    15,
      -1,    67,    68,    -1,    -1,    -1,    22,    49,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,
      62,    -1,    -1,    -1,    -1,    67,    68,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    59,    -1,     1,    62,     3,     4,     5,
       6,    67,    68,    69,    -1,    -1,    -1,    -1,    -1,    15,
      16,    17,    18,    -1,    20,    21,    22,    23,    24,    25,
      26,    -1,    -1,    29,    30,    -1,    51,    52,    53,    35,
      36,    37,    57,    58,    40,    41,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    59,    -1,    -1,    62,    -1,    64,    -1,
      -1,    67,    68,     1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,
      18,    -1,    20,    21,    22,    23,    -1,    -1,    26,    27,
      28,    29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,
      -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    -1,    -1,    62,    -1,    64,    -1,    -1,    67,
      68,     1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    -1,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    31,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,    -1,    64,    -1,    -1,    67,    68,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,    16,    17,    18,    -1,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,
      62,    -1,    64,    -1,    -1,    67,    68,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    17,    18,    -1,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,
      -1,    35,    36,    37,    -1,    -1,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    -1,
      64,    -1,    -1,    67,    68,     1,    -1,     3,     4,     5,
       6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      16,    17,    18,    -1,    20,    21,    22,    23,    -1,    -1,
      26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,
      36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    59,    -1,    -1,    62,    -1,    64,    -1,
      -1,    67,    68,     1,    -1,     3,     4,     5,     6,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,
      18,    -1,    20,    21,    22,    23,    -1,    -1,    26,    -1,
      -1,    29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,
      -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    -1,    -1,    62,    -1,    64,    -1,    -1,    67,
      68,     1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,    -1,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,    -1,    64,    -1,    -1,    67,    68,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,    16,    -1,    18,    -1,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,
      62,    -1,    64,    -1,    -1,    67,    68,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    -1,    18,    -1,    20,    21,    22,    23,
      -1,     1,    26,     3,     4,    29,    30,    -1,    -1,    -1,
      -1,    35,    36,    37,    -1,    15,    40,    41,    42,    -1,
      -1,    -1,    22,    -1,    -1,    49,    50,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    59,    -1,    -1,    62,    -1,
      64,    15,    -1,    67,    68,    -1,    -1,    -1,    22,    49,
      50,     1,    -1,     3,     4,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,    -1,    -1,    15,    -1,    67,    68,    -1,
      -1,    -1,    22,    -1,    -1,    49,    50,     1,    -1,     3,
       4,     1,    -1,     3,     4,    59,    -1,    -1,    62,    63,
      -1,    15,    -1,    67,    68,    15,    -1,    -1,    22,    49,
      50,     1,    22,     3,     4,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,    -1,    -1,    15,    -1,    67,    68,    -1,
      -1,    -1,    22,    -1,    -1,    49,    50,    -1,    -1,    49,
      50,     1,    -1,     3,     4,    59,    -1,    -1,    62,    59,
      -1,    -1,    62,    67,    68,    15,    -1,    67,    68,    49,
      50,     1,    22,     3,     4,     1,    -1,     3,     4,    59,
      -1,    -1,    62,    -1,    -1,    15,    -1,    67,    68,    15,
      -1,    -1,    22,    -1,    -1,    -1,    22,    -1,    -1,    49,
      50,     1,    -1,     3,     4,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,    -1,    -1,    15,    -1,    67,    68,    49,
      50,    -1,    22,    49,    50,     1,    -1,     3,     4,    59,
      -1,    -1,    62,    59,    -1,    -1,    62,    67,    68,    15,
      -1,    67,    68,    -1,    -1,    -1,    22,    -1,    -1,    49,
      50,     1,    -1,     3,     4,     1,    -1,     3,     4,    59,
      -1,    -1,    62,    -1,    -1,    15,    -1,    67,    68,    15,
      -1,    -1,    22,    49,    50,     1,    22,     3,     4,    -1,
      -1,    -1,    -1,    59,    -1,    -1,    62,    -1,    -1,    15,
      -1,    67,    68,    -1,    -1,    -1,    22,    -1,    -1,    49,
      50,    -1,    -1,    49,    50,     1,    -1,     3,     4,    59,
      -1,    -1,    62,    59,    -1,    -1,    62,    67,    68,    15,
      -1,    67,    68,    49,    50,     1,    22,     3,     4,     1,
      -1,     3,     4,    59,    -1,    -1,    62,    -1,    -1,    15,
      -1,    67,    68,    15,    -1,    -1,    22,    -1,    -1,    -1,
      22,    -1,    -1,    49,    50,     1,    -1,     3,     4,    -1,
      -1,    -1,    -1,    59,    -1,    -1,    62,    -1,    -1,    15,
      -1,    67,    68,    49,    50,    -1,    22,    49,    50,     1,
      -1,     3,     4,    59,    -1,    -1,    62,    59,    -1,    -1,
      62,    67,    68,    15,    -1,    67,    68,    -1,    -1,    -1,
      22,    -1,    -1,    49,    50,     1,    -1,     3,     4,     1,
      -1,     3,     4,    59,    -1,    -1,    62,    -1,    -1,    15,
      -1,    67,    68,    15,    -1,    -1,    22,    49,    50,     1,
      22,     3,     4,    -1,    -1,    -1,    -1,    59,    -1,    -1,
      62,    -1,    -1,    15,    -1,    67,    68,    -1,    -1,    -1,
      22,    -1,    -1,    49,    50,    -1,    -1,    49,    50,     1,
      -1,     3,     4,    59,    -1,    -1,    62,    59,    -1,    -1,
      62,    67,    68,    15,    -1,    67,    68,    49,    50,     1,
      22,     3,     4,     1,    -1,     3,     4,    59,    -1,    -1,
      62,    -1,    -1,    15,    -1,    67,    68,    15,    -1,    -1,
      22,    -1,    -1,    -1,    22,    -1,    -1,    49,    50,     1,
      -1,     3,     4,    -1,    -1,    -1,    -1,    59,    -1,    -1,
      62,    -1,    -1,    15,    -1,    67,    68,    49,    50,    -1,
      22,    49,    50,     1,    -1,     3,     4,    59,    -1,    -1,
      62,    59,    -1,    -1,    62,    67,    68,    15,    -1,    67,
      68,    -1,    -1,    -1,    22,    -1,    -1,    49,    50,     1,
      -1,     3,     4,    -1,    -1,     3,     4,    59,    -1,    -1,
      62,    -1,    -1,    15,    -1,    67,    68,    15,    -1,    -1,
      22,    49,    50,    -1,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    -1,    -1,    62,    -1,    -1,     3,     4,    67,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    15,
      -1,    49,    50,    -1,    -1,    -1,    22,    59,    -1,    -1,
      62,    59,     3,     4,    62,    67,    68,    -1,    -1,    67,
      68,    69,    -1,    -1,    15,    -1,    -1,    43,     3,     4,
      -1,    22,    -1,    49,    50,    -1,    52,    -1,    -1,    -1,
      15,    -1,    -1,    59,     3,     4,    62,    22,    -1,    -1,
      -1,    67,    68,    -1,    -1,    -1,    15,    -1,    49,    50,
      -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,    59,    -1,
      -1,    62,    63,    -1,    49,    50,    67,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    63,    -1,
      49,    50,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    68,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    -1,    -1,    -1,    -1,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    -1,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    64,
      65,    66,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,     1,    -1,    57,    58,    -1,    60,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    -1,    -1,    -1,
      57,    58,    -1,    60,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    57,    58,
      -1,    60,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    -1,
      -1,    -1,    57,    58,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    57,    58,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    -1,    -1,    57,
      58,    46,    47,    48,    49,    50,    51,    52,    53,    -1,
      -1,    -1,    57,    58,    49,    50,    51,    52,    53,    -1,
      -1,    -1,    57,    58
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,    15,    16,    18,    19,    20,    21,
      22,    23,    26,    29,    30,    35,    36,    37,    40,    41,
      42,    49,    50,    59,    62,    67,    68,    72,    73,    74,
      78,    79,    80,    81,    83,    84,    86,    87,    88,    89,
      90,    92,    99,   101,   102,   108,   109,   110,   111,   112,
      15,    82,     1,    62,   103,   110,   112,     1,     3,    39,
      62,    75,     1,     3,    59,   100,     1,   110,   110,    78,
       1,     3,    85,     1,    85,   110,   110,     1,   110,     1,
       5,     6,    63,   110,   117,   118,   120,     1,   110,     1,
      69,   116,   118,   120,     0,    73,    79,     5,     6,    64,
       7,     8,     9,    10,    11,    12,    13,    14,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    57,
      58,    59,    61,    68,    70,   113,    15,   117,    25,   104,
     105,   106,     5,     6,    64,    65,    66,    93,     1,     5,
       6,    59,    61,    94,     1,    61,     1,     3,    45,    76,
      77,     1,     3,    61,     1,     3,    93,    93,    93,    31,
      91,     1,     3,     1,    60,    63,   118,   117,    64,    66,
     119,    69,   118,   116,     1,   110,     1,   110,     1,   110,
       1,   110,     1,   110,     1,   110,     1,   110,     1,   110,
       1,   110,     1,   110,     1,   110,     1,   110,     1,   110,
       1,   110,     1,   110,     1,   110,     1,   110,     1,   110,
       1,   110,    43,    52,   110,   114,   115,     1,   110,   115,
       3,    59,    63,     1,   103,    24,   107,   106,    78,     1,
      60,    76,    78,     3,     1,    63,    64,     1,    59,    94,
       1,   110,     1,    61,    78,    78,    27,    95,    96,    97,
      78,    17,    61,    63,   120,    63,   118,   110,    69,   120,
      69,   118,     1,     3,     1,    60,    64,     1,    69,   110,
       1,    78,    17,    94,    60,     1,    61,    77,     1,    60,
      76,    78,     1,   110,    17,    17,   110,    28,    98,    97,
       1,     3,    63,    69,    61,   114,    60,    78,     1,    94,
      94,    60,     1,    60,    93,    78,    17,     1,    59,     1,
     110,    78,    78,     1,    94,    78,     1,   115,    78,     1,
      60
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
#line 133 "../../../libs/libFreeMat/Parser.yxx"
    {mainAST = yyvsp[0];}
    break;

  case 5:
#line 138 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-6]->toStringList();
     r->name = yyvsp[-5]->text;
     r->arguments = yyvsp[-3]->toStringList();
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
#line 154 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-5]->text;
     r->arguments = yyvsp[-3]->toStringList();
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
#line 169 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-3]->toStringList();
     r->name = yyvsp[-2]->text;
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
#line 184 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-2]->text;
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
#line 198 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-5]->toStringList();
     r->name = yyvsp[-4]->text;
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
#line 213 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-4]->text;
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

  case 11:
#line 227 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("function name or return declaration");}
    break;

  case 12:
#line 228 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("argument list or statement list");}
    break;

  case 13:
#line 229 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("(possibly empty) argument list");}
    break;

  case 14:
#line 230 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list");}
    break;

  case 15:
#line 231 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("function name");}
    break;

  case 16:
#line 232 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("argument list or statement list");}
    break;

  case 17:
#line 233 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("(possibly empty) argument list");}
    break;

  case 18:
#line 234 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list");}
    break;

  case 21:
#line 243 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 22:
#line 244 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 23:
#line 245 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2];}
    break;

  case 24:
#line 246 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("either 'varargout', a single returned variable, or a list of return variables in return declaration");}
    break;

  case 25:
#line 247 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after 'varargout' in return declaration");}
    break;

  case 26:
#line 248 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after identifier in return declaration");}
    break;

  case 27:
#line 249 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a valid list of return arguments in return declaration");}
    break;

  case 28:
#line 250 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching ']' in return declaration");}
    break;

  case 29:
#line 251 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after return declaration");}
    break;

  case 30:
#line 255 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[0];}
    break;

  case 31:
#line 256 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 33:
#line 260 "../../../libs/libFreeMat/Parser.yxx"
    {
	yyval = yyvsp[0];
	char *b = (char*) malloc(strlen(yyvsp[0]->text)+2);
	b[0] = '&';
	strcpy(b+1,yyvsp[0]->text);
	yyval->text = b;
  }
    break;

  case 34:
#line 271 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_BLOCK,yyvsp[0],yyvsp[0]->context());}
    break;

  case 35:
#line 272 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 36:
#line 276 "../../../libs/libFreeMat/Parser.yxx"
    {
  	    yyval = new AST(OP_QSTATEMENT,NULL,(int)yyvsp[0]);
	    yyval->down = yyvsp[-1];
	 }
    break;

  case 37:
#line 280 "../../../libs/libFreeMat/Parser.yxx"
    {
	    yyval = new AST(OP_RSTATEMENT,NULL,(int)yyvsp[0]);
            yyval->down = yyvsp[-1];
	 }
    break;

  case 38:
#line 284 "../../../libs/libFreeMat/Parser.yxx"
    {
	    yyval = new AST(OP_RSTATEMENT,NULL,(int)yyvsp[0]);
	    yyval->down = yyvsp[-1];
	 }
    break;

  case 41:
#line 293 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(null_node,"",-1);}
    break;

  case 57:
#line 308 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("recognizable statement type (assignment, expression, function call, etc...)");}
    break;

  case 58:
#line 312 "../../../libs/libFreeMat/Parser.yxx"
    {yyvsp[-1]->addChild(yyvsp[0]); 
				yyval = new AST(OP_SCALL,yyvsp[-1],yyvsp[-1]->context());}
    break;

  case 59:
#line 317 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_PARENS,yyvsp[0],yyvsp[0]->context());}
    break;

  case 60:
#line 318 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 61:
#line 322 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]); }
    break;

  case 62:
#line 323 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as persistent");}
    break;

  case 63:
#line 327 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 64:
#line 328 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as global");}
    break;

  case 66:
#line 333 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 67:
#line 334 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of valid identifiers");}
    break;

  case 72:
#line 354 "../../../libs/libFreeMat/Parser.yxx"
    { yyval = yyvsp[-3]; yyval->addChild(yyvsp[-2]); if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);}
    break;

  case 73:
#line 358 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[0];}
    break;

  case 74:
#line 359 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = NULL;}
    break;

  case 75:
#line 363 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-5]; yyval->addChild(yyvsp[-4]); 
	  if (yyvsp[-2] != NULL) yyval->addChild(yyvsp[-2]); 
	  if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);
	}
    break;

  case 83:
#line 380 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = NULL;}
    break;

  case 85:
#line 385 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = new AST(OP_CASEBLOCK,yyvsp[0],yyvsp[0]->context());
	}
    break;

  case 86:
#line 388 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);
	}
    break;

  case 87:
#line 394 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-3]; yyval->addChild(yyvsp[-2]); yyval->addChild(yyvsp[0]);
	}
    break;

  case 88:
#line 400 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 89:
#line 403 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = NULL;
	}
    break;

  case 90:
#line 409 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-4]; yyval->addChild(yyvsp[-3]); yyval->addChild(yyvsp[-1]);
	}
    break;

  case 91:
#line 415 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-3]; yyval->addChild(yyvsp[-1]);}
    break;

  case 92:
#line 416 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 93:
#line 417 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[0]; 
	      yyval->addChild(new AST(OP_RHS,new AST(id_node,yyvsp[0]->text,yyvsp[0]->context()),yyvsp[0]->context())); }
    break;

  case 94:
#line 419 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis");}
    break;

  case 95:
#line 420 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression");}
    break;

  case 96:
#line 421 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("equals operator after loop index");}
    break;

  case 97:
#line 422 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier that is the loop variable");}
    break;

  case 98:
#line 423 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression");}
    break;

  case 99:
#line 424 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier or assignment (id = expr) after 'for' ");}
    break;

  case 100:
#line 428 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-4]; yyval->addChild(yyvsp[-3]); yyval->addChild(yyvsp[-1]);
	}
    break;

  case 101:
#line 431 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test expression after 'while'");}
    break;

  case 102:
#line 435 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-4]; yyval->addChild(yyvsp[-3]); if (yyvsp[-2] != NULL) yyval->addChild(yyvsp[-2]); 
	  if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);
	}
    break;

  case 103:
#line 439 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("condition expression for 'if'");}
    break;

  case 104:
#line 443 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = new AST(OP_CSTAT,yyvsp[-2],yyvsp[0],yyvsp[-2]->context());
	}
    break;

  case 105:
#line 449 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = NULL;}
    break;

  case 107:
#line 454 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = new AST(OP_ELSEIFBLOCK,yyvsp[0],yyvsp[0]->context());
	}
    break;

  case 108:
#line 457 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);
	}
    break;

  case 109:
#line 463 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 110:
#line 466 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test condition for 'elseif' clause");}
    break;

  case 111:
#line 469 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 112:
#line 472 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = NULL;}
    break;

  case 113:
#line 473 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list for 'else' clause");}
    break;

  case 114:
#line 477 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_ASSIGN,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 115:
#line 478 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expression in assignment");}
    break;

  case 116:
#line 482 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[-3]->addChild(new AST(OP_PARENS,yyvsp[-1],(int)yyvsp[-2]));
	  yyval = new AST(OP_MULTICALL,yyvsp[-6],yyvsp[-3],(int)yyvsp[-7]);
	}
    break;

  case 117:
#line 486 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[0]->addChild(new AST(OP_PARENS,NULL,-1));
	  yyval = new AST(OP_MULTICALL,yyvsp[-3],yyvsp[0],(int)yyvsp[-4]);
	}
    break;

  case 118:
#line 491 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis");}
    break;

  case 119:
#line 493 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing list");}
    break;

  case 120:
#line 495 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("left parenthesis");}
    break;

  case 121:
#line 497 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier");}
    break;

  case 122:
#line 501 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_COLON,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 123:
#line 502 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after ':'");}
    break;

  case 125:
#line 504 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_PLUS,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 126:
#line 505 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '+'");}
    break;

  case 127:
#line 506 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_SUBTRACT,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 128:
#line 507 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '-'");}
    break;

  case 129:
#line 508 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_TIMES,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 130:
#line 509 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '*'");}
    break;

  case 131:
#line 510 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_RDIV,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 132:
#line 511 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '/'");}
    break;

  case 133:
#line 512 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_LDIV,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 134:
#line 513 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '\\'");}
    break;

  case 135:
#line 514 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_OR,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 136:
#line 515 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '|'");}
    break;

  case 137:
#line 516 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_AND,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 138:
#line 517 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '&'");}
    break;

  case 139:
#line 518 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_LT,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 140:
#line 519 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<'");}
    break;

  case 141:
#line 520 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_LEQ,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 142:
#line 521 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<='");}
    break;

  case 143:
#line 522 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_GT,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 144:
#line 523 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>'");}
    break;

  case 145:
#line 524 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_GEQ,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 146:
#line 525 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>='");}
    break;

  case 147:
#line 526 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_EQ,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 148:
#line 527 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '=='");}
    break;

  case 149:
#line 528 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_NEQ,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 150:
#line 529 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '~='");}
    break;

  case 151:
#line 530 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT_TIMES,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 152:
#line 531 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.*'");}
    break;

  case 153:
#line 532 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT_RDIV,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 154:
#line 533 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after './'");}
    break;

  case 155:
#line 534 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT_LDIV,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 156:
#line 535 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.\\'");}
    break;

  case 157:
#line 536 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_NEG,yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 158:
#line 537 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[0];}
    break;

  case 159:
#line 538 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_NOT,yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 160:
#line 539 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after logical not");}
    break;

  case 161:
#line 540 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_POWER,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 162:
#line 541 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '^'");}
    break;

  case 163:
#line 542 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT_POWER,yyvsp[-2],yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 164:
#line 543 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.^'");}
    break;

  case 165:
#line 544 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_TRANSPOSE,yyvsp[-1],(int)yyvsp[0]);}
    break;

  case 166:
#line 545 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT_TRANSPOSE,yyvsp[-1],(int)yyvsp[0]);}
    break;

  case 167:
#line 546 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 168:
#line 547 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a right parenthesis after expression");}
    break;

  case 169:
#line 548 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after left parenthesis");}
    break;

  case 173:
#line 555 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_RHS,yyvsp[0],yyvsp[0]->context());}
    break;

  case 174:
#line 556 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 175:
#line 557 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a matrix definition followed by a right bracket");}
    break;

  case 176:
#line 558 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 177:
#line 559 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2];}
    break;

  case 178:
#line 560 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2];}
    break;

  case 179:
#line 561 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_EMPTY,NULL,(int)yyvsp[-1]);}
    break;

  case 180:
#line 562 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 181:
#line 563 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1];}
    break;

  case 182:
#line 564 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2];}
    break;

  case 183:
#line 565 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2];}
    break;

  case 184:
#line 566 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_EMPTY_CELL,NULL,(int)yyvsp[-1]);}
    break;

  case 185:
#line 567 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a cell-array definition followed by a right brace");}
    break;

  case 187:
#line 571 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 188:
#line 575 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_PARENS,yyvsp[-1],(int)yyvsp[-2]); }
    break;

  case 189:
#line 576 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis");}
    break;

  case 190:
#line 577 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_BRACES,yyvsp[-1],(int)yyvsp[-2]); }
    break;

  case 191:
#line 578 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right brace");}
    break;

  case 192:
#line 579 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOT,yyvsp[0],(int)yyvsp[-1]); }
    break;

  case 193:
#line 580 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_DOTDYN,yyvsp[-1],(int)yyvsp[-3]);}
    break;

  case 195:
#line 585 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_ALL,NULL,(int)yyvsp[0]);}
    break;

  case 196:
#line 586 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_KEYWORD,yyvsp[-2],yyvsp[0],(int)yyvsp[-3]);}
    break;

  case 197:
#line 587 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting expression after '=' in keyword assignment");}
    break;

  case 198:
#line 588 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_KEYWORD,yyvsp[0],(int)yyvsp[-1]);}
    break;

  case 199:
#line 589 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting keyword identifier after '/' in keyword assignment");}
    break;

  case 201:
#line 594 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addPeer(yyvsp[0]);}
    break;

  case 202:
#line 598 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_BRACES,yyvsp[0],yyvsp[0]->context());}
    break;

  case 203:
#line 599 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 204:
#line 603 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_BRACKETS,yyvsp[0],yyvsp[0]->context());}
    break;

  case 205:
#line 604 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 210:
#line 616 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = new AST(OP_SEMICOLON,yyvsp[0],yyvsp[0]->context());}
    break;

  case 211:
#line 617 "../../../libs/libFreeMat/Parser.yxx"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2907 "../../../libs/libFreeMat/Parser.cxx"

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


#line 620 "../../../libs/libFreeMat/Parser.yxx"


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
    parsing_filename = fname;
    setLexFile(fp);
    yyparse();
    return parseState();
  }
  
}

