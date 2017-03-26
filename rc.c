#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xmu/CharSet.h>

#include "alloc.h"
#include "prefs.h"
#include "drawinfo.h"
#include "screen.h"
#include "gram.h"
#include "icc.h"
#include "style.h"

#ifdef AMIGAOS
#include <pragmas/xmu1_pragmas.h>
extern struct Library *Xmu1Base;
#endif

extern void set_sys_palette(void);
extern int yyparse (void);

struct prefs_struct prefs;

#ifndef RC_FILENAME
#define RC_FILENAME ".amiwmrc"
#endif

FILE *rcfile;
int ParseError=0;

void read_rc_file(char *filename, int manage_all)
{
  char *home, *fn;

  memset(&prefs, 0, sizeof(prefs));
  prefs.manage_all = manage_all;
  prefs.sizeborder=Psizeright;
  prefs.icondir=AMIWM_HOME;
  prefs.module_path=AMIWM_HOME;
  prefs.defaulticon="def_tool.info";
  prefs.borderwidth=1;
  prefs.titleclockinterval=1;
  prefs.opaquemove=False;
  prefs.opaqueresize=False;
  prefs.screenmenu=False;
  prefs.firststyle=NULL;
  prefs.laststyle=NULL;
  set_sys_palette();

  if(filename!=NULL && (rcfile=fopen(filename, "r"))) {
    yyparse();
    fclose(rcfile);
    rcfile=NULL;
    return;
  }

  home=getenv("HOME");
#ifdef AMIGAOS
  {
    char fn[256];
    strncpy(fn, home, sizeof(fn)-1);
    fn[sizeof(fn)-1]='\0';
    AddPart(fn, RC_FILENAME, sizeof(fn));
    if((rcfile=fopen(fn, "r"))) {
      yyparse();
      fclose(rcfile);
      rcfile=NULL;
      return;
    }
  }
#else
#ifdef HAVE_ALLOCA
  if((fn=alloca(strlen(home)+strlen(RC_FILENAME)+4))) {
#else
  if((fn=malloc(strlen(home)+strlen(RC_FILENAME)+4))) {
#endif
    sprintf(fn, "%s/"RC_FILENAME, home);
    if((rcfile=fopen(fn, "r"))) {
      yyparse();
      fclose(rcfile);
#ifndef HAVE_ALLOCA
      free(fn);
#endif
      return;
    }
#ifndef HAVE_ALLOCA
    free(fn);
#endif
  }
#endif
  if((rcfile=fopen(AMIWM_HOME"/system"RC_FILENAME, "r"))) {
    yyparse();
    fclose(rcfile);
  }
}

struct keyword { char *name; int token; } keywords[] = {
  { "always", ALWAYS },
  { "auto", AUTO },
  { "autoraise", AUTORAISE },
  { "backgroundpen", T_BACKGROUNDPEN },
  { "barblockpen", T_BARBLOCKPEN },
  { "bardetailpen", T_BARDETAILPEN },
  { "bartrimpen", T_BARTRIMPEN },
  { "blockpen", T_BLOCKPEN },
  { "both", BOTH },
  { "bottom", BOTTOM },
  { "class", CLASS },
  { "clicktotype", CLICKTOTYPE },
  { "customiconsonly", CUSTOMICONSONLY },
  { "defaulticon", DEFAULTICON },
  { "detailpen", T_DETAILPEN },
  { "false", NO },
  { "fastquit", FASTQUIT },
  { "fillpen", T_FILLPEN },
  { "filltextpen", T_FILLTEXTPEN },
  { "focus", FOCUS },
  { "followmouse", FOLLOWMOUSE },
  { "forcemove", FORCEMOVE },
  { "highlighttextpen", T_HIGHLIGHTTEXTPEN },
  { "icon", ICON },
  { "icondir", ICONDIR },
  { "iconfont", ICONFONT },
  { "iconpalette", ICONPALETTE },
  { "icontitle", ICONTITLE },
  { "interscreengap", INTERSCREENGAP },
  { "magicwb", MAGICWB },
  { "manual", MANUAL },
  { "module", MODULE },
  { "modulepath", MODULEPATH },
  { "no", NO },
  { "none", NONE },
  { "off", NO },
  { "on", YES },
  { "opaquemove", OPAQUEMOVE },
  { "opaqueresize", OPAQUERESIZE },
  { "right", RIGHT },
  { "schwartz", SCHWARTZ },
  { "screen", SCREEN },
  { "screenfont", SCREENFONT },
  { "screenmenu", SCREENMENU },
  { "separator", SEPARATOR },
  { "shadowpen", T_SHADOWPEN },
  { "shinepen", T_SHINEPEN },
  { "shortlabelicons", SHORTLABELICONS },
  { "sizeborder", SIZEBORDER },
  { "sloppy", SLOPPY },
  { "style", STYLE },
  { "system", SYSTEM },
  { "textpen", T_TEXTPEN },
  { "title", TITLE },
  { "titlebarclock", TITLEBARCLOCK },
  { "titleclockformat", TITLECLOCKFORMAT },
  { "toolitem", TOOLITEM },
  { "true", YES },
  { "yes", YES }
};

#define N_KW (sizeof(keywords)/sizeof(keywords[0]))

int parse_keyword(char *str)
{
  int l=0, h=N_KW-1;
  
  XmuCopyISOLatin1Lowered (str, str);
  while(h>=l) {
    int i=(h+l)>>1, c=strcmp(str, keywords[i].name);
    if(!c)
      return keywords[i].token;
    else if(c>=0)
      l=i+1;
    else
      h=i-1;
  }
  return ERRORTOKEN;
}

void free_prefs()
{
  Style *s;
  while((s = prefs.firststyle)) {
    prefs.firststyle = s->next;
    if(s->style_class) free(s->style_class);
    if(s->style_title) free(s->style_title);
    if(s->style_icon_title) free(s->style_icon_title);
    if(s->icon_name) free(s->icon_name);
    free_icon_pms(&s->icon_pms);
    free(s);
  }
  prefs.laststyle = NULL;
}
