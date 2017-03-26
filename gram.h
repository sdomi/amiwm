/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ERRORTOKEN = 258,
    LEFTBRACE = 259,
    RIGHTBRACE = 260,
    YES = 261,
    NO = 262,
    RIGHT = 263,
    BOTTOM = 264,
    BOTH = 265,
    NONE = 266,
    MAGICWB = 267,
    SYSTEM = 268,
    SCHWARTZ = 269,
    ALWAYS = 270,
    AUTO = 271,
    MANUAL = 272,
    SEPARATOR = 273,
    T_DETAILPEN = 274,
    T_BLOCKPEN = 275,
    T_TEXTPEN = 276,
    T_SHINEPEN = 277,
    T_SHADOWPEN = 278,
    T_FILLPEN = 279,
    T_FILLTEXTPEN = 280,
    T_BACKGROUNDPEN = 281,
    T_HIGHLIGHTTEXTPEN = 282,
    T_BARDETAILPEN = 283,
    T_BARBLOCKPEN = 284,
    T_BARTRIMPEN = 285,
    FASTQUIT = 286,
    SIZEBORDER = 287,
    DEFAULTICON = 288,
    ICONDIR = 289,
    ICONPALETTE = 290,
    SCREENFONT = 291,
    ICONFONT = 292,
    TOOLITEM = 293,
    FORCEMOVE = 294,
    SCREEN = 295,
    MODULE = 296,
    MODULEPATH = 297,
    INTERSCREENGAP = 298,
    AUTORAISE = 299,
    FOCUS = 300,
    FOLLOWMOUSE = 301,
    CLICKTOTYPE = 302,
    SLOPPY = 303,
    CUSTOMICONSONLY = 304,
    TITLEBARCLOCK = 305,
    TITLECLOCKFORMAT = 306,
    OPAQUEMOVE = 307,
    OPAQUERESIZE = 308,
    SCREENMENU = 309,
    STYLE = 310,
    CLASS = 311,
    TITLE = 312,
    ICONTITLE = 313,
    ICON = 314,
    SHORTLABELICONS = 315,
    STRING = 316,
    NUMBER = 317
  };
#endif
/* Tokens.  */
#define ERRORTOKEN 258
#define LEFTBRACE 259
#define RIGHTBRACE 260
#define YES 261
#define NO 262
#define RIGHT 263
#define BOTTOM 264
#define BOTH 265
#define NONE 266
#define MAGICWB 267
#define SYSTEM 268
#define SCHWARTZ 269
#define ALWAYS 270
#define AUTO 271
#define MANUAL 272
#define SEPARATOR 273
#define T_DETAILPEN 274
#define T_BLOCKPEN 275
#define T_TEXTPEN 276
#define T_SHINEPEN 277
#define T_SHADOWPEN 278
#define T_FILLPEN 279
#define T_FILLTEXTPEN 280
#define T_BACKGROUNDPEN 281
#define T_HIGHLIGHTTEXTPEN 282
#define T_BARDETAILPEN 283
#define T_BARBLOCKPEN 284
#define T_BARTRIMPEN 285
#define FASTQUIT 286
#define SIZEBORDER 287
#define DEFAULTICON 288
#define ICONDIR 289
#define ICONPALETTE 290
#define SCREENFONT 291
#define ICONFONT 292
#define TOOLITEM 293
#define FORCEMOVE 294
#define SCREEN 295
#define MODULE 296
#define MODULEPATH 297
#define INTERSCREENGAP 298
#define AUTORAISE 299
#define FOCUS 300
#define FOLLOWMOUSE 301
#define CLICKTOTYPE 302
#define SLOPPY 303
#define CUSTOMICONSONLY 304
#define TITLEBARCLOCK 305
#define TITLECLOCKFORMAT 306
#define OPAQUEMOVE 307
#define OPAQUERESIZE 308
#define SCREENMENU 309
#define STYLE 310
#define CLASS 311
#define TITLE 312
#define ICONTITLE 313
#define ICON 314
#define SHORTLABELICONS 315
#define STRING 316
#define NUMBER 317

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 50 "gram.y" /* yacc.c:1909  */

    int num;
    char *ptr;

#line 183 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
