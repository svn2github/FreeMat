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
#line 302 "../../../libs/libFreeMat/Parser.cc"

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
#define YYFINAL  94
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2574

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  50
/* YYNRULES -- Number of rules. */
#define YYNRULES  208
/* YYNRULES -- Number of states. */
#define YYNSTATES  309

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
     123,   125,   129,   131,   132,   137,   139,   142,   145,   148,
     151,   153,   155,   156,   158,   160,   162,   164,   166,   168,
     170,   172,   174,   176,   178,   180,   182,   184,   186,   188,
     191,   193,   196,   199,   202,   205,   208,   210,   213,   216,
     218,   220,   222,   224,   229,   232,   233,   240,   242,   244,
     246,   248,   249,   251,   253,   256,   261,   264,   265,   271,
     277,   281,   283,   289,   294,   298,   301,   305,   307,   313,
     316,   322,   325,   329,   330,   332,   334,   337,   340,   343,
     346,   347,   350,   354,   358,   367,   373,   382,   390,   397,
     403,   407,   411,   413,   417,   421,   425,   429,   433,   437,
     441,   445,   449,   453,   457,   461,   465,   469,   473,   477,
     481,   485,   489,   493,   497,   501,   505,   509,   513,   517,
     521,   525,   529,   533,   537,   541,   544,   547,   550,   553,
     557,   561,   565,   569,   572,   575,   579,   583,   586,   588,
     590,   592,   594,   598,   601,   606,   611,   617,   620,   624,
     629,   634,   640,   643,   646,   648,   651,   655,   659,   663,
     667,   669,   671,   673,   678,   683,   686,   689,   691,   695,
     697,   701,   703,   707,   709,   711,   713,   714,   716
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      76,     0,    -1,    83,    -1,    78,    -1,    -1,    19,    79,
       3,    32,    80,    33,    83,    -1,    19,     3,    32,    80,
      33,    83,    -1,    19,    79,     3,    83,    -1,    19,     3,
      83,    -1,    19,    79,     3,    32,    33,    83,    -1,    19,
       3,    32,    33,    83,    -1,    19,     1,    -1,    19,     3,
       1,    -1,    19,     3,    32,     1,    -1,    19,     3,    32,
      80,    33,     1,    -1,    19,    79,     1,    -1,    19,    79,
       3,     1,    -1,    19,    79,     3,    32,     1,    -1,    19,
      79,     3,    32,    80,    33,     1,    -1,    77,    -1,    78,
      77,    -1,    45,    65,    -1,     3,    65,    -1,    66,    80,
      67,    65,    -1,     1,    -1,    45,     1,    -1,     3,     1,
      -1,    66,     1,    -1,    66,    80,     1,    -1,    66,    80,
      67,     1,    -1,    81,    -1,    80,    68,    81,    -1,     3,
      -1,    -1,    51,     3,    82,     1,    -1,    84,    -1,    83,
      84,    -1,    85,     5,    -1,    85,     6,    -1,    85,    68,
      -1,   112,    -1,   114,    -1,    -1,   113,    -1,   103,    -1,
      94,    -1,    93,    -1,   105,    -1,   106,    -1,    97,    -1,
      95,    -1,    92,    -1,    91,    -1,    89,    -1,    88,    -1,
      86,    -1,    47,    -1,    48,    -1,     1,    -1,    16,    87,
      -1,    15,    -1,    87,    15,    -1,    41,    90,    -1,    41,
       1,    -1,    46,    90,    -1,    46,     1,    -1,     3,    -1,
      90,     3,    -1,    90,     1,    -1,    43,    -1,    42,    -1,
      28,    -1,    21,    -1,    29,    83,    96,    39,    -1,    30,
      83,    -1,    -1,    25,   114,    98,    99,   102,    37,    -1,
      68,    -1,     6,    -1,     5,    -1,    69,    -1,    -1,   100,
      -1,   101,    -1,   100,   101,    -1,    26,   114,    98,    83,
      -1,    27,    83,    -1,    -1,    20,   104,    98,    83,    36,
      -1,    70,     3,    65,   114,    71,    -1,     3,    65,   114,
      -1,     3,    -1,    70,     3,    65,   114,     1,    -1,    70,
       3,    65,     1,    -1,    70,     3,     1,    -1,    70,     1,
      -1,     3,    65,     1,    -1,     1,    -1,    22,   114,    98,
      83,    38,    -1,    22,     1,    -1,    18,   107,   108,   111,
      40,    -1,    18,     1,    -1,   114,    98,    83,    -1,    -1,
     109,    -1,   110,    -1,   109,   110,    -1,    24,   107,    -1,
      24,     1,    -1,    23,    83,    -1,    -1,    23,     1,    -1,
     116,    65,   114,    -1,   116,    65,     1,    -1,    66,   121,
      67,    65,     3,    32,   119,    33,    -1,    66,   121,    67,
      65,     3,    -1,    66,   121,    67,    65,     3,    32,   119,
       1,    -1,    66,   121,    67,    65,     3,    32,     1,    -1,
      66,   121,    67,    65,     3,     1,    -1,    66,   121,    67,
      65,     1,    -1,   114,    49,   114,    -1,   114,    49,     1,
      -1,   115,    -1,   114,    55,   114,    -1,   114,    55,     1,
      -1,   114,    56,   114,    -1,   114,    56,     1,    -1,   114,
      57,   114,    -1,   114,    57,     1,    -1,   114,    58,   114,
      -1,   114,    58,     1,    -1,   114,    59,   114,    -1,   114,
      59,     1,    -1,   114,    50,   114,    -1,   114,    50,     1,
      -1,   114,    51,   114,    -1,   114,    51,     1,    -1,   114,
      52,   114,    -1,   114,    52,     1,    -1,   114,     7,   114,
      -1,   114,     7,     1,    -1,   114,    53,   114,    -1,   114,
      53,     1,    -1,   114,     8,   114,    -1,   114,     8,     1,
      -1,   114,     9,   114,    -1,   114,     9,     1,    -1,   114,
      54,   114,    -1,   114,    54,     1,    -1,   114,    10,   114,
      -1,   114,    10,     1,    -1,   114,    11,   114,    -1,   114,
      11,     1,    -1,   114,    12,   114,    -1,   114,    12,     1,
      -1,    56,   114,    -1,    55,   114,    -1,    72,   114,    -1,
      72,     1,    -1,   114,    63,   114,    -1,   114,    63,     1,
      -1,   114,    13,   114,    -1,   114,    13,     1,    -1,   114,
      64,    -1,   114,    14,    -1,    70,   114,    71,    -1,    70,
     114,     1,    -1,    70,     1,    -1,     4,    -1,    15,    -1,
      17,    -1,   116,    -1,    66,   121,    67,    -1,    66,     1,
      -1,    66,   122,   121,    67,    -1,    66,   121,   122,    67,
      -1,    66,   122,   121,   122,    67,    -1,    66,    67,    -1,
      73,   120,    74,    -1,    73,   122,   120,    74,    -1,    73,
     120,   122,    74,    -1,    73,   122,   120,   122,    74,    -1,
      73,    74,    -1,    73,     1,    -1,     3,    -1,   116,   117,
      -1,    32,   119,    33,    -1,    32,   119,     1,    -1,    34,
     119,    35,    -1,    34,   119,     1,    -1,    31,    -1,   114,
      -1,    49,    -1,    58,     3,    65,   114,    -1,    58,     3,
      65,     1,    -1,    58,     3,    -1,    58,     1,    -1,   118,
      -1,   119,    68,   118,    -1,   124,    -1,   120,   122,   124,
      -1,   124,    -1,   121,   122,   124,    -1,     6,    -1,     5,
      -1,    68,    -1,    -1,   114,    -1,   124,   123,   114,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   140,   140,   141,   141,   145,   161,   176,   191,   205,
     220,   234,   235,   236,   237,   238,   239,   240,   241,   245,
     246,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     262,   263,   267,   267,   267,   279,   280,   284,   293,   302,
     314,   315,   316,   317,   318,   319,   320,   321,   322,   323,
     324,   325,   326,   327,   328,   329,   330,   330,   331,   335,
     339,   340,   344,   345,   349,   350,   354,   355,   356,   360,
     364,   368,   371,   375,   380,   381,   385,   393,   393,   393,
     393,   397,   398,   402,   405,   411,   417,   420,   426,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   444,   447,
     451,   455,   459,   465,   466,   470,   473,   479,   482,   485,
     488,   489,   493,   494,   498,   502,   506,   508,   510,   512,
     517,   518,   519,   520,   521,   522,   523,   524,   525,   526,
     527,   528,   529,   530,   531,   532,   533,   534,   535,   536,
     537,   538,   539,   540,   541,   542,   543,   544,   545,   546,
     547,   548,   549,   550,   551,   552,   553,   554,   555,   556,
     557,   558,   559,   560,   561,   562,   563,   564,   568,   569,
     570,   571,   572,   573,   574,   575,   576,   577,   578,   579,
     580,   581,   582,   583,   587,   588,   592,   593,   594,   595,
     596,   600,   601,   602,   603,   604,   605,   609,   610,   614,
     615,   619,   620,   624,   624,   628,   628,   632,   633
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
  "NEG", "POS", "'^'", "'''", "'='", "'['", "']'", "','", "';'", "'('", 
  "')'", "'~'", "'{'", "'}'", "$accept", "program", "functionDef", 
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
      80,    80,    81,    82,    81,    83,    83,    84,    84,    84,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    86,
      87,    87,    88,    88,    89,    89,    90,    90,    90,    91,
      92,    93,    94,    95,    96,    96,    97,    98,    98,    98,
      98,    99,    99,   100,   100,   101,   102,   102,   103,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   105,   105,
     106,   106,   107,   108,   108,   109,   109,   110,   110,   111,
     111,   111,   112,   112,   113,   113,   113,   113,   113,   113,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   114,   115,   115,
     115,   115,   115,   115,   115,   115,   115,   115,   115,   115,
     115,   115,   115,   115,   116,   116,   117,   117,   117,   117,
     117,   118,   118,   118,   118,   118,   118,   119,   119,   120,
     120,   121,   121,   122,   122,   123,   123,   124,   124
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
       1,     0,     1,     1,     2,     4,     2,     0,     5,     5,
       3,     1,     5,     4,     3,     2,     3,     1,     5,     2,
       5,     2,     3,     0,     1,     1,     2,     2,     2,     2,
       0,     2,     3,     3,     8,     5,     8,     7,     6,     5,
       3,     3,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     2,     2,     2,     3,
       3,     3,     3,     2,     2,     3,     3,     2,     1,     1,
       1,     1,     3,     2,     4,     4,     5,     2,     3,     4,
       4,     5,     2,     2,     1,     2,     3,     3,     3,     3,
       1,     1,     1,     4,     4,     2,     2,     1,     3,     1,
       3,     1,     3,     1,     1,     1,     0,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,    58,   184,   168,   169,     0,   170,     0,     0,     0,
      72,     0,     0,    71,     0,     0,    70,    69,     0,    56,
      57,     0,     0,     0,     0,     0,     0,     0,    19,     3,
       0,    35,     0,    55,    54,    53,    52,    51,    46,    45,
      50,    49,    44,    47,    48,    40,    43,    41,   122,   171,
      60,    59,   101,     0,   103,     0,   171,    11,     0,     0,
       0,     0,    97,    91,     0,     0,    99,     0,     0,     0,
      63,    66,     0,    65,     0,   156,   155,   173,   204,   203,
     177,   207,     0,     0,   206,   167,     0,   158,   157,   183,
     182,     0,     0,   206,     1,    20,    36,    37,    38,    39,
       0,     0,     0,     0,     0,     0,     0,   164,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     163,   190,     0,     0,     0,   185,    61,     0,     0,   110,
     104,   105,    79,    78,    77,    80,     0,    58,     0,    22,
       0,    25,    21,    27,    32,     0,     0,    30,    15,     0,
       0,    95,     0,     0,     0,    81,     0,     0,    68,    67,
     172,     0,     0,   205,     0,   166,   165,   178,     0,     0,
     140,   139,   144,   143,   146,   145,   150,   149,   152,   151,
     154,   153,   162,   161,   121,   120,   134,   133,   136,   135,
     138,   137,   142,   141,   148,   147,   124,   123,   126,   125,
     128,   127,   130,   129,   132,   131,   160,   159,   192,     0,
     191,   197,     0,     0,   113,   112,   172,   108,   107,     0,
       0,   106,     0,    13,     0,     0,    33,    28,     0,     0,
      58,     0,     0,    96,    90,    94,     0,     0,     0,     0,
      87,    82,    83,     0,    73,     0,   175,   206,   174,     0,
     208,   180,   206,   179,     0,   196,   195,   187,   186,     0,
     189,   188,    58,     0,   100,     0,     0,     0,    29,    23,
      31,    17,     0,     0,    93,     0,    88,    98,     0,     0,
       0,    84,   119,     0,   176,   181,     0,   198,    58,     0,
      34,     0,     0,    92,    89,     0,     0,    76,   118,     0,
     194,   193,    58,     0,     0,   117,     0,   116,   114
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,    27,    28,    29,    61,   146,   147,   267,    30,    31,
      32,    33,    51,    34,    35,    72,    36,    37,    38,    39,
      40,   157,    41,   136,   240,   241,   242,   280,    42,    65,
      43,    44,    54,   129,   130,   131,   220,    45,    46,    47,
      48,    56,   125,   211,   212,    91,    82,    83,   164,    84
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -132
static const short yypact[] =
{
     339,  -132,  -132,  -132,  -132,   -10,  -132,   318,   161,    12,
    -132,  1670,  2351,  -132,  1415,   199,  -132,  -132,   225,  -132,
    -132,  2351,  2351,  1626,  1701,  1729,   186,    34,  -132,     1,
     752,  -132,   176,  -132,  -132,  -132,  -132,  -132,  -132,  -132,
    -132,  -132,  -132,  -132,  -132,  -132,  -132,  2401,  -132,   215,
    -132,    37,  -132,  1626,    38,  2420,   286,   252,   929,    24,
     222,   283,  -132,    32,   304,   202,  -132,  2420,  2420,  1002,
    -132,  -132,    42,  -132,    54,     3,     3,  -132,  -132,  -132,
    -132,  2401,    85,  2351,    44,  -132,  2235,  -132,     3,  -132,
    -132,     4,  2351,     6,  -132,  -132,  -132,  -132,  -132,  -132,
    1734,  1760,  1765,  1793,  1824,  1843,  1852,  -132,  1871,  1902,
    1930,  1935,  1961,  1966,  1994,  2025,  2044,  2053,  2072,  2103,
    -132,  -132,  2292,  2292,  2131,  -132,  -132,    88,  2136,    60,
      38,  -132,  -132,  -132,  -132,  -132,  1415,   275,    53,  -132,
     398,  -132,  -132,  -132,  -132,    62,   104,  -132,  -132,  1061,
    2162,  -132,    41,  1415,  1415,    82,  1415,   111,  -132,  -132,
      94,  2319,   190,  -132,  2351,  -132,  -132,  -132,    29,    18,
    -132,   438,  -132,   438,  -132,   438,  -132,     3,  -132,     3,
    -132,     3,  -132,     3,  -132,  2483,  -132,  2497,  -132,  2510,
    -132,   438,  -132,   438,  -132,   438,  -132,   313,  -132,   313,
    -132,     3,  -132,     3,  -132,     3,  -132,     3,  -132,   308,
    2401,  -132,    20,    28,  -132,  2401,  -132,  -132,  -132,  1474,
     126,  -132,   811,  -132,  1415,   136,  -132,  -132,    70,    25,
      51,   217,   457,  -132,  2401,  -132,  2167,  1120,  1179,  2351,
      50,    82,  -132,  1238,  -132,   327,  -132,   230,  -132,  2327,
    2401,  -132,    13,  -132,   143,  -132,   108,  -132,  -132,  2292,
    -132,  -132,   144,  1297,  -132,   516,  1533,   187,  -132,  -132,
    -132,  -132,  1415,   153,  -132,  2262,  -132,  -132,  2420,  1415,
     160,  -132,  -132,   162,  -132,  -132,  2195,  -132,   142,   575,
    -132,   634,  1592,  -132,  -132,  1415,  1356,  -132,  -132,  1651,
    -132,  2401,   193,   693,   870,  -132,   112,  -132,  -132
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -132,  -132,   181,  -132,  -132,  -131,   -24,  -132,    21,    45,
    -132,  -132,  -132,  -132,  -132,   213,  -132,  -132,  -132,  -132,
    -132,  -132,  -132,   -64,  -132,  -132,   -30,  -132,  -132,  -132,
    -132,  -132,   105,  -132,  -132,   109,  -132,  -132,  -132,    15,
    -132,     0,  -132,   -11,  -121,   169,   -22,   -18,  -132,   -20
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -203
static const short yytable[] =
{
      49,   153,   213,   154,   155,    50,    93,   225,    92,    78,
      79,  -199,  -199,    62,    49,    63,   106,   107,  -200,  -200,
       8,   257,    55,    78,    79,   141,    67,    68,   144,   260,
      49,   127,     2,     3,    94,    69,    75,    76,    81,    86,
      88,    81,   235,   158,     4,   159,     6,   -62,   -62,  -201,
    -201,   -16,   126,   258,   223,   158,   144,   159,    49,   -64,
     -64,   162,   128,   261,   161,   226,   119,   120,    81,    49,
     -16,   268,    93,   168,   163,    96,   145,   279,   167,   140,
    -199,   163,    64,   219,    21,    22,   224,  -200,   259,   142,
      78,    79,   253,    78,    79,    53,   259,   150,    81,    24,
     273,    25,    26,   251,   145,   227,   236,    81,   239,   161,
     -62,  -201,   163,   307,    96,   171,   173,   175,   177,   179,
     181,   183,   -64,   185,   187,   189,   191,   193,   195,   197,
     199,   201,   203,   205,   207,   269,    49,   210,   210,   215,
      49,   247,   -14,    55,   249,   308,     2,     3,   252,    49,
     244,   254,   160,    49,    49,   216,    49,   222,     4,   245,
       6,   -14,    57,   298,    58,   234,   264,  -115,  -115,   266,
     232,   228,   229,   286,   237,   238,    81,   243,   306,   250,
     259,    97,    98,    81,  -111,    96,   292,    89,   290,     2,
       3,    78,    79,   -18,   299,    78,    79,   297,    21,    22,
      70,     4,    71,     6,   229,   270,    59,   132,   133,    53,
      95,   281,   -18,    24,   295,    25,    26,   285,   271,    49,
     144,   229,    49,   143,    49,   144,    73,    60,    71,   247,
    -115,    74,    49,   218,   252,  -202,  -202,    49,    49,   221,
     263,    21,    22,    49,    99,   265,   121,   122,   287,   123,
     272,   275,    53,   -24,   278,   -24,    24,   248,    25,    26,
      90,   169,     0,    49,    81,    49,    49,    96,   145,    81,
     134,   135,    49,   145,   210,   -12,   -26,    96,   -26,    49,
     124,     0,    96,    96,   148,     0,   149,   289,    96,    49,
       0,    49,    49,   291,   -12,    49,    49,  -202,   163,     0,
     296,   301,     0,    49,    49,   151,     0,   152,    96,   255,
      96,   256,     0,   303,   210,     0,   304,   121,   122,    52,
     123,     2,     3,   103,   104,   105,   106,   107,   282,     0,
     283,     0,     0,     4,    96,     6,    96,     0,     0,    -4,
       1,    96,     2,     3,   -42,   -42,     0,     0,    96,    96,
       0,     0,     0,     0,     4,     5,     6,     7,     8,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
     116,   117,   118,    21,    22,     0,   119,   120,     0,     0,
      15,    16,    17,     0,    53,    18,    19,    20,    24,     0,
      25,    26,     0,     0,    21,    22,     0,     0,    -8,     1,
       0,     2,     3,   -42,   -42,    23,     0,   -42,     0,    24,
       0,    25,    26,     4,     5,     6,     7,    -8,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,   103,   104,
     105,   106,   107,    21,    22,     0,     0,    -7,     1,     0,
       2,     3,   -42,   -42,    23,     0,   -42,     0,    24,     0,
      25,    26,     4,     5,     6,     7,    -7,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,     0,     0,
       0,     0,     0,   114,   115,   116,   117,   118,    15,    16,
      17,   119,   120,    18,    19,    20,     0,     0,     0,     0,
       0,     0,    21,    22,     0,     0,   -10,     1,     0,     2,
       3,   -42,   -42,    23,     0,   -42,     0,    24,     0,    25,
      26,     4,     5,     6,     7,   -10,     9,    10,    11,     0,
       0,    12,     0,     0,    13,    14,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,    -6,     1,     0,     2,     3,
     -42,   -42,    23,     0,   -42,     0,    24,     0,    25,    26,
       4,     5,     6,     7,    -6,     9,    10,    11,     0,     0,
      12,     0,     0,    13,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,    16,    17,     0,
       0,    18,    19,    20,     0,     0,     0,     0,     0,     0,
      21,    22,     0,     0,    -9,     1,     0,     2,     3,   -42,
     -42,    23,     0,   -42,     0,    24,     0,    25,    26,     4,
       5,     6,     7,    -9,     9,    10,    11,     0,     0,    12,
       0,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,    -5,     1,     0,     2,     3,   -42,   -42,
      23,     0,   -42,     0,    24,     0,    25,    26,     4,     5,
       6,     7,    -5,     9,    10,    11,     0,     0,    12,     0,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,    16,    17,     0,     0,    18,
      19,    20,     0,     0,     0,     0,     0,     0,    21,    22,
       0,     0,    -2,     1,     0,     2,     3,   -42,   -42,    23,
       0,   -42,     0,    24,     0,    25,    26,     4,     5,     6,
       7,     0,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,    16,    17,     0,     0,    18,    19,
      20,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,     1,     0,     2,     3,   -42,   -42,    23,     0,
     -42,     0,    24,     0,    25,    26,     4,     5,     6,     7,
       0,     9,    10,    11,  -102,  -102,    12,     0,     0,    13,
      14,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -102,    15,    16,    17,     0,     0,    18,    19,    20,
       0,     0,     0,     0,     0,     0,    21,    22,     0,     0,
       0,     1,     0,     2,     3,   -42,   -42,    23,     0,   -42,
       0,    24,     0,    25,    26,     4,     5,     6,     7,     0,
       9,    10,    11,     0,     0,    12,   -85,   -85,    13,    14,
       0,     0,     0,     0,     0,     0,     0,   -85,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
     137,     0,     2,     3,   -42,   -42,    23,     0,   -42,     0,
      24,     0,    25,    26,     4,     5,     6,     7,     0,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,   138,     0,     0,     0,     0,     0,     0,     0,     0,
      15,    16,    17,     0,     0,    18,    19,    20,     0,     0,
       0,     0,     0,     0,    21,    22,     0,     0,     0,     0,
       0,     0,     0,     0,   139,    23,     0,   -42,     0,    24,
       0,    25,    26,     1,     0,     2,     3,   -42,   -42,     0,
       0,     0,     0,     0,     0,     0,     0,     4,     5,     6,
       7,     0,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,   156,     0,     0,     0,     0,     0,     0,     0,
       0,   -75,     0,    15,    16,    17,     0,     0,    18,    19,
      20,     0,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,   230,     0,     2,     3,   -42,   -42,    23,     0,
     -42,     0,    24,     0,    25,    26,     4,     5,     6,     7,
       0,     9,    10,    11,     0,     0,    12,     0,     0,    13,
      14,     0,     0,   231,     0,     0,     0,     0,     0,     0,
       0,     0,    15,    16,    17,     0,     0,    18,    19,    20,
       0,     0,     0,     0,     0,     0,    21,    22,     0,     0,
       0,     1,     0,     2,     3,   -42,   -42,    23,     0,   -42,
       0,    24,     0,    25,    26,     4,     5,     6,     7,     0,
       9,    10,    11,     0,     0,    12,     0,     0,    13,    14,
       0,     0,     0,     0,     0,     0,   276,     0,     0,     0,
       0,    15,    16,    17,     0,     0,    18,    19,    20,     0,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
       1,     0,     2,     3,   -42,   -42,    23,     0,   -42,     0,
      24,     0,    25,    26,     4,     5,     6,     7,     0,     9,
      10,    11,     0,     0,    12,     0,     0,    13,    14,     0,
       0,     0,     0,     0,     0,     0,     0,   277,     0,     0,
      15,    16,    17,     0,     0,    18,    19,    20,     0,     0,
       0,     0,     0,     0,    21,    22,     0,     0,     0,     1,
       0,     2,     3,   -42,   -42,    23,     0,   -42,     0,    24,
       0,    25,    26,     4,     5,     6,     7,     0,     9,    10,
      11,     0,     0,    12,     0,     0,    13,    14,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   -74,     0,    15,
      16,    17,     0,     0,    18,    19,    20,     0,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     1,     0,
       2,     3,   -42,   -42,    23,     0,   -42,     0,    24,     0,
      25,    26,     4,     5,     6,     7,     0,     9,    10,    11,
       0,     0,    12,     0,     0,    13,    14,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -109,    15,    16,
      17,     0,     0,    18,    19,    20,     0,     0,     0,     0,
       0,     0,    21,    22,     0,     0,     0,     1,     0,     2,
       3,   -42,   -42,    23,     0,   -42,     0,    24,     0,    25,
      26,     4,     5,     6,     7,     0,     9,    10,    11,     0,
       0,    12,     0,     0,    13,    14,     0,     0,     0,     0,
       0,     0,     0,   -86,     0,     0,     0,    15,    16,    17,
       0,     0,    18,    19,    20,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,     1,     0,     2,     3,
     -42,   -42,    23,     0,   -42,     0,    24,     0,    25,    26,
       4,     5,     6,     7,     0,     9,    10,    11,     0,     0,
      12,     0,     0,    13,    14,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,    16,    17,     0,
       0,    18,    19,    20,     0,     0,     0,     0,     0,     0,
      21,    22,     0,     0,     0,   262,     0,     2,     3,   -42,
     -42,    23,     0,   -42,     0,    24,     0,    25,    26,     4,
       5,     6,     7,     0,     9,    10,    11,     0,     0,    12,
       0,     0,    13,    14,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    15,    16,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,     0,     0,    21,
      22,     0,     0,     0,   288,     0,     2,     3,   -42,   -42,
      23,     0,   -42,     0,    24,     0,    25,    26,     4,     5,
       6,     7,     0,     9,    10,    11,     0,     0,    12,     0,
       0,    13,    14,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,    16,    17,     0,     0,    18,
      19,    20,     0,     0,     0,     0,     0,     0,    21,    22,
       0,     0,     0,   302,     0,     2,     3,   -42,   -42,    23,
       0,   -42,     0,    24,     0,    25,    26,     4,     5,     6,
       7,     0,     9,    10,    11,     0,     0,    12,     0,     0,
      13,    14,     0,     0,     0,     0,     0,    77,     0,     2,
       3,    78,    79,    15,    16,    17,     0,     0,    18,    19,
      20,     4,     0,     6,     0,     0,     0,    21,    22,     0,
       0,     0,   305,     0,     2,     3,     0,     0,    23,     0,
     -42,     0,    24,     0,    25,    26,     4,     0,     6,     0,
       0,    66,     0,     2,     3,     0,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     4,     0,     6,     0,     0,
       0,     0,    53,    80,     0,     0,    24,     0,    25,    26,
     208,     0,    85,     0,     2,     3,    21,    22,     0,   209,
       0,     0,     0,     0,     0,     0,     4,    53,     6,     0,
       0,    24,     0,    25,    26,    21,    22,     0,     0,     0,
      87,     0,     2,     3,     0,   170,    53,     2,     3,     0,
      24,     0,    25,    26,     4,     0,     6,     0,     0,     4,
       0,     6,     0,     0,     0,     0,    21,    22,     0,     0,
       0,   172,     0,     2,     3,     0,   174,    53,     2,     3,
       0,    24,     0,    25,    26,     4,     0,     6,     0,     0,
       4,     0,     6,     0,    21,    22,     0,     0,     0,    21,
      22,     0,     0,     0,   176,    53,     2,     3,     0,    24,
      53,    25,    26,     0,    24,     0,    25,    26,     4,     0,
       6,     0,     0,     0,     0,    21,    22,     0,     0,     0,
      21,    22,     0,     0,     0,   178,    53,     2,     3,     0,
      24,    53,    25,    26,     0,    24,     0,    25,    26,     4,
       0,     6,     0,     0,   180,     0,     2,     3,    21,    22,
       0,     0,     0,   182,     0,     2,     3,     0,     4,    53,
       6,     0,     0,    24,     0,    25,    26,     4,     0,     6,
       0,     0,   184,     0,     2,     3,     0,     0,     0,    21,
      22,     0,     0,     0,     0,     0,     4,     0,     6,     0,
      53,     0,     0,     0,    24,     0,    25,    26,    21,    22,
       0,     0,     0,   186,     0,     2,     3,    21,    22,    53,
       0,     0,     0,    24,     0,    25,    26,     4,    53,     6,
       0,     0,    24,     0,    25,    26,    21,    22,     0,     0,
       0,   188,     0,     2,     3,     0,   190,    53,     2,     3,
       0,    24,     0,    25,    26,     4,     0,     6,     0,     0,
       4,     0,     6,     0,     0,     0,     0,    21,    22,     0,
       0,     0,   192,     0,     2,     3,     0,   194,    53,     2,
       3,     0,    24,     0,    25,    26,     4,     0,     6,     0,
       0,     4,     0,     6,     0,    21,    22,     0,     0,     0,
      21,    22,     0,     0,     0,   196,    53,     2,     3,     0,
      24,    53,    25,    26,     0,    24,     0,    25,    26,     4,
       0,     6,     0,     0,     0,     0,    21,    22,     0,     0,
       0,    21,    22,     0,     0,     0,   198,    53,     2,     3,
       0,    24,    53,    25,    26,     0,    24,     0,    25,    26,
       4,     0,     6,     0,     0,   200,     0,     2,     3,    21,
      22,     0,     0,     0,   202,     0,     2,     3,     0,     4,
      53,     6,     0,     0,    24,     0,    25,    26,     4,     0,
       6,     0,     0,   204,     0,     2,     3,     0,     0,     0,
      21,    22,     0,     0,     0,     0,     0,     4,     0,     6,
       0,    53,     0,     0,     0,    24,     0,    25,    26,    21,
      22,     0,     0,     0,   206,     0,     2,     3,    21,    22,
      53,     0,     0,     0,    24,     0,    25,    26,     4,    53,
       6,     0,     0,    24,     0,    25,    26,    21,    22,     0,
       0,     0,   214,     0,     2,     3,     0,   217,    53,     2,
       3,     0,    24,     0,    25,    26,     4,     0,     6,     0,
       0,     4,     0,     6,     0,     0,     0,     0,    21,    22,
       0,     0,     0,   233,     0,     2,     3,     0,   274,    53,
       2,     3,     0,    24,     0,    25,    26,     4,     0,     6,
       0,     0,     4,     0,     6,     0,    21,    22,     0,     0,
       0,    21,    22,     0,     0,     0,   300,    53,     2,     3,
       0,    24,    53,    25,    26,     0,    24,     0,    25,    26,
       4,     0,     6,     0,     0,     0,     0,    21,    22,     0,
       0,     0,    21,    22,     0,     0,     0,     0,    53,     0,
       0,     0,    24,    53,    25,    26,   165,    24,     0,    25,
      26,     0,   100,   101,   102,   103,   104,   105,   106,   107,
      21,    22,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    53,     0,   293,     0,    24,     0,    25,    26,   100,
     101,   102,   103,   104,   105,   106,   107,     0,     0,     0,
       0,     0,     0,     0,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,     2,     3,     0,   119,   120,
       0,     0,     0,     0,     0,     0,   166,     4,     0,     6,
       0,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,     2,     3,     0,   119,   120,     0,     0,     0,
       2,     3,     0,   294,     4,     0,     6,     0,     0,     0,
       0,   208,     4,     0,     6,     0,     0,    21,    22,     0,
     209,     0,     0,     0,     2,     3,     0,     0,    53,     0,
       0,     0,    24,     0,    25,    26,     4,     0,     6,     0,
       0,     0,     0,     0,    21,    22,     0,     0,     0,     0,
       0,     0,    21,    22,     0,    53,   246,     0,     0,    24,
       0,    25,    26,    53,   284,     0,     0,    24,     0,    25,
      26,     0,     0,     0,     0,     0,    21,    22,   100,   101,
     102,   103,   104,   105,   106,   107,     0,    53,     0,     0,
       0,    24,     0,    25,    26,   132,   133,   100,   101,   102,
     103,   104,   105,   106,   107,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,     0,     0,     0,   119,   120,     0,     0,     0,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
       0,     0,     0,   119,   120,     0,     0,     0,   134,   135,
     100,   101,   102,   103,   104,   105,   106,   107,     0,     0,
       0,     0,     0,     0,   100,   101,   102,   103,   104,   105,
     106,   107,     0,     0,     0,     0,     0,   100,   101,   102,
     103,   104,   105,   106,   107,     0,     0,     0,     0,     0,
       0,     0,     0,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,     0,     0,     0,   119,   120,   110,   111,
     112,   113,   114,   115,   116,   117,   118,     0,     0,     0,
     119,   120,   111,   112,   113,   114,   115,   116,   117,   118,
       0,     0,     0,   119,   120
};

static const short yycheck[] =
{
       0,    65,   123,    67,    68,    15,    26,   138,    26,     5,
       6,     5,     6,     1,    14,     3,    13,    14,     5,     6,
      19,     1,     7,     5,     6,     1,    11,    12,     3,     1,
      30,    53,     3,     4,     0,    14,    21,    22,    23,    24,
      25,    26,     1,     1,    15,     3,    17,     5,     6,     5,
       6,     0,    15,    33,     1,     1,     3,     3,    58,     5,
       6,    83,    24,    35,    82,     3,    63,    64,    53,    69,
      19,     1,    92,    91,    68,    30,    51,    27,    74,    58,
      74,    68,    70,    23,    55,    56,    33,    74,    68,    65,
       5,     6,    74,     5,     6,    66,    68,    65,    83,    70,
     231,    72,    73,    74,    51,     1,    65,    92,    26,   127,
      68,    67,    68,     1,    69,   100,   101,   102,   103,   104,
     105,   106,    68,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,    65,   136,   122,   123,   124,
     140,   161,     0,   128,   162,    33,     3,     4,   168,   149,
      39,   169,    67,   153,   154,    67,   156,   136,    15,    65,
      17,    19,     1,     1,     3,   150,    40,     5,     6,    33,
     149,    67,    68,    65,   153,   154,   161,   156,   299,   164,
      68,     5,     6,   168,    40,   140,    33,     1,     1,     3,
       4,     5,     6,     0,    32,     5,     6,    37,    55,    56,
       1,    15,     3,    17,    68,   229,    45,     5,     6,    66,
      29,   241,    19,    70,   278,    72,    73,    74,     1,   219,
       3,    68,   222,     1,   224,     3,     1,    66,     3,   249,
      68,    18,   232,   128,   254,     5,     6,   237,   238,   130,
     219,    55,    56,   243,    68,   224,    31,    32,   259,    34,
      33,   236,    66,     1,   239,     3,    70,    67,    72,    73,
      74,    92,    -1,   263,   249,   265,   266,   222,    51,   254,
      68,    69,   272,    51,   259,     0,     1,   232,     3,   279,
      65,    -1,   237,   238,     1,    -1,     3,   266,   243,   289,
      -1,   291,   292,   272,    19,   295,   296,    67,    68,    -1,
     279,   286,    -1,   303,   304,     1,    -1,     3,   263,     1,
     265,     3,    -1,   292,   299,    -1,   295,    31,    32,     1,
      34,     3,     4,    10,    11,    12,    13,    14,     1,    -1,
       3,    -1,    -1,    15,   289,    17,   291,    -1,    -1,     0,
       1,   296,     3,     4,     5,     6,    -1,    -1,   303,   304,
      -1,    -1,    -1,    -1,    15,    16,    17,    18,    19,    20,
      21,    22,    -1,    -1,    25,    -1,    -1,    28,    29,    -1,
      57,    58,    59,    55,    56,    -1,    63,    64,    -1,    -1,
      41,    42,    43,    -1,    66,    46,    47,    48,    70,    -1,
      72,    73,    -1,    -1,    55,    56,    -1,    -1,     0,     1,
      -1,     3,     4,     5,     6,    66,    -1,    68,    -1,    70,
      -1,    72,    73,    15,    16,    17,    18,    19,    20,    21,
      22,    -1,    -1,    25,    -1,    -1,    28,    29,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    41,
      42,    43,    -1,    -1,    46,    47,    48,    -1,    10,    11,
      12,    13,    14,    55,    56,    -1,    -1,     0,     1,    -1,
       3,     4,     5,     6,    66,    -1,    68,    -1,    70,    -1,
      72,    73,    15,    16,    17,    18,    19,    20,    21,    22,
      -1,    -1,    25,    -1,    -1,    28,    29,    -1,    -1,    -1,
      -1,    -1,    -1,    55,    56,    57,    58,    59,    41,    42,
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
      17,    18,    19,    20,    21,    22,    -1,    -1,    25,    -1,
      -1,    28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    41,    42,    43,    -1,    -1,    46,
      47,    48,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,
      -1,    -1,     0,     1,    -1,     3,     4,     5,     6,    66,
      -1,    68,    -1,    70,    -1,    72,    73,    15,    16,    17,
      18,    -1,    20,    21,    22,    -1,    -1,    25,    -1,    -1,
      28,    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    41,    42,    43,    -1,    -1,    46,    47,
      48,    -1,    -1,    -1,    -1,    -1,    -1,    55,    56,    -1,
      -1,    -1,     1,    -1,     3,     4,     5,     6,    66,    -1,
      68,    -1,    70,    -1,    72,    73,    15,    16,    17,    18,
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
      49,    -1,     1,    -1,     3,     4,    55,    56,    -1,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    66,    17,    -1,
      -1,    70,    -1,    72,    73,    55,    56,    -1,    -1,    -1,
       1,    -1,     3,     4,    -1,     1,    66,     3,     4,    -1,
      70,    -1,    72,    73,    15,    -1,    17,    -1,    -1,    15,
      -1,    17,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,
      -1,     1,    -1,     3,     4,    -1,     1,    66,     3,     4,
      -1,    70,    -1,    72,    73,    15,    -1,    17,    -1,    -1,
      15,    -1,    17,    -1,    55,    56,    -1,    -1,    -1,    55,
      56,    -1,    -1,    -1,     1,    66,     3,     4,    -1,    70,
      66,    72,    73,    -1,    70,    -1,    72,    73,    15,    -1,
      17,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,
      55,    56,    -1,    -1,    -1,     1,    66,     3,     4,    -1,
      70,    66,    72,    73,    -1,    70,    -1,    72,    73,    15,
      -1,    17,    -1,    -1,     1,    -1,     3,     4,    55,    56,
      -1,    -1,    -1,     1,    -1,     3,     4,    -1,    15,    66,
      17,    -1,    -1,    70,    -1,    72,    73,    15,    -1,    17,
      -1,    -1,     1,    -1,     3,     4,    -1,    -1,    -1,    55,
      56,    -1,    -1,    -1,    -1,    -1,    15,    -1,    17,    -1,
      66,    -1,    -1,    -1,    70,    -1,    72,    73,    55,    56,
      -1,    -1,    -1,     1,    -1,     3,     4,    55,    56,    66,
      -1,    -1,    -1,    70,    -1,    72,    73,    15,    66,    17,
      -1,    -1,    70,    -1,    72,    73,    55,    56,    -1,    -1,
      -1,     1,    -1,     3,     4,    -1,     1,    66,     3,     4,
      -1,    70,    -1,    72,    73,    15,    -1,    17,    -1,    -1,
      15,    -1,    17,    -1,    -1,    -1,    -1,    55,    56,    -1,
      -1,    -1,     1,    -1,     3,     4,    -1,     1,    66,     3,
       4,    -1,    70,    -1,    72,    73,    15,    -1,    17,    -1,
      -1,    15,    -1,    17,    -1,    55,    56,    -1,    -1,    -1,
      55,    56,    -1,    -1,    -1,     1,    66,     3,     4,    -1,
      70,    66,    72,    73,    -1,    70,    -1,    72,    73,    15,
      -1,    17,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,
      -1,    55,    56,    -1,    -1,    -1,     1,    66,     3,     4,
      -1,    70,    66,    72,    73,    -1,    70,    -1,    72,    73,
      15,    -1,    17,    -1,    -1,     1,    -1,     3,     4,    55,
      56,    -1,    -1,    -1,     1,    -1,     3,     4,    -1,    15,
      66,    17,    -1,    -1,    70,    -1,    72,    73,    15,    -1,
      17,    -1,    -1,     1,    -1,     3,     4,    -1,    -1,    -1,
      55,    56,    -1,    -1,    -1,    -1,    -1,    15,    -1,    17,
      -1,    66,    -1,    -1,    -1,    70,    -1,    72,    73,    55,
      56,    -1,    -1,    -1,     1,    -1,     3,     4,    55,    56,
      66,    -1,    -1,    -1,    70,    -1,    72,    73,    15,    66,
      17,    -1,    -1,    70,    -1,    72,    73,    55,    56,    -1,
      -1,    -1,     1,    -1,     3,     4,    -1,     1,    66,     3,
       4,    -1,    70,    -1,    72,    73,    15,    -1,    17,    -1,
      -1,    15,    -1,    17,    -1,    -1,    -1,    -1,    55,    56,
      -1,    -1,    -1,     1,    -1,     3,     4,    -1,     1,    66,
       3,     4,    -1,    70,    -1,    72,    73,    15,    -1,    17,
      -1,    -1,    15,    -1,    17,    -1,    55,    56,    -1,    -1,
      -1,    55,    56,    -1,    -1,    -1,     1,    66,     3,     4,
      -1,    70,    66,    72,    73,    -1,    70,    -1,    72,    73,
      15,    -1,    17,    -1,    -1,    -1,    -1,    55,    56,    -1,
      -1,    -1,    55,    56,    -1,    -1,    -1,    -1,    66,    -1,
      -1,    -1,    70,    66,    72,    73,     1,    70,    -1,    72,
      73,    -1,     7,     8,     9,    10,    11,    12,    13,    14,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    66,    -1,     1,    -1,    70,    -1,    72,    73,     7,
       8,     9,    10,    11,    12,    13,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,     3,     4,    -1,    63,    64,
      -1,    -1,    -1,    -1,    -1,    -1,    71,    15,    -1,    17,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,     3,     4,    -1,    63,    64,    -1,    -1,    -1,
       3,     4,    -1,    71,    15,    -1,    17,    -1,    -1,    -1,
      -1,    49,    15,    -1,    17,    -1,    -1,    55,    56,    -1,
      58,    -1,    -1,    -1,     3,     4,    -1,    -1,    66,    -1,
      -1,    -1,    70,    -1,    72,    73,    15,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    55,    56,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    56,    -1,    66,    67,    -1,    -1,    70,
      -1,    72,    73,    66,    67,    -1,    -1,    70,    -1,    72,
      73,    -1,    -1,    -1,    -1,    -1,    55,    56,     7,     8,
       9,    10,    11,    12,    13,    14,    -1,    66,    -1,    -1,
      -1,    70,    -1,    72,    73,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    -1,    -1,    63,    64,    -1,    -1,    -1,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    -1,    -1,    63,    64,    -1,    -1,    -1,    68,    69,
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
      48,    55,    56,    66,    70,    72,    73,    76,    77,    78,
      83,    84,    85,    86,    88,    89,    91,    92,    93,    94,
      95,    97,   103,   105,   106,   112,   113,   114,   115,   116,
      15,    87,     1,    66,   107,   114,   116,     1,     3,    45,
      66,    79,     1,     3,    70,   104,     1,   114,   114,    83,
       1,     3,    90,     1,    90,   114,   114,     1,     5,     6,
      67,   114,   121,   122,   124,     1,   114,     1,   114,     1,
      74,   120,   122,   124,     0,    77,    84,     5,     6,    68,
       7,     8,     9,    10,    11,    12,    13,    14,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    63,
      64,    31,    32,    34,    65,   117,    15,   121,    24,   108,
     109,   110,     5,     6,    68,    69,    98,     1,    32,    65,
      83,     1,    65,     1,     3,    51,    80,    81,     1,     3,
      65,     1,     3,    98,    98,    98,    30,    96,     1,     3,
      67,   122,   121,    68,   123,     1,    71,    74,   122,   120,
       1,   114,     1,   114,     1,   114,     1,   114,     1,   114,
       1,   114,     1,   114,     1,   114,     1,   114,     1,   114,
       1,   114,     1,   114,     1,   114,     1,   114,     1,   114,
       1,   114,     1,   114,     1,   114,     1,   114,    49,    58,
     114,   118,   119,   119,     1,   114,    67,     1,   107,    23,
     111,   110,    83,     1,    33,    80,     3,     1,    67,    68,
       1,    32,    83,     1,   114,     1,    65,    83,    83,    26,
      99,   100,   101,    83,    39,    65,    67,   124,    67,   122,
     114,    74,   124,    74,   122,     1,     3,     1,    33,    68,
       1,    35,     1,    83,    40,    83,    33,    82,     1,    65,
      81,     1,    33,    80,     1,   114,    36,    38,   114,    27,
     102,   101,     1,     3,    67,    74,    65,   118,     1,    83,
       1,    83,    33,     1,    71,    98,    83,    37,     1,    32,
       1,   114,     1,    83,    83,     1,   119,     1,    33
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
#line 140 "../../../libs/libFreeMat/Parser.yy"
    {mainAST = yyvsp[0];}
    break;

  case 5:
#line 145 "../../../libs/libFreeMat/Parser.yy"
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
#line 161 "../../../libs/libFreeMat/Parser.yy"
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
#line 176 "../../../libs/libFreeMat/Parser.yy"
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
#line 191 "../../../libs/libFreeMat/Parser.yy"
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
#line 205 "../../../libs/libFreeMat/Parser.yy"
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
#line 220 "../../../libs/libFreeMat/Parser.yy"
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
#line 234 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("function name or return declaration");}
    break;

  case 12:
#line 235 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("argument list or statement list");}
    break;

  case 13:
#line 236 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("(possibly empty) argument list");}
    break;

  case 14:
#line 237 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("statement list");}
    break;

  case 15:
#line 238 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("function name");}
    break;

  case 16:
#line 239 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("argument list or statement list");}
    break;

  case 17:
#line 240 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("(possibly empty) argument list");}
    break;

  case 18:
#line 241 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("statement list");}
    break;

  case 21:
#line 250 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 22:
#line 251 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 23:
#line 252 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2];}
    break;

  case 24:
#line 253 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("either 'varargout', a single returned variable, or a list of return variables in return declaration");}
    break;

  case 25:
#line 254 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an '=' symbol after 'varargout' in return declaration");}
    break;

  case 26:
#line 255 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an '=' symbol after identifier in return declaration");}
    break;

  case 27:
#line 256 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("a valid list of return arguments in return declaration");}
    break;

  case 28:
#line 257 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("matching ']' in return declaration");}
    break;

  case 29:
#line 258 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an '=' symbol after return declaration");}
    break;

  case 30:
#line 262 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[0];}
    break;

  case 31:
#line 263 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 33:
#line 267 "../../../libs/libFreeMat/Parser.yy"
    {
	yyval = yyvsp[0];
	char *b = (char*) malloc(strlen(yyvsp[0]->text)+2);
	b[0] = '&';
	strcpy(b+1,yyvsp[0]->text);
	yyval->text = b;
  }
    break;

  case 34:
#line 274 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("either an identifier or an ampersand '&' (indicating pass by reference) followed by an identifier in argument list");}
    break;

  case 35:
#line 279 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_BLOCK,yyvsp[0]);}
    break;

  case 36:
#line 280 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 37:
#line 284 "../../../libs/libFreeMat/Parser.yy"
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
#line 293 "../../../libs/libFreeMat/Parser.yy"
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
#line 302 "../../../libs/libFreeMat/Parser.yy"
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
#line 316 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(null_node,"");}
    break;

  case 58:
#line 331 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("recognizable statement type (assignment, expression, function call, etc...)");}
    break;

  case 59:
#line 335 "../../../libs/libFreeMat/Parser.yy"
    {yyvsp[-1]->addChild(yyvsp[0]); yyval = new AST(OP_SCALL,yyvsp[-1]);}
    break;

  case 60:
#line 339 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_PARENS,yyvsp[0]);}
    break;

  case 61:
#line 340 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 62:
#line 344 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]); }
    break;

  case 63:
#line 345 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("list of variables to be tagged as persistent");}
    break;

  case 64:
#line 349 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 65:
#line 350 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("list of variables to be tagged as global");}
    break;

  case 67:
#line 355 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 68:
#line 356 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("list of valid identifiers");}
    break;

  case 73:
#line 376 "../../../libs/libFreeMat/Parser.yy"
    { yyval = yyvsp[-3]; yyval->addChild(yyvsp[-2]); if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);}
    break;

  case 74:
#line 380 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[0];}
    break;

  case 75:
#line 381 "../../../libs/libFreeMat/Parser.yy"
    {yyval = NULL;}
    break;

  case 76:
#line 385 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-5]; yyval->addChild(yyvsp[-4]); 
	  if (yyvsp[-2] != NULL) yyval->addChild(yyvsp[-2]); 
	  if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);
	}
    break;

  case 81:
#line 397 "../../../libs/libFreeMat/Parser.yy"
    {yyval = NULL;}
    break;

  case 83:
#line 402 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = new AST(OP_CASEBLOCK,yyvsp[0]);
	}
    break;

  case 84:
#line 405 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);
	}
    break;

  case 85:
#line 411 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-3]; yyval->addChild(yyvsp[-2]); yyval->addChild(yyvsp[0]);
	}
    break;

  case 86:
#line 417 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 87:
#line 420 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = NULL;
	}
    break;

  case 88:
#line 426 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-4]; yyval->addChild(yyvsp[-3]); yyval->addChild(yyvsp[-1]);
	}
    break;

  case 89:
#line 432 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-3]; yyval->addChild(yyvsp[-1]);}
    break;

  case 90:
#line 433 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 91:
#line 434 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[0]; yyval->addChild(new AST(OP_RHS, new AST(id_node,yyvsp[0]->text))); }
    break;

  case 92:
#line 435 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("matching right parenthesis");}
    break;

  case 93:
#line 436 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("indexing expression");}
    break;

  case 94:
#line 437 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("equals operator after loop index");}
    break;

  case 95:
#line 438 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("identifier that is the loop variable");}
    break;

  case 96:
#line 439 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("indexing expression");}
    break;

  case 97:
#line 440 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("identifier or assignment (id = expr) after 'for' ");}
    break;

  case 98:
#line 444 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-4]; yyval->addChild(yyvsp[-3]); yyval->addChild(yyvsp[-1]);
	}
    break;

  case 99:
#line 447 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("test expression after 'while'");}
    break;

  case 100:
#line 451 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-4]; yyval->addChild(yyvsp[-3]); if (yyvsp[-2] != NULL) yyval->addChild(yyvsp[-2]); 
	  if (yyvsp[-1] != NULL) yyval->addChild(yyvsp[-1]);
	}
    break;

  case 101:
#line 455 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("condition expression for 'if'");}
    break;

  case 102:
#line 459 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = new AST(OP_CSTAT,yyvsp[-2],yyvsp[0]);
	}
    break;

  case 103:
#line 465 "../../../libs/libFreeMat/Parser.yy"
    {yyval = NULL;}
    break;

  case 105:
#line 470 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = new AST(OP_ELSEIFBLOCK,yyvsp[0]);
	}
    break;

  case 106:
#line 473 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);
	}
    break;

  case 107:
#line 479 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 108:
#line 482 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("test condition for 'elseif' clause");}
    break;

  case 109:
#line 485 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyval = yyvsp[0];
	}
    break;

  case 110:
#line 488 "../../../libs/libFreeMat/Parser.yy"
    {yyval = NULL;}
    break;

  case 111:
#line 489 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("statement list for 'else' clause");}
    break;

  case 112:
#line 493 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_ASSIGN,yyvsp[-2],yyvsp[0]);}
    break;

  case 113:
#line 494 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("expression in assignment");}
    break;

  case 114:
#line 498 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyvsp[-3]->addChild(new AST(OP_PARENS,yyvsp[-1]));
	  yyval = new AST(OP_MULTICALL,yyvsp[-6],yyvsp[-3]);
	}
    break;

  case 115:
#line 502 "../../../libs/libFreeMat/Parser.yy"
    {
	  yyvsp[0]->addChild(new AST(OP_PARENS,NULL));
	  yyval = new AST(OP_MULTICALL,yyvsp[-3],yyvsp[0]);
	}
    break;

  case 116:
#line 507 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("matching right parenthesis");}
    break;

  case 117:
#line 509 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("indexing list");}
    break;

  case 118:
#line 511 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("left parenthesis");}
    break;

  case 119:
#line 513 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("identifier");}
    break;

  case 120:
#line 517 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_COLON,yyvsp[-2],yyvsp[0]);}
    break;

  case 121:
#line 518 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after ':'");}
    break;

  case 123:
#line 520 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_PLUS,yyvsp[-2],yyvsp[0]);}
    break;

  case 124:
#line 521 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '+'");}
    break;

  case 125:
#line 522 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_SUBTRACT,yyvsp[-2],yyvsp[0]);}
    break;

  case 126:
#line 523 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '-'");}
    break;

  case 127:
#line 524 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_TIMES,yyvsp[-2],yyvsp[0]);}
    break;

  case 128:
#line 525 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '*'");}
    break;

  case 129:
#line 526 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_RDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 130:
#line 527 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '/'");}
    break;

  case 131:
#line 528 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_LDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 132:
#line 529 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '\\'");}
    break;

  case 133:
#line 530 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_OR,yyvsp[-2],yyvsp[0]);}
    break;

  case 134:
#line 531 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '|'");}
    break;

  case 135:
#line 532 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_AND,yyvsp[-2],yyvsp[0]);}
    break;

  case 136:
#line 533 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '&'");}
    break;

  case 137:
#line 534 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_LT,yyvsp[-2],yyvsp[0]);}
    break;

  case 138:
#line 535 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '<'");}
    break;

  case 139:
#line 536 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_LEQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 140:
#line 537 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '<='");}
    break;

  case 141:
#line 538 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_GT,yyvsp[-2],yyvsp[0]);}
    break;

  case 142:
#line 539 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '>'");}
    break;

  case 143:
#line 540 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_GEQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 144:
#line 541 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '>='");}
    break;

  case 145:
#line 542 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_EQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 146:
#line 543 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '=='");}
    break;

  case 147:
#line 544 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_NEQ,yyvsp[-2],yyvsp[0]);}
    break;

  case 148:
#line 545 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '~='");}
    break;

  case 149:
#line 546 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT_TIMES,yyvsp[-2],yyvsp[0]);}
    break;

  case 150:
#line 547 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '.*'");}
    break;

  case 151:
#line 548 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT_RDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 152:
#line 549 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after './'");}
    break;

  case 153:
#line 550 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT_LDIV,yyvsp[-2],yyvsp[0]);}
    break;

  case 154:
#line 551 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '.\\'");}
    break;

  case 155:
#line 552 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_NEG,yyvsp[0]);}
    break;

  case 156:
#line 553 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[0];}
    break;

  case 157:
#line 554 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_NOT,yyvsp[0]);}
    break;

  case 158:
#line 555 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after logical not");}
    break;

  case 159:
#line 556 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_POWER,yyvsp[-2],yyvsp[0]);}
    break;

  case 160:
#line 557 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '^'");}
    break;

  case 161:
#line 558 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT_POWER,yyvsp[-2],yyvsp[0]);}
    break;

  case 162:
#line 559 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after '.^'");}
    break;

  case 163:
#line 560 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_TRANSPOSE,yyvsp[-1]);}
    break;

  case 164:
#line 561 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT_TRANSPOSE,yyvsp[-1]);}
    break;

  case 165:
#line 562 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 166:
#line 563 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("a right parenthesis after expression");}
    break;

  case 167:
#line 564 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("an expression after left parenthesis");}
    break;

  case 171:
#line 571 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_RHS,yyvsp[0]);}
    break;

  case 172:
#line 572 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 173:
#line 573 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("a matrix definition followed by a right bracket");}
    break;

  case 174:
#line 574 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 175:
#line 575 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2];}
    break;

  case 176:
#line 576 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2];}
    break;

  case 177:
#line 577 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_EMPTY,NULL);}
    break;

  case 178:
#line 578 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 179:
#line 579 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1];}
    break;

  case 180:
#line 580 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2];}
    break;

  case 181:
#line 581 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2];}
    break;

  case 182:
#line 582 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_EMPTY_CELL,NULL);}
    break;

  case 183:
#line 583 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("a cell-array definition followed by a right brace");}
    break;

  case 185:
#line 588 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-1]; yyval->addChild(yyvsp[0]);}
    break;

  case 186:
#line 592 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_PARENS,yyvsp[-1]); }
    break;

  case 187:
#line 593 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("matching right parenthesis");}
    break;

  case 188:
#line 594 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_BRACES,yyvsp[-1]); }
    break;

  case 189:
#line 595 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("matching right brace");}
    break;

  case 190:
#line 596 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_DOT,yyvsp[0]); }
    break;

  case 192:
#line 601 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_ALL,NULL);}
    break;

  case 193:
#line 602 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_KEYWORD,yyvsp[-2],yyvsp[0]);}
    break;

  case 194:
#line 603 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("expecting expression after '=' in keyword assignment");}
    break;

  case 195:
#line 604 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_KEYWORD,yyvsp[0]);}
    break;

  case 196:
#line 605 "../../../libs/libFreeMat/Parser.yy"
    {yyxpt("expecting keyword identifier after '/' in keyword assignment");}
    break;

  case 198:
#line 610 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addPeer(yyvsp[0]);}
    break;

  case 199:
#line 614 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_BRACES,yyvsp[0]);}
    break;

  case 200:
#line 615 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 201:
#line 619 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_BRACKETS,yyvsp[0]);}
    break;

  case 202:
#line 620 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;

  case 207:
#line 632 "../../../libs/libFreeMat/Parser.yy"
    {yyval = new AST(OP_SEMICOLON,yyvsp[0]);}
    break;

  case 208:
#line 633 "../../../libs/libFreeMat/Parser.yy"
    {yyval = yyvsp[-2]; yyval->addChild(yyvsp[0]);}
    break;


    }

/* Line 999 of yacc.c.  */
#line 2925 "../../../libs/libFreeMat/Parser.cc"

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


#line 636 "../../../libs/libFreeMat/Parser.yy"


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

