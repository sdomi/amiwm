%{
#include <X11/Xmu/CharSet.h>
#include <string.h>

#include "libami.h"
#include "kbdmodule.h"

unsigned char *inptr;
extern char *progname;

#ifdef FLEX_SCANNER

#undef YY_INPUT
#define YY_INPUT(buf,result,size) ((result) = doinput((buf),(size)))
int doinput (char *buf, int size)
{
  int l=strlen((char *)inptr);
  if(l>size) l=size;
  if(l>0) {
    memcpy(buf, inptr, l);
    inptr+=l;
  }
  return l;
}
#endif

static void k_rotscreens(Window w)
{
  Window root;
  int x, y;
  unsigned int width, height, bw, d;
  if(XGetGeometry(md_display(), w, &root, &x, &y, &width, &height, &bw, &d))
    md_rotate_screen(root);
}

typedef void (*mdfuncp)(Window);

int parse_keyword(char *str, YYSTYPE *val)
{
   struct { char *name; mdfuncp func; } functab[] = {
     { "back", (mdfuncp)md_back },
     { "front", (mdfuncp)md_front },
     { "iconify", (mdfuncp)md_iconify },
     { "rotatescreens", (mdfuncp)k_rotscreens },
   };
#define N_FUNC (sizeof(functab)/sizeof(functab[0]))
   struct { char *name; int token, num; } kwtab[] = {
     { "all", WHEREABOUTS, IN_ANYTHING_MASK },
     { "b1", MODIFIER, Button1Mask },
     { "b2", MODIFIER, Button2Mask },
     { "b3", MODIFIER, Button3Mask },
     { "b4", MODIFIER, Button4Mask },
     { "b5", MODIFIER, Button5Mask },
     { "button1", MODIFIER, Button1Mask },
     { "button2", MODIFIER, Button2Mask },
     { "button3", MODIFIER, Button3Mask },
     { "button4", MODIFIER, Button4Mask },
     { "button5", MODIFIER, Button5Mask },
     { "control", MODIFIER, ControlMask },
     { "frame", WHEREABOUTS, IN_FRAME_MASK },
     { "icon", WHEREABOUTS, IN_ICON_MASK },
     { "lock", MODIFIER, LockMask },
     { "m", META, 0 },
     { "m1", MODIFIER, Mod1Mask },
     { "m2", MODIFIER, Mod2Mask },
     { "m3", MODIFIER, Mod3Mask },
     { "m4", MODIFIER, Mod4Mask },
     { "m5", MODIFIER, Mod5Mask },
     { "meta", META, 0 },
     { "mod1", MODIFIER, Mod1Mask },
     { "mod2", MODIFIER, Mod2Mask },
     { "mod3", MODIFIER, Mod3Mask },
     { "mod4", MODIFIER, Mod4Mask },
     { "mod5", MODIFIER, Mod5Mask },
     { "root", WHEREABOUTS, IN_ROOT_MASK },
     { "shift", MODIFIER, ShiftMask },
     { "window", WHEREABOUTS, IN_WINDOW_MASK },
   };
#define N_KW (sizeof(kwtab)/sizeof(kwtab[0]))
  int l=0, h=N_FUNC-1;
  
  XmuCopyISOLatin1Lowered (str, str);
  while(h>=l) {
    int i=(h+l)>>1, c=strcmp(str, functab[i].name);
    if(!c) {
      val->function=functab[i].func;
      return FUNCTION;
    } else if(c>=0)
      l=i+1;
    else
      h=i-1;
  }
  l=0; h=N_KW-1;
  while(h>=l) {
    int i=(h+l)>>1, c=strcmp(str, kwtab[i].name);
    if(!c) {
      val->num=kwtab[i].num;
      return kwtab[i].token;
    } else if(c>=0)
      l=i+1;
    else
      h=i-1;
  }
  return ERRORTOKEN;
}

%}
%%

\#[^\n]*\n	{ ; }

\<[^\n\>]*\>	{ *((char*)strchr(yytext, '>'))=0;
		  yylval.keysym=XStringToKeysym(yytext+1);
		  if(yylval.keysym == 0) {
                    fprintf(stderr, "%s: ignoring unknown keysym: %s\n",
                            progname, yytext+1);
		  } else
		    return KEYSYM;
		}

[a-zA-Z0-9]+	{ int token = parse_keyword(yytext, &yylval);
                  if(token == ERRORTOKEN) {
                    fprintf(stderr, "%s: ignoring unknown keyword: %s\n",
                            progname, yytext);
                  } else
                    return token;
                }

\:		{ return COLON; }

\|		{ return BAR; }

[\r\n\t ]       { ; }

.               { fprintf(stderr, "%s: illegal character \"%s\" ignored\n",
                          progname, yytext); }
%%

#ifndef yywrap
int yywrap() { return 1; }
#endif
#undef input
#define input() ((*inptr)?(*inptr++):0)
#undef unput
#define unput(c) (*--inptr=c)
