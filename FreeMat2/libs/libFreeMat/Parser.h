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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



