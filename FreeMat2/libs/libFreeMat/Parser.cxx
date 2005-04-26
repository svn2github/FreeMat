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
     SOR = 298,
     SAND = 299,
     NE = 300,
     NOT = 301,
     NEG = 302,
     POS = 303
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
#define SOR 298
#define SAND 299
#define NE 300
#define NOT 301
#define NEG 302
#define POS 303




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
#define YYFINAL  99
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2575

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  49
/* YYNRULES -- Number of rules. */
#define YYNRULES  226
/* YYNRULES -- Number of states. */
#define YYNSTATES  340

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   303

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    68,     2,     2,    46,    60,
      61,    62,    53,    51,    66,    52,    73,    54,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    50,    67,
      47,    63,    48,     2,    70,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    64,    55,    65,    59,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    71,    45,    72,    69,     2,     2,     2,
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
      49,    56,    57,    58
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
     366,   372,   376,   379,   382,   383,   385,   387,   390,   393,
     396,   399,   400,   403,   407,   411,   420,   426,   435,   443,
     450,   456,   460,   464,   466,   470,   474,   478,   482,   486,
     490,   494,   498,   502,   506,   510,   514,   518,   522,   526,
     530,   534,   538,   542,   546,   550,   554,   558,   562,   566,
     570,   574,   578,   582,   586,   590,   594,   598,   602,   606,
     610,   613,   616,   619,   622,   626,   630,   634,   638,   641,
     644,   648,   652,   655,   657,   659,   661,   664,   666,   670,
     673,   678,   683,   689,   692,   696,   701,   706,   712,   715,
     718,   720,   723,   727,   730,   734,   738,   742,   745,   750,
     752,   754,   759,   764,   767,   770,   772,   776,   778,   782,
     784,   788,   790,   792,   794,   796,   798
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      75,     0,    -1,    81,    -1,    77,    -1,    -1,     1,    -1,
      19,    78,     3,    61,    79,    62,    96,    81,    -1,    19,
       3,    61,    79,    62,    96,    81,    -1,    19,    78,     3,
      96,    81,    -1,    19,     3,    96,    81,    -1,    19,    78,
       3,    61,    62,    96,    81,    -1,    19,     3,    61,    62,
      96,    81,    -1,    19,     1,    -1,    19,     3,     1,    -1,
      19,     3,    61,     1,    -1,    19,     3,    61,    79,    62,
       1,    -1,    19,    78,     1,    -1,    19,    78,     3,     1,
      -1,    19,    78,     3,    61,     1,    -1,    19,    78,     3,
      61,    79,    62,     1,    -1,    76,    -1,    77,    76,    -1,
      39,    63,    -1,     3,    63,    -1,    64,    79,    65,    63,
      -1,     3,     1,    -1,    64,     1,    -1,    64,    79,     1,
      -1,    64,    79,    65,     1,    -1,    80,    -1,    79,    66,
      80,    -1,     3,    -1,    46,     3,    -1,    82,    -1,    81,
      82,    -1,    83,     5,    -1,    83,     6,    -1,    83,    66,
      -1,   110,    -1,   112,    -1,    -1,   111,    -1,   101,    -1,
      91,    -1,    90,    -1,   103,    -1,   104,    -1,    94,    -1,
      92,    -1,    89,    -1,    88,    -1,    86,    -1,    85,    -1,
      84,    -1,    41,    -1,    42,    -1,     3,     4,    -1,    15,
      15,    -1,     3,    15,    -1,     3,     3,    -1,    84,    15,
      -1,    84,     3,    -1,    84,     4,    -1,    35,    87,    -1,
      35,     1,    -1,    40,    87,    -1,    40,     1,    -1,     3,
      -1,    87,     3,    -1,    87,     1,    -1,    37,    -1,    36,
      -1,    29,    -1,    21,    -1,    30,    81,    93,    17,    -1,
      30,    81,    93,     1,    -1,    31,    81,    -1,    -1,    26,
     112,    95,    97,   100,    17,    -1,    26,   112,    95,    97,
     100,     1,    -1,    66,    -1,     6,    -1,     5,    -1,    67,
      -1,    68,    -1,     6,    -1,     5,    -1,    -1,    98,    -1,
      99,    -1,    98,    99,    -1,    27,   112,    95,    81,    -1,
      28,    81,    -1,    -1,    20,   102,    95,    81,    17,    -1,
      20,   102,    95,    81,     1,    -1,    61,     3,    63,   112,
      62,    -1,     3,    63,   112,    -1,     3,    -1,    61,     3,
      63,   112,     1,    -1,    61,     3,    63,     1,    -1,    61,
       3,     1,    -1,    61,     1,    -1,     3,    63,     1,    -1,
       1,    -1,    23,   112,    95,    81,    17,    -1,    23,     1,
      -1,    23,   112,    95,    81,     1,    -1,    18,   105,   106,
     109,    17,    -1,    18,     1,    -1,    18,   105,   106,   109,
       1,    -1,   112,    95,    81,    -1,   112,    95,    -1,   112,
       1,    -1,    -1,   107,    -1,   108,    -1,   107,   108,    -1,
      25,   105,    -1,    25,     1,    -1,    24,    81,    -1,    -1,
      24,     1,    -1,   114,    63,   112,    -1,   114,    63,     1,
      -1,    64,   119,    65,    63,     3,    61,   117,    62,    -1,
      64,   119,    65,    63,     3,    -1,    64,   119,    65,    63,
       3,    61,   117,     1,    -1,    64,   119,    65,    63,     3,
      61,     1,    -1,    64,   119,    65,    63,     3,     1,    -1,
      64,   119,    65,    63,     1,    -1,   112,    50,   112,    -1,
     112,    50,     1,    -1,   113,    -1,   112,    51,   112,    -1,
     112,    51,     1,    -1,   112,    52,   112,    -1,   112,    52,
       1,    -1,   112,    53,   112,    -1,   112,    53,     1,    -1,
     112,    54,   112,    -1,   112,    54,     1,    -1,   112,    55,
     112,    -1,   112,    55,     1,    -1,   112,    45,   112,    -1,
     112,    45,     1,    -1,   112,    46,   112,    -1,   112,    46,
       1,    -1,   112,    43,   112,    -1,   112,    43,     1,    -1,
     112,    44,   112,    -1,   112,    44,     1,    -1,   112,    47,
     112,    -1,   112,    47,     1,    -1,   112,     7,   112,    -1,
     112,     7,     1,    -1,   112,    48,   112,    -1,   112,    48,
       1,    -1,   112,     8,   112,    -1,   112,     8,     1,    -1,
     112,     9,   112,    -1,   112,     9,     1,    -1,   112,    49,
     112,    -1,   112,    49,     1,    -1,   112,    10,   112,    -1,
     112,    10,     1,    -1,   112,    11,   112,    -1,   112,    11,
       1,    -1,   112,    12,   112,    -1,   112,    12,     1,    -1,
      52,   112,    -1,    51,   112,    -1,    69,   112,    -1,    69,
       1,    -1,   112,    59,   112,    -1,   112,    59,     1,    -1,
     112,    13,   112,    -1,   112,    13,     1,    -1,   112,    60,
      -1,   112,    14,    -1,    61,   112,    62,    -1,    61,   112,
       1,    -1,    61,     1,    -1,     4,    -1,    15,    -1,    22,
      -1,    70,     3,    -1,   114,    -1,    64,   119,    65,    -1,
      64,     1,    -1,    64,   120,   119,    65,    -1,    64,   119,
     120,    65,    -1,    64,   120,   119,   120,    65,    -1,    64,
      65,    -1,    71,   118,    72,    -1,    71,   120,   118,    72,
      -1,    71,   118,   120,    72,    -1,    71,   120,   118,   120,
      72,    -1,    71,    72,    -1,    71,     1,    -1,     3,    -1,
     114,   115,    -1,    61,   117,    62,    -1,    61,    62,    -1,
      61,   117,     1,    -1,    71,   117,    72,    -1,    71,   117,
       1,    -1,    73,     3,    -1,    73,    61,   112,    62,    -1,
     112,    -1,    50,    -1,    54,     3,    63,   112,    -1,    54,
       3,    63,     1,    -1,    54,     3,    -1,    54,     1,    -1,
     116,    -1,   117,    66,   116,    -1,   122,    -1,   118,   120,
     122,    -1,   122,    -1,   119,   120,   122,    -1,     6,    -1,
       5,    -1,    66,    -1,    68,    -1,   112,    -1,   122,   121,
     112,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   152,   152,   153,   153,   154,   158,   168,   177,   186,
     194,   203,   211,   212,   214,   215,   216,   217,   219,   220,
     224,   225,   229,   230,   231,   232,   233,   234,   235,   239,
     240,   244,   244,   255,   256,   260,   264,   268,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   291,   295,   296,   297,   298,
     299,   300,   301,   314,   315,   319,   320,   324,   325,   326,
     330,   334,   338,   341,   345,   347,   352,   353,   357,   362,
     368,   368,   368,   368,   368,   372,   372,   377,   378,   382,
     385,   391,   397,   400,   406,   409,   414,   415,   416,   418,
     419,   420,   421,   422,   423,   427,   430,   431,   436,   440,
     441,   445,   448,   451,   455,   456,   460,   463,   469,   472,
     475,   478,   479,   483,   484,   488,   492,   496,   498,   500,
     502,   507,   508,   509,   510,   511,   512,   513,   514,   515,
     516,   517,   518,   519,   520,   521,   522,   523,   524,   525,
     526,   527,   528,   529,   530,   531,   532,   533,   534,   535,
     536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
     546,   547,   548,   549,   550,   551,   552,   553,   554,   555,
     556,   557,   558,   562,   563,   564,   565,   566,   567,   568,
     569,   570,   571,   572,   573,   574,   575,   576,   577,   578,
     581,   582,   586,   587,   588,   589,   590,   591,   592,   596,
     597,   598,   599,   600,   601,   605,   606,   610,   611,   615,
     616,   620,   620,   624,   624,   628,   629
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
  "VARARGOUT", "GLOBAL", "QUIT", "RETALL", "SOR", "SAND", "'|'", "'&'",
  "'<'", "'>'", "NE", "':'", "'+'", "'-'", "'*'", "'/'", "'\\\\'", "NOT",
  "NEG", "POS", "'^'", "'''", "'('", "')'", "'='", "'['", "']'", "','",
  "';'", "'#'", "'~'", "'@'", "'{'", "'}'", "'.'", "$accept", "program",
  "functionDef", "functionDefList", "returnDeclaration", "argumentList",
  "argument", "statementList", "statement", "statementType", "specialCall",
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
     295,   296,   297,   298,   299,   124,    38,    60,    62,   300,
      58,    43,    45,    42,    47,    92,   301,   302,   303,    94,
      39,    40,    41,    61,    91,    93,    44,    59,    35,   126,
      64,   123,   125,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    74,    75,    75,    75,    75,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      77,    77,    78,    78,    78,    78,    78,    78,    78,    79,
      79,    80,    80,    81,    81,    82,    82,    82,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    84,    84,    84,    84,
      84,    84,    84,    85,    85,    86,    86,    87,    87,    87,
      88,    89,    90,    91,    92,    92,    93,    93,    94,    94,
      95,    95,    95,    95,    95,    96,    96,    97,    97,    98,
      98,    99,   100,   100,   101,   101,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   103,   103,   103,   104,   104,
     104,   105,   105,   105,   106,   106,   107,   107,   108,   108,
     109,   109,   109,   110,   110,   111,   111,   111,   111,   111,
     111,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     114,   114,   115,   115,   115,   115,   115,   115,   115,   116,
     116,   116,   116,   116,   116,   117,   117,   118,   118,   119,
     119,   120,   120,   121,   121,   122,   122
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
       5,     3,     2,     2,     0,     1,     1,     2,     2,     2,
       2,     0,     2,     3,     3,     8,     5,     8,     7,     6,
       5,     3,     3,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     2,     2,     3,     3,     3,     3,     2,     2,
       3,     3,     2,     1,     1,     1,     2,     1,     3,     2,
       4,     4,     5,     2,     3,     4,     4,     5,     2,     2,
       1,     2,     3,     2,     3,     3,     3,     2,     4,     1,
       1,     4,     4,     2,     2,     1,     3,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     5,   200,   183,   184,     0,     0,     0,    73,   185,
       0,     0,    72,    40,     0,    71,    70,     0,    54,    55,
       0,     0,     0,     0,     0,     0,     0,     0,    20,     3,
      40,    33,     0,    53,    52,    51,    50,    49,    44,    43,
      48,    47,    42,    45,    46,    38,    41,    39,   133,   187,
      59,    56,    58,    57,   109,   200,   184,     0,   114,     0,
     187,    12,     0,     0,     0,     0,   104,    98,     0,     0,
     106,     0,     0,    40,    64,    67,     0,    66,     0,   171,
     170,   182,     0,   189,   222,   221,   193,   225,     0,     0,
     219,   173,   172,   186,   199,   198,     0,     0,   217,     1,
      21,    34,    35,    36,    37,    61,    62,    60,     0,     0,
       0,     0,     0,     0,     0,   179,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     178,     0,     0,     0,     0,   201,     0,     0,   121,   115,
     116,   113,    82,    81,    80,    83,    84,   112,    13,    86,
      85,     0,    23,    40,    22,    26,    31,     0,     0,    29,
      16,     0,     0,   102,     0,    40,    40,    87,    40,     0,
      69,    68,   181,   180,   188,     0,     0,   223,   224,     0,
     194,     0,     0,   155,   154,   159,   158,   161,   160,   165,
     164,   167,   166,   169,   168,   177,   176,   149,   148,   151,
     150,   145,   144,   147,   146,   153,   152,   157,   156,   163,
     162,   132,   131,   135,   134,   137,   136,   139,   138,   141,
     140,   143,   142,   175,   174,   210,     0,   203,   209,   215,
       0,   124,   123,     0,   207,     0,   188,   119,   118,     0,
       0,   117,   111,    14,     0,     0,    40,    32,    27,     0,
       0,    17,     0,    40,   103,    97,   101,     0,     0,     0,
       0,    93,    88,    89,    40,    75,    74,     0,   191,   220,
     190,     0,   226,   196,   218,   195,     0,   214,   213,   204,
     202,     0,   206,   205,     0,   122,    40,   110,   108,    40,
       0,    28,    24,    30,    18,     0,     0,    40,   100,     0,
      95,    94,   107,   105,     0,    40,     0,    90,   130,     0,
     192,   197,     0,   216,   208,    40,    15,    40,    40,     0,
      99,    96,    40,    40,    79,    78,   129,     0,   212,   211,
      40,    40,    19,    40,    91,   128,     0,    40,   127,   125
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    27,    28,    29,    65,   158,   159,    30,    31,    32,
      33,    34,    35,    76,    36,    37,    38,    39,    40,   169,
      41,   147,   153,   261,   262,   263,   306,    42,    69,    43,
      44,    58,   138,   139,   140,   240,    45,    46,    47,    48,
      60,   135,   229,   230,    96,    88,    89,   179,    90
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -150
static const short yypact[] =
{
     346,  -150,   280,  -150,    11,   230,   179,    42,  -150,  -150,
    1242,  2064,  -150,  1937,   149,  -150,  -150,   183,  -150,  -150,
    2064,  2064,  1257,  1226,  1285,    28,   743,    34,  -150,    20,
     700,  -150,   208,   299,  -150,  -150,  -150,  -150,  -150,  -150,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  2315,  -150,   199,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  1226,    38,  2131,
     318,  -150,   216,     5,   198,   248,  -150,    39,   262,    72,
    -150,  2157,  2157,  1006,  -150,  -150,    35,  -150,   203,   160,
     160,  -150,  2219,  -150,  -150,  -150,  -150,  2315,   191,  2064,
     244,  -150,   160,  -150,  -150,  -150,    10,  2064,   244,  -150,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  -150,  1301,  1316,
    1344,  1360,  1375,  1403,  1419,  -150,  1434,  1462,  1478,  1493,
    1521,  1537,  1552,  1580,  1596,  1611,  1639,  1655,  1670,  1698,
    -150,  1961,  1714,  1987,     6,  -150,   222,  1729,   112,    38,
    -150,  -150,  -150,  -150,  -150,  -150,  -150,  1878,   295,  -150,
    -150,   161,  -150,  1937,  -150,  -150,  -150,   146,     9,  -150,
    -150,   224,  1757,  -150,    21,  1937,  1937,   131,  1937,    36,
    -150,  -150,  -150,  -150,   104,  2013,   242,  -150,  -150,  2064,
    -150,   728,    19,  -150,  2464,  -150,  2464,  -150,  2464,  -150,
     160,  -150,   160,  -150,   160,  -150,   160,  -150,  2369,  -150,
    2385,  -150,  2439,  -150,  2453,  -150,  2464,  -150,  2464,  -150,
    2464,  -150,  2515,  -150,  1801,  -150,  1801,  -150,   160,  -150,
     160,  -150,   160,  -150,   160,  -150,   335,  -150,  2315,  -150,
      95,  -150,  2315,    13,  -150,  2064,  -150,  -150,  -150,   947,
      45,  -150,  1878,  -150,   210,    -5,    29,  -150,  -150,    41,
     234,  -150,   178,  1937,  -150,  2315,  -150,  1773,   805,   876,
    2064,   143,   131,  -150,  1065,  -150,  -150,   340,  -150,   244,
    -150,  2039,  2315,  -150,   244,  -150,  1816,  -150,   122,  -150,
    -150,  1987,  -150,  -150,  2295,  -150,  1124,  -150,  -150,  1937,
     126,  -150,  -150,  -150,  -150,   210,    89,   405,  -150,  2239,
    -150,  -150,  -150,  -150,  2157,  1937,    55,  -150,  -150,    82,
    -150,  -150,  1788,  -150,  -150,   464,  -150,  1937,  1937,   287,
    -150,  -150,  1937,  1183,  -150,  -150,  -150,   141,  -150,  2315,
     523,   582,  -150,  1937,  1878,  -150,   132,   641,  -150,  -150
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -150,  -150,   174,  -150,  -150,  -140,   -61,    22,    62,  -150,
    -150,  -150,  -150,   209,  -150,  -150,  -150,  -150,  -150,  -150,
    -150,   -68,  -149,  -150,  -150,   -27,  -150,  -150,  -150,  -150,
    -150,   101,  -150,  -150,   102,  -150,  -150,  -150,    -3,  -150,
       0,  -150,   -15,  -127,   170,   -29,     1,  -150,   -21
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -127
static const short yytable[] =
{
      49,   165,    59,   166,   167,    98,   233,    71,    72,   234,
     248,   245,   253,    49,   282,    84,    85,    79,    80,    82,
      87,    92,   256,    87,    84,    85,    53,    97,   136,    -9,
      49,    93,     2,     3,    99,    73,   170,   265,   171,     6,
     -63,   -63,   291,    66,     4,    67,   287,     5,    -9,     7,
       8,     9,    10,   266,    87,    11,   324,   290,    12,    13,
     176,   250,   288,   137,    14,    15,    16,   235,   154,    17,
      18,    19,   325,    49,   249,   250,    98,   142,   143,   281,
      20,    21,   180,   326,   257,   283,    87,  -126,  -126,   175,
      22,   275,   101,    23,    87,   289,   279,   181,    24,    25,
      26,   -63,   162,    68,   292,   184,   186,   188,   190,   192,
     194,   196,   296,   198,   200,   202,   204,   206,   208,   210,
     212,   214,   216,   218,   220,   222,   224,   316,   228,   232,
     228,   149,   150,   338,    59,   101,   239,   175,   144,   145,
     146,   317,   335,   327,    55,     3,   318,    49,  -126,   247,
      74,   319,    75,    49,   269,   250,    56,   280,   260,   255,
     274,   281,   243,     9,   156,    49,    49,   267,    49,   242,
     333,   305,    87,   114,   115,   246,   272,   271,    87,   294,
      61,   156,    62,   276,    77,   312,    75,   258,   259,   293,
     264,   225,    20,    21,   339,   226,    84,    85,   281,   155,
     336,   156,    22,   100,   170,    57,   171,   157,   -65,   -65,
      24,    25,    26,   102,   103,   149,   150,   148,    63,   129,
     130,   149,   150,   244,   157,   251,    78,    84,    85,   149,
     150,    54,   284,    55,     3,   307,   322,   156,   238,    49,
     295,   241,    49,    64,   157,    56,    49,    84,    85,   160,
     269,   161,     9,    49,   299,   274,   174,   304,    49,    49,
     131,   286,   132,   163,    49,   164,   313,   182,    87,   -65,
     133,     0,   134,    87,   104,   297,     0,   151,   228,   152,
     157,    20,    21,    50,    51,   252,    49,   236,   332,    49,
       0,    22,   149,   150,    57,    52,   -25,    49,   -25,    24,
      25,    26,   105,   106,   101,    49,     0,   270,   101,   329,
     177,   315,   178,     0,   107,    49,     0,    49,    49,     0,
     101,   101,    49,    49,   228,     0,   101,   323,     0,     0,
      49,    49,     0,    49,    49,     0,   277,    49,   278,   330,
     331,   308,     0,   309,   334,     0,    -4,     1,   101,     2,
       3,   -40,   -40,     0,     0,   337,     0,     0,     0,   101,
       0,     4,     0,     0,     5,     6,     7,     8,     9,    10,
       0,     0,    11,     0,     0,    12,    13,   101,     0,   131,
       0,    14,    15,    16,     0,   101,    17,    18,    19,   133,
       0,   134,   101,   101,     0,     0,   101,    20,    21,   101,
       0,     0,     0,     0,     0,    -8,     0,    22,     2,     3,
      23,     0,   -40,     0,     0,    24,    25,    26,     0,     0,
       4,     0,     0,     5,    -8,     7,     8,     9,    10,     0,
       0,    11,     0,     0,    12,    13,     0,     0,     0,     0,
      14,    15,    16,     0,     0,    17,    18,    19,     0,     0,
       0,     0,     0,     0,     0,     0,    20,    21,     0,     0,
       0,     0,     0,     0,   -11,     0,    22,     2,     3,    23,
       0,     0,     0,     0,    24,    25,    26,     0,     0,     4,
       0,     0,     5,   -11,     7,     8,     9,    10,     0,     0,
      11,     0,     0,    12,    13,     0,     0,     0,     0,    14,
      15,    16,     0,     0,    17,    18,    19,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,     0,     0,     0,
       0,     0,     0,    -7,     0,    22,     2,     3,    23,     0,
       0,     0,     0,    24,    25,    26,     0,     0,     4,     0,
       0,     5,    -7,     7,     8,     9,    10,     0,     0,    11,
       0,     0,    12,    13,     0,     0,     0,     0,    14,    15,
      16,     0,     0,    17,    18,    19,     0,     0,     0,     0,
       0,     0,     0,     0,    20,    21,     0,     0,     0,     0,
       0,     0,   -10,     0,    22,     2,     3,    23,     0,     0,
       0,     0,    24,    25,    26,     0,     0,     4,     0,     0,
       5,   -10,     7,     8,     9,    10,     0,     0,    11,     0,
       0,    12,    13,     0,     0,     0,     0,    14,    15,    16,
       0,     0,    17,    18,    19,     0,     0,     0,     0,     0,
       0,     0,     0,    20,    21,     0,     0,     0,     0,     0,
       0,    -6,     0,    22,     2,     3,    23,     0,     0,     0,
       0,    24,    25,    26,     0,     0,     4,     0,     0,     5,
      -6,     7,     8,     9,    10,     0,     0,    11,     0,     0,
      12,    13,     0,     0,     0,     0,    14,    15,    16,     0,
       0,    17,    18,    19,     0,     0,     0,     0,     0,     0,
       0,     0,    20,    21,     0,     0,     0,     0,     0,     0,
      -2,     0,    22,     2,     3,    23,     0,     0,     0,     0,
      24,    25,    26,     0,     0,     4,     0,     0,     5,     0,
       7,     8,     9,    10,     0,     0,    11,     0,     0,    12,
      13,    55,     3,     0,     0,    14,    15,    16,     0,     0,
      17,    18,    19,    56,    94,     0,    55,     3,    84,    85,
       9,    20,    21,     0,     0,     0,     0,     0,    56,     0,
       0,    22,     0,     0,    23,     9,     0,     0,     0,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,    20,
      21,     0,     0,     0,     0,     0,     0,     0,     0,    22,
       0,     0,    57,     0,    20,    21,     0,    24,    25,    26,
     273,     0,     0,     0,    22,     0,   300,    57,     2,     3,
     -40,   -40,    24,    25,    26,    95,     0,     0,     0,     0,
       4,     0,   301,     5,     0,     7,     8,     9,    10,     0,
       0,    11,     0,     0,    12,    13,     0,     0,     0,     0,
      14,    15,    16,     0,     0,    17,    18,    19,     0,     0,
       0,     0,     0,     0,     0,     0,    20,    21,     0,     0,
       0,     0,     0,     0,     0,     0,    22,     0,     0,    23,
       0,   -40,     0,     0,    24,    25,    26,   302,     0,     2,
       3,   -40,   -40,     0,     0,     0,     0,     0,     0,     0,
       0,     4,     0,   303,     5,     0,     7,     8,     9,    10,
       0,     0,    11,     0,     0,    12,    13,     0,     0,     0,
       0,    14,    15,    16,     0,     0,    17,    18,    19,     0,
       0,     0,     0,     0,     0,     0,     0,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,    22,     0,     0,
      23,     0,   -40,     0,     0,    24,    25,    26,   285,     0,
       2,     3,   -40,   -40,     0,     0,     0,     0,     0,     0,
       0,     0,     4,     0,     0,     5,     0,     7,     8,     9,
      10,     0,     0,    11,     0,     0,    12,    13,     0,     0,
       0,     0,    14,    15,    16,     0,     0,    17,    18,    19,
       0,     0,     0,     0,     0,     0,     0,     0,    20,    21,
       0,     0,     0,     0,     0,     0,     0,   -77,    22,     2,
       3,    23,     0,   -40,     0,     0,    24,    25,    26,     0,
       0,     4,     0,   -77,     5,     0,     7,     8,     9,    10,
       0,     0,    11,     0,     0,    12,    13,   168,     0,     0,
       0,    14,    15,    16,     0,     0,    17,    18,    19,     0,
       0,     0,     0,     0,     0,     0,     0,    20,    21,     0,
       0,     0,     0,     0,     0,     0,   -76,    22,     2,     3,
      23,     0,     0,     0,     0,    24,    25,    26,     0,     0,
       4,     0,   -76,     5,     0,     7,     8,     9,    10,     0,
       0,    11,     0,     0,    12,    13,     0,     0,     0,     0,
      14,    15,    16,     0,     0,    17,    18,    19,     0,     0,
       0,     0,     0,     0,     0,     0,    20,    21,     0,     0,
       0,     0,     0,     0,     0,  -120,    22,     2,     3,    23,
       0,     0,     0,     0,    24,    25,    26,     0,     0,     4,
       0,  -120,     5,     0,     7,     8,     9,    10,     0,     0,
      11,     0,     0,    12,    13,     0,     0,     0,     0,    14,
      15,    16,     0,     0,    17,    18,    19,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,     0,     0,     0,
       0,     0,     0,     0,   -92,    22,     2,     3,    23,     0,
       0,     0,     0,    24,    25,    26,     0,     0,     4,     0,
     -92,     5,     0,     7,     8,     9,    10,     0,     0,    11,
       0,     0,    12,    13,     0,     0,     0,     0,    14,    15,
      16,     0,     0,    17,    18,    19,     0,    83,     0,    55,
       3,    84,    85,     0,    20,    21,     0,     0,     0,     0,
       0,    56,     0,    70,    22,    55,     3,    23,     9,     0,
       0,     0,    24,    25,    26,     0,     0,    56,    81,     0,
      55,     3,     0,     0,     9,     0,     0,     0,     0,     0,
       0,     0,    56,     0,     0,     0,     0,    20,    21,     9,
       0,     0,     0,     0,     0,     0,    91,    22,    55,     3,
      57,    86,     0,    20,    21,    24,    25,    26,     0,     0,
      56,     0,   183,    22,    55,     3,    57,     9,    20,    21,
       0,    24,    25,    26,     0,     0,    56,   185,    22,    55,
       3,    57,     0,     9,     0,     0,    24,    25,    26,     0,
       0,    56,     0,     0,     0,     0,    20,    21,     9,     0,
       0,     0,     0,     0,     0,   187,    22,    55,     3,    57,
       0,     0,    20,    21,    24,    25,    26,     0,     0,    56,
       0,   189,    22,    55,     3,    57,     9,    20,    21,     0,
      24,    25,    26,     0,     0,    56,   191,    22,    55,     3,
      57,     0,     9,     0,     0,    24,    25,    26,     0,     0,
      56,     0,     0,     0,     0,    20,    21,     9,     0,     0,
       0,     0,     0,     0,   193,    22,    55,     3,    57,     0,
       0,    20,    21,    24,    25,    26,     0,     0,    56,     0,
     195,    22,    55,     3,    57,     9,    20,    21,     0,    24,
      25,    26,     0,     0,    56,   197,    22,    55,     3,    57,
       0,     9,     0,     0,    24,    25,    26,     0,     0,    56,
       0,     0,     0,     0,    20,    21,     9,     0,     0,     0,
       0,     0,     0,   199,    22,    55,     3,    57,     0,     0,
      20,    21,    24,    25,    26,     0,     0,    56,     0,   201,
      22,    55,     3,    57,     9,    20,    21,     0,    24,    25,
      26,     0,     0,    56,   203,    22,    55,     3,    57,     0,
       9,     0,     0,    24,    25,    26,     0,     0,    56,     0,
       0,     0,     0,    20,    21,     9,     0,     0,     0,     0,
       0,     0,   205,    22,    55,     3,    57,     0,     0,    20,
      21,    24,    25,    26,     0,     0,    56,     0,   207,    22,
      55,     3,    57,     9,    20,    21,     0,    24,    25,    26,
       0,     0,    56,   209,    22,    55,     3,    57,     0,     9,
       0,     0,    24,    25,    26,     0,     0,    56,     0,     0,
       0,     0,    20,    21,     9,     0,     0,     0,     0,     0,
       0,   211,    22,    55,     3,    57,     0,     0,    20,    21,
      24,    25,    26,     0,     0,    56,     0,   213,    22,    55,
       3,    57,     9,    20,    21,     0,    24,    25,    26,     0,
       0,    56,   215,    22,    55,     3,    57,     0,     9,     0,
       0,    24,    25,    26,     0,     0,    56,     0,     0,     0,
       0,    20,    21,     9,     0,     0,     0,     0,     0,     0,
     217,    22,    55,     3,    57,     0,     0,    20,    21,    24,
      25,    26,     0,     0,    56,     0,   219,    22,    55,     3,
      57,     9,    20,    21,     0,    24,    25,    26,     0,     0,
      56,   221,    22,    55,     3,    57,     0,     9,     0,     0,
      24,    25,    26,     0,     0,    56,     0,     0,     0,     0,
      20,    21,     9,     0,     0,     0,     0,     0,     0,   223,
      22,    55,     3,    57,     0,     0,    20,    21,    24,    25,
      26,     0,     0,    56,     0,   231,    22,    55,     3,    57,
       9,    20,    21,     0,    24,    25,    26,     0,     0,    56,
     237,    22,    55,     3,    57,     0,     9,     0,     0,    24,
      25,    26,     0,     0,    56,     0,     0,     0,     0,    20,
      21,     9,     0,     0,     0,     0,     0,     0,   254,    22,
      55,     3,    57,     0,     0,    20,    21,    24,    25,    26,
       0,     0,    56,     0,   298,    22,    55,     3,    57,     9,
      20,    21,     0,    24,    25,    26,     0,     0,    56,   328,
      22,    55,     3,    57,     0,     9,     0,     0,    24,    25,
      26,     0,     0,    56,     0,     0,     0,     0,    20,    21,
       9,   111,   112,   113,   114,   115,     0,     0,    22,    55,
       3,    57,     0,     0,    20,    21,    24,    25,    26,     0,
       0,    56,     0,     0,    22,     0,     0,    57,     9,    20,
      21,     0,    24,    25,    26,     0,     0,     0,     0,    22,
       0,     0,    57,     0,   126,   127,   128,    24,    25,    26,
     129,   130,     0,     0,     0,     0,     0,    20,    21,     0,
       0,     0,     0,     0,     0,     0,     0,    22,     0,     0,
      57,     2,     3,   -40,   -40,    24,    25,    26,   311,     0,
       0,     0,     0,     4,     0,     0,     5,     0,     7,     8,
       9,    10,     0,     0,    11,     0,     0,    12,    13,     0,
       0,     0,     0,    14,    15,    16,     0,     0,    17,    18,
      19,     0,     0,     0,     0,     0,     0,     0,     0,    20,
      21,     0,     0,     0,     0,     0,     0,     0,     0,    22,
       2,     3,    23,     0,   -40,     0,     0,    24,    25,    26,
       0,     0,     4,     0,     0,     5,     0,     7,     8,     9,
      10,     0,     0,    11,    55,     3,    12,    13,     0,     0,
       0,     0,    14,    15,    16,     0,    56,    17,    18,    19,
       0,     0,     0,     9,     0,     0,     0,     0,    20,    21,
      55,     3,     0,     0,     0,     0,     0,     0,    22,     0,
       0,    23,    56,     0,     0,     0,    24,    25,    26,     9,
       0,   225,    20,    21,     0,   226,    55,     3,     0,     0,
       0,     0,    22,   227,     0,    57,     0,     0,    56,     0,
      24,    25,    26,     0,     0,     9,     0,   225,    20,    21,
       0,   226,    55,     3,     0,     0,     0,     0,    22,     0,
       0,    57,     0,     0,    56,     0,    24,    25,    26,     0,
       0,     9,     0,     0,    20,    21,     0,    55,     3,     0,
       0,     0,     0,     0,    22,     0,     0,    57,   268,    56,
       0,     0,    24,    25,    26,     0,     9,     0,     0,     0,
      20,    21,     0,     0,     0,     0,     0,     0,     0,     0,
      22,     0,     0,    57,   310,     0,     0,     0,    24,    25,
      26,     0,     0,     0,     0,    20,    21,     0,     0,     0,
       0,     0,     0,     0,     0,    22,     0,     0,    57,     0,
       0,     0,   141,    24,    25,    26,   142,   143,   108,   109,
     110,   111,   112,   113,   114,   115,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   142,   143,   108,   109,   110,   111,   112,   113,
     114,   115,     0,     0,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,     0,     0,     0,
     129,   130,     0,     0,     0,     0,     0,   144,   145,   146,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,     0,     0,     0,   129,   130,     0,     0,
     172,     0,     0,   144,   145,   146,   108,   109,   110,   111,
     112,   113,   114,   115,     0,     0,     0,     0,     0,     0,
     320,     0,     0,     0,     0,     0,   108,   109,   110,   111,
     112,   113,   114,   115,     0,     0,     0,     0,     0,     0,
       0,     0,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,     0,     0,     0,   129,   130,
       0,   173,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,     0,     0,     0,   129,   130,
       0,   321,   108,   109,   110,   111,   112,   113,   114,   115,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   108,   109,   110,   111,   112,   113,   114,   115,
       0,     0,     0,     0,     0,     0,     0,     0,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,     0,     0,     0,   129,   130,     0,   314,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,     0,     0,     0,   129,   130,   108,   109,   110,   111,
     112,   113,   114,   115,     0,     0,     0,     0,     0,     0,
       0,     0,   108,   109,   110,   111,   112,   113,   114,   115,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,     0,     0,     0,   129,   130,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,     0,     0,     0,   129,   130,   108,   109,   110,   111,
     112,   113,   114,   115,     0,     0,     0,     0,     0,     0,
     108,   109,   110,   111,   112,   113,   114,   115,     0,     0,
       0,     0,     0,     0,   111,   112,   113,   114,   115,     0,
       0,     0,     0,     0,     0,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,     0,     0,     0,   129,   130,
     120,   121,   122,   123,   124,   125,   126,   127,   128,     0,
       0,     0,   129,   130,   123,   124,   125,   126,   127,   128,
       0,     0,     0,   129,   130,   111,   112,   113,   114,   115,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   124,   125,   126,   127,
     128,     0,     0,     0,   129,   130
};

static const short yycheck[] =
{
       0,    69,     5,    71,    72,    26,   133,    10,    11,     3,
       1,   151,   161,    13,     1,     5,     6,    20,    21,    22,
      23,    24,     1,    26,     5,     6,    15,    26,    57,     0,
      30,     3,     3,     4,     0,    13,     1,     1,     3,    19,
       5,     6,     1,     1,    15,     3,     1,    18,    19,    20,
      21,    22,    23,    17,    57,    26,     1,    62,    29,    30,
      89,    66,    17,    25,    35,    36,    37,    61,    63,    40,
      41,    42,    17,    73,    65,    66,    97,     5,     6,    66,
      51,    52,    72,     1,    63,    72,    89,     5,     6,    88,
      61,    72,    30,    64,    97,   244,     1,    96,    69,    70,
      71,    66,    63,    61,    63,   108,   109,   110,   111,   112,
     113,   114,   252,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,     1,   131,   132,
     133,     5,     6,     1,   137,    73,    24,   136,    66,    67,
      68,   290,     1,    61,     3,     4,   295,   147,    66,     3,
       1,    62,     3,   153,   175,    66,    15,    62,    27,   162,
     181,    66,     1,    22,     3,   165,   166,    63,   168,   147,
     319,    28,   175,    13,    14,   153,   179,   176,   181,     1,
       1,     3,     3,   182,     1,    63,     3,   165,   166,   250,
     168,    50,    51,    52,    62,    54,     5,     6,    66,     1,
     327,     3,    61,    29,     1,    64,     3,    46,     5,     6,
      69,    70,    71,     5,     6,     5,     6,     1,    39,    59,
      60,     5,     6,    62,    46,     1,    17,     5,     6,     5,
       6,     1,   235,     3,     4,   262,   304,     3,   137,   239,
      62,   139,   242,    64,    46,    15,   246,     5,     6,     1,
     271,     3,    22,   253,   257,   276,    65,   260,   258,   259,
      61,   239,    63,     1,   264,     3,   281,    97,   271,    66,
      71,    -1,    73,   276,    66,   253,    -1,    61,   281,    63,
      46,    51,    52,     3,     4,    61,   286,    65,     1,   289,
      -1,    61,     5,     6,    64,    15,     1,   297,     3,    69,
      70,    71,     3,     4,   242,   305,    -1,    65,   246,   312,
      66,   289,    68,    -1,    15,   315,    -1,   317,   318,    -1,
     258,   259,   322,   323,   327,    -1,   264,   305,    -1,    -1,
     330,   331,    -1,   333,   334,    -1,     1,   337,     3,   317,
     318,     1,    -1,     3,   322,    -1,     0,     1,   286,     3,
       4,     5,     6,    -1,    -1,   333,    -1,    -1,    -1,   297,
      -1,    15,    -1,    -1,    18,    19,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,   315,    -1,    61,
      -1,    35,    36,    37,    -1,   323,    40,    41,    42,    71,
      -1,    73,   330,   331,    -1,    -1,   334,    51,    52,   337,
      -1,    -1,    -1,    -1,    -1,     0,    -1,    61,     3,     4,
      64,    -1,    66,    -1,    -1,    69,    70,    71,    -1,    -1,
      15,    -1,    -1,    18,    19,    20,    21,    22,    23,    -1,
      -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,
      35,    36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    51,    52,    -1,    -1,
      -1,    -1,    -1,    -1,     0,    -1,    61,     3,     4,    64,
      -1,    -1,    -1,    -1,    69,    70,    71,    -1,    -1,    15,
      -1,    -1,    18,    19,    20,    21,    22,    23,    -1,    -1,
      26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,
      36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    52,    -1,    -1,    -1,
      -1,    -1,    -1,     0,    -1,    61,     3,     4,    64,    -1,
      -1,    -1,    -1,    69,    70,    71,    -1,    -1,    15,    -1,
      -1,    18,    19,    20,    21,    22,    23,    -1,    -1,    26,
      -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    51,    52,    -1,    -1,    -1,    -1,
      -1,    -1,     0,    -1,    61,     3,     4,    64,    -1,    -1,
      -1,    -1,    69,    70,    71,    -1,    -1,    15,    -1,    -1,
      18,    19,    20,    21,    22,    23,    -1,    -1,    26,    -1,
      -1,    29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,
      -1,    -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    51,    52,    -1,    -1,    -1,    -1,    -1,
      -1,     0,    -1,    61,     3,     4,    64,    -1,    -1,    -1,
      -1,    69,    70,    71,    -1,    -1,    15,    -1,    -1,    18,
      19,    20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,
      29,    30,    -1,    -1,    -1,    -1,    35,    36,    37,    -1,
      -1,    40,    41,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,
       0,    -1,    61,     3,     4,    64,    -1,    -1,    -1,    -1,
      69,    70,    71,    -1,    -1,    15,    -1,    -1,    18,    -1,
      20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,    29,
      30,     3,     4,    -1,    -1,    35,    36,    37,    -1,    -1,
      40,    41,    42,    15,     1,    -1,     3,     4,     5,     6,
      22,    51,    52,    -1,    -1,    -1,    -1,    -1,    15,    -1,
      -1,    61,    -1,    -1,    64,    22,    -1,    -1,    -1,    69,
      70,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    64,    -1,    51,    52,    -1,    69,    70,    71,
      72,    -1,    -1,    -1,    61,    -1,     1,    64,     3,     4,
       5,     6,    69,    70,    71,    72,    -1,    -1,    -1,    -1,
      15,    -1,    17,    18,    -1,    20,    21,    22,    23,    -1,
      -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,
      35,    36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    51,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    64,
      -1,    66,    -1,    -1,    69,    70,    71,     1,    -1,     3,
       4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    15,    -1,    17,    18,    -1,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,
      -1,    35,    36,    37,    -1,    -1,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      64,    -1,    66,    -1,    -1,    69,    70,    71,     1,    -1,
       3,     4,     5,     6,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    15,    -1,    -1,    18,    -1,    20,    21,    22,
      23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,    -1,
      -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    61,     3,
       4,    64,    -1,    66,    -1,    -1,    69,    70,    71,    -1,
      -1,    15,    -1,    17,    18,    -1,    20,    21,    22,    23,
      -1,    -1,    26,    -1,    -1,    29,    30,    31,    -1,    -1,
      -1,    35,    36,    37,    -1,    -1,    40,    41,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    61,     3,     4,
      64,    -1,    -1,    -1,    -1,    69,    70,    71,    -1,    -1,
      15,    -1,    17,    18,    -1,    20,    21,    22,    23,    -1,
      -1,    26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,
      35,    36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    51,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    61,     3,     4,    64,
      -1,    -1,    -1,    -1,    69,    70,    71,    -1,    -1,    15,
      -1,    17,    18,    -1,    20,    21,    22,    23,    -1,    -1,
      26,    -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,
      36,    37,    -1,    -1,    40,    41,    42,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     1,    61,     3,     4,    64,    -1,
      -1,    -1,    -1,    69,    70,    71,    -1,    -1,    15,    -1,
      17,    18,    -1,    20,    21,    22,    23,    -1,    -1,    26,
      -1,    -1,    29,    30,    -1,    -1,    -1,    -1,    35,    36,
      37,    -1,    -1,    40,    41,    42,    -1,     1,    -1,     3,
       4,     5,     6,    -1,    51,    52,    -1,    -1,    -1,    -1,
      -1,    15,    -1,     1,    61,     3,     4,    64,    22,    -1,
      -1,    -1,    69,    70,    71,    -1,    -1,    15,     1,    -1,
       3,     4,    -1,    -1,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    15,    -1,    -1,    -1,    -1,    51,    52,    22,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    61,     3,     4,
      64,    65,    -1,    51,    52,    69,    70,    71,    -1,    -1,
      15,    -1,     1,    61,     3,     4,    64,    22,    51,    52,
      -1,    69,    70,    71,    -1,    -1,    15,     1,    61,     3,
       4,    64,    -1,    22,    -1,    -1,    69,    70,    71,    -1,
      -1,    15,    -1,    -1,    -1,    -1,    51,    52,    22,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    61,     3,     4,    64,
      -1,    -1,    51,    52,    69,    70,    71,    -1,    -1,    15,
      -1,     1,    61,     3,     4,    64,    22,    51,    52,    -1,
      69,    70,    71,    -1,    -1,    15,     1,    61,     3,     4,
      64,    -1,    22,    -1,    -1,    69,    70,    71,    -1,    -1,
      15,    -1,    -1,    -1,    -1,    51,    52,    22,    -1,    -1,
      -1,    -1,    -1,    -1,     1,    61,     3,     4,    64,    -1,
      -1,    51,    52,    69,    70,    71,    -1,    -1,    15,    -1,
       1,    61,     3,     4,    64,    22,    51,    52,    -1,    69,
      70,    71,    -1,    -1,    15,     1,    61,     3,     4,    64,
      -1,    22,    -1,    -1,    69,    70,    71,    -1,    -1,    15,
      -1,    -1,    -1,    -1,    51,    52,    22,    -1,    -1,    -1,
      -1,    -1,    -1,     1,    61,     3,     4,    64,    -1,    -1,
      51,    52,    69,    70,    71,    -1,    -1,    15,    -1,     1,
      61,     3,     4,    64,    22,    51,    52,    -1,    69,    70,
      71,    -1,    -1,    15,     1,    61,     3,     4,    64,    -1,
      22,    -1,    -1,    69,    70,    71,    -1,    -1,    15,    -1,
      -1,    -1,    -1,    51,    52,    22,    -1,    -1,    -1,    -1,
      -1,    -1,     1,    61,     3,     4,    64,    -1,    -1,    51,
      52,    69,    70,    71,    -1,    -1,    15,    -1,     1,    61,
       3,     4,    64,    22,    51,    52,    -1,    69,    70,    71,
      -1,    -1,    15,     1,    61,     3,     4,    64,    -1,    22,
      -1,    -1,    69,    70,    71,    -1,    -1,    15,    -1,    -1,
      -1,    -1,    51,    52,    22,    -1,    -1,    -1,    -1,    -1,
      -1,     1,    61,     3,     4,    64,    -1,    -1,    51,    52,
      69,    70,    71,    -1,    -1,    15,    -1,     1,    61,     3,
       4,    64,    22,    51,    52,    -1,    69,    70,    71,    -1,
      -1,    15,     1,    61,     3,     4,    64,    -1,    22,    -1,
      -1,    69,    70,    71,    -1,    -1,    15,    -1,    -1,    -1,
      -1,    51,    52,    22,    -1,    -1,    -1,    -1,    -1,    -1,
       1,    61,     3,     4,    64,    -1,    -1,    51,    52,    69,
      70,    71,    -1,    -1,    15,    -1,     1,    61,     3,     4,
      64,    22,    51,    52,    -1,    69,    70,    71,    -1,    -1,
      15,     1,    61,     3,     4,    64,    -1,    22,    -1,    -1,
      69,    70,    71,    -1,    -1,    15,    -1,    -1,    -1,    -1,
      51,    52,    22,    -1,    -1,    -1,    -1,    -1,    -1,     1,
      61,     3,     4,    64,    -1,    -1,    51,    52,    69,    70,
      71,    -1,    -1,    15,    -1,     1,    61,     3,     4,    64,
      22,    51,    52,    -1,    69,    70,    71,    -1,    -1,    15,
       1,    61,     3,     4,    64,    -1,    22,    -1,    -1,    69,
      70,    71,    -1,    -1,    15,    -1,    -1,    -1,    -1,    51,
      52,    22,    -1,    -1,    -1,    -1,    -1,    -1,     1,    61,
       3,     4,    64,    -1,    -1,    51,    52,    69,    70,    71,
      -1,    -1,    15,    -1,     1,    61,     3,     4,    64,    22,
      51,    52,    -1,    69,    70,    71,    -1,    -1,    15,     1,
      61,     3,     4,    64,    -1,    22,    -1,    -1,    69,    70,
      71,    -1,    -1,    15,    -1,    -1,    -1,    -1,    51,    52,
      22,    10,    11,    12,    13,    14,    -1,    -1,    61,     3,
       4,    64,    -1,    -1,    51,    52,    69,    70,    71,    -1,
      -1,    15,    -1,    -1,    61,    -1,    -1,    64,    22,    51,
      52,    -1,    69,    70,    71,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    64,    -1,    53,    54,    55,    69,    70,    71,
      59,    60,    -1,    -1,    -1,    -1,    -1,    51,    52,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      64,     3,     4,     5,     6,    69,    70,    71,    72,    -1,
      -1,    -1,    -1,    15,    -1,    -1,    18,    -1,    20,    21,
      22,    23,    -1,    -1,    26,    -1,    -1,    29,    30,    -1,
      -1,    -1,    -1,    35,    36,    37,    -1,    -1,    40,    41,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,
      52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
       3,     4,    64,    -1,    66,    -1,    -1,    69,    70,    71,
      -1,    -1,    15,    -1,    -1,    18,    -1,    20,    21,    22,
      23,    -1,    -1,    26,     3,     4,    29,    30,    -1,    -1,
      -1,    -1,    35,    36,    37,    -1,    15,    40,    41,    42,
      -1,    -1,    -1,    22,    -1,    -1,    -1,    -1,    51,    52,
       3,     4,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,
      -1,    64,    15,    -1,    -1,    -1,    69,    70,    71,    22,
      -1,    50,    51,    52,    -1,    54,     3,     4,    -1,    -1,
      -1,    -1,    61,    62,    -1,    64,    -1,    -1,    15,    -1,
      69,    70,    71,    -1,    -1,    22,    -1,    50,    51,    52,
      -1,    54,     3,     4,    -1,    -1,    -1,    -1,    61,    -1,
      -1,    64,    -1,    -1,    15,    -1,    69,    70,    71,    -1,
      -1,    22,    -1,    -1,    51,    52,    -1,     3,     4,    -1,
      -1,    -1,    -1,    -1,    61,    -1,    -1,    64,    65,    15,
      -1,    -1,    69,    70,    71,    -1,    22,    -1,    -1,    -1,
      51,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      61,    -1,    -1,    64,    65,    -1,    -1,    -1,    69,    70,
      71,    -1,    -1,    -1,    -1,    51,    52,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,    64,    -1,
      -1,    -1,     1,    69,    70,    71,     5,     6,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    -1,    -1,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    -1,
      59,    60,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    -1,    -1,    59,    60,    -1,    -1,
       1,    -1,    -1,    66,    67,    68,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
       1,    -1,    -1,    -1,    -1,    -1,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    59,    60,
      -1,    62,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    59,    60,
      -1,    62,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    -1,    59,    60,    -1,    62,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    -1,    59,    60,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     7,     8,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    59,    60,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    -1,    59,    60,     7,     8,     9,    10,
      11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,    -1,
       7,     8,     9,    10,    11,    12,    13,    14,    -1,    -1,
      -1,    -1,    -1,    -1,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    59,    60,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    -1,
      -1,    -1,    59,    60,    50,    51,    52,    53,    54,    55,
      -1,    -1,    -1,    59,    60,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    51,    52,    53,    54,
      55,    -1,    -1,    -1,    59,    60
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     1,     3,     4,    15,    18,    19,    20,    21,    22,
      23,    26,    29,    30,    35,    36,    37,    40,    41,    42,
      51,    52,    61,    64,    69,    70,    71,    75,    76,    77,
      81,    82,    83,    84,    85,    86,    88,    89,    90,    91,
      92,    94,   101,   103,   104,   110,   111,   112,   113,   114,
       3,     4,    15,    15,     1,     3,    15,    64,   105,   112,
     114,     1,     3,    39,    64,    78,     1,     3,    61,   102,
       1,   112,   112,    81,     1,     3,    87,     1,    87,   112,
     112,     1,   112,     1,     5,     6,    65,   112,   119,   120,
     122,     1,   112,     3,     1,    72,   118,   120,   122,     0,
      76,    82,     5,     6,    66,     3,     4,    15,     7,     8,
       9,    10,    11,    12,    13,    14,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    59,
      60,    61,    63,    71,    73,   115,   119,    25,   106,   107,
     108,     1,     5,     6,    66,    67,    68,    95,     1,     5,
       6,    61,    63,    96,    63,     1,     3,    46,    79,    80,
       1,     3,    63,     1,     3,    95,    95,    95,    31,    93,
       1,     3,     1,    62,    65,   120,   119,    66,    68,   121,
      72,   120,   118,     1,   112,     1,   112,     1,   112,     1,
     112,     1,   112,     1,   112,     1,   112,     1,   112,     1,
     112,     1,   112,     1,   112,     1,   112,     1,   112,     1,
     112,     1,   112,     1,   112,     1,   112,     1,   112,     1,
     112,     1,   112,     1,   112,    50,    54,    62,   112,   116,
     117,     1,   112,   117,     3,    61,    65,     1,   105,    24,
     109,   108,    81,     1,    62,    79,    81,     3,     1,    65,
      66,     1,    61,    96,     1,   112,     1,    63,    81,    81,
      27,    97,    98,    99,    81,     1,    17,    63,    65,   122,
      65,   120,   112,    72,   122,    72,   120,     1,     3,     1,
      62,    66,     1,    72,   112,     1,    81,     1,    17,    96,
      62,     1,    63,    80,     1,    62,    79,    81,     1,   112,
       1,    17,     1,    17,   112,    28,   100,    99,     1,     3,
      65,    72,    63,   116,    62,    81,     1,    96,    96,    62,
       1,    62,    95,    81,     1,    17,     1,    61,     1,   112,
      81,    81,     1,    96,    81,     1,   117,    81,     1,    62
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
#line 152 "../../../libs/libFreeMat/Parser.yxx"
    {mainAST = yyvsp[0].v.p;}
    break;

  case 5:
#line 154 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list or function definition",yyvsp[0]);}
    break;

  case 6:
#line 158 "../../../libs/libFreeMat/Parser.yxx"
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
#line 168 "../../../libs/libFreeMat/Parser.yxx"
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
#line 177 "../../../libs/libFreeMat/Parser.yxx"
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
#line 186 "../../../libs/libFreeMat/Parser.yxx"
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
#line 194 "../../../libs/libFreeMat/Parser.yxx"
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
#line 203 "../../../libs/libFreeMat/Parser.yxx"
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
#line 211 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("legal function name or return declaration after 'function'",yyvsp[-1]);}
    break;

  case 13:
#line 212 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("argument list or statement list after identifier '") + 
	yyvsp[-1].v.p->text + "'",yyvsp[-1]);}
    break;

  case 14:
#line 214 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("(possibly empty) argument list after '('",yyvsp[-1]);}
    break;

  case 15:
#line 215 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list after ')'",yyvsp[-1]);}
    break;

  case 16:
#line 216 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("function name for function declared",yyvsp[-2]);}
    break;

  case 17:
#line 217 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("argument list or statement list following function name :") + 
	yyvsp[-1].v.p->text,yyvsp[-1]);}
    break;

  case 18:
#line 219 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("(possibly empty) argument list after '('",yyvsp[-1]);}
    break;

  case 19:
#line 220 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list after ')'",yyvsp[-1]);}
    break;

  case 22:
#line 229 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 23:
#line 230 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 24:
#line 231 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 25:
#line 232 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after identifier in return declaration",yyvsp[-1]);}
    break;

  case 26:
#line 233 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a valid list of return arguments in return declaration",yyvsp[-1]);}
    break;

  case 27:
#line 234 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching ']' in return declaration for '['",yyvsp[-2]);}
    break;

  case 28:
#line 235 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an '=' symbol after return declaration",yyvsp[-1]);}
    break;

  case 29:
#line 239 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 30:
#line 240 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 32:
#line 244 "../../../libs/libFreeMat/Parser.yxx"
    {
	yyval.v.p = yyvsp[0].v.p;
	char *b = (char*) malloc(strlen(yyvsp[0].v.p->text)+2);
	b[0] = '&';
	strcpy(b+1,yyvsp[0].v.p->text);
	yyval.v.p->text = b;
  }
    break;

  case 33:
#line 255 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 34:
#line 256 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 35:
#line 260 "../../../libs/libFreeMat/Parser.yxx"
    {
  	    yyval.v.p = new AST(OP_QSTATEMENT,NULL,yyvsp[0].v.i);
	    yyval.v.p->down = yyvsp[-1].v.p;
	 }
    break;

  case 36:
#line 264 "../../../libs/libFreeMat/Parser.yxx"
    {
	    yyval.v.p = new AST(OP_RSTATEMENT,NULL,yyvsp[0].v.i);
            yyval.v.p->down = yyvsp[-1].v.p;
	 }
    break;

  case 37:
#line 268 "../../../libs/libFreeMat/Parser.yxx"
    {
	    yyval.v.p = new AST(OP_RSTATEMENT,NULL,yyvsp[0].v.i);
	    yyval.v.p->down = yyvsp[-1].v.p;
	 }
    break;

  case 40:
#line 277 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(null_node,"",-1);}
    break;

  case 56:
#line 295 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SCALL,yyvsp[-1].v.p,yyvsp[0].v.p,yyvsp[-1].v.p->context());}
    break;

  case 57:
#line 296 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SCALL,yyvsp[-1].v.p,yyvsp[0].v.p,yyvsp[-1].v.p->context());}
    break;

  case 58:
#line 297 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SCALL,yyvsp[-1].v.p,yyvsp[0].v.p,yyvsp[-1].v.p->context());}
    break;

  case 59:
#line 298 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SCALL,yyvsp[-1].v.p,yyvsp[0].v.p,yyvsp[-1].v.p->context()); }
    break;

  case 60:
#line 299 "../../../libs/libFreeMat/Parser.yxx"
    {yyvsp[-1].v.p->addChild(yyvsp[0].v.p);}
    break;

  case 61:
#line 300 "../../../libs/libFreeMat/Parser.yxx"
    {yyvsp[-1].v.p->addChild(yyvsp[0].v.p);}
    break;

  case 62:
#line 301 "../../../libs/libFreeMat/Parser.yxx"
    {yyvsp[-1].v.p->addChild(yyvsp[0].v.p);}
    break;

  case 63:
#line 314 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p); }
    break;

  case 64:
#line 315 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as persistent",yyvsp[-1]);}
    break;

  case 65:
#line 319 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 66:
#line 320 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of variables to be tagged as global",yyvsp[-1]);}
    break;

  case 68:
#line 325 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 69:
#line 326 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("list of valid identifiers",yyvsp[0]);}
    break;

  case 74:
#line 346 "../../../libs/libFreeMat/Parser.yxx"
    { yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-2].v.p); if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);}
    break;

  case 75:
#line 348 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("matching 'end' to 'try' clause from line ") + decodeline(yyvsp[-3]),yyvsp[0]);}
    break;

  case 76:
#line 352 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 77:
#line 353 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 78:
#line 357 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-5].v.p; yyval.v.p->addChild(yyvsp[-4].v.p); 
	  if (yyvsp[-2].v.p != NULL) yyval.v.p->addChild(yyvsp[-2].v.p); 
	  if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 79:
#line 362 "../../../libs/libFreeMat/Parser.yxx"
    {
          yyxpt(std::string("matching 'end' to 'switch' clause from line ") + decodeline(yyvsp[-5]),yyvsp[0]);
        }
    break;

  case 87:
#line 377 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 89:
#line 382 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_CASEBLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());
	}
    break;

  case 90:
#line 385 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 91:
#line 391 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-2].v.p); yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 92:
#line 397 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 93:
#line 400 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = NULL;
	}
    break;

  case 94:
#line 406 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 95:
#line 410 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'for' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 96:
#line 414 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-3].v.p; yyval.v.p->addChild(yyvsp[-1].v.p);}
    break;

  case 97:
#line 415 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 98:
#line 416 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p; 
	      yyval.v.p->addChild(new AST(OP_RHS,new AST(id_node,yyvsp[0].v.p->text,yyvsp[0].v.p->context()),yyvsp[0].v.p->context())); }
    break;

  case 99:
#line 418 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-4]);}
    break;

  case 100:
#line 419 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression",yyvsp[-1]);}
    break;

  case 101:
#line 420 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("equals operator after loop index",yyvsp[-1]);}
    break;

  case 102:
#line 421 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier that is the loop variable",yyvsp[-1]);}
    break;

  case 103:
#line 422 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing expression",yyvsp[-1]);}
    break;

  case 104:
#line 423 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier or assignment (id = expr) after 'for' ",yyvsp[0]);}
    break;

  case 105:
#line 427 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 106:
#line 430 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test expression after 'while'",yyvsp[-1]);}
    break;

  case 107:
#line 432 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'while' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 108:
#line 436 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-4].v.p; yyval.v.p->addChild(yyvsp[-3].v.p); if (yyvsp[-2].v.p != NULL) yyval.v.p->addChild(yyvsp[-2].v.p); 
	  if (yyvsp[-1].v.p != NULL) yyval.v.p->addChild(yyvsp[-1].v.p);
	}
    break;

  case 109:
#line 440 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("condition expression for 'if'",yyvsp[-1]);}
    break;

  case 110:
#line 441 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt(std::string("'end' to match 'if' statement from line ") + decodeline(yyvsp[-4]),yyvsp[0]);}
    break;

  case 111:
#line 445 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_CSTAT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-2].v.p->context());
	}
    break;

  case 112:
#line 448 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_CSTAT,yyvsp[-1].v.p,yyvsp[-1].v.p->context());
	}
    break;

  case 113:
#line 451 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("valid list of statements after condition",yyvsp[0]);}
    break;

  case 114:
#line 455 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 116:
#line 460 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = new AST(OP_ELSEIFBLOCK,yyvsp[0].v.p,yyvsp[0].v.p->context());
	}
    break;

  case 117:
#line 463 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);
	}
    break;

  case 118:
#line 469 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 119:
#line 472 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("test condition for 'elseif' clause",yyvsp[-1]);}
    break;

  case 120:
#line 475 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyval.v.p = yyvsp[0].v.p;
	}
    break;

  case 121:
#line 478 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = NULL;}
    break;

  case 122:
#line 479 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("statement list for 'else' clause",yyvsp[-1]);}
    break;

  case 123:
#line 483 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ASSIGN,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 124:
#line 484 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expression in assignment",yyvsp[-1]);}
    break;

  case 125:
#line 488 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[-3].v.p->addChild(new AST(OP_PARENS,yyvsp[-1].v.p,yyvsp[-2].v.i));
	  yyval.v.p = new AST(OP_MULTICALL,yyvsp[-6].v.p,yyvsp[-3].v.p,yyvsp[-7].v.i);
	}
    break;

  case 126:
#line 492 "../../../libs/libFreeMat/Parser.yxx"
    {
	  yyvsp[0].v.p->addChild(new AST(OP_PARENS,NULL,-1));
	  yyval.v.p = new AST(OP_MULTICALL,yyvsp[-3].v.p,yyvsp[0].v.p,yyvsp[-4].v.i);
	}
    break;

  case 127:
#line 497 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-2]);}
    break;

  case 128:
#line 499 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("indexing list",yyvsp[-1]);}
    break;

  case 129:
#line 501 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("left parenthesis",yyvsp[-1]);}
    break;

  case 130:
#line 503 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("identifier",yyvsp[-1]);}
    break;

  case 131:
#line 507 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_COLON,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 132:
#line 508 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after ':'",yyvsp[-1]);}
    break;

  case 134:
#line 510 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PLUS,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 135:
#line 511 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '+'",yyvsp[-1]);}
    break;

  case 136:
#line 512 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SUBTRACT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 137:
#line 513 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '-'",yyvsp[-1]);}
    break;

  case 138:
#line 514 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_TIMES,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 139:
#line 515 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '*'",yyvsp[-1]);}
    break;

  case 140:
#line 516 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_RDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 141:
#line 517 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '/'",yyvsp[-1]);}
    break;

  case 142:
#line 518 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 143:
#line 519 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '\\'",yyvsp[-1]);}
    break;

  case 144:
#line 520 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_OR,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 145:
#line 521 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '|'",yyvsp[-1]);}
    break;

  case 146:
#line 522 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_AND,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 147:
#line 523 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '&'",yyvsp[-1]);}
    break;

  case 148:
#line 524 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SOR,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 149:
#line 525 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '||'",yyvsp[-1]);}
    break;

  case 150:
#line 526 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SAND,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 151:
#line 527 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '&&'",yyvsp[-1]);}
    break;

  case 152:
#line 528 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 153:
#line 529 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<'",yyvsp[-1]);}
    break;

  case 154:
#line 530 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_LEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 155:
#line 531 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '<='",yyvsp[-1]);}
    break;

  case 156:
#line 532 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_GT,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 157:
#line 533 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>'",yyvsp[-1]);}
    break;

  case 158:
#line 534 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_GEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 159:
#line 535 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '>='",yyvsp[-1]);}
    break;

  case 160:
#line 536 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 161:
#line 537 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '=='",yyvsp[-1]);}
    break;

  case 162:
#line 538 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NEQ,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 163:
#line 539 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '~='",yyvsp[-1]);}
    break;

  case 164:
#line 540 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_TIMES,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 165:
#line 541 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.*'",yyvsp[-1]);}
    break;

  case 166:
#line 542 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_RDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 167:
#line 543 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after './'",yyvsp[-1]);}
    break;

  case 168:
#line 544 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_LDIV,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 169:
#line 545 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.\\'",yyvsp[-1]);}
    break;

  case 170:
#line 546 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NEG,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 171:
#line 547 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[0].v.p;}
    break;

  case 172:
#line 548 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_NOT,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 173:
#line 549 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after logical not",yyvsp[0]);}
    break;

  case 174:
#line 550 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_POWER,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 175:
#line 551 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '^'",yyvsp[-1]);}
    break;

  case 176:
#line 552 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_POWER,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 177:
#line 553 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after '.^'",yyvsp[-1]);}
    break;

  case 178:
#line 554 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_TRANSPOSE,yyvsp[-1].v.p,yyvsp[0].v.i);}
    break;

  case 179:
#line 555 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT_TRANSPOSE,yyvsp[-1].v.p,yyvsp[0].v.i);}
    break;

  case 180:
#line 556 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 181:
#line 557 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a right parenthesis after expression to match this one",yyvsp[-2]);}
    break;

  case 182:
#line 558 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("an expression after left parenthesis",yyvsp[-1]);}
    break;

  case 186:
#line 565 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ADDRESS,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 187:
#line 566 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_RHS,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 188:
#line 567 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 189:
#line 568 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a matrix definition followed by a right bracket",yyvsp[-1]);}
    break;

  case 190:
#line 569 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 191:
#line 570 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 192:
#line 571 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 193:
#line 572 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EMPTY,NULL,yyvsp[-1].v.i);}
    break;

  case 194:
#line 573 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 195:
#line 574 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p;}
    break;

  case 196:
#line 575 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 197:
#line 576 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p;}
    break;

  case 198:
#line 577 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_EMPTY_CELL,NULL,yyvsp[-1].v.i);}
    break;

  case 199:
#line 578 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("a cell-array definition followed by a right brace",yyvsp[-1]);}
    break;

  case 201:
#line 582 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-1].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 202:
#line 586 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PARENS,yyvsp[-1].v.p,yyvsp[-2].v.i); }
    break;

  case 203:
#line 587 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_PARENS,NULL,yyvsp[-1].v.i); }
    break;

  case 204:
#line 588 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right parenthesis",yyvsp[-2]);}
    break;

  case 205:
#line 589 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACES,yyvsp[-1].v.p,yyvsp[-2].v.i); }
    break;

  case 206:
#line 590 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("matching right brace",yyvsp[-2]);}
    break;

  case 207:
#line 591 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOT,yyvsp[0].v.p,yyvsp[-1].v.i); }
    break;

  case 208:
#line 592 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_DOTDYN,yyvsp[-1].v.p,yyvsp[-3].v.i);}
    break;

  case 210:
#line 597 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_ALL,NULL,yyvsp[0].v.i);}
    break;

  case 211:
#line 598 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_KEYWORD,yyvsp[-2].v.p,yyvsp[0].v.p,yyvsp[-3].v.i);}
    break;

  case 212:
#line 599 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting expression after '=' in keyword assignment",yyvsp[-1]);}
    break;

  case 213:
#line 600 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_KEYWORD,yyvsp[0].v.p,yyvsp[-1].v.i);}
    break;

  case 214:
#line 601 "../../../libs/libFreeMat/Parser.yxx"
    {yyxpt("expecting keyword identifier after '/' in keyword assignment",yyvsp[-1]);}
    break;

  case 216:
#line 606 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addPeer(yyvsp[0].v.p);}
    break;

  case 217:
#line 610 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACES,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 218:
#line 611 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 219:
#line 615 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_BRACKETS,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 220:
#line 616 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;

  case 225:
#line 628 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = new AST(OP_SEMICOLON,yyvsp[0].v.p,yyvsp[0].v.p->context());}
    break;

  case 226:
#line 629 "../../../libs/libFreeMat/Parser.yxx"
    {yyval.v.p = yyvsp[-2].v.p; yyval.v.p->addChild(yyvsp[0].v.p);}
    break;


    }

/* Line 1000 of yacc.c.  */
#line 2987 "../../../libs/libFreeMat/Parser.cxx"

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


#line 632 "../../../libs/libFreeMat/Parser.yxx"


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

