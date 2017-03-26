%{
#include <stdio.h>
#include <string.h>
#include "libami.h"

void docmd(XEvent *e, void *callback)
{
  ((void (*)(Window))callback)(e->xany.window);
}

%}

%union
{
  int num;
  char *ptr;
  KeySym keysym;
  struct { unsigned int mods; int meta; } modifiers;
  void (*function)(Window);
};

%token <num> ERRORTOKEN
%token <num> META MODIFIER WHEREABOUTS COLON BAR
%token <keysym> KEYSYM
%token <function> FUNCTION

%type <num> whereabouts
%type <modifiers> modifiers
%type <function> command

%left BAR

%start bindings

%%

bindings		: bindings binding
			|
			;

binding			: modifiers KEYSYM COLON whereabouts COLON command { cx_hotkey($2, $1.mods, $1.meta, $4, docmd, (void*)$6); }
			;

modifiers		: modifiers MODIFIER { $$.mods=$1.mods|$2; $$.meta=$1.meta; }
			| modifiers META { $$.mods=$1.mods; $$.meta=1; }
			| { $$.mods=$$.meta=0; }
			;

whereabouts		: whereabouts BAR whereabouts { $$=$1|$3; }
			| WHEREABOUTS { $$=$1; }
			;

command			: FUNCTION { $$=$1; }
			;

%%

char *progname;

int yyerror(s) char *s;
{
    fprintf (stderr, "%s: error in input file:  %s\n", progname, s ? s : "");
    return 0;
}

int main(int argc, char *argv[])
{
  char *arg=md_init(argc, argv);
  progname=argv[0];
  if(arg) {
    extern unsigned char *inptr;
    inptr=(unsigned char *)arg;
    yyparse();
    md_main_loop();
  }
  return 0;
}
