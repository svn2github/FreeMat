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
#define YYLAST   2289

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  71
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  50
/* YYNRULES -- Number of rules. */
#define YYNRULES  215
/* YYNRULES -- Number of states. */
#define YYNSTATES  326

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
     609,   612,   616,   620,   623,   625,   627,   629,   631,   635,
     638,   643,   648,   654,   657,   661,   666,   671,   677,   680,
     683,   685,   688,   692,   696,   700,   704,   707,   712,   714,
     716,   721,   726,   729,   732,   734,   738,   740,   744,   746,
     750,   752,   754,   756,   758,   760
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      72,     0,    -1,    78,    -1,    74,    -1,    -1,     1,    -1,
      19,    75,     3,    59,    76,    60,    94,    78,    -1,    19,
       3,    59,    76,    60,    94,    78,    -1,    19,    75,     3,
      94,    78,    -1,    19,     3,    94,    78,    -1,    19,    75,
       3,    59,    60,    94,    78,    -1,    19,     3,    59,    60,
      94,    78,    -1,    19,     1,    -1,    19,     3,     1,    -1,
      19,     3,    59,     1,    -1,    19,     3,    59,    76,    60,
       1,    -1,    19,    75,     1,    -1,    19,    75,     3,     1,
      -1,    19,    75,     3,    59,     1,    -1,    19,    75,     3,
      59,    76,    60,     1,    -1,    73,    -1,    74,    73,    -1,
      39,    61,    -1,     3,    61,    -1,    62,    76,    63,    61,
      -1,     3,     1,    -1,    62,     1,    -1,    62,    76,     1,
      -1,    62,    76,    63,     1,    -1,    77,    -1,    76,    64,
      77,    -1,     3,    -1,    45,     3,    -1,    79,    -1,    78,
      79,    -1,    80,     5,    -1,    80,     6,    -1,    80,    64,
      -1,   108,    -1,   110,    -1,    -1,   109,    -1,    99,    -1,
      89,    -1,    88,    -1,   101,    -1,   102,    -1,    92,    -1,
      90,    -1,    87,    -1,    86,    -1,    84,    -1,    83,    -1,
      81,    -1,    41,    -1,    42,    -1,    16,    82,    -1,    15,
      -1,    82,    15,    -1,    35,    85,    -1,    35,     1,    -1,
      40,    85,    -1,    40,     1,    -1,     3,    -1,    85,     3,
      -1,    85,     1,    -1,    37,    -1,    36,    -1,    29,    -1,
      21,    -1,    30,    78,    91,    17,    -1,    30,    78,    91,
       1,    -1,    31,    78,    -1,    -1,    26,   110,    93,    95,
      98,    17,    -1,    26,   110,    93,    95,    98,     1,    -1,
      64,    -1,     6,    -1,     5,    -1,    65,    -1,    66,    -1,
       6,    -1,     5,    -1,    -1,    96,    -1,    97,    -1,    96,
      97,    -1,    27,   110,    93,    78,    -1,    28,    78,    -1,
      -1,    20,   100,    93,    78,    17,    -1,    20,   100,    93,
      78,     1,    -1,    59,     3,    61,   110,    60,    -1,     3,
      61,   110,    -1,     3,    -1,    59,     3,    61,   110,     1,
      -1,    59,     3,    61,     1,    -1,    59,     3,     1,    -1,
      59,     1,    -1,     3,    61,     1,    -1,     1,    -1,    23,
     110,    93,    78,    17,    -1,    23,     1,    -1,    23,   110,
      93,    78,     1,    -1,    18,   103,   104,   107,    17,    -1,
      18,     1,    -1,    18,   103,   104,   107,     1,    -1,   110,
      93,    78,    -1,   110,     1,    -1,    -1,   105,    -1,   106,
      -1,   105,   106,    -1,    25,   103,    -1,    25,     1,    -1,
      24,    78,    -1,    -1,    24,     1,    -1,   112,    61,   110,
      -1,   112,    61,     1,    -1,    62,   117,    63,    61,     3,
      59,   115,    60,    -1,    62,   117,    63,    61,     3,    -1,
      62,   117,    63,    61,     3,    59,   115,     1,    -1,    62,
     117,    63,    61,     3,    59,     1,    -1,    62,   117,    63,
      61,     3,     1,    -1,    62,   117,    63,    61,     1,    -1,
     110,    43,   110,    -1,   110,    43,     1,    -1,   111,    -1,
     110,    49,   110,    -1,   110,    49,     1,    -1,   110,    50,
     110,    -1,   110,    50,     1,    -1,   110,    51,   110,    -1,
     110,    51,     1,    -1,   110,    52,   110,    -1,   110,    52,
       1,    -1,   110,    53,   110,    -1,   110,    53,     1,    -1,
     110,    44,   110,    -1,   110,    44,     1,    -1,   110,    45,
     110,    -1,   110,    45,     1,    -1,   110,    46,   110,    -1,
     110,    46,     1,    -1,   110,     7,   110,    -1,   110,     7,
       1,    -1,   110,    47,   110,    -1,   110,    47,     1,    -1,
     110,     8,   110,    -1,   110,     8,     1,    -1,   110,     9,
     110,    -1,   110,     9,     1,    -1,   110,    48,   110,    -1,
     110,    48,     1,    -1,   110,    10,   110,    -1,   110,    10,
       1,    -1,   110,    11,   110,    -1,   110,    11,     1,    -1,
     110,    12,   110,    -1,   110,    12,     1,    -1,    50,   110,
      -1,    49,   110,    -1,    67,   110,    -1,    67,     1,    -1,
     110,    57,   110,    -1,   110,    57,     1,    -1,   110,    13,
     110,    -1,   110,    13,     1,    -1,   110,    58,    -1,   110,
      14,    -1,    59,   110,    60,    -1,    59,   110,     1,    -1,
      59,     1,    -1,     4,    -1,    15,    -1,    22,    -1,   112,
      -1,    62,   117,    63,    -1,    62,     1,    -1,    62,   118,
     117,    63,    -1,    62,   117,   118,    63,    -1,    62,   118,
     117,   118,    63,    -1,    62,    63,    -1,    68,   116,    69,
      -1,    68,   118,   116,    69,    -1,    68,   116,   118,    69,
      -1,    68,   118,   116,   118,    69,    -1,    68,    69,    -1,
      68,     1,    -1,     3,    -1,   112,   113,    -1,    59,   115,
      60,    -1,    59,   115,     1,    -1,    68,   115,    69,    -1,
      68,   115,     1,    -1,    70,     3,    -1,    70,    59,   110,
      60,    -1,   110,    -1,    43,    -1,    52,     3,    61,   110,
      -1,    52,     3,    61,     1,    -1,    52,     3,    -1,    52,
       1,    -1,   114,    -1,   115,    64,   114,    -1,   120,    -1,
     116,   118,   120,    -1,   120,    -1,   117,   118,   120,    -1,
       6,    -1,     5,    -1,    64,    -1,    66,    -1,   110,    -1,
     120,   119,   110,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   133,   133,   134,   134,   135,   139,   156,   172,   188,
     203,   219,   234,   235,   237,   238,   239,   240,   242,   243,
     247,   248,   252,   253,   254,   255,   256,   257,   258,   262,
     263,   267,   267,   278,   279,   283,   287,   291,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   314,   318,   323,   324,   328,
     329,   333,   334,   338,   339,   340,   344,   348,   352,   355,
     359,   361,   366,   367,   371,   376,   382,   382,   382,   382,
     382,   386,   386,   391,   392,   396,   399,   405,   411,   414,
     420,   423,   428,   429,   430,   432,   433,   434,   435,   436,
     437,   441,   444,   445,   450,   454,   455,   459,   462,   466,
     467,   471,   474,   480,   483,   486,   489,   490,   494,   495,
     499,   503,   507,   509,   511,   513,   518,   519,   520,   521,
     522,   523,   524,   525,   526,   527,   528,   529,   530,   531,
     532,   533,   534,   535,   536,   537,   538,   539,   540,   541,
     542,   543,   544,   545,   546,   547,   548,   549,   550,   551,
     552,   553,   554,   555,   556,   557,   558,   559,   560,   561,
     562,   563,   564,   565,   569,   570,   571,   572,   573,   574,
     575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
     587,   588,   592,   593,   594,   595,   596,   597,   601,   602,
     603,   604,   605,   606,   610,   611,   615,   616,   620,   621,
     625,   625,   629,   629,   633,   634
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
       0,    71,    72,    72,    72,    72,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      74,    74,    75,    75,    75,    75,    75,    75,    75,    76,
      76,    77,    77,    78,    78,    79,    79,    79,    80,    80,
      80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
      80,    80,    80,    80,    80,    80,    81,    82,    82,    83,
      83,    84,    84,    85,    85,    85,    86,    87,    88,    89,
      90,    90,    91,    91,    92,    92,    93,    93,    93,    93,
      93,    94,    94,    95,    95,    96,    96,    97,    98,    98,
      99,    99,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   101,   101,   101,   102,   102,   102,   103,   103,   104,
     104,   105,   105,   106,   106,   107,   107,   107,   108,   108,
     109,   109,   109,   109,   109,   109,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     112,   112,   113,   113,   113,   113,   113,   113,   114,   114,
     114,   114,   114,   114,   115,   115,   116,   116,   117,   117,
     118,   118,   119,   119,   120,   120
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
       2,     3,     3,     2,     1,     1,     1,     1,     3,     2,
       4,     4,     5,     2,     3,     4,     4,     5,     2,     2,
       1,     2,     3,     3,     3,     3,     2,     4,     1,     1,
       4,     4,     2,     2,     1,     3,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     5,   190,   174,   175,     0,     0,     0,     0,    69,
     176,     0,     0,    68,    40,     0,    67,    66,     0,    54,
      55,     0,     0,     0,     0,     0,     0,     0,    20,     3,
      40,    33,     0,    53,    52,    51,    50,    49,    44,    43,
      48,    47,    42,    45,    46,    38,    41,    39,   128,   177,
      57,    56,   105,     0,   109,     0,   177,    12,     0,     0,
       0,     0,   100,    94,     0,     0,   102,     0,     0,    40,
      60,    63,     0,    62,     0,   162,   161,   173,     0,   179,
     211,   210,   183,   214,     0,     0,   208,   164,   163,   189,
     188,     0,     0,   206,     1,    21,    34,    35,    36,    37,
       0,     0,     0,     0,     0,     0,     0,   170,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     169,     0,     0,     0,     0,   191,    58,     0,     0,   116,
     110,   111,   108,    78,    77,    76,    79,    80,    40,    13,
      82,    81,     0,    23,    40,    22,    26,    31,     0,     0,
      29,    16,     0,     0,    98,     0,    40,    40,    83,    40,
       0,    65,    64,   172,   171,   178,     0,     0,   212,   213,
       0,   184,     0,     0,   146,   145,   150,   149,   152,   151,
     156,   155,   158,   157,   160,   159,   168,   167,   127,   126,
     140,   139,   142,   141,   144,   143,   148,   147,   154,   153,
     130,   129,   132,   131,   134,   133,   136,   135,   138,   137,
     166,   165,   199,     0,   198,   204,     0,   119,   118,     0,
     196,     0,   178,   114,   113,     0,     0,   112,   107,    14,
       0,     0,    40,    32,    27,     0,     0,    17,     0,    40,
      99,    93,    97,     0,     0,     0,     0,    89,    84,    85,
      40,    71,    70,     0,   181,   209,   180,     0,   215,   186,
     207,   185,     0,   203,   202,   193,   192,     0,   195,   194,
       0,   117,    40,   106,   104,    40,     0,    28,    24,    30,
      18,     0,     0,    40,    96,     0,    91,    90,   103,   101,
       0,    40,     0,    86,   125,     0,   182,   187,     0,   205,
     197,    40,    15,    40,    40,     0,    95,    92,    40,    40,
      75,    74,   124,     0,   201,   200,    40,    40,    19,    40,
      87,   123,     0,    40,   122,   120
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    27,    28,    29,    61,   149,   150,    30,    31,    32,
      33,    51,    34,    35,    72,    36,    37,    38,    39,    40,
     160,    41,   138,   144,   247,   248,   249,   292,    42,    65,
      43,    44,    54,   129,   130,   131,   226,    45,    46,    47,
      48,    56,   125,   215,   216,    91,    84,    85,   170,    86
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -148
static const short yypact[] =
{
     356,  -148,  -148,  -148,  -148,    19,    73,   174,    90,  -148,
    -148,  1245,    13,  -148,  1921,    58,  -148,  -148,   211,  -148,
    -148,    13,    13,  1260,  1230,  1284,   776,    41,  -148,    25,
     734,  -148,   167,  -148,  -148,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,  -148,  -148,  -148,  -148,  -148,  2141,  -148,   270,
    -148,    34,  -148,  1230,    54,  1811,   -22,  -148,   287,    -1,
     179,   234,  -148,    66,   248,   131,  -148,  2009,  2009,  1026,
    -148,  -148,   182,  -148,   201,   276,   276,  -148,  1989,  -148,
    -148,  -148,  -148,  2141,     7,    13,   210,  -148,   276,  -148,
    -148,    27,    13,   210,  -148,  -148,  -148,  -148,  -148,  -148,
    1299,  1314,  1338,  1353,  1368,  1392,  1407,  -148,  1422,  1446,
    1461,  1476,  1500,  1515,  1530,  1554,  1569,  1584,  1608,  1623,
    -148,   149,  1638,   149,   151,  -148,  -148,   140,  1662,   109,
      54,  -148,  -148,  -148,  -148,  -148,  -148,  -148,  1921,   281,
    -148,  -148,   254,  -148,  1921,  -148,  -148,  -148,   136,    23,
    -148,  -148,   247,  1677,  -148,    10,  1921,  1921,   138,  1921,
      28,  -148,  -148,  -148,  -148,   115,  1782,   156,  -148,  -148,
      13,  -148,  1731,    48,  -148,  2231,  -148,  2231,  -148,  2231,
    -148,   276,  -148,   276,  -148,   276,  -148,   276,  -148,  2156,
    -148,  2208,  -148,  2221,  -148,  2231,  -148,  2231,  -148,  2231,
    -148,   817,  -148,   817,  -148,   276,  -148,   276,  -148,   276,
    -148,   276,  -148,   293,  2141,  -148,    30,  -148,  2141,     9,
    -148,    13,  -148,  -148,  -148,   972,    38,  -148,  1867,  -148,
      61,    91,   410,  -148,  -148,    63,    12,  -148,   255,  1921,
    -148,  2141,  -148,  1692,   836,   904,    13,   142,   138,  -148,
    1080,  -148,  -148,   321,  -148,   210,  -148,  1945,  2141,  -148,
     210,  -148,  1746,  -148,   132,  -148,  -148,   149,  -148,  -148,
    2087,  -148,  1134,  -148,  -148,  1921,    37,  -148,  -148,  -148,
    -148,    61,   126,   464,  -148,  2069,  -148,  -148,  -148,  -148,
    2009,  1921,    39,  -148,  -148,   125,  -148,  -148,  1716,  -148,
    -148,   518,  -148,  1921,  1921,    46,  -148,  -148,  1921,  1188,
    -148,  -148,  -148,   219,  -148,  2141,   572,   626,  -148,  1921,
    1867,  -148,    83,   680,  -148,  -148
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -148,  -148,   165,  -148,  -148,  -117,   -36,    22,    35,  -148,
    -148,  -148,  -148,  -148,   187,  -148,  -148,  -148,  -148,  -148,
    -148,  -148,   -61,  -147,  -148,  -148,   -39,  -148,  -148,  -148,
    -148,  -148,    87,  -148,  -148,    96,  -148,  -148,  -148,    -3,
    -148,     0,  -148,   -46,  -122,   135,   -27,     1,  -148,   -24
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -122
static const short yytable[] =
{
      49,   219,    93,    55,   156,   239,   157,   158,    67,    68,
     268,   242,    80,    81,    49,   147,     2,     3,    75,    76,
      78,    83,    88,    83,   234,   231,   127,    92,     4,   251,
      49,   265,    80,    81,    50,    10,    69,   121,   302,   273,
     310,    94,   140,   141,     7,   252,   123,   318,   124,   126,
      83,   140,   141,    80,    81,   274,   311,   148,   167,    70,
     145,    71,    21,    22,   277,    96,   140,   141,    93,    49,
     165,   243,    23,   267,    52,    53,     2,     3,   269,   128,
      25,    26,    83,   275,   324,   166,   235,   236,     4,    83,
     266,    62,   172,    63,   267,    10,   171,   175,   177,   179,
     181,   183,   185,   187,    96,   189,   191,   193,   195,   197,
     199,   201,   203,   205,   207,   209,   211,   261,   214,   218,
     214,   282,    21,    22,   278,    55,   312,   153,   166,   303,
    -121,  -121,    23,   225,   304,    53,   133,   134,    49,   233,
      25,    26,   255,   325,    49,    80,    81,   267,   260,    64,
     241,   276,     2,     3,   220,   236,    49,    49,   319,    49,
     228,    80,    81,    83,     4,   246,   232,   258,   257,    83,
     291,    10,    97,    98,   262,    57,   253,    58,   244,   245,
     146,   250,   147,   161,   313,   162,   305,   -59,   -59,  -121,
     236,   322,   212,   298,    95,   135,   136,   137,    21,    22,
     279,   213,   161,   222,   162,    74,   -61,   -61,    23,   293,
     221,    53,    73,    59,    71,   224,    25,    26,   270,   256,
     321,   299,     2,     3,   148,    49,   227,   173,    49,   308,
       0,    99,    49,   255,     4,   151,    60,   152,   260,    49,
     285,    10,     0,   290,    49,    49,   -59,   272,   237,   154,
      49,   155,   140,   141,    83,   229,   280,   147,   147,    83,
       0,   283,   212,    96,   214,   -61,     0,    96,    21,    22,
       0,   213,    49,     0,   168,    49,   169,     0,    23,    96,
      96,    53,   -25,    49,   -25,    96,    25,    26,   139,   106,
     107,    49,   140,   141,   263,   315,   264,   301,     0,   148,
     148,    49,     0,    49,    49,     0,   238,    96,    49,    49,
     214,     0,     0,   309,   230,   281,    49,    49,    96,    49,
      49,     0,   294,    49,   295,   316,   317,     0,     0,   121,
     320,   122,     0,   119,   120,     0,    96,     0,   123,     0,
     124,   323,     0,     0,    96,     0,   142,     0,   143,     0,
       0,    96,    96,     0,     0,    96,    -4,     1,    96,     2,
       3,   -40,   -40,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     5,     0,     6,     7,     8,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
      -9,     0,     0,     2,     3,    23,     0,     0,    24,     0,
     -40,     0,     0,    25,    26,     4,     5,     0,     6,    -9,
       8,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,    -8,     0,     0,     2,     3,    23,
       0,     0,    24,     0,     0,     0,     0,    25,    26,     4,
       5,     0,     6,    -8,     8,     9,    10,    11,     0,     0,
      12,     0,     0,    13,    14,     0,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,   -11,     0,
       0,     2,     3,    23,     0,     0,    24,     0,     0,     0,
       0,    25,    26,     4,     5,     0,     6,   -11,     8,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,     0,     0,    15,    16,    17,     0,     0,    18,    19,
      20,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,    -7,     0,     0,     2,     3,    23,     0,     0,
      24,     0,     0,     0,     0,    25,    26,     4,     5,     0,
       6,    -7,     8,     9,    10,    11,     0,     0,    12,     0,
       0,    13,    14,     0,     0,     0,     0,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,   -10,     0,     0,     2,
       3,    23,     0,     0,    24,     0,     0,     0,     0,    25,
      26,     4,     5,     0,     6,   -10,     8,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
      -6,     0,     0,     2,     3,    23,     0,     0,    24,     0,
       0,     0,     0,    25,    26,     4,     5,     0,     6,    -6,
       8,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,    -2,     0,     0,     2,     3,    23,
       0,     0,    24,     0,     0,     0,     0,    25,    26,     4,
       5,     0,     6,     0,     8,     9,    10,    11,     0,     0,
      12,     0,     0,    13,    14,     0,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,    89,     0,     2,
       3,    80,    81,    21,    22,     0,     0,     0,     0,     0,
       0,     4,     0,    23,     0,     0,    24,     0,    10,     0,
       0,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,    22,   103,   104,   105,
     106,   107,     0,     0,     0,    23,     0,   286,    53,     2,
       3,   -40,   -40,    25,    26,    90,     0,     0,     0,     0,
       0,     4,     5,   287,     6,     0,     8,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,   116,   117,
     118,    15,    16,    17,   119,   120,    18,    19,    20,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,     0,    24,     0,
     -40,     0,     0,    25,    26,   288,     0,     2,     3,   -40,
     -40,     0,     0,     0,     0,     0,     0,     0,     0,     4,
       5,   289,     6,     0,     8,     9,    10,    11,     0,     0,
      12,     0,     0,    13,    14,     0,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,    24,     0,   -40,     0,
       0,    25,    26,   271,     0,     2,     3,   -40,   -40,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,     0,
       6,     0,     8,     9,    10,    11,     0,     0,    12,     0,
       0,    13,    14,     0,     0,     0,     0,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,     0,   -73,     0,     2,
       3,    23,     0,     0,    24,     0,   -40,     0,     0,    25,
      26,     4,     5,   -73,     6,     0,     8,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,   159,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
       0,   -72,     0,     2,     3,    23,     0,     0,    24,     0,
       0,     0,     0,    25,    26,     4,     5,   -72,     6,     0,
       8,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,     0,  -115,     0,     2,     3,    23,
       0,     0,    24,     0,     0,     0,     0,    25,    26,     4,
       5,  -115,     6,     0,     8,     9,    10,    11,     0,     0,
      12,     0,     0,    13,    14,     0,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,   -88,
       0,     2,     3,    23,     0,     0,    24,     0,     0,     0,
       0,    25,    26,     4,     5,   -88,     6,     0,     8,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,     0,     0,    15,    16,    17,     0,     0,    18,    19,
      20,    79,     0,     2,     3,    80,    81,    21,    22,     0,
       0,     0,     0,     0,     0,     4,    66,    23,     2,     3,
      24,     0,    10,     0,     0,    25,    26,     0,     0,     0,
       4,    77,     0,     2,     3,     0,     0,    10,     0,     0,
       0,     0,     0,     0,     0,     4,     0,     0,     0,    21,
      22,     0,    10,     0,     0,    87,     0,     2,     3,    23,
       0,     0,    53,    82,    21,    22,     0,    25,    26,     4,
     174,     0,     2,     3,    23,     0,    10,    53,     0,    21,
      22,     0,    25,    26,     4,   176,     0,     2,     3,    23,
       0,    10,    53,     0,     0,     0,     0,    25,    26,     4,
       0,     0,     0,    21,    22,     0,    10,     0,     0,   178,
       0,     2,     3,    23,     0,     0,    53,     0,    21,    22,
       0,    25,    26,     4,   180,     0,     2,     3,    23,     0,
      10,    53,     0,    21,    22,     0,    25,    26,     4,   182,
       0,     2,     3,    23,     0,    10,    53,     0,     0,     0,
       0,    25,    26,     4,     0,     0,     0,    21,    22,     0,
      10,     0,     0,   184,     0,     2,     3,    23,     0,     0,
      53,     0,    21,    22,     0,    25,    26,     4,   186,     0,
       2,     3,    23,     0,    10,    53,     0,    21,    22,     0,
      25,    26,     4,   188,     0,     2,     3,    23,     0,    10,
      53,     0,     0,     0,     0,    25,    26,     4,     0,     0,
       0,    21,    22,     0,    10,     0,     0,   190,     0,     2,
       3,    23,     0,     0,    53,     0,    21,    22,     0,    25,
      26,     4,   192,     0,     2,     3,    23,     0,    10,    53,
       0,    21,    22,     0,    25,    26,     4,   194,     0,     2,
       3,    23,     0,    10,    53,     0,     0,     0,     0,    25,
      26,     4,     0,     0,     0,    21,    22,     0,    10,     0,
       0,   196,     0,     2,     3,    23,     0,     0,    53,     0,
      21,    22,     0,    25,    26,     4,   198,     0,     2,     3,
      23,     0,    10,    53,     0,    21,    22,     0,    25,    26,
       4,   200,     0,     2,     3,    23,     0,    10,    53,     0,
       0,     0,     0,    25,    26,     4,     0,     0,     0,    21,
      22,     0,    10,     0,     0,   202,     0,     2,     3,    23,
       0,     0,    53,     0,    21,    22,     0,    25,    26,     4,
     204,     0,     2,     3,    23,     0,    10,    53,     0,    21,
      22,     0,    25,    26,     4,   206,     0,     2,     3,    23,
       0,    10,    53,     0,     0,     0,     0,    25,    26,     4,
       0,     0,     0,    21,    22,     0,    10,     0,     0,   208,
       0,     2,     3,    23,     0,     0,    53,     0,    21,    22,
       0,    25,    26,     4,   210,     0,     2,     3,    23,     0,
      10,    53,     0,    21,    22,     0,    25,    26,     4,   217,
       0,     2,     3,    23,     0,    10,    53,     0,     0,     0,
       0,    25,    26,     4,     0,     0,     0,    21,    22,     0,
      10,     0,     0,   223,     0,     2,     3,    23,     0,     0,
      53,     0,    21,    22,     0,    25,    26,     4,   240,     0,
       2,     3,    23,     0,    10,    53,     0,    21,    22,     0,
      25,    26,     4,   284,     0,     2,     3,    23,     0,    10,
      53,     0,     0,     0,     0,    25,    26,     4,     0,     0,
       0,    21,    22,     0,    10,     0,     0,   314,     0,     2,
       3,    23,     0,     0,    53,     0,    21,    22,     0,    25,
      26,     4,     0,     0,     2,     3,    23,     0,    10,    53,
       0,    21,    22,     0,    25,    26,     4,     0,     0,     2,
       3,    23,     0,    10,    53,     0,     0,     0,     0,    25,
      26,     4,     0,     0,     0,    21,    22,     0,    10,     0,
       0,     0,     0,     0,     0,    23,     0,     0,    53,     0,
      21,    22,     0,    25,    26,     2,     3,     0,     0,     0,
      23,     0,     0,    53,     0,    21,    22,     4,    25,    26,
     259,     0,     0,     0,    10,    23,     0,     0,    53,     0,
       0,     0,   132,    25,    26,   297,   133,   134,   100,   101,
     102,   103,   104,   105,   106,   107,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,    53,   254,     0,     0,     0,    25,
      26,     0,     0,     0,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,     0,     0,     0,   119,   120,
       2,     3,   -40,   -40,     0,   135,   136,   137,     0,     0,
       0,     0,     4,     5,     0,     6,     0,     8,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,     0,     0,
       0,     0,    15,    16,    17,     0,     0,    18,    19,    20,
       0,     0,     0,     0,     0,     0,    21,    22,     0,     0,
       0,     0,     0,     0,     2,     3,    23,     0,     0,    24,
       0,   -40,     0,     0,    25,    26,     4,     5,     0,     6,
       0,     8,     9,    10,    11,     0,     0,    12,     2,     3,
      13,    14,     0,     0,     0,     0,    15,    16,    17,     0,
       4,    18,    19,    20,     0,     0,     0,    10,     0,     0,
      21,    22,     0,     0,     0,     0,     0,     0,     0,     0,
      23,     0,     0,    24,     0,     0,     0,     0,    25,    26,
     163,     0,     0,     0,    21,    22,   100,   101,   102,   103,
     104,   105,   106,   107,    23,     0,     0,    53,   296,     0,
       0,     0,    25,    26,   133,   134,   100,   101,   102,   103,
     104,   105,   106,   107,     0,     0,     0,     0,     0,     0,
       0,     0,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,     0,     0,     0,   119,   120,     0,   164,
       0,     0,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,     0,     0,     0,   119,   120,     0,     0,
     306,     0,     0,   135,   136,   137,   100,   101,   102,   103,
     104,   105,   106,   107,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   100,   101,   102,   103,   104,   105,
     106,   107,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,     0,     0,     0,   119,   120,     0,   307,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,     0,     0,     0,   119,   120,     0,   300,   100,   101,
     102,   103,   104,   105,   106,   107,     0,     0,     0,     0,
       0,     0,     0,   100,   101,   102,   103,   104,   105,   106,
     107,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,     0,     0,     0,   119,   120,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
       0,     0,     0,   119,   120,   100,   101,   102,   103,   104,
     105,   106,   107,     0,     0,     0,     0,     0,   100,   101,
     102,   103,   104,   105,   106,   107,     0,     0,     0,     0,
       0,   103,   104,   105,   106,   107,     0,     0,     0,     0,
       0,     0,     0,   110,   111,   112,   113,   114,   115,   116,
     117,   118,     0,     0,     0,   119,   120,   111,   112,   113,
     114,   115,   116,   117,   118,     0,     0,     0,   119,   120,
     114,   115,   116,   117,   118,     0,     0,     0,   119,   120
};

static const short yycheck[] =
{
       0,   123,    26,     6,    65,   152,    67,    68,    11,    12,
       1,     1,     5,     6,    14,     3,     3,     4,    21,    22,
      23,    24,    25,    26,     1,   142,    53,    26,    15,     1,
      30,     1,     5,     6,    15,    22,    14,    59,     1,     1,
       1,     0,     5,     6,    19,    17,    68,     1,    70,    15,
      53,     5,     6,     5,     6,    17,    17,    45,    85,     1,
      61,     3,    49,    50,     1,    30,     5,     6,    92,    69,
      63,    61,    59,    64,     1,    62,     3,     4,    69,    25,
      67,    68,    85,   230,     1,    84,    63,    64,    15,    92,
      60,     1,    91,     3,    64,    22,    69,   100,   101,   102,
     103,   104,   105,   106,    69,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    69,   121,   122,
     123,   238,    49,    50,    61,   128,     1,    61,   127,   276,
       5,     6,    59,    24,   281,    62,     5,     6,   138,     3,
      67,    68,   166,    60,   144,     5,     6,    64,   172,    59,
     153,    60,     3,     4,     3,    64,   156,   157,   305,   159,
     138,     5,     6,   166,    15,    27,   144,   170,   167,   172,
      28,    22,     5,     6,   173,     1,    61,     3,   156,   157,
       1,   159,     3,     1,    59,     3,    60,     5,     6,    64,
      64,   313,    43,    61,    29,    64,    65,    66,    49,    50,
     236,    52,     1,    63,     3,    18,     5,     6,    59,   248,
      59,    62,     1,    39,     3,   128,    67,    68,   221,    63,
       1,   267,     3,     4,    45,   225,   130,    92,   228,   290,
      -1,    64,   232,   257,    15,     1,    62,     3,   262,   239,
     243,    22,    -1,   246,   244,   245,    64,   225,     1,     1,
     250,     3,     5,     6,   257,     1,     1,     3,     3,   262,
      -1,   239,    43,   228,   267,    64,    -1,   232,    49,    50,
      -1,    52,   272,    -1,    64,   275,    66,    -1,    59,   244,
     245,    62,     1,   283,     3,   250,    67,    68,     1,    13,
      14,   291,     5,     6,     1,   298,     3,   275,    -1,    45,
      45,   301,    -1,   303,   304,    -1,    59,   272,   308,   309,
     313,    -1,    -1,   291,    60,    60,   316,   317,   283,   319,
     320,    -1,     1,   323,     3,   303,   304,    -1,    -1,    59,
     308,    61,    -1,    57,    58,    -1,   301,    -1,    68,    -1,
      70,   319,    -1,    -1,   309,    -1,    59,    -1,    61,    -1,
      -1,   316,   317,    -1,    -1,   320,     0,     1,   323,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    16,    -1,    18,    19,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,
      -1,    35,    36,    37,    -1,    -1,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    -1,    -1,
       0,    -1,    -1,     3,     4,    59,    -1,    -1,    62,    -1,
      64,    -1,    -1,    67,    68,    15,    16,    -1,    18,    19,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,     0,    -1,    -1,     3,     4,    59,
      -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    68,    15,
      16,    -1,    18,    19,    20,    21,    22,    23,    -1,    -1,
      26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,
      36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    -1,    -1,    -1,     0,    -1,
      -1,     3,     4,    59,    -1,    -1,    62,    -1,    -1,    -1,
      -1,    67,    68,    15,    16,    -1,    18,    19,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    -1,
      -1,    -1,     0,    -1,    -1,     3,     4,    59,    -1,    -1,
      62,    -1,    -1,    -1,    -1,    67,    68,    15,    16,    -1,
      18,    19,    20,    21,    22,    23,    -1,    -1,    26,    -1,
      -1,    29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,
      -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    -1,    -1,    -1,     0,    -1,    -1,     3,
       4,    59,    -1,    -1,    62,    -1,    -1,    -1,    -1,    67,
      68,    15,    16,    -1,    18,    19,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,
      -1,    35,    36,    37,    -1,    -1,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    -1,    -1,
       0,    -1,    -1,     3,     4,    59,    -1,    -1,    62,    -1,
      -1,    -1,    -1,    67,    68,    15,    16,    -1,    18,    19,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,     0,    -1,    -1,     3,     4,    59,
      -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    68,    15,
      16,    -1,    18,    -1,    20,    21,    22,    23,    -1,    -1,
      26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,
      36,    37,    -1,    -1,    40,    41,    42,     1,    -1,     3,
       4,     5,     6,    49,    50,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    -1,    59,    -1,    -1,    62,    -1,    22,    -1,
      -1,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    59,    -1,     1,    62,     3,
       4,     5,     6,    67,    68,    69,    -1,    -1,    -1,    -1,
      -1,    15,    16,    17,    18,    -1,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    51,    52,
      53,    35,    36,    37,    57,    58,    40,    41,    42,    -1,
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
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    15,    16,    -1,
      18,    -1,    20,    21,    22,    23,    -1,    -1,    26,    -1,
      -1,    29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,
      -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    -1,    -1,    -1,    -1,     1,    -1,     3,
       4,    59,    -1,    -1,    62,    -1,    64,    -1,    -1,    67,
      68,    15,    16,    17,    18,    -1,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    31,    -1,    -1,
      -1,    35,    36,    37,    -1,    -1,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    -1,    -1,
      -1,     1,    -1,     3,     4,    59,    -1,    -1,    62,    -1,
      -1,    -1,    -1,    67,    68,    15,    16,    17,    18,    -1,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    -1,    -1,    -1,    -1,     1,    -1,     3,     4,    59,
      -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    68,    15,
      16,    17,    18,    -1,    20,    21,    22,    23,    -1,    -1,
      26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,
      36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    -1,    -1,    -1,    -1,     1,
      -1,     3,     4,    59,    -1,    -1,    62,    -1,    -1,    -1,
      -1,    67,    68,    15,    16,    17,    18,    -1,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,
      42,     1,    -1,     3,     4,     5,     6,    49,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    15,     1,    59,     3,     4,
      62,    -1,    22,    -1,    -1,    67,    68,    -1,    -1,    -1,
      15,     1,    -1,     3,     4,    -1,    -1,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,     1,    -1,     3,     4,    59,
      -1,    -1,    62,    63,    49,    50,    -1,    67,    68,    15,
       1,    -1,     3,     4,    59,    -1,    22,    62,    -1,    49,
      50,    -1,    67,    68,    15,     1,    -1,     3,     4,    59,
      -1,    22,    62,    -1,    -1,    -1,    -1,    67,    68,    15,
      -1,    -1,    -1,    49,    50,    -1,    22,    -1,    -1,     1,
      -1,     3,     4,    59,    -1,    -1,    62,    -1,    49,    50,
      -1,    67,    68,    15,     1,    -1,     3,     4,    59,    -1,
      22,    62,    -1,    49,    50,    -1,    67,    68,    15,     1,
      -1,     3,     4,    59,    -1,    22,    62,    -1,    -1,    -1,
      -1,    67,    68,    15,    -1,    -1,    -1,    49,    50,    -1,
      22,    -1,    -1,     1,    -1,     3,     4,    59,    -1,    -1,
      62,    -1,    49,    50,    -1,    67,    68,    15,     1,    -1,
       3,     4,    59,    -1,    22,    62,    -1,    49,    50,    -1,
      67,    68,    15,     1,    -1,     3,     4,    59,    -1,    22,
      62,    -1,    -1,    -1,    -1,    67,    68,    15,    -1,    -1,
      -1,    49,    50,    -1,    22,    -1,    -1,     1,    -1,     3,
       4,    59,    -1,    -1,    62,    -1,    49,    50,    -1,    67,
      68,    15,     1,    -1,     3,     4,    59,    -1,    22,    62,
      -1,    49,    50,    -1,    67,    68,    15,     1,    -1,     3,
       4,    59,    -1,    22,    62,    -1,    -1,    -1,    -1,    67,
      68,    15,    -1,    -1,    -1,    49,    50,    -1,    22,    -1,
      -1,     1,    -1,     3,     4,    59,    -1,    -1,    62,    -1,
      49,    50,    -1,    67,    68,    15,     1,    -1,     3,     4,
      59,    -1,    22,    62,    -1,    49,    50,    -1,    67,    68,
      15,     1,    -1,     3,     4,    59,    -1,    22,    62,    -1,
      -1,    -1,    -1,    67,    68,    15,    -1,    -1,    -1,    49,
      50,    -1,    22,    -1,    -1,     1,    -1,     3,     4,    59,
      -1,    -1,    62,    -1,    49,    50,    -1,    67,    68,    15,
       1,    -1,     3,     4,    59,    -1,    22,    62,    -1,    49,
      50,    -1,    67,    68,    15,     1,    -1,     3,     4,    59,
      -1,    22,    62,    -1,    -1,    -1,    -1,    67,    68,    15,
      -1,    -1,    -1,    49,    50,    -1,    22,    -1,    -1,     1,
      -1,     3,     4,    59,    -1,    -1,    62,    -1,    49,    50,
      -1,    67,    68,    15,     1,    -1,     3,     4,    59,    -1,
      22,    62,    -1,    49,    50,    -1,    67,    68,    15,     1,
      -1,     3,     4,    59,    -1,    22,    62,    -1,    -1,    -1,
      -1,    67,    68,    15,    -1,    -1,    -1,    49,    50,    -1,
      22,    -1,    -1,     1,    -1,     3,     4,    59,    -1,    -1,
      62,    -1,    49,    50,    -1,    67,    68,    15,     1,    -1,
       3,     4,    59,    -1,    22,    62,    -1,    49,    50,    -1,
      67,    68,    15,     1,    -1,     3,     4,    59,    -1,    22,
      62,    -1,    -1,    -1,    -1,    67,    68,    15,    -1,    -1,
      -1,    49,    50,    -1,    22,    -1,    -1,     1,    -1,     3,
       4,    59,    -1,    -1,    62,    -1,    49,    50,    -1,    67,
      68,    15,    -1,    -1,     3,     4,    59,    -1,    22,    62,
      -1,    49,    50,    -1,    67,    68,    15,    -1,    -1,     3,
       4,    59,    -1,    22,    62,    -1,    -1,    -1,    -1,    67,
      68,    15,    -1,    -1,    -1,    49,    50,    -1,    22,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,    62,    -1,
      49,    50,    -1,    67,    68,     3,     4,    -1,    -1,    -1,
      59,    -1,    -1,    62,    -1,    49,    50,    15,    67,    68,
      69,    -1,    -1,    -1,    22,    59,    -1,    -1,    62,    -1,
      -1,    -1,     1,    67,    68,    69,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    -1,    -1,    62,    63,    -1,    -1,    -1,    67,
      68,    -1,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    57,    58,
       3,     4,     5,     6,    -1,    64,    65,    66,    -1,    -1,
      -1,    -1,    15,    16,    -1,    18,    -1,    20,    21,    22,
      23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,
      -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,    59,    -1,    -1,    62,
      -1,    64,    -1,    -1,    67,    68,    15,    16,    -1,    18,
      -1,    20,    21,    22,    23,    -1,    -1,    26,     3,     4,
      29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,
      15,    40,    41,    42,    -1,    -1,    -1,    22,    -1,    -1,
      49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      59,    -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    68,
       1,    -1,    -1,    -1,    49,    50,     7,     8,     9,    10,
      11,    12,    13,    14,    59,    -1,    -1,    62,    63,    -1,
      -1,    -1,    67,    68,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    -1,    -1,    57,    58,    -1,    60,
      -1,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    -1,    -1,    57,    58,    -1,    -1,
       1,    -1,    -1,    64,    65,    66,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    -1,    -1,    57,    58,    -1,    60,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    -1,    57,    58,    -1,    60,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     7,     8,     9,    10,    11,    12,    13,
      14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    57,    58,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      -1,    -1,    -1,    57,    58,     7,     8,     9,    10,    11,
      12,    13,    14,    -1,    -1,    -1,    -1,    -1,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    57,    58,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    57,    58,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    57,    58
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
     105,   106,     1,     5,     6,    64,    65,    66,    93,     1,
       5,     6,    59,    61,    94,    61,     1,     3,    45,    76,
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
      78,     1,    17,    61,    63,   120,    63,   118,   110,    69,
     120,    69,   118,     1,     3,     1,    60,    64,     1,    69,
     110,     1,    78,     1,    17,    94,    60,     1,    61,    77,
       1,    60,    76,    78,     1,   110,     1,    17,     1,    17,
     110,    28,    98,    97,     1,     3,    63,    69,    61,   114,
      60,    78,     1,    94,    94,    60,     1,    60,    93,    78,
       1,    17,     1,    59,     1,   110,    78,    78,     1,    94,
      78,     1,   115,    78,     1,    60
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
    {mainAST = yyvsp[0].v.p;}
    break;

  case 5:
#line 135 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list or function definition",yyvsp[0]);}
    break;

  case 6:
#line 139 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-6].v.p->toStringList();
     r->name = yyvsp[-5].v.p->text;
     r->arguments = yyvsp[-3].v.p->toStringList();
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
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
#line 156 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-5].v.p->text;
     r->arguments = yyvsp[-3].v.p->toStringList();
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
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
#line 172 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-3].v.p->toStringList();
     r->name = yyvsp[-2].v.p->text;
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
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
#line 188 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-2].v.p->text;
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
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
#line 203 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->returnVals = yyvsp[-5].v.p->toStringList();
     r->name = yyvsp[-4].v.p->text;
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
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
#line 219 "../../../libs/libFreeMat/Parser.yxx"
    {
     MFunctionDef *r;
     r = new MFunctionDef();
     r->name = yyvsp[-4].v.p->text;
     r->code = yyvsp[0].v.p;
     r->fileName = parsing_filename;
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

  case 12:
#line 234 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("legal function name or return declaration after 'function'",yyvsp[-1]);}
    break;

  case 13:
#line 235 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("argument list or statement list after identifier '") + 
	yyvsp[-1].v.p->text + "'",yyvsp[-1]);}
    break;

  case 14:
#line 237 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("(possibly empty) argument list after '('",yyvsp[-1]);}
    break;

  case 15:
#line 238 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list after ')'",yyvsp[-1]);}
    break;

  case 16:
#line 239 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("function name for function declared",yyvsp[-2]);}
    break;

  case 17:
#line 240 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("argument list or statement list following function name :") + 
	yyvsp[-1].v.p->text,yyvsp[-1]);}
    break;

  case 18:
#line 242 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("(possibly empty) argument list after '('",yyvsp[-1]);}
    break;

  case 19:
#line 243 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list after ')'",yyvsp[-1]);}
    break;

  case 22:
#line 252 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 23:
#line 253 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 24:
#line 254 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 25:
#line 255 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after identifier in return declaration",yyvsp[-1]);}
    break;

  case 26:
#line 256 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a valid list of return arguments in return declaration",yyvsp[-1]);}
    break;

  case 27:
#line 257 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching ']' in return declaration for '['",yyvsp[-2]);}
    break;

  case 28:
#line 258 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after return declaration",yyvsp[-1]);}
    break;

  case 29:
#line 262 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 30:
#line 263 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 32:
#line 267 "../../../libs/libFreeMat/Parser.yxx"
    {
	yyval.v.p = yyvsp[0].v.p;
	char *b = (char*) malloc(strlen(yyvsp[0].v.p->text)+2);
	b[0] = '&';
	strcpy(b+1,yyvsp[0].v.p->text);
	yyval.v.p->text = b;
  }
    break;

  case 33:
#line 278 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 34:
#line 279 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 35:
#line 283 "../../../libs/libFreeMat/Parser.yxx"
    {
  	    yyval.v.p = new AST(OP_QSTATEMENT,NULL,yyvsp[0].v.i);
	    yyval.v.p->down = yyvsp[-1].v.p;
	 }
    break;

  case 36:
#line 287 "../../../libs/libFreeMat/Parser.yxx"
    {
	    yyval.v.p = new AST(OP_RSTATEMENT,NULL,yyvsp[0].v.i);
            yyval.v.p->down = yyvsp[-1].v.p;
	 }
    break;

  case 37:
#line 291 "../../../libs/libFreeMat/Parser.yxx"
    {
	    yyval.v.p = new AST(OP_RSTATEMENT,NULL,yyvsp[0].v.i);
	    yyval.v.p->down = yyvsp[-1].v.p;
	 }
    break;

  case 40:
#line 300 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(null_node,"",-1);}
    break;

  case 56:
#line 318 "../../../libs/libFreeMat/Parser.yxx"
    {yyvsp[-1].v.p->addChild(yyvsp[0].v.p); 
				yyval.v.p = new AST(OP_SCALL,yyvsp[-1].v.p,yyvsp[-1].v.p->context());}
    break;

  case 57:
#line 323 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PARENS,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 58:
#line 324 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 59:
#line 328 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p); }
    break;

  case 60:
#line 329 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as persistent",yyvsp[-1]);}
    break;

  case 61:
#line 333 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 62:
#line 334 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as global",yyvsp[-1]);}
    break;

  case 64:
#line 339 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 65:
#line 340 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of valid identifiers",yyvsp[0]);}
    break;

  case 70:
#line 360 "../../../libs/libFreeMat/Parser.yxx"
    { yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-2].v.p); if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);}
    break;

  case 71:
#line 362 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("matching 'end' to 'try' clause from line ") + decodeline(yyvsp[-3]),yyvsp[0]);}
    break;

  case 72:
#line 366 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 73:
#line 367 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 74:
#line 371 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-5].v.p; yyval.v.p->addChild(yyvsp[-4].v.p); 
	  if (yyvsp[-2].v.p != NULL) yyval.v.p->addChild(yyvsp[-2].v.p); 
	  if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 75:
#line 376 "../../../libs/libFreeMat/Parser.yxx"
    {
          yyxpt(std::string("matching 'end' to 'switch' clause from line ") + decodeline(yyvsp[-5]),yyvsp[0]);
        }
    break;

  case 83:
#line 391 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 85:
#line 396 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_CASEBLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());
	}
    break;

  case 86:
#line 399 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 87:
#line 405 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-2].v.p); yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 88:
#line 411 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 89:
#line 414 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = NULL;
	}
    break;

  case 90:
#line 420 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 91:
#line 424 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'for' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 92:
#line 428 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-1].v.p);}
    break;

  case 93:
#line 429 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 94:
#line 430 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p; 
	      yyval.v.p->addChild(new AST(OP_RHS,new AST(id_node,yyvsp[0].v.p->text,yyvsp[0].v.p->context()),yyvsp[0].v.p->context())); }
    break;

  case 95:
#line 432 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-4]);}
    break;

  case 96:
#line 433 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression",yyvsp[-1]);}
    break;

  case 97:
#line 434 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("equals operator after loop index",yyvsp[-1]);}
    break;

  case 98:
#line 435 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier that is the loop variable",yyvsp[-1]);}
    break;

  case 99:
#line 436 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression",yyvsp[-1]);}
    break;

  case 100:
#line 437 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier or assignment (id = expr) after 'for' ",yyvsp[0]);}
    break;

  case 101:
#line 441 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 102:
#line 444 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test expression after 'while'",yyvsp[-1]);}
    break;

  case 103:
#line 446 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'while' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 104:
#line 450 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); if (yyvsp[-2].v.p != NULL) yyval.v.p->addChild(yyvsp[-2].v.p); 
	  if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 105:
#line 454 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("condition expression for 'if'",yyvsp[-1]);}
    break;

  case 106:
#line 455 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'if' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 107:
#line 459 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_CSTAT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-2].v.p->context());
	}
    break;

  case 108:
#line 462 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("valid list of statements after condition",yyvsp[0]);}
    break;

  case 109:
#line 466 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 111:
#line 471 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_ELSEIFBLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());
	}
    break;

  case 112:
#line 474 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 113:
#line 480 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 114:
#line 483 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test condition for 'elseif' clause",yyvsp[-1]);}
    break;

  case 115:
#line 486 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 116:
#line 489 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 117:
#line 490 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list for 'else' clause",yyvsp[-1]);}
    break;

  case 118:
#line 494 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ASSIGN,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 119:
#line 495 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expression in assignment",yyvsp[-1]);}
    break;

  case 120:
#line 499 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[-3].v.p->addChild(new AST(OP_PARENS,yyvsp[-1].v.p,yyvsp[-2].v.i));
	  yyval.v.p = new AST(OP_MULTICALL,yyvsp[-6].v.p,yyvsp[-3].v.p,yyvsp[-7].v.i);
	}
    break;

  case 121:
#line 503 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[0].v.p->addChild(new AST(OP_PARENS,NULL,-1));
	  yyval.v.p = new AST(OP_MULTICALL,yyvsp[-3].v.p,yyvsp[0].v.p,yyvsp[-4].v.i);
	}
    break;

  case 122:
#line 508 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-2]);}
    break;

  case 123:
#line 510 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing list",yyvsp[-1]);}
    break;

  case 124:
#line 512 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("left parenthesis",yyvsp[-1]);}
    break;

  case 125:
#line 514 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier",yyvsp[-1]);}
    break;

  case 126:
#line 518 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_COLON,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 127:
#line 519 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after ':'",yyvsp[-1]);}
    break;

  case 129:
#line 521 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PLUS,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 130:
#line 522 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '+'",yyvsp[-1]);}
    break;

  case 131:
#line 523 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SUBTRACT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 132:
#line 524 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '-'",yyvsp[-1]);}
    break;

  case 133:
#line 525 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_TIMES,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 134:
#line 526 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '*'",yyvsp[-1]);}
    break;

  case 135:
#line 527 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_RDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 136:
#line 528 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '/'",yyvsp[-1]);}
    break;

  case 137:
#line 529 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 138:
#line 530 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '\\'",yyvsp[-1]);}
    break;

  case 139:
#line 531 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_OR,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 140:
#line 532 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '|'",yyvsp[-1]);}
    break;

  case 141:
#line 533 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_AND,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 142:
#line 534 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '&'",yyvsp[-1]);}
    break;

  case 143:
#line 535 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 144:
#line 536 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<'",yyvsp[-1]);}
    break;

  case 145:
#line 537 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 146:
#line 538 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<='",yyvsp[-1]);}
    break;

  case 147:
#line 539 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_GT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 148:
#line 540 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>'",yyvsp[-1]);}
    break;

  case 149:
#line 541 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_GEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 150:
#line 542 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>='",yyvsp[-1]);}
    break;

  case 151:
#line 543 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 152:
#line 544 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '=='",yyvsp[-1]);}
    break;

  case 153:
#line 545 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 154:
#line 546 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '~='",yyvsp[-1]);}
    break;

  case 155:
#line 547 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_TIMES,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 156:
#line 548 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.*'",yyvsp[-1]);}
    break;

  case 157:
#line 549 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_RDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 158:
#line 550 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after './'",yyvsp[-1]);}
    break;

  case 159:
#line 551 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_LDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 160:
#line 552 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.\\'",yyvsp[-1]);}
    break;

  case 161:
#line 553 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NEG,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 162:
#line 554 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 163:
#line 555 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NOT,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 164:
#line 556 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after logical not",yyvsp[0]);}
    break;

  case 165:
#line 557 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_POWER,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 166:
#line 558 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '^'",yyvsp[-1]);}
    break;

  case 167:
#line 559 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_POWER,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 168:
#line 560 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.^'",yyvsp[-1]);}
    break;

  case 169:
#line 561 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_TRANSPOSE,yyvsp[-1].v.p,yyvsp[0].v.i);}
    break;

  case 170:
#line 562 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_TRANSPOSE,yyvsp[-1].v.p,yyvsp[0].v.i);}
    break;

  case 171:
#line 563 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 172:
#line 564 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a right parenthesis after expression to match this one",yyvsp[-2]);}
    break;

  case 173:
#line 565 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after left parenthesis",yyvsp[-1]);}
    break;

  case 177:
#line 572 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_RHS,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 178:
#line 573 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 179:
#line 574 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a matrix definition followed by a right bracket",yyvsp[-1]);}
    break;

  case 180:
#line 575 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 181:
#line 576 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 182:
#line 577 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 183:
#line 578 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EMPTY,NULL,yyvsp[-1].v.i);}
    break;

  case 184:
#line 579 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 185:
#line 580 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 186:
#line 581 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 187:
#line 582 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 188:
#line 583 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EMPTY_CELL,NULL,yyvsp[-1].v.i);}
    break;

  case 189:
#line 584 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a cell-array definition followed by a right brace",yyvsp[-1]);}
    break;

  case 191:
#line 588 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 192:
#line 592 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PARENS,yyvsp[-1].v.p,yyvsp[-2].v.i); }
    break;

  case 193:
#line 593 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-2]);}
    break;

  case 194:
#line 594 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACES,yyvsp[-1].v.p,yyvsp[-2].v.i); }
    break;

  case 195:
#line 595 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right brace",yyvsp[-2]);}
    break;

  case 196:
#line 596 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT,yyvsp[0].v.p,yyvsp[-1].v.i); }
    break;

  case 197:
#line 597 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOTDYN,yyvsp[-1].v.p,yyvsp[-3].v.i);}
    break;

  case 199:
#line 602 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ALL,NULL,yyvsp[0].v.i);}
    break;

  case 200:
#line 603 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_KEYWORD,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-3].v.i);}
    break;

  case 201:
#line 604 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting expression after '=' in keyword assignment",yyvsp[-1]);}
    break;

  case 202:
#line 605 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_KEYWORD,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 203:
#line 606 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting keyword identifier after '/' in keyword assignment",yyvsp[-1]);}
    break;

  case 205:
#line 611 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addPeer(yyvsp[0].v.p);}
    break;

  case 206:
#line 615 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACES,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 207:
#line 616 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 208:
#line 620 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACKETS,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 209:
#line 621 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 214:
#line 633 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SEMICOLON,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 215:
#line 634 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2884 "../../../libs/libFreeMat/Parser.cxx"

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


#line 637 "../../../libs/libFreeMat/Parser.yxx"


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
    resetParser();
    interactiveMode = false;
    parsing_filename = fname;
    setLexFile(fp);
    yyparse();
    return parseState();
  }
  
}

