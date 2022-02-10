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
#ifdef YYSTYPE
#undef  YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
#endif
#ifndef YYSTYPE_IS_DECLARED
#define YYSTYPE_IS_DECLARED 1
typedef union
{
    int num;
    char *ptr;
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;