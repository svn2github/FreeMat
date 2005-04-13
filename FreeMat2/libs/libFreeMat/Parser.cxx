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

#define YYDEBUG 1
#define YYSTYPE ParseRHS

#define MSGBUFLEN 2048
static char msgBuffer[MSGBUFLEN];

#include "LexerInterface.hpp"

//extern char* yytext;
extern int yylex(void);

extern int yydebug;
extern const char *parsing_filename;

bool interactiveMode;

#ifdef WIN32
#define snprintf _snprintf
#endif

namespace FreeMat {
  static ASTPtr mainAST;
  static MFunctionDef *mainMDef;

  void yyerror(const char *s) {
     return;
  }

  void chainFunction(MFunctionDef *r) {
     r->nextFunction = NULL;
     r->prevFunction = NULL;
     if (mainMDef == NULL)
	mainMDef = r;
     else {
	r->localFunction = true;
        r->nextFunction = mainMDef->nextFunction;
	if (r->nextFunction)
	  r->nextFunction->prevFunction = r;
	mainMDef->nextFunction = r;
	r->prevFunction = mainMDef;
     }
  }

  std::string decodeline(ParseRHS val) {
    int tokenID;
    int linenumber, colnumber;
    if (val.isToken) 
      tokenID = val.v.i;
    else
      tokenID = val.v.p->context();
    linenumber = tokenID & 0xFFFF;
    char buffer[256];
    sprintf(buffer,"%d",linenumber);
    return(std::string(buffer));
  }
  
  int yyxpt(std::string xStr, ParseRHS val) {
    char buffer[256];
    int addone = 1;
    int tokenID;
    int linenumber, colnumber;
    if (val.isToken) 
      tokenID = val.v.i;
    else
      tokenID = val.v.p->context();
    linenumber = tokenID & 0xFFFF;
    colnumber = tokenID >> 16;
    if (!interactiveMode)
      snprintf(msgBuffer,MSGBUFLEN,
      "Expecting %s\n\tat line %d, column %d of file %s",
       xStr.c_str(),linenumber,colnumber,parsing_filename);
    else
      snprintf(msgBuffer,MSGBUFLEN,"Expecting %s",xStr.c_str());
    throw Exception(msgBuffer);
    return 0;
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
#line 298 "../../../libs/libFreeMat/Parser.cxx"

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
#define YYFINAL  99
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2665

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  49
/* YYNRULES -- Number of rules. */
#define YYNRULES  221
/* YYNRULES -- Number of states. */
#define YYNSTATES  334

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
      59,    60,    51,    49,    64,    50,    71,    52,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    43,    65,
      46,    61,    47,     2,    68,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    62,    53,    63,    57,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    69,    44,    70,    67,     2,     2,     2,
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
       0,     0,     3,     5,     7,     8,    10,    19,    27,    33,
      38,    46,    53,    56,    60,    65,    72,    76,    81,    87,
      95,    97,   100,   103,   106,   111,   114,   117,   121,   126,
     128,   132,   134,   137,   139,   142,   145,   148,   151,   153,
     155,   156,   158,   160,   162,   164,   166,   168,   170,   172,
     174,   176,   178,   180,   182,   184,   186,   189,   192,   195,
     198,   201,   204,   207,   210,   213,   216,   219,   221,   224,
     227,   229,   231,   233,   235,   240,   245,   248,   249,   256,
     263,   265,   267,   269,   271,   273,   275,   277,   278,   280,
     282,   285,   290,   293,   294,   300,   306,   312,   316,   318,
     324,   329,   333,   336,   340,   342,   348,   351,   357,   363,
     366,   372,   376,   379,   380,   382,   384,   387,   390,   393,
     396,   397,   400,   404,   408,   417,   423,   432,   440,   447,
     453,   457,   461,   463,   467,   471,   475,   479,   483,   487,
     491,   495,   499,   503,   507,   511,   515,   519,   523,   527,
     531,   535,   539,   543,   547,   551,   555,   559,   563,   567,
     571,   575,   579,   583,   587,   591,   594,   597,   600,   603,
     607,   611,   615,   619,   622,   625,   629,   633,   636,   638,
     640,   642,   645,   647,   651,   654,   659,   664,   670,   673,
     677,   682,   687,   693,   696,   699,   701,   704,   708,   711,
     715,   719,   723,   726,   731,   733,   735,   740,   745,   748,
     751,   753,   757,   759,   763,   765,   769,   771,   773,   775,
     777,   779
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      73,     0,    -1,    79,    -1,    75,    -1,    -1,     1,    -1,
      19,    76,     3,    59,    77,    60,    94,    79,    -1,    19,
       3,    59,    77,    60,    94,    79,    -1,    19,    76,     3,
      94,    79,    -1,    19,     3,    94,    79,    -1,    19,    76,
       3,    59,    60,    94,    79,    -1,    19,     3,    59,    60,
      94,    79,    -1,    19,     1,    -1,    19,     3,     1,    -1,
      19,     3,    59,     1,    -1,    19,     3,    59,    77,    60,
       1,    -1,    19,    76,     1,    -1,    19,    76,     3,     1,
      -1,    19,    76,     3,    59,     1,    -1,    19,    76,     3,
      59,    77,    60,     1,    -1,    74,    -1,    75,    74,    -1,
      39,    61,    -1,     3,    61,    -1,    62,    77,    63,    61,
      -1,     3,     1,    -1,    62,     1,    -1,    62,    77,     1,
      -1,    62,    77,    63,     1,    -1,    78,    -1,    77,    64,
      78,    -1,     3,    -1,    45,     3,    -1,    80,    -1,    79,
      80,    -1,    81,     5,    -1,    81,     6,    -1,    81,    64,
      -1,   108,    -1,   110,    -1,    -1,   109,    -1,    99,    -1,
      89,    -1,    88,    -1,   101,    -1,   102,    -1,    92,    -1,
      90,    -1,    87,    -1,    86,    -1,    84,    -1,    83,    -1,
      82,    -1,    41,    -1,    42,    -1,     3,     4,    -1,    15,
      15,    -1,     3,    15,    -1,     3,     3,    -1,    82,    15,
      -1,    82,     3,    -1,    82,     4,    -1,    35,    85,    -1,
      35,     1,    -1,    40,    85,    -1,    40,     1,    -1,     3,
      -1,    85,     3,    -1,    85,     1,    -1,    37,    -1,    36,
      -1,    29,    -1,    21,    -1,    30,    79,    91,    17,    -1,
      30,    79,    91,     1,    -1,    31,    79,    -1,    -1,    26,
     110,    93,    95,    98,    17,    -1,    26,   110,    93,    95,
      98,     1,    -1,    64,    -1,     6,    -1,     5,    -1,    65,
      -1,    66,    -1,     6,    -1,     5,    -1,    -1,    96,    -1,
      97,    -1,    96,    97,    -1,    27,   110,    93,    79,    -1,
      28,    79,    -1,    -1,    20,   100,    93,    79,    17,    -1,
      20,   100,    93,    79,     1,    -1,    59,     3,    61,   110,
      60,    -1,     3,    61,   110,    -1,     3,    -1,    59,     3,
      61,   110,     1,    -1,    59,     3,    61,     1,    -1,    59,
       3,     1,    -1,    59,     1,    -1,     3,    61,     1,    -1,
       1,    -1,    23,   110,    93,    79,    17,    -1,    23,     1,
      -1,    23,   110,    93,    79,     1,    -1,    18,   103,   104,
     107,    17,    -1,    18,     1,    -1,    18,   103,   104,   107,
       1,    -1,   110,    93,    79,    -1,   110,     1,    -1,    -1,
     105,    -1,   106,    -1,   105,   106,    -1,    25,   103,    -1,
      25,     1,    -1,    24,    79,    -1,    -1,    24,     1,    -1,
     112,    61,   110,    -1,   112,    61,     1,    -1,    62,   117,
      63,    61,     3,    59,   115,    60,    -1,    62,   117,    63,
      61,     3,    -1,    62,   117,    63,    61,     3,    59,   115,
       1,    -1,    62,   117,    63,    61,     3,    59,     1,    -1,
      62,   117,    63,    61,     3,     1,    -1,    62,   117,    63,
      61,     1,    -1,   110,    43,   110,    -1,   110,    43,     1,
      -1,   111,    -1,   110,    49,   110,    -1,   110,    49,     1,
      -1,   110,    50,   110,    -1,   110,    50,     1,    -1,   110,
      51,   110,    -1,   110,    51,     1,    -1,   110,    52,   110,
      -1,   110,    52,     1,    -1,   110,    53,   110,    -1,   110,
      53,     1,    -1,   110,    44,   110,    -1,   110,    44,     1,
      -1,   110,    45,   110,    -1,   110,    45,     1,    -1,   110,
      46,   110,    -1,   110,    46,     1,    -1,   110,     7,   110,
      -1,   110,     7,     1,    -1,   110,    47,   110,    -1,   110,
      47,     1,    -1,   110,     8,   110,    -1,   110,     8,     1,
      -1,   110,     9,   110,    -1,   110,     9,     1,    -1,   110,
      48,   110,    -1,   110,    48,     1,    -1,   110,    10,   110,
      -1,   110,    10,     1,    -1,   110,    11,   110,    -1,   110,
      11,     1,    -1,   110,    12,   110,    -1,   110,    12,     1,
      -1,    50,   110,    -1,    49,   110,    -1,    67,   110,    -1,
      67,     1,    -1,   110,    57,   110,    -1,   110,    57,     1,
      -1,   110,    13,   110,    -1,   110,    13,     1,    -1,   110,
      58,    -1,   110,    14,    -1,    59,   110,    60,    -1,    59,
     110,     1,    -1,    59,     1,    -1,     4,    -1,    15,    -1,
      22,    -1,    68,     3,    -1,   112,    -1,    62,   117,    63,
      -1,    62,     1,    -1,    62,   118,   117,    63,    -1,    62,
     117,   118,    63,    -1,    62,   118,   117,   118,    63,    -1,
      62,    63,    -1,    69,   116,    70,    -1,    69,   118,   116,
      70,    -1,    69,   116,   118,    70,    -1,    69,   118,   116,
     118,    70,    -1,    69,    70,    -1,    69,     1,    -1,     3,
      -1,   112,   113,    -1,    59,   115,    60,    -1,    59,    60,
      -1,    59,   115,     1,    -1,    69,   115,    70,    -1,    69,
     115,     1,    -1,    71,     3,    -1,    71,    59,   110,    60,
      -1,   110,    -1,    43,    -1,    52,     3,    61,   110,    -1,
      52,     3,    61,     1,    -1,    52,     3,    -1,    52,     1,
      -1,   114,    -1,   115,    64,   114,    -1,   120,    -1,   116,
     118,   120,    -1,   120,    -1,   117,   118,   120,    -1,     6,
      -1,     5,    -1,    64,    -1,    66,    -1,   110,    -1,   120,
     119,   110,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   149,   149,   150,   150,   151,   155,   165,   174,   183,
     191,   200,   208,   209,   211,   212,   213,   214,   216,   217,
     221,   222,   226,   227,   228,   229,   230,   231,   232,   236,
     237,   241,   241,   252,   253,   257,   261,   265,   272,   273,
     274,   275,   276,   277,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   288,   292,   293,   294,   295,
     296,   297,   298,   311,   312,   316,   317,   321,   322,   323,
     327,   331,   335,   338,   342,   344,   349,   350,   354,   359,
     365,   365,   365,   365,   365,   369,   369,   374,   375,   379,
     382,   388,   394,   397,   403,   406,   411,   412,   413,   415,
     416,   417,   418,   419,   420,   424,   427,   428,   433,   437,
     438,   442,   445,   449,   450,   454,   457,   463,   466,   469,
     472,   473,   477,   478,   482,   486,   490,   492,   494,   496,
     501,   502,   503,   504,   505,   506,   507,   508,   509,   510,
     511,   512,   513,   514,   515,   516,   517,   518,   519,   520,
     521,   522,   523,   524,   525,   526,   527,   528,   529,   530,
     531,   532,   533,   534,   535,   536,   537,   538,   539,   540,
     541,   542,   543,   544,   545,   546,   547,   548,   552,   553,
     554,   555,   556,   557,   558,   559,   560,   561,   562,   563,
     564,   565,   566,   567,   568,   571,   572,   576,   577,   578,
     579,   580,   581,   582,   586,   587,   588,   589,   590,   591,
     595,   596,   600,   601,   605,   606,   610,   610,   614,   614,
     618,   619
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
  "'~'", "'@'", "'{'", "'}'", "'.'", "$accept", "program", "functionDef",
  "functionDefList", "returnDeclaration", "argumentList", "argument",
  "statementList", "statement", "statementType", "specialCall",
  "persistentStatement", "globalStatement", "identList", "returnStatement",
  "keyboardStatement", "continueStatement", "breakStatement",
  "tryStatement", "optionalCatch", "switchStatement",
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
      41,    61,    91,    93,    44,    59,    35,   126,    64,   123,
     125,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    72,    73,    73,    73,    73,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      75,    75,    76,    76,    76,    76,    76,    76,    76,    77,
      77,    78,    78,    79,    79,    80,    80,    80,    81,    81,
      81,    81,    81,    81,    81,    81,    81,    81,    81,    81,
      81,    81,    81,    81,    81,    81,    82,    82,    82,    82,
      82,    82,    82,    83,    83,    84,    84,    85,    85,    85,
      86,    87,    88,    89,    90,    90,    91,    91,    92,    92,
      93,    93,    93,    93,    93,    94,    94,    95,    95,    96,
      96,    97,    98,    98,    99,    99,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   101,   101,   101,   102,   102,
     102,   103,   103,   104,   104,   105,   105,   106,   106,   107,
     107,   107,   108,   108,   109,   109,   109,   109,   109,   109,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   112,   112,   113,   113,   113,
     113,   113,   113,   113,   114,   114,   114,   114,   114,   114,
     115,   115,   116,   116,   117,   117,   118,   118,   119,   119,
     120,   120
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     0,     1,     8,     7,     5,     4,
       7,     6,     2,     3,     4,     6,     3,     4,     5,     7,
       1,     2,     2,     2,     4,     2,     2,     3,     4,     1,
       3,     1,     2,     1,     2,     2,     2,     2,     1,     1,
       0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     1,     2,     2,
       1,     1,     1,     1,     4,     4,     2,     0,     6,     6,
       1,     1,     1,     1,     1,     1,     1,     0,     1,     1,
       2,     4,     2,     0,     5,     5,     5,     3,     1,     5,
       4,     3,     2,     3,     1,     5,     2,     5,     5,     2,
       5,     3,     2,     0,     1,     1,     2,     2,     2,     2,
       0,     2,     3,     3,     8,     5,     8,     7,     6,     5,
       3,     3,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     2,     3,
       3,     3,     3,     2,     2,     3,     3,     2,     1,     1,
       1,     2,     1,     3,     2,     4,     4,     5,     2,     3,
       4,     4,     5,     2,     2,     1,     2,     3,     2,     3,
       3,     3,     2,     4,     1,     1,     4,     4,     2,     2,
       1,     3,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     5,   195,   178,   179,     0,     0,     0,    73,   180,
       0,     0,    72,    40,     0,    71,    70,     0,    54,    55,
       0,     0,     0,     0,     0,     0,     0,     0,    20,     3,
      40,    33,     0,    53,    52,    51,    50,    49,    44,    43,
      48,    47,    42,    45,    46,    38,    41,    39,   132,   182,
      59,    56,    58,    57,   109,   195,   179,     0,   113,     0,
     182,    12,     0,     0,     0,     0,   104,    98,     0,     0,
     106,     0,     0,    40,    64,    67,     0,    66,     0,   166,
     165,   177,     0,   184,   217,   216,   188,   220,     0,     0,
     214,   168,   167,   181,   194,   193,     0,     0,   212,     1,
      21,    34,    35,    36,    37,    61,    62,    60,     0,     0,
       0,     0,     0,     0,     0,   174,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   173,     0,
       0,     0,     0,   196,     0,     0,   120,   114,   115,   112,
      82,    81,    80,    83,    84,    40,    13,    86,    85,     0,
      23,    40,    22,    26,    31,     0,     0,    29,    16,     0,
       0,   102,     0,    40,    40,    87,    40,     0,    69,    68,
     176,   175,   183,     0,     0,   218,   219,     0,   189,     0,
       0,   150,   149,   154,   153,   156,   155,   160,   159,   162,
     161,   164,   163,   172,   171,   131,   130,   144,   143,   146,
     145,   148,   147,   152,   151,   158,   157,   134,   133,   136,
     135,   138,   137,   140,   139,   142,   141,   170,   169,   205,
       0,   198,   204,   210,     0,   123,   122,     0,   202,     0,
     183,   118,   117,     0,     0,   116,   111,    14,     0,     0,
      40,    32,    27,     0,     0,    17,     0,    40,   103,    97,
     101,     0,     0,     0,     0,    93,    88,    89,    40,    75,
      74,     0,   186,   215,   185,     0,   221,   191,   213,   190,
       0,   209,   208,   199,   197,     0,   201,   200,     0,   121,
      40,   110,   108,    40,     0,    28,    24,    30,    18,     0,
       0,    40,   100,     0,    95,    94,   107,   105,     0,    40,
       0,    90,   129,     0,   187,   192,     0,   211,   203,    40,
      15,    40,    40,     0,    99,    96,    40,    40,    79,    78,
     128,     0,   207,   206,    40,    40,    19,    40,    91,   127,
       0,    40,   126,   124
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    27,    28,    29,    65,   156,   157,    30,    31,    32,
      33,    34,    35,    76,    36,    37,    38,    39,    40,   167,
      41,   145,   151,   255,   256,   257,   300,    42,    69,    43,
      44,    58,   136,   137,   138,   234,    45,    46,    47,    48,
      60,   133,   223,   224,    96,    88,    89,   177,    90
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -149
static const short yypact[] =
{
     361,  -149,    54,  -149,    10,  1261,   181,    39,  -149,  -149,
    1300,   516,  -149,  2197,    58,  -149,  -149,   146,  -149,  -149,
     516,   516,  1330,  1228,  1360,    53,    33,    16,  -149,    34,
     760,  -149,   174,   127,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  2479,  -149,   207,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  1228,    52,  2305,
      -9,  -149,   216,     3,   253,   191,  -149,    27,   256,   163,
    -149,  2329,  2329,  1015,  -149,  -149,    40,  -149,    65,   250,
     250,  -149,  2389,  -149,  -149,  -149,  -149,  2479,   134,   516,
     -40,  -149,   250,  -149,  -149,  -149,    23,   516,   -40,  -149,
    -149,  -149,  -149,  -149,  -149,  -149,  -149,  -149,  1390,  1420,
    1450,  1480,  1510,  1540,  1570,  -149,  1600,  1630,  1660,  1690,
    1720,  1750,  1780,  1810,  1840,  1870,  1900,  1930,  -149,  2226,
    1960,  2240,    78,  -149,   198,  1990,    75,    52,  -149,  -149,
    -149,  -149,  -149,  -149,  -149,  2197,   270,  -149,  -149,   153,
    -149,  2197,  -149,  -149,  -149,   145,    21,  -149,  -149,   236,
    2020,  -149,    13,  2197,  2197,   150,  2197,    32,  -149,  -149,
    -149,  -149,   100,   156,   226,  -149,  -149,   516,  -149,   140,
      26,  -149,  2607,  -149,  2607,  -149,  2607,  -149,   250,  -149,
     250,  -149,   250,  -149,   250,  -149,  2531,  -149,  2545,  -149,
    2558,  -149,  2607,  -149,  2607,  -149,  2607,  -149,   801,  -149,
     801,  -149,   250,  -149,   250,  -149,   250,  -149,   250,  -149,
     281,  -149,  2479,  -149,    86,  -149,  2479,     8,  -149,   516,
    -149,  -149,  -149,   958,    46,  -149,  2140,  -149,   195,   123,
     418,  -149,  -149,    64,     7,  -149,   190,  2197,  -149,  2479,
    -149,  2050,   820,   889,   516,   144,   150,  -149,  1072,  -149,
    -149,   293,  -149,   -40,  -149,   459,  2479,  -149,   -40,  -149,
     402,  -149,   173,  -149,  -149,  2240,  -149,  -149,  2461,  -149,
    1129,  -149,  -149,  2197,   280,  -149,  -149,  -149,  -149,   195,
     210,   475,  -149,  2407,  -149,  -149,  -149,  -149,  2329,  2197,
      50,  -149,  -149,    74,  -149,  -149,  2080,  -149,  -149,   532,
    -149,  2197,  2197,   287,  -149,  -149,  2197,  1186,  -149,  -149,
    -149,  1257,  -149,  2479,   589,   646,  -149,  2197,  2140,  -149,
     152,   703,  -149,  -149
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -149,  -149,   166,  -149,  -149,  -134,   -33,    22,    61,  -149,
    -149,  -149,  -149,   197,  -149,  -149,  -149,  -149,  -149,  -149,
    -149,   -68,  -148,  -149,  -149,   -17,  -149,  -149,  -149,  -149,
    -149,   110,  -149,  -149,   109,  -149,  -149,  -149,    -3,  -149,
       0,  -149,   -15,  -125,   168,   -45,     1,  -149,   -21
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -126
static const short yytable[] =
{
      49,   163,    59,   164,   165,    98,   227,    71,    72,   276,
     154,   247,   134,    49,   250,   239,    99,    79,    80,    82,
      87,    92,   242,    87,   175,    53,   176,    97,    84,    85,
      49,    84,    85,   259,    94,    73,    55,     3,    84,    85,
      66,   168,    67,   169,   174,   -63,   -63,   281,    56,   260,
     129,   318,   155,     6,    87,     9,    93,    50,    51,    74,
     131,    75,   132,   282,   152,   285,   168,   319,   169,    52,
     -65,   -65,   275,    49,   251,   320,    98,   135,   277,  -125,
    -125,   228,    20,    21,   243,   244,    87,   273,   160,   173,
     283,   101,    22,   178,    87,    57,   269,   179,    68,   233,
      24,    25,    26,    95,   -63,   182,   184,   186,   188,   190,
     192,   194,   290,   196,   198,   200,   202,   204,   206,   208,
     210,   212,   214,   216,   218,   286,   222,   226,   222,   -65,
     105,   106,    59,   321,   101,   173,   311,   229,  -125,    84,
      85,   312,   107,    55,     3,    49,   274,    77,   241,    75,
     275,    49,   263,   332,   237,    56,   154,   249,   268,    55,
       3,   261,     9,    49,    49,   327,    49,   236,   140,   141,
      87,    56,   299,   240,   266,   265,    87,   254,     9,   102,
     103,   270,    61,   284,    62,   252,   253,   244,   258,    20,
      21,   288,   158,   154,   159,   100,   330,   172,   155,    22,
     147,   148,    57,    84,    85,    20,    21,    24,    25,    26,
     267,   287,   333,   238,    78,    22,   275,   146,    57,   262,
      63,   147,   148,    24,    25,    26,   278,   142,   143,   144,
     316,    84,    85,    49,   306,   155,    49,   245,   104,   301,
      49,   147,   148,    64,   263,   232,   235,    49,   293,   268,
     289,   298,    49,    49,   153,   280,   154,   161,    49,   162,
     307,   230,    87,   114,   115,   180,   129,    87,   130,   291,
     313,   -25,   222,   -25,   244,   149,   131,   150,   132,     0,
      49,   310,   271,    49,   272,   147,   148,     0,   326,   264,
       0,    49,   147,   148,   302,   246,   303,   101,   155,    49,
       0,   101,     0,   323,     0,   309,     0,   127,   128,    49,
       0,    49,    49,   101,   101,     0,    49,    49,   222,   101,
       0,   317,     0,     0,    49,    49,     0,    49,    49,     0,
       0,    49,     0,   324,   325,     0,     0,     0,   328,     0,
       0,   101,     0,     0,     0,     0,     0,     0,     0,   331,
       0,     0,   101,     0,     0,     0,     0,     0,     0,     0,
       0,    -4,     1,     0,     2,     3,   -40,   -40,     0,     0,
     101,     0,     0,     0,     0,     0,     4,     0,   101,     5,
       6,     7,     8,     9,    10,   101,   101,    11,     0,   101,
      12,    13,   101,     0,     0,     0,    14,    15,    16,     0,
       0,    17,    18,    19,     0,    55,     3,     0,     0,     0,
      20,    21,     0,     0,     0,     0,     0,    56,    -9,     0,
      22,     2,     3,    23,     9,   -40,     0,     0,    24,    25,
      26,     0,     0,     4,     0,     0,     5,    -9,     7,     8,
       9,    10,     0,     0,    11,     0,     0,    12,    13,     0,
       0,    20,    21,    14,    15,    16,     0,     0,    17,    18,
      19,    22,    55,     3,    57,     0,     0,    20,    21,    24,
      25,    26,   305,     0,    56,    -8,     0,    22,     2,     3,
      23,     9,     0,     0,     0,    24,    25,    26,     0,     0,
       4,     0,     0,     5,    -8,     7,     8,     9,    10,     0,
       0,    11,     0,     0,    12,    13,     0,     0,    20,    21,
      14,    15,    16,     0,     0,    17,    18,    19,    22,    55,
       3,    57,   304,     0,    20,    21,    24,    25,    26,     0,
       0,    56,   -11,     0,    22,     2,     3,    23,     9,     0,
       0,     0,    24,    25,    26,     0,     0,     4,     0,     0,
       5,   -11,     7,     8,     9,    10,     0,     0,    11,     0,
       0,    12,    13,     0,     0,    20,    21,    14,    15,    16,
       0,     0,    17,    18,    19,    22,     0,     0,    57,     0,
       0,    20,    21,    24,    25,    26,     0,     0,     0,    -7,
       0,    22,     2,     3,    23,     0,     0,     0,     0,    24,
      25,    26,     0,     0,     4,     0,     0,     5,    -7,     7,
       8,     9,    10,     0,     0,    11,     0,     0,    12,    13,
       0,     0,     0,     0,    14,    15,    16,     0,     0,    17,
      18,    19,     0,     0,     0,     0,     0,     0,    20,    21,
       0,     0,     0,     0,     0,     0,   -10,     0,    22,     2,
       3,    23,     0,     0,     0,     0,    24,    25,    26,     0,
       0,     4,     0,     0,     5,   -10,     7,     8,     9,    10,
       0,     0,    11,     0,     0,    12,    13,     0,     0,     0,
       0,    14,    15,    16,     0,     0,    17,    18,    19,     0,
       0,     0,     0,     0,     0,    20,    21,     0,     0,     0,
       0,     0,     0,    -6,     0,    22,     2,     3,    23,     0,
       0,     0,     0,    24,    25,    26,     0,     0,     4,     0,
       0,     5,    -6,     7,     8,     9,    10,     0,     0,    11,
       0,     0,    12,    13,     0,     0,     0,     0,    14,    15,
      16,     0,     0,    17,    18,    19,     0,     0,     0,     0,
       0,     0,    20,    21,     0,     0,     0,     0,     0,     0,
      -2,     0,    22,     2,     3,    23,     0,     0,     0,     0,
      24,    25,    26,     0,     0,     4,     0,     0,     5,     0,
       7,     8,     9,    10,     0,     0,    11,     0,     0,    12,
      13,     0,     0,     0,     0,    14,    15,    16,     0,     0,
      17,    18,    19,     0,     0,     0,     0,     0,     0,    20,
      21,   111,   112,   113,   114,   115,     0,     0,     0,    22,
       0,   294,    23,     2,     3,   -40,   -40,    24,    25,    26,
       0,     0,     0,     0,     0,     4,     0,   295,     5,     0,
       7,     8,     9,    10,     0,     0,    11,     0,     0,    12,
      13,     0,   124,   125,   126,    14,    15,    16,   127,   128,
      17,    18,    19,     0,     0,     0,     0,     0,     0,    20,
      21,     0,     0,     0,     0,     0,     0,     0,     0,    22,
       0,     0,    23,     0,   -40,     0,     0,    24,    25,    26,
     296,     0,     2,     3,   -40,   -40,     0,     0,     0,     0,
       0,     0,     0,     0,     4,     0,   297,     5,     0,     7,
       8,     9,    10,     0,     0,    11,     0,     0,    12,    13,
       0,     0,     0,     0,    14,    15,    16,     0,     0,    17,
      18,    19,     0,     0,     0,     0,     0,     0,    20,    21,
       0,     0,     0,     0,     0,     0,     0,     0,    22,     0,
       0,    23,     0,   -40,     0,     0,    24,    25,    26,   279,
       0,     2,     3,   -40,   -40,     0,     0,     0,     0,     0,
       0,     0,     0,     4,     0,     0,     5,     0,     7,     8,
       9,    10,     0,     0,    11,     0,     0,    12,    13,     0,
       0,     0,     0,    14,    15,    16,     0,     0,    17,    18,
      19,     0,     0,     0,     0,     0,     0,    20,    21,     0,
       0,     0,     0,     0,     0,     0,   -77,    22,     2,     3,
      23,     0,   -40,     0,     0,    24,    25,    26,     0,     0,
       4,     0,   -77,     5,     0,     7,     8,     9,    10,     0,
       0,    11,     0,     0,    12,    13,   166,     0,     0,     0,
      14,    15,    16,     0,     0,    17,    18,    19,     0,     0,
       0,     0,     0,     0,    20,    21,     0,     0,     0,     0,
       0,     0,     0,   -76,    22,     2,     3,    23,     0,     0,
       0,     0,    24,    25,    26,     0,     0,     4,     0,   -76,
       5,     0,     7,     8,     9,    10,     0,     0,    11,     0,
       0,    12,    13,     0,     0,     0,     0,    14,    15,    16,
       0,     0,    17,    18,    19,     0,     0,     0,     0,     0,
       0,    20,    21,     0,     0,     0,     0,     0,     0,     0,
    -119,    22,     2,     3,    23,     0,     0,     0,     0,    24,
      25,    26,     0,     0,     4,     0,  -119,     5,     0,     7,
       8,     9,    10,     0,     0,    11,     0,     0,    12,    13,
       0,     0,     0,     0,    14,    15,    16,     0,     0,    17,
      18,    19,     0,     0,     0,     0,     0,     0,    20,    21,
       0,     0,     0,     0,     0,     0,     0,   -92,    22,     2,
       3,    23,     0,     0,     0,     0,    24,    25,    26,     0,
       0,     4,     0,   -92,     5,     0,     7,     8,     9,    10,
       0,     0,    11,     0,     0,    12,    13,     0,     0,     0,
       0,    14,    15,    16,     0,     0,    17,    18,    19,    83,
       0,    55,     3,    84,    85,    20,    21,     0,     0,     0,
       0,     0,     0,    56,     0,    22,     0,     0,    23,     0,
       9,     0,     0,    24,    25,    26,     0,     0,   329,     0,
      55,     3,    54,     0,    55,     3,     0,     0,     0,     0,
       0,     0,    56,     0,     0,     0,    56,    20,    21,     9,
       0,     0,     0,     9,     0,     0,     0,    22,     0,     0,
      57,    86,     0,     0,     0,    24,    25,    26,     0,     0,
     219,    70,     0,    55,     3,     0,    20,    21,     0,   220,
      20,    21,     0,     0,     0,    56,    22,     0,     0,    57,
      22,     0,     9,    57,    24,    25,    26,     0,    24,    25,
      26,    81,     0,    55,     3,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,    91,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   181,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   183,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   185,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   187,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   189,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   191,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   193,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   195,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   197,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   199,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   201,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   203,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   205,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   207,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   209,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   211,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   213,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   215,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   217,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   225,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   231,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   248,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   292,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,   322,    57,    55,     3,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,    56,     0,     0,     0,    20,
      21,     0,     9,     0,     0,     0,     0,     0,     0,    22,
       0,     0,    57,     0,     0,     0,     0,    24,    25,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    20,
      21,     0,     0,     0,     0,     0,     0,     0,     0,    22,
       0,     0,    57,     2,     3,   -40,   -40,    24,    25,    26,
       0,     0,     0,     0,     0,     4,     0,     0,     5,     0,
       7,     8,     9,    10,     0,     0,    11,     0,     0,    12,
      13,     0,     0,     0,     0,    14,    15,    16,     0,     0,
      17,    18,    19,     0,     0,     0,     0,     0,     0,    20,
      21,     0,     0,     0,     0,     0,     0,     0,     0,    22,
       2,     3,    23,     0,   -40,     0,     0,    24,    25,    26,
       0,     0,     4,     0,     0,     5,     0,     7,     8,     9,
      10,     0,     0,    11,     0,     0,    12,    13,     0,    55,
       3,     0,    14,    15,    16,     0,     0,    17,    18,    19,
       0,    56,     0,    55,     3,     0,    20,    21,     9,     0,
       0,     0,     0,     0,     0,    56,    22,     0,     0,    23,
       0,     0,     9,     0,    24,    25,    26,     0,     0,   219,
       0,     0,     0,     0,     0,    20,    21,     0,   220,     0,
       0,     0,     0,   219,     0,    22,   221,     0,    57,    20,
      21,     0,   220,    24,    25,    26,     0,     0,     0,    22,
       0,     0,    57,     0,     0,     0,   139,    24,    25,    26,
     140,   141,   108,   109,   110,   111,   112,   113,   114,   115,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   140,   141,   108,   109,   110,   111,
     112,   113,   114,   115,     0,     0,     0,     0,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,     0,
       0,     0,   127,   128,     0,     0,     0,     0,     0,   142,
     143,   144,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,     0,     0,     0,   127,   128,     0,     0,
     170,     0,     0,   142,   143,   144,   108,   109,   110,   111,
     112,   113,   114,   115,     0,     0,     0,     0,   314,     0,
       0,     0,     0,     0,   108,   109,   110,   111,   112,   113,
     114,   115,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,     0,     0,     0,   127,   128,     0,   171,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,     0,     0,     0,   127,   128,     0,   315,   108,   109,
     110,   111,   112,   113,   114,   115,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   108,   109,   110,   111,
     112,   113,   114,   115,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,     0,     0,     0,   127,   128,
       0,   308,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,     0,     0,     0,   127,   128,   108,   109,
     110,   111,   112,   113,   114,   115,     0,     0,     0,     0,
       0,     0,   108,   109,   110,   111,   112,   113,   114,   115,
       0,     0,     0,     0,     0,   108,   109,   110,   111,   112,
     113,   114,   115,     0,     0,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,     0,     0,     0,   127,   128,
     118,   119,   120,   121,   122,   123,   124,   125,   126,     0,
       0,     0,   127,   128,   119,   120,   121,   122,   123,   124,
     125,   126,     0,     0,     0,   127,   128,   111,   112,   113,
     114,   115,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   122,   123,   124,   125,
     126,     0,     0,     0,   127,   128
};

static const short yycheck[] =
{
       0,    69,     5,    71,    72,    26,   131,    10,    11,     1,
       3,   159,    57,    13,     1,   149,     0,    20,    21,    22,
      23,    24,     1,    26,    64,    15,    66,    26,     5,     6,
      30,     5,     6,     1,     1,    13,     3,     4,     5,     6,
       1,     1,     3,     3,    89,     5,     6,     1,    15,    17,
      59,     1,    45,    19,    57,    22,     3,     3,     4,     1,
      69,     3,    71,    17,    61,     1,     1,    17,     3,    15,
       5,     6,    64,    73,    61,     1,    97,    25,    70,     5,
       6,     3,    49,    50,    63,    64,    89,     1,    61,    88,
     238,    30,    59,    70,    97,    62,    70,    96,    59,    24,
      67,    68,    69,    70,    64,   108,   109,   110,   111,   112,
     113,   114,   246,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,    61,   129,   130,   131,    64,
       3,     4,   135,    59,    73,   134,   284,    59,    64,     5,
       6,   289,    15,     3,     4,   145,    60,     1,     3,     3,
      64,   151,   173,     1,     1,    15,     3,   160,   179,     3,
       4,    61,    22,   163,   164,   313,   166,   145,     5,     6,
     173,    15,    28,   151,   177,   174,   179,    27,    22,     5,
       6,   180,     1,    60,     3,   163,   164,    64,   166,    49,
      50,     1,     1,     3,     3,    29,   321,    63,    45,    59,
       5,     6,    62,     5,     6,    49,    50,    67,    68,    69,
      70,   244,    60,    60,    17,    59,    64,     1,    62,    63,
      39,     5,     6,    67,    68,    69,   229,    64,    65,    66,
     298,     5,     6,   233,    61,    45,   236,     1,    64,   256,
     240,     5,     6,    62,   265,   135,   137,   247,   251,   270,
      60,   254,   252,   253,     1,   233,     3,     1,   258,     3,
     275,    63,   265,    13,    14,    97,    59,   270,    61,   247,
      60,     1,   275,     3,    64,    59,    69,    61,    71,    -1,
     280,     1,     1,   283,     3,     5,     6,    -1,     1,    63,
      -1,   291,     5,     6,     1,    59,     3,   236,    45,   299,
      -1,   240,    -1,   306,    -1,   283,    -1,    57,    58,   309,
      -1,   311,   312,   252,   253,    -1,   316,   317,   321,   258,
      -1,   299,    -1,    -1,   324,   325,    -1,   327,   328,    -1,
      -1,   331,    -1,   311,   312,    -1,    -1,    -1,   316,    -1,
      -1,   280,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   327,
      -1,    -1,   291,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     0,     1,    -1,     3,     4,     5,     6,    -1,    -1,
     309,    -1,    -1,    -1,    -1,    -1,    15,    -1,   317,    18,
      19,    20,    21,    22,    23,   324,   325,    26,    -1,   328,
      29,    30,   331,    -1,    -1,    -1,    35,    36,    37,    -1,
      -1,    40,    41,    42,    -1,     3,     4,    -1,    -1,    -1,
      49,    50,    -1,    -1,    -1,    -1,    -1,    15,     0,    -1,
      59,     3,     4,    62,    22,    64,    -1,    -1,    67,    68,
      69,    -1,    -1,    15,    -1,    -1,    18,    19,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      -1,    49,    50,    35,    36,    37,    -1,    -1,    40,    41,
      42,    59,     3,     4,    62,    -1,    -1,    49,    50,    67,
      68,    69,    70,    -1,    15,     0,    -1,    59,     3,     4,
      62,    22,    -1,    -1,    -1,    67,    68,    69,    -1,    -1,
      15,    -1,    -1,    18,    19,    20,    21,    22,    23,    -1,
      -1,    26,    -1,    -1,    29,    30,    -1,    -1,    49,    50,
      35,    36,    37,    -1,    -1,    40,    41,    42,    59,     3,
       4,    62,    63,    -1,    49,    50,    67,    68,    69,    -1,
      -1,    15,     0,    -1,    59,     3,     4,    62,    22,    -1,
      -1,    -1,    67,    68,    69,    -1,    -1,    15,    -1,    -1,
      18,    19,    20,    21,    22,    23,    -1,    -1,    26,    -1,
      -1,    29,    30,    -1,    -1,    49,    50,    35,    36,    37,
      -1,    -1,    40,    41,    42,    59,    -1,    -1,    62,    -1,
      -1,    49,    50,    67,    68,    69,    -1,    -1,    -1,     0,
      -1,    59,     3,     4,    62,    -1,    -1,    -1,    -1,    67,
      68,    69,    -1,    -1,    15,    -1,    -1,    18,    19,    20,
      21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,
      -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,
      41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,
      -1,    -1,    -1,    -1,    -1,    -1,     0,    -1,    59,     3,
       4,    62,    -1,    -1,    -1,    -1,    67,    68,    69,    -1,
      -1,    15,    -1,    -1,    18,    19,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,
      -1,    35,    36,    37,    -1,    -1,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    -1,    -1,
      -1,    -1,    -1,     0,    -1,    59,     3,     4,    62,    -1,
      -1,    -1,    -1,    67,    68,    69,    -1,    -1,    15,    -1,
      -1,    18,    19,    20,    21,    22,    23,    -1,    -1,    26,
      -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,
       0,    -1,    59,     3,     4,    62,    -1,    -1,    -1,    -1,
      67,    68,    69,    -1,    -1,    15,    -1,    -1,    18,    -1,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    10,    11,    12,    13,    14,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,     5,     6,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    17,    18,    -1,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    51,    52,    53,    35,    36,    37,    57,    58,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,    -1,    64,    -1,    -1,    67,    68,    69,
       1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    15,    -1,    17,    18,    -1,    20,
      21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,
      -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,
      41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,
      -1,    62,    -1,    64,    -1,    -1,    67,    68,    69,     1,
      -1,     3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,    -1,    -1,    18,    -1,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    59,     3,     4,
      62,    -1,    64,    -1,    -1,    67,    68,    69,    -1,    -1,
      15,    -1,    17,    18,    -1,    20,    21,    22,    23,    -1,
      -1,    26,    -1,    -1,    29,    30,    31,    -1,    -1,    -1,
      35,    36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    49,    50,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    59,     3,     4,    62,    -1,    -1,
      -1,    -1,    67,    68,    69,    -1,    -1,    15,    -1,    17,
      18,    -1,    20,    21,    22,    23,    -1,    -1,    26,    -1,
      -1,    29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,
      -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       1,    59,     3,     4,    62,    -1,    -1,    -1,    -1,    67,
      68,    69,    -1,    -1,    15,    -1,    17,    18,    -1,    20,
      21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,
      -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,
      41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    59,     3,
       4,    62,    -1,    -1,    -1,    -1,    67,    68,    69,    -1,
      -1,    15,    -1,    17,    18,    -1,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,
      -1,    35,    36,    37,    -1,    -1,    40,    41,    42,     1,
      -1,     3,     4,     5,     6,    49,    50,    -1,    -1,    -1,
      -1,    -1,    -1,    15,    -1,    59,    -1,    -1,    62,    -1,
      22,    -1,    -1,    67,    68,    69,    -1,    -1,     1,    -1,
       3,     4,     1,    -1,     3,     4,    -1,    -1,    -1,    -1,
      -1,    -1,    15,    -1,    -1,    -1,    15,    49,    50,    22,
      -1,    -1,    -1,    22,    -1,    -1,    -1,    59,    -1,    -1,
      62,    63,    -1,    -1,    -1,    67,    68,    69,    -1,    -1,
      43,     1,    -1,     3,     4,    -1,    49,    50,    -1,    52,
      49,    50,    -1,    -1,    -1,    15,    59,    -1,    -1,    62,
      59,    -1,    22,    62,    67,    68,    69,    -1,    67,    68,
      69,     1,    -1,     3,     4,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,     1,    62,     3,     4,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,     3,     4,     5,     6,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    18,    -1,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,
       3,     4,    62,    -1,    64,    -1,    -1,    67,    68,    69,
      -1,    -1,    15,    -1,    -1,    18,    -1,    20,    21,    22,
      23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,     3,
       4,    -1,    35,    36,    37,    -1,    -1,    40,    41,    42,
      -1,    15,    -1,     3,     4,    -1,    49,    50,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    59,    -1,    -1,    62,
      -1,    -1,    22,    -1,    67,    68,    69,    -1,    -1,    43,
      -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    52,    -1,
      -1,    -1,    -1,    43,    -1,    59,    60,    -1,    62,    49,
      50,    -1,    52,    67,    68,    69,    -1,    -1,    -1,    59,
      -1,    -1,    62,    -1,    -1,    -1,     1,    67,    68,    69,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    -1,
      -1,    -1,    57,    58,    -1,    -1,    -1,    -1,    -1,    64,
      65,    66,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    -1,    -1,    57,    58,    -1,    -1,
       1,    -1,    -1,    64,    65,    66,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,     1,    -1,
      -1,    -1,    -1,    -1,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    -1,    -1,    57,    58,    -1,    60,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    -1,    57,    58,    -1,    60,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    57,    58,
      -1,    60,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    -1,    -1,    57,    58,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    57,    58,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    -1,
      -1,    -1,    57,    58,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    57,    58,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,
      53,    -1,    -1,    -1,    57,    58
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,    15,    18,    19,    20,    21,    22,
      23,    26,    29,    30,    35,    36,    37,    40,    41,    42,
      49,    50,    59,    62,    67,    68,    69,    73,    74,    75,
      79,    80,    81,    82,    83,    84,    86,    87,    88,    89,
      90,    92,    99,   101,   102,   108,   109,   110,   111,   112,
       3,     4,    15,    15,     1,     3,    15,    62,   103,   110,
     112,     1,     3,    39,    62,    76,     1,     3,    59,   100,
       1,   110,   110,    79,     1,     3,    85,     1,    85,   110,
     110,     1,   110,     1,     5,     6,    63,   110,   117,   118,
     120,     1,   110,     3,     1,    70,   116,   118,   120,     0,
      74,    80,     5,     6,    64,     3,     4,    15,     7,     8,
       9,    10,    11,    12,    13,    14,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    57,    58,    59,
      61,    69,    71,   113,   117,    25,   104,   105,   106,     1,
       5,     6,    64,    65,    66,    93,     1,     5,     6,    59,
      61,    94,    61,     1,     3,    45,    77,    78,     1,     3,
      61,     1,     3,    93,    93,    93,    31,    91,     1,     3,
       1,    60,    63,   118,   117,    64,    66,   119,    70,   118,
     116,     1,   110,     1,   110,     1,   110,     1,   110,     1,
     110,     1,   110,     1,   110,     1,   110,     1,   110,     1,
     110,     1,   110,     1,   110,     1,   110,     1,   110,     1,
     110,     1,   110,     1,   110,     1,   110,     1,   110,    43,
      52,    60,   110,   114,   115,     1,   110,   115,     3,    59,
      63,     1,   103,    24,   107,   106,    79,     1,    60,    77,
      79,     3,     1,    63,    64,     1,    59,    94,     1,   110,
       1,    61,    79,    79,    27,    95,    96,    97,    79,     1,
      17,    61,    63,   120,    63,   118,   110,    70,   120,    70,
     118,     1,     3,     1,    60,    64,     1,    70,   110,     1,
      79,     1,    17,    94,    60,     1,    61,    78,     1,    60,
      77,    79,     1,   110,     1,    17,     1,    17,   110,    28,
      98,    97,     1,     3,    63,    70,    61,   114,    60,    79,
       1,    94,    94,    60,     1,    60,    93,    79,     1,    17,
       1,    59,     1,   110,    79,    79,     1,    94,    79,     1,
     115,    79,     1,    60
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
#line 149 "../../../libs/libFreeMat/Parser.yxx"
    {mainAST = yyvsp[0].v.p;}
    break;

  case 5:
#line 151 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list or function definition",yyvsp[0]);}
    break;

  case 6:
#line 155 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-6].v.p->toStringList();
     r->name = yyvsp[-5].v.p->text;
     r->arguments = yyvsp[-3].v.p->toStringList();
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
     chainFunction(r);
   }
    break;

  case 7:
#line 165 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-5].v.p->text;
     r->arguments = yyvsp[-3].v.p->toStringList();
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
     chainFunction(r);
   }
    break;

  case 8:
#line 174 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-3].v.p->toStringList();
     r->name = yyvsp[-2].v.p->text;
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
     chainFunction(r);
   }
    break;

  case 9:
#line 183 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-2].v.p->text;
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
     chainFunction(r);
   }
    break;

  case 10:
#line 191 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-5].v.p->toStringList();
     r->name = yyvsp[-4].v.p->text;
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
     chainFunction(r);
   }
    break;

  case 11:
#line 200 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-4].v.p->text;
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
     chainFunction(r);
   }
    break;

  case 12:
#line 208 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("legal function name or return declaration after 'function'",yyvsp[-1]);}
    break;

  case 13:
#line 209 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("argument list or statement list after identifier '") + 
	yyvsp[-1].v.p->text + "'",yyvsp[-1]);}
    break;

  case 14:
#line 211 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("(possibly empty) argument list after '('",yyvsp[-1]);}
    break;

  case 15:
#line 212 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list after ')'",yyvsp[-1]);}
    break;

  case 16:
#line 213 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("function name for function declared",yyvsp[-2]);}
    break;

  case 17:
#line 214 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("argument list or statement list following function name :") + 
	yyvsp[-1].v.p->text,yyvsp[-1]);}
    break;

  case 18:
#line 216 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("(possibly empty) argument list after '('",yyvsp[-1]);}
    break;

  case 19:
#line 217 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list after ')'",yyvsp[-1]);}
    break;

  case 22:
#line 226 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 23:
#line 227 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 24:
#line 228 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 25:
#line 229 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after identifier in return declaration",yyvsp[-1]);}
    break;

  case 26:
#line 230 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a valid list of return arguments in return declaration",yyvsp[-1]);}
    break;

  case 27:
#line 231 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching ']' in return declaration for '['",yyvsp[-2]);}
    break;

  case 28:
#line 232 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after return declaration",yyvsp[-1]);}
    break;

  case 29:
#line 236 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 30:
#line 237 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 32:
#line 241 "../../../libs/libFreeMat/Parser.yxx"
    {
	yyval.v.p = yyvsp[0].v.p;
	char *b = (char*) malloc(strlen(yyvsp[0].v.p->text)+2);
	b[0] = '&';
	strcpy(b+1,yyvsp[0].v.p->text);
	yyval.v.p->text = b;
  }
    break;

  case 33:
#line 252 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 34:
#line 253 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 35:
#line 257 "../../../libs/libFreeMat/Parser.yxx"
    {
  	    yyval.v.p = new AST(OP_QSTATEMENT,NULL,yyvsp[0].v.i);
	    yyval.v.p->down = yyvsp[-1].v.p;
	 }
    break;

  case 36:
#line 261 "../../../libs/libFreeMat/Parser.yxx"
    {
	    yyval.v.p = new AST(OP_RSTATEMENT,NULL,yyvsp[0].v.i);
            yyval.v.p->down = yyvsp[-1].v.p;
	 }
    break;

  case 37:
#line 265 "../../../libs/libFreeMat/Parser.yxx"
    {
	    yyval.v.p = new AST(OP_RSTATEMENT,NULL,yyvsp[0].v.i);
	    yyval.v.p->down = yyvsp[-1].v.p;
	 }
    break;

  case 40:
#line 274 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(null_node,"",-1);}
    break;

  case 56:
#line 292 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SCALL,yyvsp[-1].v.p,yyvsp[0].v.p,yyvsp[-1].v.p->context());}
    break;

  case 57:
#line 293 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SCALL,yyvsp[-1].v.p,yyvsp[0].v.p,yyvsp[-1].v.p->context());}
    break;

  case 58:
#line 294 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SCALL,yyvsp[-1].v.p,yyvsp[0].v.p,yyvsp[-1].v.p->context());}
    break;

  case 59:
#line 295 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SCALL,yyvsp[-1].v.p,yyvsp[0].v.p,yyvsp[-1].v.p->context()); }
    break;

  case 60:
#line 296 "../../../libs/libFreeMat/Parser.yxx"
    {yyvsp[-1].v.p->addChild(yyvsp[0].v.p);}
    break;

  case 61:
#line 297 "../../../libs/libFreeMat/Parser.yxx"
    {yyvsp[-1].v.p->addChild(yyvsp[0].v.p);}
    break;

  case 62:
#line 298 "../../../libs/libFreeMat/Parser.yxx"
    {yyvsp[-1].v.p->addChild(yyvsp[0].v.p);}
    break;

  case 63:
#line 311 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p); }
    break;

  case 64:
#line 312 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as persistent",yyvsp[-1]);}
    break;

  case 65:
#line 316 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 66:
#line 317 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as global",yyvsp[-1]);}
    break;

  case 68:
#line 322 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 69:
#line 323 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of valid identifiers",yyvsp[0]);}
    break;

  case 74:
#line 343 "../../../libs/libFreeMat/Parser.yxx"
    { yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-2].v.p); if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);}
    break;

  case 75:
#line 345 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("matching 'end' to 'try' clause from line ") + decodeline(yyvsp[-3]),yyvsp[0]);}
    break;

  case 76:
#line 349 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 77:
#line 350 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 78:
#line 354 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-5].v.p; yyval.v.p->addChild(yyvsp[-4].v.p); 
	  if (yyvsp[-2].v.p != NULL) yyval.v.p->addChild(yyvsp[-2].v.p); 
	  if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 79:
#line 359 "../../../libs/libFreeMat/Parser.yxx"
    {
          yyxpt(std::string("matching 'end' to 'switch' clause from line ") + decodeline(yyvsp[-5]),yyvsp[0]);
        }
    break;

  case 87:
#line 374 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 89:
#line 379 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_CASEBLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());
	}
    break;

  case 90:
#line 382 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 91:
#line 388 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-2].v.p); yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 92:
#line 394 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 93:
#line 397 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = NULL;
	}
    break;

  case 94:
#line 403 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 95:
#line 407 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'for' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 96:
#line 411 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-1].v.p);}
    break;

  case 97:
#line 412 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 98:
#line 413 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p; 
	      yyval.v.p->addChild(new AST(OP_RHS,new AST(id_node,yyvsp[0].v.p->text,yyvsp[0].v.p->context()),yyvsp[0].v.p->context())); }
    break;

  case 99:
#line 415 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-4]);}
    break;

  case 100:
#line 416 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression",yyvsp[-1]);}
    break;

  case 101:
#line 417 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("equals operator after loop index",yyvsp[-1]);}
    break;

  case 102:
#line 418 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier that is the loop variable",yyvsp[-1]);}
    break;

  case 103:
#line 419 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression",yyvsp[-1]);}
    break;

  case 104:
#line 420 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier or assignment (id = expr) after 'for' ",yyvsp[0]);}
    break;

  case 105:
#line 424 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 106:
#line 427 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test expression after 'while'",yyvsp[-1]);}
    break;

  case 107:
#line 429 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'while' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 108:
#line 433 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); if (yyvsp[-2].v.p != NULL) yyval.v.p->addChild(yyvsp[-2].v.p); 
	  if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 109:
#line 437 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("condition expression for 'if'",yyvsp[-1]);}
    break;

  case 110:
#line 438 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'if' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 111:
#line 442 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_CSTAT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-2].v.p->context());
	}
    break;

  case 112:
#line 445 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("valid list of statements after condition",yyvsp[0]);}
    break;

  case 113:
#line 449 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 115:
#line 454 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_ELSEIFBLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());
	}
    break;

  case 116:
#line 457 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 117:
#line 463 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 118:
#line 466 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test condition for 'elseif' clause",yyvsp[-1]);}
    break;

  case 119:
#line 469 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 120:
#line 472 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 121:
#line 473 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list for 'else' clause",yyvsp[-1]);}
    break;

  case 122:
#line 477 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ASSIGN,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 123:
#line 478 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expression in assignment",yyvsp[-1]);}
    break;

  case 124:
#line 482 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[-3].v.p->addChild(new AST(OP_PARENS,yyvsp[-1].v.p,yyvsp[-2].v.i));
	  yyval.v.p = new AST(OP_MULTICALL,yyvsp[-6].v.p,yyvsp[-3].v.p,yyvsp[-7].v.i);
	}
    break;

  case 125:
#line 486 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[0].v.p->addChild(new AST(OP_PARENS,NULL,-1));
	  yyval.v.p = new AST(OP_MULTICALL,yyvsp[-3].v.p,yyvsp[0].v.p,yyvsp[-4].v.i);
	}
    break;

  case 126:
#line 491 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-2]);}
    break;

  case 127:
#line 493 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing list",yyvsp[-1]);}
    break;

  case 128:
#line 495 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("left parenthesis",yyvsp[-1]);}
    break;

  case 129:
#line 497 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier",yyvsp[-1]);}
    break;

  case 130:
#line 501 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_COLON,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 131:
#line 502 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after ':'",yyvsp[-1]);}
    break;

  case 133:
#line 504 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PLUS,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 134:
#line 505 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '+'",yyvsp[-1]);}
    break;

  case 135:
#line 506 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SUBTRACT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 136:
#line 507 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '-'",yyvsp[-1]);}
    break;

  case 137:
#line 508 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_TIMES,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 138:
#line 509 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '*'",yyvsp[-1]);}
    break;

  case 139:
#line 510 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_RDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 140:
#line 511 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '/'",yyvsp[-1]);}
    break;

  case 141:
#line 512 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 142:
#line 513 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '\\'",yyvsp[-1]);}
    break;

  case 143:
#line 514 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_OR,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 144:
#line 515 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '|'",yyvsp[-1]);}
    break;

  case 145:
#line 516 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_AND,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 146:
#line 517 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '&'",yyvsp[-1]);}
    break;

  case 147:
#line 518 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 148:
#line 519 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<'",yyvsp[-1]);}
    break;

  case 149:
#line 520 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 150:
#line 521 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<='",yyvsp[-1]);}
    break;

  case 151:
#line 522 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_GT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 152:
#line 523 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>'",yyvsp[-1]);}
    break;

  case 153:
#line 524 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_GEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 154:
#line 525 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>='",yyvsp[-1]);}
    break;

  case 155:
#line 526 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 156:
#line 527 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '=='",yyvsp[-1]);}
    break;

  case 157:
#line 528 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 158:
#line 529 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '~='",yyvsp[-1]);}
    break;

  case 159:
#line 530 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_TIMES,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 160:
#line 531 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.*'",yyvsp[-1]);}
    break;

  case 161:
#line 532 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_RDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 162:
#line 533 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after './'",yyvsp[-1]);}
    break;

  case 163:
#line 534 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_LDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 164:
#line 535 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.\\'",yyvsp[-1]);}
    break;

  case 165:
#line 536 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NEG,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 166:
#line 537 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 167:
#line 538 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NOT,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 168:
#line 539 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after logical not",yyvsp[0]);}
    break;

  case 169:
#line 540 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_POWER,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 170:
#line 541 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '^'",yyvsp[-1]);}
    break;

  case 171:
#line 542 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_POWER,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 172:
#line 543 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.^'",yyvsp[-1]);}
    break;

  case 173:
#line 544 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_TRANSPOSE,yyvsp[-1].v.p,yyvsp[0].v.i);}
    break;

  case 174:
#line 545 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_TRANSPOSE,yyvsp[-1].v.p,yyvsp[0].v.i);}
    break;

  case 175:
#line 546 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 176:
#line 547 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a right parenthesis after expression to match this one",yyvsp[-2]);}
    break;

  case 177:
#line 548 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after left parenthesis",yyvsp[-1]);}
    break;

  case 181:
#line 555 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ADDRESS,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 182:
#line 556 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_RHS,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 183:
#line 557 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 184:
#line 558 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a matrix definition followed by a right bracket",yyvsp[-1]);}
    break;

  case 185:
#line 559 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 186:
#line 560 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 187:
#line 561 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 188:
#line 562 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EMPTY,NULL,yyvsp[-1].v.i);}
    break;

  case 189:
#line 563 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 190:
#line 564 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 191:
#line 565 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 192:
#line 566 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 193:
#line 567 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EMPTY_CELL,NULL,yyvsp[-1].v.i);}
    break;

  case 194:
#line 568 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a cell-array definition followed by a right brace",yyvsp[-1]);}
    break;

  case 196:
#line 572 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 197:
#line 576 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PARENS,yyvsp[-1].v.p,yyvsp[-2].v.i); }
    break;

  case 198:
#line 577 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PARENS,NULL,yyvsp[-1].v.i); }
    break;

  case 199:
#line 578 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-2]);}
    break;

  case 200:
#line 579 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACES,yyvsp[-1].v.p,yyvsp[-2].v.i); }
    break;

  case 201:
#line 580 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right brace",yyvsp[-2]);}
    break;

  case 202:
#line 581 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT,yyvsp[0].v.p,yyvsp[-1].v.i); }
    break;

  case 203:
#line 582 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOTDYN,yyvsp[-1].v.p,yyvsp[-3].v.i);}
    break;

  case 205:
#line 587 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ALL,NULL,yyvsp[0].v.i);}
    break;

  case 206:
#line 588 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_KEYWORD,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-3].v.i);}
    break;

  case 207:
#line 589 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting expression after '=' in keyword assignment",yyvsp[-1]);}
    break;

  case 208:
#line 590 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_KEYWORD,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 209:
#line 591 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting keyword identifier after '/' in keyword assignment",yyvsp[-1]);}
    break;

  case 211:
#line 596 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addPeer(yyvsp[0].v.p);}
    break;

  case 212:
#line 600 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACES,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 213:
#line 601 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 214:
#line 605 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACKETS,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 215:
#line 606 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 220:
#line 618 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SEMICOLON,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 221:
#line 619 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2972 "../../../libs/libFreeMat/Parser.cxx"

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


#line 622 "../../../libs/libFreeMat/Parser.yxx"


namespace FreeMat {
  
  void resetParser() {
    mainAST = NULL;
    mainMDef = NULL;
  }
  
  ASTPtr getParsedScriptBlock() {
    return mainAST;
  }
  
  MFunctionDef* getParsedFunctionDef() {
    return mainMDef;
  }
  
  ParserState parseState() {
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
//    yydebug = 1; 
    resetParser();
    interactiveMode = false;
    parsing_filename = fname;
    setLexFile(fp);
    yyparse();
    return parseState();
  }
  
}

