#ifndef BISON__________LIBS_LIBFREEMAT_PARSER_H
# define BISON__________LIBS_LIBFREEMAT_PARSER_H

# ifndef YYSTYPE
#  define YYSTYPE int
#  define YYSTYPE_IS_TRIVIAL 1
# endif
# define	IDENT	257
# define	NUMERIC	258
# define	ENDQSTMNT	259
# define	ENDSTMNT	260
# define	LE	261
# define	GE	262
# define	EQ	263
# define	DOTTIMES	264
# define	DOTRDIV	265
# define	DOTLDIV	266
# define	DOTPOWER	267
# define	DOTTRANSPOSE	268
# define	STRING	269
# define	SPECIALCALL	270
# define	END	271
# define	IF	272
# define	FUNCTION	273
# define	FOR	274
# define	BREAK	275
# define	WHILE	276
# define	ELSE	277
# define	ELSEIF	278
# define	SWITCH	279
# define	CASE	280
# define	OTHERWISE	281
# define	CONTINUE	282
# define	TRY	283
# define	CATCH	284
# define	FIELD	285
# define	REFLPAREN	286
# define	REFRPAREN	287
# define	REFLBRACE	288
# define	REFRBRACE	289
# define	ENDFOR	290
# define	ENDSWITCH	291
# define	ENDWHILE	292
# define	ENDTRY	293
# define	ENDIF	294
# define	PERSISTENT	295
# define	KEYBOARD	296
# define	RETURN	297
# define	VARARGIN	298
# define	VARARGOUT	299
# define	GLOBAL	300
# define	QUIT	301
# define	RETALL	302
# define	NE	303
# define	POS	304
# define	NEG	305
# define	NOT	306


extern YYSTYPE yylval;

#endif /* not BISON__________LIBS_LIBFREEMAT_PARSER_H */
