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
#define YYFINAL  96
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2488

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  50
/* YYNRULES -- Number of rules. */
#define YYNRULES  217
/* YYNRULES -- Number of states. */
#define YYNSTATES  329

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
     174,   176,   178,   180,   182,   184,   186,   189,   191,   194,
     197,   200,   203,   206,   208,   211,   214,   216,   218,   220,
     222,   227,   232,   235,   236,   243,   250,   252,   254,   256,
     258,   260,   262,   264,   265,   267,   269,   272,   277,   280,
     281,   287,   293,   299,   303,   305,   311,   316,   320,   323,
     327,   329,   335,   338,   344,   350,   353,   359,   363,   366,
     367,   369,   371,   374,   377,   380,   383,   384,   387,   391,
     395,   404,   410,   419,   427,   434,   440,   444,   448,   450,
     454,   458,   462,   466,   470,   474,   478,   482,   486,   490,
     494,   498,   502,   506,   510,   514,   518,   522,   526,   530,
     534,   538,   542,   546,   550,   554,   558,   562,   566,   570,
     574,   578,   581,   584,   587,   590,   594,   598,   602,   606,
     609,   612,   616,   620,   623,   625,   627,   629,   632,   634,
     638,   641,   646,   651,   657,   660,   664,   669,   674,   680,
     683,   686,   688,   691,   695,   698,   702,   706,   710,   713,
     718,   720,   722,   727,   732,   735,   738,   740,   744,   746,
     750,   752,   756,   758,   760,   762,   764,   766
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      73,     0,    -1,    79,    -1,    75,    -1,    -1,     1,    -1,
      19,    76,     3,    59,    77,    60,    95,    79,    -1,    19,
       3,    59,    77,    60,    95,    79,    -1,    19,    76,     3,
      95,    79,    -1,    19,     3,    95,    79,    -1,    19,    76,
       3,    59,    60,    95,    79,    -1,    19,     3,    59,    60,
      95,    79,    -1,    19,     1,    -1,    19,     3,     1,    -1,
      19,     3,    59,     1,    -1,    19,     3,    59,    77,    60,
       1,    -1,    19,    76,     1,    -1,    19,    76,     3,     1,
      -1,    19,    76,     3,    59,     1,    -1,    19,    76,     3,
      59,    77,    60,     1,    -1,    74,    -1,    75,    74,    -1,
      39,    61,    -1,     3,    61,    -1,    62,    77,    63,    61,
      -1,     3,     1,    -1,    62,     1,    -1,    62,    77,     1,
      -1,    62,    77,    63,     1,    -1,    78,    -1,    77,    64,
      78,    -1,     3,    -1,    45,     3,    -1,    80,    -1,    79,
      80,    -1,    81,     5,    -1,    81,     6,    -1,    81,    64,
      -1,   109,    -1,   111,    -1,    -1,   110,    -1,   100,    -1,
      90,    -1,    89,    -1,   102,    -1,   103,    -1,    93,    -1,
      91,    -1,    88,    -1,    87,    -1,    85,    -1,    84,    -1,
      82,    -1,    41,    -1,    42,    -1,    16,    83,    -1,    15,
      -1,    83,    15,    -1,    35,    86,    -1,    35,     1,    -1,
      40,    86,    -1,    40,     1,    -1,     3,    -1,    86,     3,
      -1,    86,     1,    -1,    37,    -1,    36,    -1,    29,    -1,
      21,    -1,    30,    79,    92,    17,    -1,    30,    79,    92,
       1,    -1,    31,    79,    -1,    -1,    26,   111,    94,    96,
      99,    17,    -1,    26,   111,    94,    96,    99,     1,    -1,
      64,    -1,     6,    -1,     5,    -1,    65,    -1,    66,    -1,
       6,    -1,     5,    -1,    -1,    97,    -1,    98,    -1,    97,
      98,    -1,    27,   111,    94,    79,    -1,    28,    79,    -1,
      -1,    20,   101,    94,    79,    17,    -1,    20,   101,    94,
      79,     1,    -1,    59,     3,    61,   111,    60,    -1,     3,
      61,   111,    -1,     3,    -1,    59,     3,    61,   111,     1,
      -1,    59,     3,    61,     1,    -1,    59,     3,     1,    -1,
      59,     1,    -1,     3,    61,     1,    -1,     1,    -1,    23,
     111,    94,    79,    17,    -1,    23,     1,    -1,    23,   111,
      94,    79,     1,    -1,    18,   104,   105,   108,    17,    -1,
      18,     1,    -1,    18,   104,   105,   108,     1,    -1,   111,
      94,    79,    -1,   111,     1,    -1,    -1,   106,    -1,   107,
      -1,   106,   107,    -1,    25,   104,    -1,    25,     1,    -1,
      24,    79,    -1,    -1,    24,     1,    -1,   113,    61,   111,
      -1,   113,    61,     1,    -1,    62,   118,    63,    61,     3,
      59,   116,    60,    -1,    62,   118,    63,    61,     3,    -1,
      62,   118,    63,    61,     3,    59,   116,     1,    -1,    62,
     118,    63,    61,     3,    59,     1,    -1,    62,   118,    63,
      61,     3,     1,    -1,    62,   118,    63,    61,     1,    -1,
     111,    43,   111,    -1,   111,    43,     1,    -1,   112,    -1,
     111,    49,   111,    -1,   111,    49,     1,    -1,   111,    50,
     111,    -1,   111,    50,     1,    -1,   111,    51,   111,    -1,
     111,    51,     1,    -1,   111,    52,   111,    -1,   111,    52,
       1,    -1,   111,    53,   111,    -1,   111,    53,     1,    -1,
     111,    44,   111,    -1,   111,    44,     1,    -1,   111,    45,
     111,    -1,   111,    45,     1,    -1,   111,    46,   111,    -1,
     111,    46,     1,    -1,   111,     7,   111,    -1,   111,     7,
       1,    -1,   111,    47,   111,    -1,   111,    47,     1,    -1,
     111,     8,   111,    -1,   111,     8,     1,    -1,   111,     9,
     111,    -1,   111,     9,     1,    -1,   111,    48,   111,    -1,
     111,    48,     1,    -1,   111,    10,   111,    -1,   111,    10,
       1,    -1,   111,    11,   111,    -1,   111,    11,     1,    -1,
     111,    12,   111,    -1,   111,    12,     1,    -1,    50,   111,
      -1,    49,   111,    -1,    67,   111,    -1,    67,     1,    -1,
     111,    57,   111,    -1,   111,    57,     1,    -1,   111,    13,
     111,    -1,   111,    13,     1,    -1,   111,    58,    -1,   111,
      14,    -1,    59,   111,    60,    -1,    59,   111,     1,    -1,
      59,     1,    -1,     4,    -1,    15,    -1,    22,    -1,    68,
       3,    -1,   113,    -1,    62,   118,    63,    -1,    62,     1,
      -1,    62,   119,   118,    63,    -1,    62,   118,   119,    63,
      -1,    62,   119,   118,   119,    63,    -1,    62,    63,    -1,
      69,   117,    70,    -1,    69,   119,   117,    70,    -1,    69,
     117,   119,    70,    -1,    69,   119,   117,   119,    70,    -1,
      69,    70,    -1,    69,     1,    -1,     3,    -1,   113,   114,
      -1,    59,   116,    60,    -1,    59,    60,    -1,    59,   116,
       1,    -1,    69,   116,    70,    -1,    69,   116,     1,    -1,
      71,     3,    -1,    71,    59,   111,    60,    -1,   111,    -1,
      43,    -1,    52,     3,    61,   111,    -1,    52,     3,    61,
       1,    -1,    52,     3,    -1,    52,     1,    -1,   115,    -1,
     116,    64,   115,    -1,   121,    -1,   117,   119,   121,    -1,
     121,    -1,   118,   119,   121,    -1,     6,    -1,     5,    -1,
      64,    -1,    66,    -1,   111,    -1,   121,   120,   111,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   149,   149,   150,   150,   151,   155,   165,   174,   183,
     191,   200,   208,   209,   211,   212,   213,   214,   216,   217,
     221,   222,   226,   227,   228,   229,   230,   231,   232,   236,
     237,   241,   241,   252,   253,   257,   261,   265,   272,   273,
     274,   275,   276,   277,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   288,   292,   297,   298,   302,
     303,   307,   308,   312,   313,   314,   318,   322,   326,   329,
     333,   335,   340,   341,   345,   350,   356,   356,   356,   356,
     356,   360,   360,   365,   366,   370,   373,   379,   385,   388,
     394,   397,   402,   403,   404,   406,   407,   408,   409,   410,
     411,   415,   418,   419,   424,   428,   429,   433,   436,   440,
     441,   445,   448,   454,   457,   460,   463,   464,   468,   469,
     473,   477,   481,   483,   485,   487,   492,   493,   494,   495,
     496,   497,   498,   499,   500,   501,   502,   503,   504,   505,
     506,   507,   508,   509,   510,   511,   512,   513,   514,   515,
     516,   517,   518,   519,   520,   521,   522,   523,   524,   525,
     526,   527,   528,   529,   530,   531,   532,   533,   534,   535,
     536,   537,   538,   539,   543,   544,   545,   546,   547,   548,
     549,   550,   551,   552,   553,   554,   555,   556,   557,   558,
     559,   562,   563,   567,   568,   569,   570,   571,   572,   573,
     577,   578,   579,   580,   581,   582,   586,   587,   591,   592,
     596,   597,   601,   601,   605,   605,   609,   610
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
      81,    81,    81,    81,    81,    81,    82,    83,    83,    84,
      84,    85,    85,    86,    86,    86,    87,    88,    89,    90,
      91,    91,    92,    92,    93,    93,    94,    94,    94,    94,
      94,    95,    95,    96,    96,    97,    97,    98,    99,    99,
     100,   100,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   102,   102,   102,   103,   103,   103,   104,   104,   105,
     105,   106,   106,   107,   107,   108,   108,   108,   109,   109,
     110,   110,   110,   110,   110,   110,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   113,   113,   114,   114,   114,   114,   114,   114,   114,
     115,   115,   115,   115,   115,   115,   116,   116,   117,   117,
     118,   118,   119,   119,   120,   120,   121,   121
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     1,     0,     1,     8,     7,     5,     4,
       7,     6,     2,     3,     4,     6,     3,     4,     5,     7,
       1,     2,     2,     2,     4,     2,     2,     3,     4,     1,
       3,     1,     2,     1,     2,     2,     2,     2,     1,     1,
       0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     2,
       2,     2,     2,     1,     2,     2,     1,     1,     1,     1,
       4,     4,     2,     0,     6,     6,     1,     1,     1,     1,
       1,     1,     1,     0,     1,     1,     2,     4,     2,     0,
       5,     5,     5,     3,     1,     5,     4,     3,     2,     3,
       1,     5,     2,     5,     5,     2,     5,     3,     2,     0,
       1,     1,     2,     2,     2,     2,     0,     2,     3,     3,
       8,     5,     8,     7,     6,     5,     3,     3,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     2,     2,     3,     3,     3,     3,     2,
       2,     3,     3,     2,     1,     1,     1,     2,     1,     3,
       2,     4,     4,     5,     2,     3,     4,     4,     5,     2,
       2,     1,     2,     3,     2,     3,     3,     3,     2,     4,
       1,     1,     4,     4,     2,     2,     1,     3,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     5,   191,   174,   175,     0,     0,     0,     0,    69,
     176,     0,     0,    68,    40,     0,    67,    66,     0,    54,
      55,     0,     0,     0,     0,     0,     0,     0,     0,    20,
       3,    40,    33,     0,    53,    52,    51,    50,    49,    44,
      43,    48,    47,    42,    45,    46,    38,    41,    39,   128,
     178,    57,    56,   105,     0,   109,     0,   178,    12,     0,
       0,     0,     0,   100,    94,     0,     0,   102,     0,     0,
      40,    60,    63,     0,    62,     0,   162,   161,   173,     0,
     180,   213,   212,   184,   216,     0,     0,   210,   164,   163,
     177,   190,   189,     0,     0,   208,     1,    21,    34,    35,
      36,    37,     0,     0,     0,     0,     0,     0,     0,   170,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   169,     0,     0,     0,     0,   192,    58,     0,
       0,   116,   110,   111,   108,    78,    77,    76,    79,    80,
      40,    13,    82,    81,     0,    23,    40,    22,    26,    31,
       0,     0,    29,    16,     0,     0,    98,     0,    40,    40,
      83,    40,     0,    65,    64,   172,   171,   179,     0,     0,
     214,   215,     0,   185,     0,     0,   146,   145,   150,   149,
     152,   151,   156,   155,   158,   157,   160,   159,   168,   167,
     127,   126,   140,   139,   142,   141,   144,   143,   148,   147,
     154,   153,   130,   129,   132,   131,   134,   133,   136,   135,
     138,   137,   166,   165,   201,     0,   194,   200,   206,     0,
     119,   118,     0,   198,     0,   179,   114,   113,     0,     0,
     112,   107,    14,     0,     0,    40,    32,    27,     0,     0,
      17,     0,    40,    99,    93,    97,     0,     0,     0,     0,
      89,    84,    85,    40,    71,    70,     0,   182,   211,   181,
       0,   217,   187,   209,   186,     0,   205,   204,   195,   193,
       0,   197,   196,     0,   117,    40,   106,   104,    40,     0,
      28,    24,    30,    18,     0,     0,    40,    96,     0,    91,
      90,   103,   101,     0,    40,     0,    86,   125,     0,   183,
     188,     0,   207,   199,    40,    15,    40,    40,     0,    95,
      92,    40,    40,    75,    74,   124,     0,   203,   202,    40,
      40,    19,    40,    87,   123,     0,    40,   122,   120
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    28,    29,    30,    62,   151,   152,    31,    32,    33,
      34,    52,    35,    36,    73,    37,    38,    39,    40,    41,
     162,    42,   140,   146,   250,   251,   252,   295,    43,    66,
      44,    45,    55,   131,   132,   133,   229,    46,    47,    48,
      49,    57,   127,   218,   219,    93,    85,    86,   172,    87
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -142
static const short yypact[] =
{
     331,  -142,  -142,  -142,  -142,    18,  1255,    86,     9,  -142,
    -142,  1270,   600,  -142,  2020,   201,  -142,  -142,   210,  -142,
    -142,   600,   600,  1312,  1240,  1327,    36,   772,    50,  -142,
      34,   730,  -142,   131,  -142,  -142,  -142,  -142,  -142,  -142,
    -142,  -142,  -142,  -142,  -142,  -142,  -142,  -142,  2302,  -142,
      -7,  -142,    46,  -142,  1240,    33,  2128,    95,  -142,   173,
       6,    43,   235,  -142,    21,   248,    32,  -142,  2152,  2152,
    1027,  -142,  -142,    29,  -142,    42,   202,   202,  -142,  2212,
    -142,  -142,  -142,  -142,  2302,   144,   600,   197,  -142,   202,
    -142,  -142,  -142,    11,   600,   197,  -142,  -142,  -142,  -142,
    -142,  -142,  1342,  1384,  1399,  1414,  1456,  1471,  1486,  -142,
    1528,  1543,  1558,  1600,  1615,  1630,  1672,  1687,  1702,  1744,
    1759,  1774,  -142,  2049,  1816,  2063,    26,  -142,  -142,   167,
    1831,    71,    33,  -142,  -142,  -142,  -142,  -142,  -142,  -142,
    2020,   265,  -142,  -142,   132,  -142,  2020,  -142,  -142,  -142,
     128,    14,  -142,  -142,   181,  1846,  -142,    65,  2020,  2020,
     114,  2020,    40,  -142,  -142,  -142,  -142,    90,   486,   191,
    -142,  -142,   600,  -142,   372,    20,  -142,  2430,  -142,  2430,
    -142,  2430,  -142,   202,  -142,   202,  -142,   202,  -142,   202,
    -142,  2354,  -142,  2368,  -142,  2381,  -142,  2430,  -142,  2430,
    -142,  2430,  -142,   813,  -142,   813,  -142,   202,  -142,   202,
    -142,   202,  -142,   202,  -142,   268,  -142,  2302,  -142,    59,
    -142,  2302,    10,  -142,   600,  -142,  -142,  -142,   970,    48,
    -142,  1963,  -142,   271,    -1,   388,  -142,  -142,    78,    39,
    -142,   154,  2020,  -142,  2302,  -142,  1888,   832,   901,   600,
     147,   114,  -142,  1084,  -142,  -142,   269,  -142,   197,  -142,
     543,  2302,  -142,   197,  -142,   429,  -142,    99,  -142,  -142,
    2063,  -142,  -142,  2284,  -142,  1141,  -142,  -142,  2020,   219,
    -142,  -142,  -142,  -142,   271,   125,   445,  -142,  2230,  -142,
    -142,  -142,  -142,  2152,  2020,    55,  -142,  -142,    70,  -142,
    -142,  1903,  -142,  -142,   502,  -142,  2020,  2020,   221,  -142,
    -142,  2020,  1198,  -142,  -142,  -142,   141,  -142,  2302,   559,
     616,  -142,  2020,  1963,  -142,    68,   673,  -142,  -142
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -142,  -142,   158,  -142,  -142,  -117,   -45,    22,   -30,  -142,
    -142,  -142,  -142,  -142,   188,  -142,  -142,  -142,  -142,  -142,
    -142,  -142,   -64,  -141,  -142,  -142,   -39,  -142,  -142,  -142,
    -142,  -142,    89,  -142,  -142,   101,  -142,  -142,  -142,    -3,
    -142,     0,  -142,   -33,  -118,   161,   -31,     1,  -142,   -21
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -122
static const short yytable[] =
{
      50,    98,   158,    56,   159,   160,    95,   222,    68,    69,
      63,   271,    64,   242,    50,   237,    81,    82,    76,    77,
      79,    84,    89,   129,    84,    81,    82,   234,    94,   223,
     163,    50,   164,    51,   -59,   -59,    70,   135,   136,    90,
      98,   254,   149,   163,   148,   164,   149,   -61,   -61,   276,
      96,    84,   123,     7,   124,   169,   313,   255,   130,   279,
     268,   128,   125,   239,   126,   277,   245,   147,    65,   327,
      50,   315,   314,    95,   270,  -121,  -121,   238,   239,   280,
     272,   173,   155,    84,   150,   224,   168,    58,   150,    59,
     264,    84,   278,   -59,   174,   228,   137,   138,   139,   177,
     179,   181,   183,   185,   187,   189,   -61,   191,   193,   195,
     197,   199,   201,   203,   205,   207,   209,   211,   213,   269,
     217,   221,   217,   270,   285,    60,   246,    56,   328,   316,
     168,   236,   270,   232,  -121,   149,    99,   100,   306,   281,
      50,   249,   324,   307,     2,     3,    50,   258,    61,    81,
      82,   256,   244,   263,   123,   283,     4,   149,    50,    50,
     301,    50,   231,    10,   125,    84,   126,   322,   235,   261,
     260,    84,    81,    82,   141,   294,   265,   150,   142,   143,
     247,   248,   240,   253,   214,   308,   142,   143,    97,   239,
      21,    22,   233,   215,   282,   101,    81,    82,   325,   150,
      23,    98,    71,    54,    72,    98,    75,   167,    25,    26,
      27,    74,   296,    72,   284,   108,   109,    98,    98,   227,
     305,   273,   321,    98,   142,   143,   142,   143,    50,   311,
     225,    50,   144,   230,   145,    50,   153,   302,   154,   258,
     241,     0,    50,   288,   263,    98,   293,    50,    50,   156,
     275,   157,     0,    50,   259,   175,    98,    84,     0,   121,
     122,   170,    84,   171,   286,     0,   -25,   217,   -25,   266,
     297,   267,   298,     0,    98,    50,   142,   143,    50,     0,
       0,     0,    98,     0,     0,     0,    50,     0,     0,    98,
      98,     0,     0,    98,    50,     0,    98,     0,   318,     0,
     304,     0,     0,     0,    50,     0,    50,    50,     0,     0,
       0,    50,    50,   217,     0,     0,   312,     0,     0,    50,
      50,     0,    50,    50,     0,     0,    50,     0,   319,   320,
       0,    -4,     1,   323,     2,     3,   -40,   -40,     0,     0,
       0,     0,     0,     0,   326,     0,     4,     5,     0,     6,
       7,     8,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,     0,     0,     0,     0,    15,    16,    17,     0,
       0,    18,    19,    20,     0,     2,     3,     0,     0,     0,
      21,    22,     0,     0,     0,     0,     0,     4,    -9,     0,
      23,     2,     3,    24,    10,   -40,     0,     0,    25,    26,
      27,     0,     0,     4,     5,     0,     6,    -9,     8,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,    21,    22,    15,    16,    17,     0,     0,    18,    19,
      20,    23,     2,     3,    54,     0,     0,    21,    22,    25,
      26,    27,   262,     0,     4,    -8,     0,    23,     2,     3,
      24,    10,     0,     0,     0,    25,    26,    27,     0,     0,
       4,     5,     0,     6,    -8,     8,     9,    10,    11,     0,
       0,    12,     0,     0,    13,    14,     0,     0,    21,    22,
      15,    16,    17,     0,     0,    18,    19,    20,    23,     2,
       3,    54,     0,     0,    21,    22,    25,    26,    27,   300,
       0,     4,   -11,     0,    23,     2,     3,    24,    10,     0,
       0,     0,    25,    26,    27,     0,     0,     4,     5,     0,
       6,   -11,     8,     9,    10,    11,     0,     0,    12,     0,
       0,    13,    14,     0,     0,    21,    22,    15,    16,    17,
       0,     0,    18,    19,    20,    23,     2,     3,    54,   257,
       0,    21,    22,    25,    26,    27,     0,     0,     4,    -7,
       0,    23,     2,     3,    24,    10,     0,     0,     0,    25,
      26,    27,     0,     0,     4,     5,     0,     6,    -7,     8,
       9,    10,    11,     0,     0,    12,     0,     0,    13,    14,
       0,     0,    21,    22,    15,    16,    17,     0,     0,    18,
      19,    20,    23,     2,     3,    54,   299,     0,    21,    22,
      25,    26,    27,     0,     0,     4,   -10,     0,    23,     2,
       3,    24,    10,     0,     0,     0,    25,    26,    27,     0,
       0,     4,     5,     0,     6,   -10,     8,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,     0,    21,
      22,    15,    16,    17,     0,     0,    18,    19,    20,    23,
       0,     0,    54,     0,     0,    21,    22,    25,    26,    27,
       0,     0,     0,    -6,     0,    23,     2,     3,    24,     0,
       0,     0,     0,    25,    26,    27,     0,     0,     4,     5,
       0,     6,    -6,     8,     9,    10,    11,     0,     0,    12,
       0,     0,    13,    14,     0,     0,     0,     0,    15,    16,
      17,     0,     0,    18,    19,    20,     0,     0,     0,     0,
       0,     0,    21,    22,     0,     0,     0,     0,     0,     0,
      -2,     0,    23,     2,     3,    24,     0,     0,     0,     0,
      25,    26,    27,     0,     0,     4,     5,     0,     6,     0,
       8,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,    91,     0,     2,     3,    81,    82,    21,
      22,     0,     0,     0,     0,     0,     0,     4,     0,    23,
       0,     0,    24,     0,    10,     0,     0,    25,    26,    27,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,    22,   105,   106,   107,   108,   109,     0,     0,
       0,    23,     0,   289,    54,     2,     3,   -40,   -40,    25,
      26,    27,    92,     0,     0,     0,     0,     4,     5,   290,
       6,     0,     8,     9,    10,    11,     0,     0,    12,     0,
       0,    13,    14,     0,   118,   119,   120,    15,    16,    17,
     121,   122,    18,    19,    20,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,    24,     0,   -40,     0,     0,    25,
      26,    27,   291,     0,     2,     3,   -40,   -40,     0,     0,
       0,     0,     0,     0,     0,     0,     4,     5,   292,     6,
       0,     8,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,     0,     0,     0,     0,    15,    16,    17,     0,
       0,    18,    19,    20,     0,     0,     0,     0,     0,     0,
      21,    22,     0,     0,     0,     0,     0,     0,     0,     0,
      23,     0,     0,    24,     0,   -40,     0,     0,    25,    26,
      27,   274,     0,     2,     3,   -40,   -40,     0,     0,     0,
       0,     0,     0,     0,     0,     4,     5,     0,     6,     0,
       8,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,     0,     0,     0,     0,   -73,    23,
       2,     3,    24,     0,   -40,     0,     0,    25,    26,    27,
       0,     0,     4,     5,   -73,     6,     0,     8,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,   161,     0,
       0,     0,    15,    16,    17,     0,     0,    18,    19,    20,
       0,     0,     0,     0,     0,     0,    21,    22,     0,     0,
       0,     0,     0,     0,     0,   -72,    23,     2,     3,    24,
       0,     0,     0,     0,    25,    26,    27,     0,     0,     4,
       5,   -72,     6,     0,     8,     9,    10,    11,     0,     0,
      12,     0,     0,    13,    14,     0,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,  -115,    23,     2,     3,    24,     0,     0,     0,
       0,    25,    26,    27,     0,     0,     4,     5,  -115,     6,
       0,     8,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,     0,     0,     0,     0,    15,    16,    17,     0,
       0,    18,    19,    20,     0,     0,     0,     0,     0,     0,
      21,    22,     0,     0,     0,     0,     0,     0,     0,   -88,
      23,     2,     3,    24,     0,     0,     0,     0,    25,    26,
      27,     0,     0,     4,     5,   -88,     6,     0,     8,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,     0,     0,    15,    16,    17,     0,     0,    18,    19,
      20,    80,     0,     2,     3,    81,    82,    21,    22,     0,
       0,     0,     0,     0,     0,     4,    53,    23,     2,     3,
      24,     0,    10,     0,     0,    25,    26,    27,     0,     0,
       4,    67,     0,     2,     3,     0,     0,    10,     0,     0,
       0,     0,     0,     0,     0,     4,     0,     0,     0,    21,
      22,     0,    10,     0,     0,     0,     0,     0,     0,    23,
       0,     0,    54,    83,    21,    22,     0,    25,    26,    27,
       0,     0,     0,    78,    23,     2,     3,    54,     0,    21,
      22,     0,    25,    26,    27,     0,     0,     4,    88,    23,
       2,     3,    54,     0,    10,     0,     0,    25,    26,    27,
       0,     0,     4,   176,     0,     2,     3,     0,     0,    10,
       0,     0,     0,     0,     0,     0,     0,     4,     0,     0,
       0,    21,    22,     0,    10,     0,     0,     0,     0,     0,
       0,    23,     0,     0,    54,     0,    21,    22,     0,    25,
      26,    27,     0,     0,     0,   178,    23,     2,     3,    54,
       0,    21,    22,     0,    25,    26,    27,     0,     0,     4,
     180,    23,     2,     3,    54,     0,    10,     0,     0,    25,
      26,    27,     0,     0,     4,   182,     0,     2,     3,     0,
       0,    10,     0,     0,     0,     0,     0,     0,     0,     4,
       0,     0,     0,    21,    22,     0,    10,     0,     0,     0,
       0,     0,     0,    23,     0,     0,    54,     0,    21,    22,
       0,    25,    26,    27,     0,     0,     0,   184,    23,     2,
       3,    54,     0,    21,    22,     0,    25,    26,    27,     0,
       0,     4,   186,    23,     2,     3,    54,     0,    10,     0,
       0,    25,    26,    27,     0,     0,     4,   188,     0,     2,
       3,     0,     0,    10,     0,     0,     0,     0,     0,     0,
       0,     4,     0,     0,     0,    21,    22,     0,    10,     0,
       0,     0,     0,     0,     0,    23,     0,     0,    54,     0,
      21,    22,     0,    25,    26,    27,     0,     0,     0,   190,
      23,     2,     3,    54,     0,    21,    22,     0,    25,    26,
      27,     0,     0,     4,   192,    23,     2,     3,    54,     0,
      10,     0,     0,    25,    26,    27,     0,     0,     4,   194,
       0,     2,     3,     0,     0,    10,     0,     0,     0,     0,
       0,     0,     0,     4,     0,     0,     0,    21,    22,     0,
      10,     0,     0,     0,     0,     0,     0,    23,     0,     0,
      54,     0,    21,    22,     0,    25,    26,    27,     0,     0,
       0,   196,    23,     2,     3,    54,     0,    21,    22,     0,
      25,    26,    27,     0,     0,     4,   198,    23,     2,     3,
      54,     0,    10,     0,     0,    25,    26,    27,     0,     0,
       4,   200,     0,     2,     3,     0,     0,    10,     0,     0,
       0,     0,     0,     0,     0,     4,     0,     0,     0,    21,
      22,     0,    10,     0,     0,     0,     0,     0,     0,    23,
       0,     0,    54,     0,    21,    22,     0,    25,    26,    27,
       0,     0,     0,   202,    23,     2,     3,    54,     0,    21,
      22,     0,    25,    26,    27,     0,     0,     4,   204,    23,
       2,     3,    54,     0,    10,     0,     0,    25,    26,    27,
       0,     0,     4,   206,     0,     2,     3,     0,     0,    10,
       0,     0,     0,     0,     0,     0,     0,     4,     0,     0,
       0,    21,    22,     0,    10,     0,     0,     0,     0,     0,
       0,    23,     0,     0,    54,     0,    21,    22,     0,    25,
      26,    27,     0,     0,     0,   208,    23,     2,     3,    54,
       0,    21,    22,     0,    25,    26,    27,     0,     0,     4,
     210,    23,     2,     3,    54,     0,    10,     0,     0,    25,
      26,    27,     0,     0,     4,   212,     0,     2,     3,     0,
       0,    10,     0,     0,     0,     0,     0,     0,     0,     4,
       0,     0,     0,    21,    22,     0,    10,     0,     0,     0,
       0,     0,     0,    23,     0,     0,    54,     0,    21,    22,
       0,    25,    26,    27,     0,     0,     0,   220,    23,     2,
       3,    54,     0,    21,    22,     0,    25,    26,    27,     0,
       0,     4,   226,    23,     2,     3,    54,     0,    10,     0,
       0,    25,    26,    27,     0,     0,     4,   243,     0,     2,
       3,     0,     0,    10,     0,     0,     0,     0,     0,     0,
       0,     4,     0,     0,     0,    21,    22,     0,    10,     0,
       0,     0,     0,     0,     0,    23,     0,     0,    54,     0,
      21,    22,     0,    25,    26,    27,     0,     0,     0,   287,
      23,     2,     3,    54,     0,    21,    22,     0,    25,    26,
      27,     0,     0,     4,   317,    23,     2,     3,    54,     0,
      10,     0,     0,    25,    26,    27,     0,     0,     4,     0,
       0,     0,     0,     0,     0,    10,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,     0,
      54,     0,    21,    22,     0,    25,    26,    27,     0,     0,
       0,     0,    23,     0,     0,    54,     2,     3,   -40,   -40,
      25,    26,    27,     0,     0,     0,     0,     0,     4,     5,
       0,     6,     0,     8,     9,    10,    11,     0,     0,    12,
       0,     0,    13,    14,     0,     0,     0,     0,    15,    16,
      17,     0,     0,    18,    19,    20,     0,     0,     0,     0,
       0,     0,    21,    22,     0,     0,     0,     0,     0,     0,
       0,     0,    23,     2,     3,    24,     0,   -40,     0,     0,
      25,    26,    27,     0,     0,     4,     5,     0,     6,     0,
       8,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     2,     3,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     4,     0,     2,     3,     0,    21,
      22,    10,     0,     0,     0,     0,     0,     0,     4,    23,
       0,     0,    24,     0,     0,    10,     0,    25,    26,    27,
       0,     0,   214,     0,     0,     0,     0,     0,    21,    22,
       0,   215,     0,     0,     0,     0,   214,     0,    23,   216,
       0,    54,    21,    22,     0,   215,    25,    26,    27,     0,
       0,     0,    23,     0,     0,    54,     0,     0,     0,   134,
      25,    26,    27,   135,   136,   102,   103,   104,   105,   106,
     107,   108,   109,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   135,   136,   102,
     103,   104,   105,   106,   107,   108,   109,     0,     0,     0,
       0,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,     0,     0,     0,   121,   122,     0,     0,     0,
       0,     0,   137,   138,   139,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,     0,     0,     0,   121,
     122,     0,     0,   165,     0,     0,   137,   138,   139,   102,
     103,   104,   105,   106,   107,   108,   109,     0,     0,     0,
       0,   309,     0,     0,     0,     0,     0,   102,   103,   104,
     105,   106,   107,   108,   109,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,     0,     0,     0,   121,
     122,     0,   166,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,     0,     0,     0,   121,   122,     0,
     310,   102,   103,   104,   105,   106,   107,   108,   109,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   102,
     103,   104,   105,   106,   107,   108,   109,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,     0,     0,
       0,   121,   122,     0,   303,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,     0,     0,     0,   121,
     122,   102,   103,   104,   105,   106,   107,   108,   109,     0,
       0,     0,     0,     0,     0,   102,   103,   104,   105,   106,
     107,   108,   109,     0,     0,     0,     0,     0,   102,   103,
     104,   105,   106,   107,   108,   109,     0,     0,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,     0,     0,
       0,   121,   122,   112,   113,   114,   115,   116,   117,   118,
     119,   120,     0,     0,     0,   121,   122,   113,   114,   115,
     116,   117,   118,   119,   120,     0,     0,     0,   121,   122,
     105,   106,   107,   108,   109,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   116,
     117,   118,   119,   120,     0,     0,     0,   121,   122
};

static const short yycheck[] =
{
       0,    31,    66,     6,    68,    69,    27,   125,    11,    12,
       1,     1,     3,   154,    14,     1,     5,     6,    21,    22,
      23,    24,    25,    54,    27,     5,     6,   144,    27,     3,
       1,    31,     3,    15,     5,     6,    14,     5,     6,     3,
      70,     1,     3,     1,     1,     3,     3,     5,     6,     1,
       0,    54,    59,    19,    61,    86,     1,    17,    25,    60,
       1,    15,    69,    64,    71,    17,     1,    61,    59,     1,
      70,     1,    17,    94,    64,     5,     6,    63,    64,     1,
      70,    70,    61,    86,    45,    59,    85,     1,    45,     3,
      70,    94,   233,    64,    93,    24,    64,    65,    66,   102,
     103,   104,   105,   106,   107,   108,    64,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    60,
     123,   124,   125,    64,   241,    39,    61,   130,    60,    59,
     129,     3,    64,     1,    64,     3,     5,     6,   279,    61,
     140,    27,     1,   284,     3,     4,   146,   168,    62,     5,
       6,    61,   155,   174,    59,     1,    15,     3,   158,   159,
      61,   161,   140,    22,    69,   168,    71,   308,   146,   172,
     169,   174,     5,     6,     1,    28,   175,    45,     5,     6,
     158,   159,     1,   161,    43,    60,     5,     6,    30,    64,
      49,    50,    60,    52,   239,    64,     5,     6,   316,    45,
      59,   231,     1,    62,     3,   235,    18,    63,    67,    68,
      69,     1,   251,     3,    60,    13,    14,   247,   248,   130,
       1,   224,     1,   253,     5,     6,     5,     6,   228,   293,
      63,   231,    59,   132,    61,   235,     1,   270,     3,   260,
      59,    -1,   242,   246,   265,   275,   249,   247,   248,     1,
     228,     3,    -1,   253,    63,    94,   286,   260,    -1,    57,
      58,    64,   265,    66,   242,    -1,     1,   270,     3,     1,
       1,     3,     3,    -1,   304,   275,     5,     6,   278,    -1,
      -1,    -1,   312,    -1,    -1,    -1,   286,    -1,    -1,   319,
     320,    -1,    -1,   323,   294,    -1,   326,    -1,   301,    -1,
     278,    -1,    -1,    -1,   304,    -1,   306,   307,    -1,    -1,
      -1,   311,   312,   316,    -1,    -1,   294,    -1,    -1,   319,
     320,    -1,   322,   323,    -1,    -1,   326,    -1,   306,   307,
      -1,     0,     1,   311,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    -1,   322,    -1,    15,    16,    -1,    18,
      19,    20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,
      29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,
      -1,    40,    41,    42,    -1,     3,     4,    -1,    -1,    -1,
      49,    50,    -1,    -1,    -1,    -1,    -1,    15,     0,    -1,
      59,     3,     4,    62,    22,    64,    -1,    -1,    67,    68,
      69,    -1,    -1,    15,    16,    -1,    18,    19,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      -1,    49,    50,    35,    36,    37,    -1,    -1,    40,    41,
      42,    59,     3,     4,    62,    -1,    -1,    49,    50,    67,
      68,    69,    70,    -1,    15,     0,    -1,    59,     3,     4,
      62,    22,    -1,    -1,    -1,    67,    68,    69,    -1,    -1,
      15,    16,    -1,    18,    19,    20,    21,    22,    23,    -1,
      -1,    26,    -1,    -1,    29,    30,    -1,    -1,    49,    50,
      35,    36,    37,    -1,    -1,    40,    41,    42,    59,     3,
       4,    62,    -1,    -1,    49,    50,    67,    68,    69,    70,
      -1,    15,     0,    -1,    59,     3,     4,    62,    22,    -1,
      -1,    -1,    67,    68,    69,    -1,    -1,    15,    16,    -1,
      18,    19,    20,    21,    22,    23,    -1,    -1,    26,    -1,
      -1,    29,    30,    -1,    -1,    49,    50,    35,    36,    37,
      -1,    -1,    40,    41,    42,    59,     3,     4,    62,    63,
      -1,    49,    50,    67,    68,    69,    -1,    -1,    15,     0,
      -1,    59,     3,     4,    62,    22,    -1,    -1,    -1,    67,
      68,    69,    -1,    -1,    15,    16,    -1,    18,    19,    20,
      21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,
      -1,    -1,    49,    50,    35,    36,    37,    -1,    -1,    40,
      41,    42,    59,     3,     4,    62,    63,    -1,    49,    50,
      67,    68,    69,    -1,    -1,    15,     0,    -1,    59,     3,
       4,    62,    22,    -1,    -1,    -1,    67,    68,    69,    -1,
      -1,    15,    16,    -1,    18,    19,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,    49,
      50,    35,    36,    37,    -1,    -1,    40,    41,    42,    59,
      -1,    -1,    62,    -1,    -1,    49,    50,    67,    68,    69,
      -1,    -1,    -1,     0,    -1,    59,     3,     4,    62,    -1,
      -1,    -1,    -1,    67,    68,    69,    -1,    -1,    15,    16,
      -1,    18,    19,    20,    21,    22,    23,    -1,    -1,    26,
      -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,
       0,    -1,    59,     3,     4,    62,    -1,    -1,    -1,    -1,
      67,    68,    69,    -1,    -1,    15,    16,    -1,    18,    -1,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,     1,    -1,     3,     4,     5,     6,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    59,
      -1,    -1,    62,    -1,    22,    -1,    -1,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    59,    -1,     1,    62,     3,     4,     5,     6,    67,
      68,    69,    70,    -1,    -1,    -1,    -1,    15,    16,    17,
      18,    -1,    20,    21,    22,    23,    -1,    -1,    26,    -1,
      -1,    29,    30,    -1,    51,    52,    53,    35,    36,    37,
      57,    58,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    -1,    -1,    62,    -1,    64,    -1,    -1,    67,
      68,    69,     1,    -1,     3,     4,     5,     6,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    17,    18,
      -1,    20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,
      29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,
      -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    -1,    -1,    62,    -1,    64,    -1,    -1,    67,    68,
      69,     1,    -1,     3,     4,     5,     6,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    16,    -1,    18,    -1,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    59,
       3,     4,    62,    -1,    64,    -1,    -1,    67,    68,    69,
      -1,    -1,    15,    16,    17,    18,    -1,    20,    21,    22,
      23,    -1,    -1,    26,    -1,    -1,    29,    30,    31,    -1,
      -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    59,     3,     4,    62,
      -1,    -1,    -1,    -1,    67,    68,    69,    -1,    -1,    15,
      16,    17,    18,    -1,    20,    21,    22,    23,    -1,    -1,
      26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,
      36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    59,     3,     4,    62,    -1,    -1,    -1,
      -1,    67,    68,    69,    -1,    -1,    15,    16,    17,    18,
      -1,    20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,
      29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,
      -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,
      59,     3,     4,    62,    -1,    -1,    -1,    -1,    67,    68,
      69,    -1,    -1,    15,    16,    17,    18,    -1,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,
      42,     1,    -1,     3,     4,     5,     6,    49,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    15,     1,    59,     3,     4,
      62,    -1,    22,    -1,    -1,    67,    68,    69,    -1,    -1,
      15,     1,    -1,     3,     4,    -1,    -1,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,    63,    49,    50,    -1,    67,    68,    69,
      -1,    -1,    -1,     1,    59,     3,     4,    62,    -1,    49,
      50,    -1,    67,    68,    69,    -1,    -1,    15,     1,    59,
       3,     4,    62,    -1,    22,    -1,    -1,    67,    68,    69,
      -1,    -1,    15,     1,    -1,     3,     4,    -1,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,
      -1,    49,    50,    -1,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    -1,    -1,    62,    -1,    49,    50,    -1,    67,
      68,    69,    -1,    -1,    -1,     1,    59,     3,     4,    62,
      -1,    49,    50,    -1,    67,    68,    69,    -1,    -1,    15,
       1,    59,     3,     4,    62,    -1,    22,    -1,    -1,    67,
      68,    69,    -1,    -1,    15,     1,    -1,     3,     4,    -1,
      -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      -1,    -1,    -1,    49,    50,    -1,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    59,    -1,    -1,    62,    -1,    49,    50,
      -1,    67,    68,    69,    -1,    -1,    -1,     1,    59,     3,
       4,    62,    -1,    49,    50,    -1,    67,    68,    69,    -1,
      -1,    15,     1,    59,     3,     4,    62,    -1,    22,    -1,
      -1,    67,    68,    69,    -1,    -1,    15,     1,    -1,     3,
       4,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    -1,    -1,    -1,    49,    50,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    -1,
      49,    50,    -1,    67,    68,    69,    -1,    -1,    -1,     1,
      59,     3,     4,    62,    -1,    49,    50,    -1,    67,    68,
      69,    -1,    -1,    15,     1,    59,     3,     4,    62,    -1,
      22,    -1,    -1,    67,    68,    69,    -1,    -1,    15,     1,
      -1,     3,     4,    -1,    -1,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    15,    -1,    -1,    -1,    49,    50,    -1,
      22,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,
      62,    -1,    49,    50,    -1,    67,    68,    69,    -1,    -1,
      -1,     1,    59,     3,     4,    62,    -1,    49,    50,    -1,
      67,    68,    69,    -1,    -1,    15,     1,    59,     3,     4,
      62,    -1,    22,    -1,    -1,    67,    68,    69,    -1,    -1,
      15,     1,    -1,     3,     4,    -1,    -1,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    59,
      -1,    -1,    62,    -1,    49,    50,    -1,    67,    68,    69,
      -1,    -1,    -1,     1,    59,     3,     4,    62,    -1,    49,
      50,    -1,    67,    68,    69,    -1,    -1,    15,     1,    59,
       3,     4,    62,    -1,    22,    -1,    -1,    67,    68,    69,
      -1,    -1,    15,     1,    -1,     3,     4,    -1,    -1,    22,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,
      -1,    49,    50,    -1,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    -1,    -1,    62,    -1,    49,    50,    -1,    67,
      68,    69,    -1,    -1,    -1,     1,    59,     3,     4,    62,
      -1,    49,    50,    -1,    67,    68,    69,    -1,    -1,    15,
       1,    59,     3,     4,    62,    -1,    22,    -1,    -1,    67,
      68,    69,    -1,    -1,    15,     1,    -1,     3,     4,    -1,
      -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,
      -1,    -1,    -1,    49,    50,    -1,    22,    -1,    -1,    -1,
      -1,    -1,    -1,    59,    -1,    -1,    62,    -1,    49,    50,
      -1,    67,    68,    69,    -1,    -1,    -1,     1,    59,     3,
       4,    62,    -1,    49,    50,    -1,    67,    68,    69,    -1,
      -1,    15,     1,    59,     3,     4,    62,    -1,    22,    -1,
      -1,    67,    68,    69,    -1,    -1,    15,     1,    -1,     3,
       4,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    -1,    -1,    -1,    49,    50,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    -1,
      49,    50,    -1,    67,    68,    69,    -1,    -1,    -1,     1,
      59,     3,     4,    62,    -1,    49,    50,    -1,    67,    68,
      69,    -1,    -1,    15,     1,    59,     3,     4,    62,    -1,
      22,    -1,    -1,    67,    68,    69,    -1,    -1,    15,    -1,
      -1,    -1,    -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,
      62,    -1,    49,    50,    -1,    67,    68,    69,    -1,    -1,
      -1,    -1,    59,    -1,    -1,    62,     3,     4,     5,     6,
      67,    68,    69,    -1,    -1,    -1,    -1,    -1,    15,    16,
      -1,    18,    -1,    20,    21,    22,    23,    -1,    -1,    26,
      -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    59,     3,     4,    62,    -1,    64,    -1,    -1,
      67,    68,    69,    -1,    -1,    15,    16,    -1,    18,    -1,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,     3,     4,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    -1,    15,    -1,     3,     4,    -1,    49,
      50,    22,    -1,    -1,    -1,    -1,    -1,    -1,    15,    59,
      -1,    -1,    62,    -1,    -1,    22,    -1,    67,    68,    69,
      -1,    -1,    43,    -1,    -1,    -1,    -1,    -1,    49,    50,
      -1,    52,    -1,    -1,    -1,    -1,    43,    -1,    59,    60,
      -1,    62,    49,    50,    -1,    52,    67,    68,    69,    -1,
      -1,    -1,    59,    -1,    -1,    62,    -1,    -1,    -1,     1,
      67,    68,    69,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    57,    58,    -1,    -1,    -1,
      -1,    -1,    64,    65,    66,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    -1,    -1,    57,
      58,    -1,    -1,     1,    -1,    -1,    64,    65,    66,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,     1,    -1,    -1,    -1,    -1,    -1,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    -1,    -1,    57,
      58,    -1,    60,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    57,    58,    -1,
      60,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    -1,    -1,
      -1,    57,    58,    -1,    60,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    -1,    -1,    -1,    57,
      58,     7,     8,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    -1,    -1,
      -1,    57,    58,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    57,    58,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    57,    58,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    57,    58
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,    15,    16,    18,    19,    20,    21,
      22,    23,    26,    29,    30,    35,    36,    37,    40,    41,
      42,    49,    50,    59,    62,    67,    68,    69,    73,    74,
      75,    79,    80,    81,    82,    84,    85,    87,    88,    89,
      90,    91,    93,   100,   102,   103,   109,   110,   111,   112,
     113,    15,    83,     1,    62,   104,   111,   113,     1,     3,
      39,    62,    76,     1,     3,    59,   101,     1,   111,   111,
      79,     1,     3,    86,     1,    86,   111,   111,     1,   111,
       1,     5,     6,    63,   111,   118,   119,   121,     1,   111,
       3,     1,    70,   117,   119,   121,     0,    74,    80,     5,
       6,    64,     7,     8,     9,    10,    11,    12,    13,    14,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    57,    58,    59,    61,    69,    71,   114,    15,   118,
      25,   105,   106,   107,     1,     5,     6,    64,    65,    66,
      94,     1,     5,     6,    59,    61,    95,    61,     1,     3,
      45,    77,    78,     1,     3,    61,     1,     3,    94,    94,
      94,    31,    92,     1,     3,     1,    60,    63,   119,   118,
      64,    66,   120,    70,   119,   117,     1,   111,     1,   111,
       1,   111,     1,   111,     1,   111,     1,   111,     1,   111,
       1,   111,     1,   111,     1,   111,     1,   111,     1,   111,
       1,   111,     1,   111,     1,   111,     1,   111,     1,   111,
       1,   111,     1,   111,    43,    52,    60,   111,   115,   116,
       1,   111,   116,     3,    59,    63,     1,   104,    24,   108,
     107,    79,     1,    60,    77,    79,     3,     1,    63,    64,
       1,    59,    95,     1,   111,     1,    61,    79,    79,    27,
      96,    97,    98,    79,     1,    17,    61,    63,   121,    63,
     119,   111,    70,   121,    70,   119,     1,     3,     1,    60,
      64,     1,    70,   111,     1,    79,     1,    17,    95,    60,
       1,    61,    78,     1,    60,    77,    79,     1,   111,     1,
      17,     1,    17,   111,    28,    99,    98,     1,     3,    63,
      70,    61,   115,    60,    79,     1,    95,    95,    60,     1,
      60,    94,    79,     1,    17,     1,    59,     1,   111,    79,
      79,     1,    95,    79,     1,   116,    79,     1,    60
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
    {yyvsp[-1].v.p->addChild(yyvsp[0].v.p); 
				yyval.v.p = new AST(OP_SCALL,yyvsp[-1].v.p,yyvsp[-1].v.p->context());}
    break;

  case 57:
#line 297 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PARENS,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 58:
#line 298 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 59:
#line 302 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p); }
    break;

  case 60:
#line 303 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as persistent",yyvsp[-1]);}
    break;

  case 61:
#line 307 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 62:
#line 308 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as global",yyvsp[-1]);}
    break;

  case 64:
#line 313 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 65:
#line 314 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of valid identifiers",yyvsp[0]);}
    break;

  case 70:
#line 334 "../../../libs/libFreeMat/Parser.yxx"
    { yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-2].v.p); if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);}
    break;

  case 71:
#line 336 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("matching 'end' to 'try' clause from line ") + decodeline(yyvsp[-3]),yyvsp[0]);}
    break;

  case 72:
#line 340 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 73:
#line 341 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 74:
#line 345 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-5].v.p; yyval.v.p->addChild(yyvsp[-4].v.p); 
	  if (yyvsp[-2].v.p != NULL) yyval.v.p->addChild(yyvsp[-2].v.p); 
	  if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 75:
#line 350 "../../../libs/libFreeMat/Parser.yxx"
    {
          yyxpt(std::string("matching 'end' to 'switch' clause from line ") + decodeline(yyvsp[-5]),yyvsp[0]);
        }
    break;

  case 83:
#line 365 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 85:
#line 370 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_CASEBLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());
	}
    break;

  case 86:
#line 373 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 87:
#line 379 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-2].v.p); yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 88:
#line 385 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 89:
#line 388 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = NULL;
	}
    break;

  case 90:
#line 394 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 91:
#line 398 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'for' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 92:
#line 402 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-1].v.p);}
    break;

  case 93:
#line 403 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 94:
#line 404 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p; 
	      yyval.v.p->addChild(new AST(OP_RHS,new AST(id_node,yyvsp[0].v.p->text,yyvsp[0].v.p->context()),yyvsp[0].v.p->context())); }
    break;

  case 95:
#line 406 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-4]);}
    break;

  case 96:
#line 407 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression",yyvsp[-1]);}
    break;

  case 97:
#line 408 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("equals operator after loop index",yyvsp[-1]);}
    break;

  case 98:
#line 409 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier that is the loop variable",yyvsp[-1]);}
    break;

  case 99:
#line 410 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression",yyvsp[-1]);}
    break;

  case 100:
#line 411 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier or assignment (id = expr) after 'for' ",yyvsp[0]);}
    break;

  case 101:
#line 415 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 102:
#line 418 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test expression after 'while'",yyvsp[-1]);}
    break;

  case 103:
#line 420 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'while' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 104:
#line 424 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); if (yyvsp[-2].v.p != NULL) yyval.v.p->addChild(yyvsp[-2].v.p); 
	  if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 105:
#line 428 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("condition expression for 'if'",yyvsp[-1]);}
    break;

  case 106:
#line 429 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'if' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 107:
#line 433 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_CSTAT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-2].v.p->context());
	}
    break;

  case 108:
#line 436 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("valid list of statements after condition",yyvsp[0]);}
    break;

  case 109:
#line 440 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 111:
#line 445 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_ELSEIFBLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());
	}
    break;

  case 112:
#line 448 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 113:
#line 454 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 114:
#line 457 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test condition for 'elseif' clause",yyvsp[-1]);}
    break;

  case 115:
#line 460 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 116:
#line 463 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 117:
#line 464 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list for 'else' clause",yyvsp[-1]);}
    break;

  case 118:
#line 468 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ASSIGN,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 119:
#line 469 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expression in assignment",yyvsp[-1]);}
    break;

  case 120:
#line 473 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[-3].v.p->addChild(new AST(OP_PARENS,yyvsp[-1].v.p,yyvsp[-2].v.i));
	  yyval.v.p = new AST(OP_MULTICALL,yyvsp[-6].v.p,yyvsp[-3].v.p,yyvsp[-7].v.i);
	}
    break;

  case 121:
#line 477 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[0].v.p->addChild(new AST(OP_PARENS,NULL,-1));
	  yyval.v.p = new AST(OP_MULTICALL,yyvsp[-3].v.p,yyvsp[0].v.p,yyvsp[-4].v.i);
	}
    break;

  case 122:
#line 482 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-2]);}
    break;

  case 123:
#line 484 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing list",yyvsp[-1]);}
    break;

  case 124:
#line 486 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("left parenthesis",yyvsp[-1]);}
    break;

  case 125:
#line 488 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier",yyvsp[-1]);}
    break;

  case 126:
#line 492 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_COLON,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 127:
#line 493 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after ':'",yyvsp[-1]);}
    break;

  case 129:
#line 495 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PLUS,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 130:
#line 496 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '+'",yyvsp[-1]);}
    break;

  case 131:
#line 497 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SUBTRACT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 132:
#line 498 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '-'",yyvsp[-1]);}
    break;

  case 133:
#line 499 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_TIMES,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 134:
#line 500 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '*'",yyvsp[-1]);}
    break;

  case 135:
#line 501 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_RDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 136:
#line 502 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '/'",yyvsp[-1]);}
    break;

  case 137:
#line 503 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 138:
#line 504 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '\\'",yyvsp[-1]);}
    break;

  case 139:
#line 505 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_OR,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 140:
#line 506 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '|'",yyvsp[-1]);}
    break;

  case 141:
#line 507 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_AND,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 142:
#line 508 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '&'",yyvsp[-1]);}
    break;

  case 143:
#line 509 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 144:
#line 510 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<'",yyvsp[-1]);}
    break;

  case 145:
#line 511 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 146:
#line 512 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<='",yyvsp[-1]);}
    break;

  case 147:
#line 513 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_GT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 148:
#line 514 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>'",yyvsp[-1]);}
    break;

  case 149:
#line 515 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_GEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 150:
#line 516 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>='",yyvsp[-1]);}
    break;

  case 151:
#line 517 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 152:
#line 518 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '=='",yyvsp[-1]);}
    break;

  case 153:
#line 519 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 154:
#line 520 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '~='",yyvsp[-1]);}
    break;

  case 155:
#line 521 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_TIMES,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 156:
#line 522 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.*'",yyvsp[-1]);}
    break;

  case 157:
#line 523 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_RDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 158:
#line 524 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after './'",yyvsp[-1]);}
    break;

  case 159:
#line 525 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_LDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 160:
#line 526 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.\\'",yyvsp[-1]);}
    break;

  case 161:
#line 527 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NEG,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 162:
#line 528 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 163:
#line 529 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NOT,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 164:
#line 530 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after logical not",yyvsp[0]);}
    break;

  case 165:
#line 531 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_POWER,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 166:
#line 532 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '^'",yyvsp[-1]);}
    break;

  case 167:
#line 533 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_POWER,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 168:
#line 534 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.^'",yyvsp[-1]);}
    break;

  case 169:
#line 535 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_TRANSPOSE,yyvsp[-1].v.p,yyvsp[0].v.i);}
    break;

  case 170:
#line 536 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_TRANSPOSE,yyvsp[-1].v.p,yyvsp[0].v.i);}
    break;

  case 171:
#line 537 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 172:
#line 538 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a right parenthesis after expression to match this one",yyvsp[-2]);}
    break;

  case 173:
#line 539 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after left parenthesis",yyvsp[-1]);}
    break;

  case 177:
#line 546 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ADDRESS,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 178:
#line 547 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_RHS,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 179:
#line 548 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 180:
#line 549 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a matrix definition followed by a right bracket",yyvsp[-1]);}
    break;

  case 181:
#line 550 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 182:
#line 551 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 183:
#line 552 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 184:
#line 553 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EMPTY,NULL,yyvsp[-1].v.i);}
    break;

  case 185:
#line 554 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 186:
#line 555 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 187:
#line 556 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 188:
#line 557 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 189:
#line 558 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EMPTY_CELL,NULL,yyvsp[-1].v.i);}
    break;

  case 190:
#line 559 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a cell-array definition followed by a right brace",yyvsp[-1]);}
    break;

  case 192:
#line 563 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 193:
#line 567 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PARENS,yyvsp[-1].v.p,yyvsp[-2].v.i); }
    break;

  case 194:
#line 568 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PARENS,NULL,yyvsp[-1].v.i); }
    break;

  case 195:
#line 569 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-2]);}
    break;

  case 196:
#line 570 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACES,yyvsp[-1].v.p,yyvsp[-2].v.i); }
    break;

  case 197:
#line 571 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right brace",yyvsp[-2]);}
    break;

  case 198:
#line 572 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT,yyvsp[0].v.p,yyvsp[-1].v.i); }
    break;

  case 199:
#line 573 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOTDYN,yyvsp[-1].v.p,yyvsp[-3].v.i);}
    break;

  case 201:
#line 578 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ALL,NULL,yyvsp[0].v.i);}
    break;

  case 202:
#line 579 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_KEYWORD,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-3].v.i);}
    break;

  case 203:
#line 580 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting expression after '=' in keyword assignment",yyvsp[-1]);}
    break;

  case 204:
#line 581 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_KEYWORD,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 205:
#line 582 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting keyword identifier after '/' in keyword assignment",yyvsp[-1]);}
    break;

  case 207:
#line 587 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addPeer(yyvsp[0].v.p);}
    break;

  case 208:
#line 591 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACES,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 209:
#line 592 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 210:
#line 596 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACKETS,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 211:
#line 597 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 216:
#line 609 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SEMICOLON,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 217:
#line 610 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2908 "../../../libs/libFreeMat/Parser.cxx"

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


#line 613 "../../../libs/libFreeMat/Parser.yxx"


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

