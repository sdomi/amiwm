/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 1 "gram.y" /* yacc.c:339  */

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

#line 114 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
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
#line 50 "gram.y" /* yacc.c:355  */

    int num;
    char *ptr;

#line 283 "y.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 300 "y.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   126

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  63
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  16
/* YYNRULES -- Number of rules.  */
#define YYNRULES  75
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  111

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   317

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    81,    81,    84,    85,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   120,   121,   124,   127,   128,
     131,   132,   133,   136,   149,   150,   153,   154,   155,   156,
     159,   162,   163,   166,   167,   168,   169,   170,   173,   174,
     175,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   192,   193,   194
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ERRORTOKEN", "LEFTBRACE", "RIGHTBRACE",
  "YES", "NO", "RIGHT", "BOTTOM", "BOTH", "NONE", "MAGICWB", "SYSTEM",
  "SCHWARTZ", "ALWAYS", "AUTO", "MANUAL", "SEPARATOR", "T_DETAILPEN",
  "T_BLOCKPEN", "T_TEXTPEN", "T_SHINEPEN", "T_SHADOWPEN", "T_FILLPEN",
  "T_FILLTEXTPEN", "T_BACKGROUNDPEN", "T_HIGHLIGHTTEXTPEN",
  "T_BARDETAILPEN", "T_BARBLOCKPEN", "T_BARTRIMPEN", "FASTQUIT",
  "SIZEBORDER", "DEFAULTICON", "ICONDIR", "ICONPALETTE", "SCREENFONT",
  "ICONFONT", "TOOLITEM", "FORCEMOVE", "SCREEN", "MODULE", "MODULEPATH",
  "INTERSCREENGAP", "AUTORAISE", "FOCUS", "FOLLOWMOUSE", "CLICKTOTYPE",
  "SLOPPY", "CUSTOMICONSONLY", "TITLEBARCLOCK", "TITLECLOCKFORMAT",
  "OPAQUEMOVE", "OPAQUERESIZE", "SCREENMENU", "STYLE", "CLASS", "TITLE",
  "ICONTITLE", "ICON", "SHORTLABELICONS", "STRING", "NUMBER", "$accept",
  "amiwmrc", "stmts", "stmt", "toolsubmenu", "toolitems", "toolitem",
  "stylespec", "styleitems", "styleitem", "string", "truth", "sizeborder",
  "focuspolicy", "dri_pen", "forcemove_policy", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317
};
# endif

#define YYPACT_NINF -46

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-46)))

#define YYTABLE_NINF -3

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -46,    14,    66,   -46,   -46,   -46,   -46,   -46,   -46,   -46,
     -46,   -46,   -46,   -46,   -46,   -46,   -46,    33,    24,   -45,
     -45,    -1,   -45,   -45,   -12,     3,   -20,   -45,   -45,   -40,
      33,   -10,    33,    33,   -17,    33,    33,    33,    25,    33,
     -46,   -46,   -46,   -46,   -45,   -46,   -46,   -46,   -46,   -46,
     -46,   -46,   -46,   -46,   -46,   -46,   -46,   -46,   -46,   -46,
     -46,   -46,   -46,   -46,    -2,   -46,   -46,   -46,   -46,   -45,
     -46,   -15,   -46,   -46,   -46,   -46,   -46,   -46,   -46,   -46,
     -46,   -45,   -46,   -46,   -46,   -46,   -46,   -46,     5,    12,
     -46,   -46,   -45,   -46,   -46,   -46,   -46,   -12,   -46,   -46,
     -45,   -45,   -45,   -45,   -46,   -46,   -45,   -46,   -46,   -46,
     -46
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     0,     1,     5,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       3,    39,     6,    45,     0,    51,    52,     8,    57,    53,
      54,    55,    56,     9,    50,    10,    11,    13,    12,    14,
      15,    17,    18,    42,     0,    73,    74,    75,    19,     0,
      20,    24,    22,    25,    26,    58,    60,    59,    29,    30,
      32,     0,    33,    27,    28,    35,    43,    31,     0,     0,
      16,    37,    40,    21,    23,    34,     7,     0,    38,    36,
       0,     0,     0,     0,    44,    41,     0,    46,    47,    48,
      49
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -46,   -46,   -46,   -46,   -46,   -46,   -41,   -46,   -46,   -46,
     -19,    -9,   -46,   -46,   -46,   -46
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    40,    41,    88,    42,    43,    89,   104,
      92,    47,    53,    78,    44,    68
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      55,    56,    91,    61,    62,    64,    63,    70,    71,    72,
      96,    57,    58,    59,     3,    82,    54,    99,    65,    66,
      67,    74,    73,    79,    80,    90,    83,    84,    85,    86,
      87,    48,    49,    50,    51,    52,    75,    76,    77,    45,
      46,    54,    69,    97,    54,    81,    94,    98,     0,    54,
      93,     0,     0,     0,     0,     0,     0,     0,     0,    54,
      60,     0,    95,     0,     0,     0,    -2,     4,   100,   101,
     102,   103,     0,   105,     0,     0,     0,     0,   106,     0,
       0,   107,   108,   109,   110,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,     0,     0,     0,    32,    33,    34,    35,    36,
      37,    38,     0,     0,     0,     0,    39
};

static const yytype_int8 yycheck[] =
{
      19,    20,     4,    22,    23,    24,    18,    26,    27,    28,
       5,    12,    13,    14,     0,    34,    61,     5,    15,    16,
      17,    30,    62,    32,    33,    44,    35,    36,    37,     4,
      39,     7,     8,     9,    10,    11,    46,    47,    48,     6,
       7,    61,    62,    38,    61,    62,    61,    88,    -1,    61,
      69,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      61,    -1,    81,    -1,    -1,    -1,     0,     1,    56,    57,
      58,    59,    -1,    92,    -1,    -1,    -1,    -1,    97,    -1,
      -1,   100,   101,   102,   103,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      54,    55,    -1,    -1,    -1,    -1,    60
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    64,    65,     0,     1,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    49,    50,    51,    52,    53,    54,    55,    60,
      66,    67,    69,    70,    77,     6,     7,    74,     7,     8,
       9,    10,    11,    75,    61,    73,    73,    12,    13,    14,
      61,    73,    73,    18,    73,    15,    16,    17,    78,    62,
      73,    73,    73,    62,    74,    46,    47,    48,    76,    74,
      74,    62,    73,    74,    74,    74,     4,    74,    68,    71,
      73,     4,    73,    73,    61,    73,     5,    38,    69,     5,
      56,    57,    58,    59,    72,    73,    73,    73,    73,    73,
      73
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    63,    64,    65,    65,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    67,    68,    68,
      69,    69,    69,    70,    71,    71,    72,    72,    72,    72,
      73,    74,    74,    75,    75,    75,    75,    75,    76,    76,
      76,    77,    77,    77,    77,    77,    77,    77,    77,    77,
      77,    77,    77,    78,    78,    78
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     1,     1,     3,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     2,     3,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     2,     3,     3,     2,     0,
       3,     4,     2,     2,     2,     0,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 7:
#line 90 "gram.y" /* yacc.c:1646  */
    { ti_level=0; }
#line 1470 "y.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 91 "gram.y" /* yacc.c:1646  */
    { prefs.fastquit = (yyvsp[0].num); }
#line 1476 "y.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 92 "gram.y" /* yacc.c:1646  */
    { prefs.sizeborder = (yyvsp[0].num); }
#line 1482 "y.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 93 "gram.y" /* yacc.c:1646  */
    { prefs.defaulticon = (yyvsp[0].ptr); }
#line 1488 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 94 "gram.y" /* yacc.c:1646  */
    { prefs.icondir = (yyvsp[0].ptr); }
#line 1494 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 95 "gram.y" /* yacc.c:1646  */
    { set_sys_palette(); }
#line 1500 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 96 "gram.y" /* yacc.c:1646  */
    { set_mwb_palette(); }
#line 1506 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 97 "gram.y" /* yacc.c:1646  */
    { set_schwartz_palette(); }
#line 1512 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 98 "gram.y" /* yacc.c:1646  */
    { set_custom_palette((yyvsp[0].ptr)); }
#line 1518 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 99 "gram.y" /* yacc.c:1646  */
    { default_colors[(yyvsp[-1].num)] = (yyvsp[0].ptr); }
#line 1524 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 100 "gram.y" /* yacc.c:1646  */
    { default_screenfont = (yyvsp[0].ptr); }
#line 1530 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 101 "gram.y" /* yacc.c:1646  */
    { label_font_name = (yyvsp[0].ptr); }
#line 1536 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 102 "gram.y" /* yacc.c:1646  */
    { prefs.forcemove = (yyvsp[0].num); }
#line 1542 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 103 "gram.y" /* yacc.c:1646  */
    { openscreen((yyvsp[0].ptr),DefaultRootWindow(dpy)); }
#line 1548 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 104 "gram.y" /* yacc.c:1646  */
    { if(((yyvsp[-1].num)==DefaultScreen(dpy)||prefs.manage_all) && (yyvsp[-1].num)<ScreenCount(dpy)) openscreen((yyvsp[0].ptr),RootWindow(dpy,(yyvsp[-1].num))); }
#line 1554 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 105 "gram.y" /* yacc.c:1646  */
    { prefs.module_path = (yyvsp[0].ptr); }
#line 1560 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 106 "gram.y" /* yacc.c:1646  */
    { create_module((front? front->upfront:NULL), (yyvsp[-1].ptr), (yyvsp[0].ptr)); }
#line 1566 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 107 "gram.y" /* yacc.c:1646  */
    { create_module((front? front->upfront:NULL), (yyvsp[0].ptr), NULL); }
#line 1572 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 108 "gram.y" /* yacc.c:1646  */
    { prefs.borderwidth=(yyvsp[0].num); }
#line 1578 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 109 "gram.y" /* yacc.c:1646  */
    { prefs.autoraise=(yyvsp[0].num); }
#line 1584 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 110 "gram.y" /* yacc.c:1646  */
    { prefs.opaquemove=(yyvsp[0].num); }
#line 1590 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 111 "gram.y" /* yacc.c:1646  */
    { prefs.opaqueresize=(yyvsp[0].num); }
#line 1596 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 112 "gram.y" /* yacc.c:1646  */
    { prefs.focus=(yyvsp[0].num); }
#line 1602 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 113 "gram.y" /* yacc.c:1646  */
    { prefs.customiconsonly = (yyvsp[0].num); }
#line 1608 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 114 "gram.y" /* yacc.c:1646  */
    { prefs.shortlabelicons = (yyvsp[0].num); }
#line 1614 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 115 "gram.y" /* yacc.c:1646  */
    { prefs.titlebarclock = (yyvsp[0].num); }
#line 1620 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 116 "gram.y" /* yacc.c:1646  */
    { prefs.titleclockformat = (yyvsp[0].ptr); }
#line 1626 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 117 "gram.y" /* yacc.c:1646  */
    { 
					prefs.titleclockinterval=(yyvsp[-1].num); 
					prefs.titleclockformat=(yyvsp[0].ptr); }
#line 1634 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 120 "gram.y" /* yacc.c:1646  */
    { prefs.screenmenu=(yyvsp[0].num); }
#line 1640 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 124 "gram.y" /* yacc.c:1646  */
    { add_toolitem((yyvsp[-1].ptr), NULL, NULL, -1); ti_level=1; }
#line 1646 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 131 "gram.y" /* yacc.c:1646  */
    { add_toolitem((yyvsp[-1].ptr), (yyvsp[0].ptr), NULL, ti_level); }
#line 1652 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 132 "gram.y" /* yacc.c:1646  */
    { add_toolitem((yyvsp[-2].ptr), (yyvsp[-1].ptr), (yyvsp[0].ptr), ti_level); }
#line 1658 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 133 "gram.y" /* yacc.c:1646  */
    { add_toolitem(NULL, NULL, NULL, ti_level); }
#line 1664 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 136 "gram.y" /* yacc.c:1646  */
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
#line 1681 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 153 "gram.y" /* yacc.c:1646  */
    { append_to(&prefs.laststyle->style_class, (yyvsp[0].ptr));}
#line 1687 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 154 "gram.y" /* yacc.c:1646  */
    { append_to(&prefs.laststyle->style_title, (yyvsp[0].ptr));}
#line 1693 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 155 "gram.y" /* yacc.c:1646  */
    { append_to(&prefs.laststyle->style_icon_title, (yyvsp[0].ptr));}
#line 1699 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 156 "gram.y" /* yacc.c:1646  */
    { prefs.laststyle->icon_name=(yyvsp[0].ptr); }
#line 1705 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 159 "gram.y" /* yacc.c:1646  */
    { (yyval.ptr) = strdup((yyvsp[0].ptr)); }
#line 1711 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 162 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = True; }
#line 1717 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 163 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = False; }
#line 1723 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 166 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = Psizeright; }
#line 1729 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 167 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = Psizebottom; }
#line 1735 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 168 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = Psizeright|Psizebottom; }
#line 1741 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 169 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = Psizetrans; }
#line 1747 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 170 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = Psizetrans; }
#line 1753 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 173 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = FOC_FOLLOWMOUSE; }
#line 1759 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 174 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = FOC_SLOPPY; }
#line 1765 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 175 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = FOC_CLICKTOTYPE; }
#line 1771 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 178 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = DETAILPEN; }
#line 1777 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 179 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = BLOCKPEN; }
#line 1783 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 180 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = TEXTPEN; }
#line 1789 "y.tab.c" /* yacc.c:1646  */
    break;

  case 64:
#line 181 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = SHINEPEN; }
#line 1795 "y.tab.c" /* yacc.c:1646  */
    break;

  case 65:
#line 182 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = SHADOWPEN; }
#line 1801 "y.tab.c" /* yacc.c:1646  */
    break;

  case 66:
#line 183 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = FILLPEN; }
#line 1807 "y.tab.c" /* yacc.c:1646  */
    break;

  case 67:
#line 184 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = FILLTEXTPEN; }
#line 1813 "y.tab.c" /* yacc.c:1646  */
    break;

  case 68:
#line 185 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = BACKGROUNDPEN; }
#line 1819 "y.tab.c" /* yacc.c:1646  */
    break;

  case 69:
#line 186 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = HIGHLIGHTTEXTPEN; }
#line 1825 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 187 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = BARDETAILPEN; }
#line 1831 "y.tab.c" /* yacc.c:1646  */
    break;

  case 71:
#line 188 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = BARBLOCKPEN; }
#line 1837 "y.tab.c" /* yacc.c:1646  */
    break;

  case 72:
#line 189 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = BARTRIMPEN; }
#line 1843 "y.tab.c" /* yacc.c:1646  */
    break;

  case 73:
#line 192 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = FM_ALWAYS; }
#line 1849 "y.tab.c" /* yacc.c:1646  */
    break;

  case 74:
#line 193 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = FM_AUTO; }
#line 1855 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 194 "gram.y" /* yacc.c:1646  */
    { (yyval.num) = FM_MANUAL; }
#line 1861 "y.tab.c" /* yacc.c:1646  */
    break;


#line 1865 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 197 "gram.y" /* yacc.c:1906  */

extern char *progname;
extern int ParseError;
int yyerror(s) char *s;
{
    fprintf (stderr, "%s: error in input file:  %s\n", progname, s ? s : "");
    ParseError = 1;
    return 0;
}
