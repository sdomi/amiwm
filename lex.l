%{
#include <stdio.h>
#include "gram.h"
extern char *progname;
extern int ParseError;
extern FILE *rcfile;
#undef yyin
#define yyin rcfile
#define yywrap() 1
extern int parse_keyword(char *);
void fixup_string(char *, char *);
#define YY_SKIP_YYWRAP
%}
%%

\#[^\n]*\n	{ ; }

\"([^"\n\\]|\\.|\\\n)*\"    { fixup_string(yytext, yytext); yylval.ptr=yytext;
			  return STRING; }

[0-9]+ { yylval.num=atoi(yytext); return NUMBER; }

[a-zA-Z]+	{ int token = parse_keyword(yytext);
		  if(token == ERRORTOKEN) {
		    fprintf(stderr, "%s: ignoring unknown keyword: %s\n",
			    progname, yytext);
		    ParseError=1;
		  } else
		    return token;
		}

\{		{ return LEFTBRACE; }

\}		{ return RIGHTBRACE; }

[\r\n\t ]	{ ; }

.		{ fprintf(stderr, "%s: illegal character \"%s\" ignored\n",
			  progname, yytext);
		  ParseError=1; }

%%
void fixup_string(char *src, char *dest)
{
  src++;
  while(*src!='"')
    if((*dest++=*src++)=='\\')
      switch((dest[-1]=*src++)) {
      case 'n':
	dest[-1]='\n';
	break;
      case 'r':
	dest[-1]='\r';
	break;
      case 'a':
	dest[-1]='\a';
	break;
      case 'b':
	dest[-1]='\b';
	break;
      case 't':
	dest[-1]='\t';
	break;
      case 'e':
	dest[-1]='\033';
	break;
      }
  *dest='\0';
}
