#define ERRORTOKEN 257
#define META 258
#define MODIFIER 259
#define WHEREABOUTS 260
#define COLON 261
#define BAR 262
#define KEYSYM 263
#define FUNCTION 264
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
  KeySym keysym;
  struct { unsigned int mods; int meta; } modifiers;
  void (*function)(Window);
} YYSTYPE;
#endif /* !YYSTYPE_IS_DECLARED */
extern YYSTYPE yylval;
