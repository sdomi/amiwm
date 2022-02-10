/* original parser id follows */
/* yysccsid[] = "@(#)yaccpar	1.9 (Berkeley) 02/21/93" */
/* (use YYMAJOR/YYMINOR for ifdefs dependent on parser version) */

#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYPATCH 20200330

#define YYEMPTY        (-1)
#define yyclearin      (yychar = YYEMPTY)
#define yyerrok        (yyerrflag = 0)
#define YYRECOVERING() (yyerrflag != 0)
#define YYENOMEM       (-2)
#define YYEOF          0
#define YYPREFIX "yy"

#define YYPURE 0

#line 2 "gram.y"
#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "prefs.h"
#include "drawinfo.h"
#include "screen.h"
#include "icc.h"
#include "style.h"
extern void set_sys_palette(void);
extern void set_mwb_palette(void);
extern void set_schwartz_palette(void);
extern void set_custom_palette(char *fn);
extern void add_toolitem(char *, char *, char *, int);
extern Scrn *openscreen(char *, Window);
extern void create_module(Scrn *, char *, char *);
extern char *default_colors[NUMDRIPENS];
extern char *default_screenfont, *label_font_name;
extern Display *dpy;
#ifndef HAVE_ALLOCA
#define alloca malloc
#endif
#ifndef HAVE_STRDUP
char *strdup(s) char *s;
{
  char *ptr=(char *)malloc(strlen(s)+1);
  if(ptr) strcpy(ptr, s);
  return ptr;
}
#endif

static void append_to(char **x, char *y)
{
  if(*x==NULL)
    *x=y;
  else {
    char *t = malloc(strlen(y)+strlen(*x)+2);
    sprintf(t, "%s\n%s", *x, y);
    free(*x);
    free(y);
    *x=t;
  }
}

static int ti_level=0;
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#line 49 "gram.y"
typedef union
{
    int num;
    char *ptr;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
#line 80 "y.tab.c"

/* compatibility with bison */
#ifdef YYPARSE_PARAM
/* compatibility with FreeBSD */
# ifdef YYPARSE_PARAM_TYPE
#  define YYPARSE_DECL() yyparse(YYPARSE_PARAM_TYPE YYPARSE_PARAM)
# else
#  define YYPARSE_DECL() yyparse(void *YYPARSE_PARAM)
# endif
#else
# define YYPARSE_DECL() yyparse(void)
#endif

/* Parameters sent to lex. */
#ifdef YYLEX_PARAM
# define YYLEX_DECL() yylex(void *YYLEX_PARAM)
# define YYLEX yylex(YYLEX_PARAM)
#else
# define YYLEX_DECL() yylex(void)
# define YYLEX yylex()
#endif

#if !(defined(yylex) || defined(YYSTATE))
int YYLEX_DECL();
#endif

/* Parameters sent to yyerror. */
#ifndef YYERROR_DECL
#define YYERROR_DECL() yyerror(const char *s)
#endif
#ifndef YYERROR_CALL
#define YYERROR_CALL(msg) yyerror(msg)
#endif

extern int YYPARSE_DECL();

#define ERRORTOKEN 257
#define LEFTBRACE 258
#define RIGHTBRACE 259
#define YES 260
#define NO 261
#define RIGHT 262
#define BOTTOM 263
#define BOTH 264
#define NONE 265
#define MAGICWB 266
#define SYSTEM 267
#define SCHWARTZ 268
#define ALWAYS 269
#define AUTO 270
#define MANUAL 271
#define SEPARATOR 272
#define T_DETAILPEN 273
#define T_BLOCKPEN 274
#define T_TEXTPEN 275
#define T_SHINEPEN 276
#define T_SHADOWPEN 277
#define T_FILLPEN 278
#define T_FILLTEXTPEN 279
#define T_BACKGROUNDPEN 280
#define T_HIGHLIGHTTEXTPEN 281
#define T_BARDETAILPEN 282
#define T_BARBLOCKPEN 283
#define T_BARTRIMPEN 284
#define FASTQUIT 285
#define SIZEBORDER 286
#define DEFAULTICON 287
#define ICONDIR 288
#define ICONPALETTE 289
#define SCREENFONT 290
#define ICONFONT 291
#define TOOLITEM 292
#define FORCEMOVE 293
#define SCREEN 294
#define MODULE 295
#define MODULEPATH 296
#define INTERSCREENGAP 297
#define AUTORAISE 298
#define FOCUS 299
#define FOLLOWMOUSE 300
#define CLICKTOTYPE 301
#define SLOPPY 302
#define CUSTOMICONSONLY 303
#define TITLEBARCLOCK 304
#define TITLECLOCKFORMAT 305
#define OPAQUEMOVE 306
#define OPAQUERESIZE 307
#define SCREENMENU 308
#define STYLE 309
#define CLASS 310
#define TITLE 311
#define ICONTITLE 312
#define ICON 313
#define SHORTLABELICONS 314
#define STRING 315
#define NUMBER 316
#define YYERRCODE 256
typedef int YYINT;
static const YYINT yylhs[] = {                           -1,
    0,    7,    7,    8,    8,    8,    8,    8,    8,    8,
    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,
    8,    8,    8,    8,    8,    8,    8,    8,    8,    8,
    8,    8,    8,    8,    8,   10,   11,   11,    9,    9,
    9,   12,   13,   13,   14,   14,   14,   14,    6,    1,
    1,    2,    2,    2,    2,    2,    5,    5,    5,    3,
    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,
    3,    4,    4,    4,
};
static const YYINT yylen[] = {                            2,
    1,    2,    0,    1,    1,    3,    2,    2,    2,    2,
    2,    2,    2,    2,    2,    2,    2,    2,    2,    3,
    2,    3,    2,    2,    2,    2,    2,    2,    2,    2,
    2,    2,    3,    2,    3,    3,    2,    0,    3,    4,
    2,    2,    2,    0,    2,    2,    2,    2,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,
};
static const YYINT yydefred[] = {                         3,
    0,    0,    4,   60,   61,   62,   63,   64,   65,   66,
   67,   68,   69,   70,   71,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    2,
    5,   38,   44,   50,   51,    7,   56,   52,   53,   54,
   55,    8,   49,    9,   10,   12,   11,   13,   14,   16,
   17,   41,    0,   72,   73,   74,   18,    0,   19,    0,
   21,   24,   25,   57,   59,   58,   28,   29,   31,    0,
   32,   26,   27,   34,   42,   30,   15,    0,    0,   36,
    0,   20,   22,   33,    6,    0,   37,   35,    0,    0,
    0,    0,   43,   40,    0,   45,   46,   47,   48,
};
static const YYINT yydgoto[] = {                          1,
   46,   52,   39,   67,   77,   91,    2,   40,   41,   42,
   88,   43,   89,  103,
};
static const YYINT yysindex[] = {                         0,
    0, -186,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0, -220, -232, -303, -303, -252,
 -303, -303, -268, -235, -273, -303, -303, -298, -220, -263,
 -220, -220, -267, -220, -220, -220, -238, -220, -303,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0, -258,    0,    0,    0,    0, -303,    0, -288,
    0,    0,    0,    0,    0,    0,    0,    0,    0, -303,
    0,    0,    0,    0,    0,    0,    0, -246, -251,    0,
 -303,    0,    0,    0,    0, -268,    0,    0, -303, -303,
 -303, -303,    0,    0, -303,    0,    0,    0,    0,
};
static const YYINT yyrindex[] = {                         0,
    0,   44,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   45,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    1,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,
};
static const YYINT yygindex[] = {                         0,
  -10,    0,    0,    0,    0,  -16,    0,    0,  -38,    0,
    0,    0,    0,    0,
};
#define YYTABLESIZE 359
static const YYINT yytable[] = {                         90,
   39,   54,   55,   62,   60,   61,   63,   98,   69,   70,
   71,   53,   95,   56,   57,   58,   81,   72,   73,   85,
   78,   79,   87,   82,   83,   84,   93,   86,   47,   48,
   49,   50,   51,   64,   65,   66,   74,   75,   76,   44,
   45,   53,   68,    1,   23,   96,   53,   53,   80,   97,
    0,   92,    0,    0,    0,    0,   53,    0,   99,  100,
  101,  102,   59,   94,    0,    0,    0,    0,    0,    3,
    0,    0,    0,    0,  104,    0,    0,    0,    0,  105,
    0,    0,  106,  107,  108,  109,    4,    5,    6,    7,
    8,    9,   10,   11,   12,   13,   14,   15,   16,   17,
   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,
   28,   29,   30,    0,    0,    0,   31,   32,   33,   34,
   35,   36,   37,    0,    0,    0,    0,   38,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   39,    0,    0,   39,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   39,   39,   39,   39,   39,   39,   39,
   39,   39,   39,   39,   39,   39,   39,   39,   39,   39,
   39,   39,   39,   39,   39,   39,   39,   39,   39,   39,
   23,    0,    0,   39,   39,   39,   39,   39,   39,   39,
    0,    0,    0,    0,   39,    0,    0,   23,   23,   23,
   23,   23,   23,   23,   23,   23,   23,   23,   23,   23,
   23,   23,   23,   23,   23,   23,   23,   23,   23,   23,
   23,   23,   23,   23,    0,    0,    0,   23,   23,   23,
   23,   23,   23,   23,    0,    0,    0,    0,   23,
};
static const YYINT yycheck[] = {                        258,
    0,   18,   19,  272,   21,   22,   23,  259,   25,   26,
   27,  315,  259,  266,  267,  268,   33,  316,   29,  258,
   31,   32,   39,   34,   35,   36,  315,   38,  261,  262,
  263,  264,  265,  269,  270,  271,  300,  301,  302,  260,
  261,  315,  316,    0,    0,  292,  315,  315,  316,   88,
   -1,   68,   -1,   -1,   -1,   -1,  315,   -1,  310,  311,
  312,  313,  315,   80,   -1,   -1,   -1,   -1,   -1,  256,
   -1,   -1,   -1,   -1,   91,   -1,   -1,   -1,   -1,   96,
   -1,   -1,   99,  100,  101,  102,  273,  274,  275,  276,
  277,  278,  279,  280,  281,  282,  283,  284,  285,  286,
  287,  288,  289,  290,  291,  292,  293,  294,  295,  296,
  297,  298,  299,   -1,   -1,   -1,  303,  304,  305,  306,
  307,  308,  309,   -1,   -1,   -1,   -1,  314,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  256,   -1,   -1,  259,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  273,  274,  275,  276,  277,  278,  279,
  280,  281,  282,  283,  284,  285,  286,  287,  288,  289,
  290,  291,  292,  293,  294,  295,  296,  297,  298,  299,
  256,   -1,   -1,  303,  304,  305,  306,  307,  308,  309,
   -1,   -1,   -1,   -1,  314,   -1,   -1,  273,  274,  275,
  276,  277,  278,  279,  280,  281,  282,  283,  284,  285,
  286,  287,  288,  289,  290,  291,  292,  293,  294,  295,
  296,  297,  298,  299,   -1,   -1,   -1,  303,  304,  305,
  306,  307,  308,  309,   -1,   -1,   -1,   -1,  314,
};
#define YYFINAL 1
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 316
#define YYUNDFTOKEN 333
#define YYTRANSLATE(a) ((a) > YYMAXTOKEN ? YYUNDFTOKEN : (a))
#if YYDEBUG
static const char *const yyname[] = {

"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"ERRORTOKEN","LEFTBRACE",
"RIGHTBRACE","YES","NO","RIGHT","BOTTOM","BOTH","NONE","MAGICWB","SYSTEM",
"SCHWARTZ","ALWAYS","AUTO","MANUAL","SEPARATOR","T_DETAILPEN","T_BLOCKPEN",
"T_TEXTPEN","T_SHINEPEN","T_SHADOWPEN","T_FILLPEN","T_FILLTEXTPEN",
"T_BACKGROUNDPEN","T_HIGHLIGHTTEXTPEN","T_BARDETAILPEN","T_BARBLOCKPEN",
"T_BARTRIMPEN","FASTQUIT","SIZEBORDER","DEFAULTICON","ICONDIR","ICONPALETTE",
"SCREENFONT","ICONFONT","TOOLITEM","FORCEMOVE","SCREEN","MODULE","MODULEPATH",
"INTERSCREENGAP","AUTORAISE","FOCUS","FOLLOWMOUSE","CLICKTOTYPE","SLOPPY",
"CUSTOMICONSONLY","TITLEBARCLOCK","TITLECLOCKFORMAT","OPAQUEMOVE",
"OPAQUERESIZE","SCREENMENU","STYLE","CLASS","TITLE","ICONTITLE","ICON",
"SHORTLABELICONS","STRING","NUMBER",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"illegal-symbol",
};
static const char *const yyrule[] = {
"$accept : amiwmrc",
"amiwmrc : stmts",
"stmts : stmts stmt",
"stmts :",
"stmt : error",
"stmt : toolitem",
"stmt : toolsubmenu toolitems RIGHTBRACE",
"stmt : FASTQUIT truth",
"stmt : SIZEBORDER sizeborder",
"stmt : DEFAULTICON string",
"stmt : ICONDIR string",
"stmt : ICONPALETTE SYSTEM",
"stmt : ICONPALETTE MAGICWB",
"stmt : ICONPALETTE SCHWARTZ",
"stmt : ICONPALETTE STRING",
"stmt : dri_pen string",
"stmt : SCREENFONT string",
"stmt : ICONFONT string",
"stmt : FORCEMOVE forcemove_policy",
"stmt : SCREEN string",
"stmt : SCREEN NUMBER string",
"stmt : MODULEPATH string",
"stmt : MODULE string STRING",
"stmt : MODULE string",
"stmt : INTERSCREENGAP NUMBER",
"stmt : AUTORAISE truth",
"stmt : OPAQUEMOVE truth",
"stmt : OPAQUERESIZE truth",
"stmt : FOCUS focuspolicy",
"stmt : CUSTOMICONSONLY truth",
"stmt : SHORTLABELICONS truth",
"stmt : TITLEBARCLOCK truth",
"stmt : TITLECLOCKFORMAT string",
"stmt : TITLECLOCKFORMAT NUMBER string",
"stmt : SCREENMENU truth",
"stmt : stylespec styleitems RIGHTBRACE",
"toolsubmenu : TOOLITEM string LEFTBRACE",
"toolitems : toolitems toolitem",
"toolitems :",
"toolitem : TOOLITEM string string",
"toolitem : TOOLITEM string string string",
"toolitem : TOOLITEM SEPARATOR",
"stylespec : STYLE LEFTBRACE",
"styleitems : styleitems styleitem",
"styleitems :",
"styleitem : CLASS string",
"styleitem : TITLE string",
"styleitem : ICONTITLE string",
"styleitem : ICON string",
"string : STRING",
"truth : YES",
"truth : NO",
"sizeborder : RIGHT",
"sizeborder : BOTTOM",
"sizeborder : BOTH",
"sizeborder : NONE",
"sizeborder : NO",
"focuspolicy : FOLLOWMOUSE",
"focuspolicy : SLOPPY",
"focuspolicy : CLICKTOTYPE",
"dri_pen : T_DETAILPEN",
"dri_pen : T_BLOCKPEN",
"dri_pen : T_TEXTPEN",
"dri_pen : T_SHINEPEN",
"dri_pen : T_SHADOWPEN",
"dri_pen : T_FILLPEN",
"dri_pen : T_FILLTEXTPEN",
"dri_pen : T_BACKGROUNDPEN",
"dri_pen : T_HIGHLIGHTTEXTPEN",
"dri_pen : T_BARDETAILPEN",
"dri_pen : T_BARBLOCKPEN",
"dri_pen : T_BARTRIMPEN",
"forcemove_policy : ALWAYS",
"forcemove_policy : AUTO",
"forcemove_policy : MANUAL",

};
#endif

#if YYDEBUG
int      yydebug;
#endif

int      yyerrflag;
int      yychar;
YYSTYPE  yyval;
YYSTYPE  yylval;
int      yynerrs;

/* define the initial stack-sizes */
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH  YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH  10000
#endif
#endif

#define YYINITSTACKSIZE 200

typedef struct {
    unsigned stacksize;
    YYINT    *s_base;
    YYINT    *s_mark;
    YYINT    *s_last;
    YYSTYPE  *l_base;
    YYSTYPE  *l_mark;
} YYSTACKDATA;
/* variables for the parser stack */
static YYSTACKDATA yystack;
#line 198 "gram.y"
extern char *progname;
extern int ParseError;
int yyerror(s) char *s;
{
    fprintf (stderr, "%s: error in input file:  %s\n", progname, s ? s : "");
    ParseError = 1;
    return 0;
}
#line 476 "y.tab.c"

#if YYDEBUG
#include <stdio.h>	/* needed for printf */
#endif

#include <stdlib.h>	/* needed for malloc, etc */
#include <string.h>	/* needed for memset */

/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack(YYSTACKDATA *data)
{
    int i;
    unsigned newsize;
    YYINT *newss;
    YYSTYPE *newvs;

    if ((newsize = data->stacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return YYENOMEM;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;

    i = (int) (data->s_mark - data->s_base);
    newss = (YYINT *)realloc(data->s_base, newsize * sizeof(*newss));
    if (newss == NULL)
        return YYENOMEM;

    data->s_base = newss;
    data->s_mark = newss + i;

    newvs = (YYSTYPE *)realloc(data->l_base, newsize * sizeof(*newvs));
    if (newvs == NULL)
        return YYENOMEM;

    data->l_base = newvs;
    data->l_mark = newvs + i;

    data->stacksize = newsize;
    data->s_last = data->s_base + newsize - 1;
    return 0;
}

#if YYPURE || defined(YY_NO_LEAKS)
static void yyfreestack(YYSTACKDATA *data)
{
    free(data->s_base);
    free(data->l_base);
    memset(data, 0, sizeof(*data));
}
#else
#define yyfreestack(data) /* nothing */
#endif

#define YYABORT  goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR  goto yyerrlab

int
YYPARSE_DECL()
{
    int yym, yyn, yystate;
#if YYDEBUG
    const char *yys;

    if ((yys = getenv("YYDEBUG")) != NULL)
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yym = 0;
    yyn = 0;
    yynerrs = 0;
    yyerrflag = 0;
    yychar = YYEMPTY;
    yystate = 0;

#if YYPURE
    memset(&yystack, 0, sizeof(yystack));
#endif

    if (yystack.s_base == NULL && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    yystack.s_mark = yystack.s_base;
    yystack.l_mark = yystack.l_base;
    yystate = 0;
    *yystack.s_mark = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        yychar = YYLEX;
        if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if (((yyn = yysindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
        yystate = yytable[yyn];
        *++yystack.s_mark = yytable[yyn];
        *++yystack.l_mark = yylval;
        yychar = YYEMPTY;
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if (((yyn = yyrindex[yystate]) != 0) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag != 0) goto yyinrecovery;

    YYERROR_CALL("syntax error");

    goto yyerrlab; /* redundant goto avoids 'unused label' warning */
yyerrlab:
    ++yynerrs;

yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if (((yyn = yysindex[*yystack.s_mark]) != 0) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yystack.s_mark, yytable[yyn]);
#endif
                if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
                yystate = yytable[yyn];
                *++yystack.s_mark = yytable[yyn];
                *++yystack.l_mark = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yystack.s_mark);
#endif
                if (yystack.s_mark <= yystack.s_base) goto yyabort;
                --yystack.s_mark;
                --yystack.l_mark;
            }
        }
    }
    else
    {
        if (yychar == YYEOF) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = YYEMPTY;
        goto yyloop;
    }

yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    if (yym > 0)
        yyval = yystack.l_mark[1-yym];
    else
        memset(&yyval, 0, sizeof yyval);

    switch (yyn)
    {
case 6:
#line 90 "gram.y"
	{ ti_level=0; }
break;
case 7:
#line 91 "gram.y"
	{ prefs.fastquit = yystack.l_mark[0].num; }
break;
case 8:
#line 92 "gram.y"
	{ prefs.sizeborder = yystack.l_mark[0].num; }
break;
case 9:
#line 93 "gram.y"
	{ prefs.defaulticon = yystack.l_mark[0].ptr; }
break;
case 10:
#line 94 "gram.y"
	{ prefs.icondir = yystack.l_mark[0].ptr; }
break;
case 11:
#line 95 "gram.y"
	{ set_sys_palette(); }
break;
case 12:
#line 96 "gram.y"
	{ set_mwb_palette(); }
break;
case 13:
#line 97 "gram.y"
	{ set_schwartz_palette(); }
break;
case 14:
#line 98 "gram.y"
	{ set_custom_palette(yystack.l_mark[0].ptr); }
break;
case 15:
#line 99 "gram.y"
	{ default_colors[yystack.l_mark[-1].num] = yystack.l_mark[0].ptr; }
break;
case 16:
#line 100 "gram.y"
	{ default_screenfont = yystack.l_mark[0].ptr; }
break;
case 17:
#line 101 "gram.y"
	{ label_font_name = yystack.l_mark[0].ptr; }
break;
case 18:
#line 102 "gram.y"
	{ prefs.forcemove = yystack.l_mark[0].num; }
break;
case 19:
#line 103 "gram.y"
	{ openscreen(yystack.l_mark[0].ptr,DefaultRootWindow(dpy)); }
break;
case 20:
#line 104 "gram.y"
	{ if((yystack.l_mark[-1].num==DefaultScreen(dpy)||prefs.manage_all) && yystack.l_mark[-1].num<ScreenCount(dpy)) openscreen(yystack.l_mark[0].ptr,RootWindow(dpy,yystack.l_mark[-1].num)); }
break;
case 21:
#line 105 "gram.y"
	{ prefs.module_path = yystack.l_mark[0].ptr; }
break;
case 22:
#line 106 "gram.y"
	{ create_module((front? front->upfront:NULL), yystack.l_mark[-1].ptr, yystack.l_mark[0].ptr); }
break;
case 23:
#line 107 "gram.y"
	{ create_module((front? front->upfront:NULL), yystack.l_mark[0].ptr, NULL); }
break;
case 24:
#line 108 "gram.y"
	{ prefs.borderwidth=yystack.l_mark[0].num; }
break;
case 25:
#line 109 "gram.y"
	{ prefs.autoraise=yystack.l_mark[0].num; }
break;
case 26:
#line 110 "gram.y"
	{ prefs.opaquemove=yystack.l_mark[0].num; }
break;
case 27:
#line 111 "gram.y"
	{ prefs.opaqueresize=yystack.l_mark[0].num; }
break;
case 28:
#line 112 "gram.y"
	{ prefs.focus=yystack.l_mark[0].num; }
break;
case 29:
#line 113 "gram.y"
	{ prefs.customiconsonly = yystack.l_mark[0].num; }
break;
case 30:
#line 114 "gram.y"
	{ prefs.shortlabelicons = yystack.l_mark[0].num; }
break;
case 31:
#line 115 "gram.y"
	{ prefs.titlebarclock = yystack.l_mark[0].num; }
break;
case 32:
#line 116 "gram.y"
	{ prefs.titleclockformat = yystack.l_mark[0].ptr; }
break;
case 33:
#line 117 "gram.y"
	{ 
					prefs.titleclockinterval=yystack.l_mark[-1].num; 
					prefs.titleclockformat=yystack.l_mark[0].ptr; }
break;
case 34:
#line 120 "gram.y"
	{ prefs.screenmenu=yystack.l_mark[0].num; }
break;
case 36:
#line 124 "gram.y"
	{ add_toolitem(yystack.l_mark[-1].ptr, NULL, NULL, -1); ti_level=1; }
break;
case 39:
#line 131 "gram.y"
	{ add_toolitem(yystack.l_mark[-1].ptr, yystack.l_mark[0].ptr, NULL, ti_level); }
break;
case 40:
#line 132 "gram.y"
	{ add_toolitem(yystack.l_mark[-2].ptr, yystack.l_mark[-1].ptr, yystack.l_mark[0].ptr, ti_level); }
break;
case 41:
#line 133 "gram.y"
	{ add_toolitem(NULL, NULL, NULL, ti_level); }
break;
case 42:
#line 136 "gram.y"
	{ Style *s = malloc(sizeof(Style));
				    memset(s, 0, sizeof(*s));
				    s->next=NULL; s->style_class=NULL;
				    s->style_title=s->style_icon_title=NULL;
				    s->icon_name=NULL;
				    s->icon_pms.cs.colors=NULL;
				    s->icon_pms.cs2.colors=NULL;
				    if(prefs.firststyle)
				      prefs.laststyle->next=s;
				    else
				      prefs.firststyle=s;
				    prefs.laststyle=s; }
break;
case 45:
#line 153 "gram.y"
	{ append_to(&prefs.laststyle->style_class, yystack.l_mark[0].ptr);}
break;
case 46:
#line 154 "gram.y"
	{ append_to(&prefs.laststyle->style_title, yystack.l_mark[0].ptr);}
break;
case 47:
#line 155 "gram.y"
	{ append_to(&prefs.laststyle->style_icon_title, yystack.l_mark[0].ptr);}
break;
case 48:
#line 156 "gram.y"
	{ prefs.laststyle->icon_name=yystack.l_mark[0].ptr; }
break;
case 49:
#line 159 "gram.y"
	{ yyval.ptr = strdup(yystack.l_mark[0].ptr); }
break;
case 50:
#line 162 "gram.y"
	{ yyval.num = True; }
break;
case 51:
#line 163 "gram.y"
	{ yyval.num = False; }
break;
case 52:
#line 166 "gram.y"
	{ yyval.num = Psizeright; }
break;
case 53:
#line 167 "gram.y"
	{ yyval.num = Psizebottom; }
break;
case 54:
#line 168 "gram.y"
	{ yyval.num = Psizeright|Psizebottom; }
break;
case 55:
#line 169 "gram.y"
	{ yyval.num = Psizetrans; }
break;
case 56:
#line 170 "gram.y"
	{ yyval.num = Psizetrans; }
break;
case 57:
#line 173 "gram.y"
	{ yyval.num = FOC_FOLLOWMOUSE; }
break;
case 58:
#line 174 "gram.y"
	{ yyval.num = FOC_SLOPPY; }
break;
case 59:
#line 175 "gram.y"
	{ yyval.num = FOC_CLICKTOTYPE; }
break;
case 60:
#line 178 "gram.y"
	{ yyval.num = DETAILPEN; }
break;
case 61:
#line 179 "gram.y"
	{ yyval.num = BLOCKPEN; }
break;
case 62:
#line 180 "gram.y"
	{ yyval.num = TEXTPEN; }
break;
case 63:
#line 181 "gram.y"
	{ yyval.num = SHINEPEN; }
break;
case 64:
#line 182 "gram.y"
	{ yyval.num = SHADOWPEN; }
break;
case 65:
#line 183 "gram.y"
	{ yyval.num = FILLPEN; }
break;
case 66:
#line 184 "gram.y"
	{ yyval.num = FILLTEXTPEN; }
break;
case 67:
#line 185 "gram.y"
	{ yyval.num = BACKGROUNDPEN; }
break;
case 68:
#line 186 "gram.y"
	{ yyval.num = HIGHLIGHTTEXTPEN; }
break;
case 69:
#line 187 "gram.y"
	{ yyval.num = BARDETAILPEN; }
break;
case 70:
#line 188 "gram.y"
	{ yyval.num = BARBLOCKPEN; }
break;
case 71:
#line 189 "gram.y"
	{ yyval.num = BARTRIMPEN; }
break;
case 72:
#line 192 "gram.y"
	{ yyval.num = FM_ALWAYS; }
break;
case 73:
#line 193 "gram.y"
	{ yyval.num = FM_AUTO; }
break;
case 74:
#line 194 "gram.y"
	{ yyval.num = FM_MANUAL; }
break;
#line 944 "y.tab.c"
    }
    yystack.s_mark -= yym;
    yystate = *yystack.s_mark;
    yystack.l_mark -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yystack.s_mark = YYFINAL;
        *++yystack.l_mark = yyval;
        if (yychar < 0)
        {
            yychar = YYLEX;
            if (yychar < 0) yychar = YYEOF;
#if YYDEBUG
            if (yydebug)
            {
                if ((yys = yyname[YYTRANSLATE(yychar)]) == NULL) yys = yyname[YYUNDFTOKEN];
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == YYEOF) goto yyaccept;
        goto yyloop;
    }
    if (((yyn = yygindex[yym]) != 0) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == (YYINT) yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yystack.s_mark, yystate);
#endif
    if (yystack.s_mark >= yystack.s_last && yygrowstack(&yystack) == YYENOMEM) goto yyoverflow;
    *++yystack.s_mark = (YYINT) yystate;
    *++yystack.l_mark = yyval;
    goto yyloop;

yyoverflow:
    YYERROR_CALL("yacc stack overflow");

yyabort:
    yyfreestack(&yystack);
    return (1);

yyaccept:
    yyfreestack(&yystack);
    return (0);
}
