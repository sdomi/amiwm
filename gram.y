%{
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
%}

%union
{
    int num;
    char *ptr;
};

%token <num> ERRORTOKEN LEFTBRACE RIGHTBRACE
%token <num> YES NO
%token <num> RIGHT BOTTOM BOTH NONE
%token <num> MAGICWB SYSTEM SCHWARTZ
%token <num> ALWAYS AUTO MANUAL
%token <num> SEPARATOR
%token <num> T_DETAILPEN T_BLOCKPEN T_TEXTPEN T_SHINEPEN T_SHADOWPEN
%token <num> T_FILLPEN T_FILLTEXTPEN T_BACKGROUNDPEN T_HIGHLIGHTTEXTPEN
%token <num> T_BARDETAILPEN T_BARBLOCKPEN T_BARTRIMPEN
%token <num> FASTQUIT SIZEBORDER DEFAULTICON ICONDIR ICONPALETTE SCREENFONT
%token <num> ICONFONT TOOLITEM FORCEMOVE SCREEN MODULE MODULEPATH
%token <num> INTERSCREENGAP AUTORAISE FOCUS FOLLOWMOUSE CLICKTOTYPE SLOPPY
%token <num> CUSTOMICONSONLY
%token <num> TITLEBARCLOCK TITLECLOCKFORMAT
%token <num> OPAQUEMOVE OPAQUERESIZE SCREENMENU STYLE CLASS TITLE ICONTITLE ICON
%token <num> SHORTLABELICONS
%token <ptr> STRING
%token <num> NUMBER

%type <num> truth sizeborder dri_pen forcemove_policy focuspolicy
%type <ptr> string

%start amiwmrc

%%

amiwmrc		: stmts
		;

stmts		: stmts stmt
		|
		;

stmt		: error
		| toolitem
		| toolsubmenu toolitems RIGHTBRACE { ti_level=0; }
		| FASTQUIT truth { prefs.fastquit = $2; }
		| SIZEBORDER sizeborder { prefs.sizeborder = $2; }
		| DEFAULTICON string { prefs.defaulticon = $2; }
		| ICONDIR string { prefs.icondir = $2; }
		| ICONPALETTE SYSTEM { set_sys_palette(); }
		| ICONPALETTE MAGICWB { set_mwb_palette(); }
		| ICONPALETTE SCHWARTZ { set_schwartz_palette(); }
		| ICONPALETTE STRING { set_custom_palette($2); }
		| dri_pen string { default_colors[$1] = $2; }
		| SCREENFONT string { default_screenfont = $2; }
		| ICONFONT string { label_font_name = $2; }
		| FORCEMOVE forcemove_policy { prefs.forcemove = $2; }
		| SCREEN string { openscreen($2,DefaultRootWindow(dpy)); }
		| SCREEN NUMBER string { if(($2==DefaultScreen(dpy)||prefs.manage_all) && $2<ScreenCount(dpy)) openscreen($3,RootWindow(dpy,$2)); }
		| MODULEPATH string { prefs.module_path = $2; }
		| MODULE string STRING { create_module((front? front->upfront:NULL), $2, $3); }
		| MODULE string { create_module((front? front->upfront:NULL), $2, NULL); }
		| INTERSCREENGAP NUMBER { prefs.borderwidth=$2; }
		| AUTORAISE truth { prefs.autoraise=$2; }
		| OPAQUEMOVE truth { prefs.opaquemove=$2; }
		| OPAQUERESIZE truth { prefs.opaqueresize=$2; }
		| FOCUS focuspolicy { prefs.focus=$2; }
		| CUSTOMICONSONLY truth { prefs.customiconsonly = $2; }
		| SHORTLABELICONS truth { prefs.shortlabelicons = $2; }
		| TITLEBARCLOCK truth { prefs.titlebarclock = $2; }
		| TITLECLOCKFORMAT string { prefs.titleclockformat = $2; }
		| TITLECLOCKFORMAT NUMBER string { 
					prefs.titleclockinterval=$2; 
					prefs.titleclockformat=$3; }
		| SCREENMENU truth { prefs.screenmenu=$2; }
		| stylespec styleitems RIGHTBRACE
		;

toolsubmenu	: TOOLITEM string LEFTBRACE { add_toolitem($2, NULL, NULL, -1); ti_level=1; }
		;

toolitems	: toolitems toolitem
		|
		;

toolitem	: TOOLITEM string string { add_toolitem($2, $3, NULL, ti_level); }
		| TOOLITEM string string string { add_toolitem($2, $3, $4, ti_level); }
		| TOOLITEM SEPARATOR { add_toolitem(NULL, NULL, NULL, ti_level); }
		;

stylespec	: STYLE LEFTBRACE { Style *s = malloc(sizeof(Style));
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

styleitems	: styleitems styleitem
		|
		;

styleitem	: CLASS string { append_to(&prefs.laststyle->style_class, $2);}
		| TITLE string { append_to(&prefs.laststyle->style_title, $2);}
		| ICONTITLE string { append_to(&prefs.laststyle->style_icon_title, $2);}
		| ICON string { prefs.laststyle->icon_name=$2; }


string		: STRING { $$ = strdup($1); }
		;

truth		: YES { $$ = True; }
		| NO { $$ = False; }
		;

sizeborder	: RIGHT { $$ = Psizeright; }
		| BOTTOM { $$ = Psizebottom; }
		| BOTH { $$ = Psizeright|Psizebottom; }
		| NONE { $$ = Psizetrans; }
		| NO { $$ = Psizetrans; }
		;

focuspolicy	: FOLLOWMOUSE { $$ = FOC_FOLLOWMOUSE; }
		| SLOPPY { $$ = FOC_SLOPPY; }
		| CLICKTOTYPE { $$ = FOC_CLICKTOTYPE; }
		;

dri_pen		: T_DETAILPEN { $$ = DETAILPEN; }
		| T_BLOCKPEN { $$ = BLOCKPEN; }
		| T_TEXTPEN { $$ = TEXTPEN; }
		| T_SHINEPEN { $$ = SHINEPEN; }
		| T_SHADOWPEN { $$ = SHADOWPEN; }
		| T_FILLPEN { $$ = FILLPEN; }
		| T_FILLTEXTPEN { $$ = FILLTEXTPEN; }
		| T_BACKGROUNDPEN { $$ = BACKGROUNDPEN; }
		| T_HIGHLIGHTTEXTPEN { $$ = HIGHLIGHTTEXTPEN; }
		| T_BARDETAILPEN { $$ = BARDETAILPEN; }
		| T_BARBLOCKPEN { $$ = BARBLOCKPEN; }
		| T_BARTRIMPEN { $$ = BARTRIMPEN; }
		;

forcemove_policy : ALWAYS { $$ = FM_ALWAYS; }
		 | AUTO { $$ = FM_AUTO; }
		 | MANUAL { $$ = FM_MANUAL; }
		 ;

%%
extern char *progname;
extern int ParseError;
int yyerror(s) char *s;
{
    fprintf (stderr, "%s: error in input file:  %s\n", progname, s ? s : "");
    ParseError = 1;
    return 0;
}
