#include "libami.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef AMIGAOS

void initargs(int argc, char **argv)
{
}

#else

static char *cmdline=NULL;
int cmdline_allocated=0, cmdline_used=0;

static void addachar(char c)
{
  if(cmdline_used>=cmdline_allocated) {
    if(!cmdline_allocated) cmdline_allocated=128;
    while(cmdline_allocated<=cmdline_used)
      cmdline_allocated<<=1;
    if(cmdline!=NULL)
      cmdline=realloc(cmdline, cmdline_allocated);
    else
      cmdline=malloc(cmdline_allocated);
  }
  cmdline[cmdline_used++]=c;
}

static void addquoted(const char *arg)
{
  char ch;
  addachar(' '); addachar('"');
  while((ch=*arg++)) switch(ch) {
  case '\n': addachar('*'); addachar('N'); break;
  case '\"': addachar('*'); addachar('\"'); break;
  case '*': addachar(ch);
  default: addachar(ch);
  }
  addachar('"');
}

static void addunquoted(const char *arg, int cnt)
{
  if(cnt) {
    addachar(' ');
    while(cnt--)
      addachar(*arg++);
  }
}

static void addarg(const char *arg)
{
  if(*arg!='\"') {
    int plain=strcspn(arg, "\t\n ;=");
    if(!arg[plain]) {
      addunquoted(arg, plain);
      return;
    } else if(arg[plain]=='=') {
      addunquoted(arg, plain);
      addunquoted("=", 1);
      arg+=plain+1;
      if(!*arg)
	return;
    }
  }
  addquoted(arg);
}

void initargs(int argc, char **argv)
{
  while(--argc) addarg(*++argv);
  addachar('\n');
  addachar('\0');
}

#endif

#ifndef AMIGAOS

#define RDAF_PRIVATE1 0x40000000
#define RDAF_PRIVATE2 0x20000000


void FreeArgs(struct RDArgs *rdargs)
{
  if(rdargs) {
    APTR daelt, nextda = (APTR)rdargs->RDA_DAList;

    rdargs->RDA_Buffer = NULL;
    rdargs->RDA_Flags &= (RDAF_PRIVATE1 | RDAF_PRIVATE2);
    rdargs->RDA_DAList = NULL;
    while((daelt = nextda)) {
      nextda = ((Argtype *)daelt)->ptr;
      free(daelt);
    }
  }
}



static UBYTE * ra_alloc(struct RDArgs *rdargs, ULONG size)
{
  UBYTE *ptr=NULL;

  if((!(rdargs->RDA_Flags & RDAF_NOALLOC)) &&
     (ptr=malloc(size+sizeof(Argtype)))) {
    memset(ptr, 0, size+sizeof(Argtype));
    ((Argtype *)ptr)->ptr=rdargs->RDA_DAList;
    rdargs->RDA_DAList=ptr;
    ptr+=sizeof(Argtype);
  }
  return ptr;
}

static UBYTE * ra_realloc(struct RDArgs *rdargs, UBYTE **start, UBYTE **end)
{
  UBYTE *oldstart=*start;
  ULONG newlength=((*end-oldstart+144)&~0x7f)-8;
  UBYTE *newstart=ra_alloc(rdargs, newlength+4);

  if(!newstart)
    return NULL;

  *start=newstart;
  *end=newstart+newlength;
  return oldstart;
}



static LONG CS_ReadChar(struct CSource *cSource)
{
  if(!cSource || ((LONG)cSource)==-1 || !cSource->CS_Buffer) {
    if(cmdline && *cmdline) {
      return (unsigned char)(*cmdline++);
    } else
      return getchar();
  }
  if(cSource->CS_CurChr >= cSource->CS_Length)
    return -1;
  return cSource->CS_Buffer[cSource->CS_CurChr++];
}

static void CS_UnReadChar(struct CSource *cSource, UBYTE ch)
{
  if(!cSource || ((LONG)cSource)==-1 || !cSource->CS_Buffer) {
    if(cmdline && *cmdline)
      --cmdline;
    else
      ungetc(ch, stdin);
  } else if(cSource->CS_CurChr > 0)
    --cSource->CS_CurChr;
}

static LONG intreaditem(STRPTR buffer, LONG maxchars, struct CSource *input,
		int mode, UBYTE **start, UBYTE **end)
{
  int itemlen=0;
  int ch, quote='\0';

  buffer[0]='\0';
  do ch=CS_ReadChar(input); while(ch==' ' || ch=='\t');
  if(ch=='"') { quote=ch; ch=CS_ReadChar(input); }

  while(ch!='\n' && ch>=0) {
    if(quote) {
      if(ch==quote)
        return ITEM_QUOTED;
      if(ch=='*') {
	ch=CS_ReadChar(input);
	if(ch<0 || ch=='\n') break;
	if(ch=='e' || ch=='E') ch='\033';
	if(ch=='n' || ch=='N') ch='\n';
      }
    } else {
      if(ch==';')
        break;
      if(ch==' ' || ch=='\t' || (ch=='=' && itemlen)) {
        if(mode<0)
          CS_UnReadChar(input, ch);
        return ITEM_UNQUOTED;
      }
      if(ch=='=')
        return ITEM_EQUAL;
    }
    if(++itemlen>=maxchars) {
      UBYTE *bufswap;
      if(mode<=0 || !(bufswap=ra_realloc((struct RDArgs *)input, start, end)))
        return ITEM_ERROR;
      memcpy(*start, bufswap, itemlen-1);
      buffer=*start+itemlen-1;
      maxchars=*end-*start;
    }
    if(mode<0) {
      buffer[itemlen]=ch;
      buffer[itemlen+1]='\0';
      buffer[0]=itemlen;
    } else {
      *buffer++=ch;
      *buffer='\0';
    }
    ch=CS_ReadChar(input);
  }
  CS_UnReadChar(input, ch);
  return (quote? ITEM_ERROR : (itemlen? ITEM_UNQUOTED : ITEM_NOTHING));
}

LONG ReadItem(STRPTR buffer, LONG maxchars, struct CSource *input)
{
  return intreaditem(buffer, maxchars, input, 0, NULL, NULL);
}





LONG FindArg(STRPTR template, STRPTR keyword)
{
  int kwlen=strlen((char *)keyword);
  int kwindex, argindex=0;
  char ch;

  for(;;) {

    kwindex=0;
    do {
      ch=*template++;
      if(!ch)
        return (kwindex == kwlen? argindex : -1);
      if(ch == ',' || ch == '=' || ch == '/')
        if(kwindex == kwlen)
          return argindex;
        else
          break;
    } while(ToUpper(ch) == ToUpper(keyword[kwindex++]));

    while(ch != '=') {
      if(ch == ',') {
        argindex++;
        break;
      }
      ch=*template++;
      if(!ch)
        return -1;
    }
  }
}

#define RA_ALWAYS  1
#define RA_KEYWORD 2
#define RA_SWITCH  4
#define RA_NUMERIC 8
#define RA_FORCE   16
#define RA_TOGGLE  32
#define RA_MULTI   64
#define RA_FOUND   128

static LONG parseswitches(STRPTR template, LONG len, UBYTE *flags, int *numargs)
{
  static struct ra_switch { UBYTE flag, ch; } switches[] = {
    { RA_ALWAYS,  'A' },
    { RA_KEYWORD, 'K' },
    { RA_SWITCH,  'S' },
    { RA_NUMERIC, 'N' },
    { RA_FORCE,   'F' },
    { RA_TOGGLE,  'T' },
    { RA_MULTI,   '.' },
    { RA_MULTI,   'M' },
    {  0,  0  }
  };
  struct ra_switch *sw;
  int m_used=0;
  UBYTE ch;

  *numargs=0;
  while(++*numargs<=MAX_TEMPLATE_ITEMS) {
    *flags++=0;
    do {
      if(--len<0)
        return 0;

      if((ch=*template++)=='/') {
        ch=ToUpper(*template++);
        --len;

        for(sw=switches; sw->ch; sw++)
          if(sw->ch == ch) {
            flags[-1]|=sw->flag;
            if((sw->flag&RA_MULTI) && m_used++)
              return ERROR_BAD_TEMPLATE;
          }
      }
    } while(ch != ',');
  }
  return ERROR_LINE_TOO_LONG;
}

static LONG rareaditem(struct RDArgs *rdargs, UBYTE **start, UBYTE **end)
{
  while(*end-*start-1<0)
    if(!rdargs || !ra_realloc(rdargs, start, end))
      return ITEM_ERROR;
  return (rdargs?
    intreaditem(*start, *end-*start-1, &rdargs->RDA_Source, 1, start, end):
    intreaditem(*start, *end-*start-2, (struct CSource *)-1, -1, start, end));
}

struct RDArgs * ReadArgs(STRPTR template, LONG *array, struct RDArgs *rdargs)
{
  LONG itemtype;
  UBYTE *multiargs[MAX_MULTIARGS+1];
  UBYTE switches[MAX_TEMPLATE_ITEMS];
  UBYTE *start, *end, sw, *swptr;
  Argtype *lastmarg=NULL;
  int multinum=0;
  int argnum;
  LONG ch, error=0;
  int numargs;
  int munched;

  if(rdargs) {
    rdargs->RDA_DAList=NULL;
    if(!rdargs->RDA_Buffer) {
      if(!(rdargs->RDA_Buffer=ra_alloc(rdargs, 128+4)))
        goto nomemfail;
      rdargs->RDA_BufSiz=128;
    }
  } else {
    rdargs=malloc(128+4+sizeof(struct RDArgs)+sizeof(Argtype));
    if(!rdargs)
      goto nomemfail;
    memset(rdargs, 0, 128+4+sizeof(struct RDArgs)+sizeof(Argtype));
    rdargs=(struct RDArgs *)(((char *)rdargs)+sizeof(Argtype));
    rdargs->RDA_Buffer=((UBYTE*)rdargs)+sizeof(struct RDArgs);
    rdargs->RDA_DAList=(((char *)rdargs)-sizeof(Argtype));
    rdargs->RDA_BufSiz=128;
  }
  start=rdargs->RDA_Buffer;
  end=rdargs->RDA_Buffer+rdargs->RDA_BufSiz;
  if((error=parseswitches(template, strlen((char *)template),
			  switches, &numargs)))
    goto fail;

  for(;;) {
    argnum=-1;
    switch(itemtype=rareaditem(rdargs, &start, &end)) {
    case ITEM_NOTHING:

      swptr=switches;

      while(numargs-->0) {
        int isnumeric=0;

        if((*swptr)&RA_MULTI)
	  if(multinum) {
	    ((Argtype *)array)->ptr=ra_alloc(rdargs,
					     sizeof(Argtype)*(multinum+1));
            if(!((Argtype *)array)->ptr)
              goto nomemfail;
            memcpy(((Argtype *)array)->ptr, multiargs, sizeof(Argtype)*multinum);
            lastmarg=((Argtype *)(((Argtype *)array)->ptr))+multinum-1;
            lastmarg[1].ptr=NULL;
            isnumeric=((*swptr)&RA_NUMERIC)!=0;
          }
	  else ((Argtype *)array)->ptr=NULL;
        array=(LONG*)(((Argtype *)array)+1);
        if(!((sw=*swptr++)&RA_FOUND) && (sw&RA_ALWAYS)) {
          if((!lastmarg) || (sw&RA_KEYWORD) || multinum<2 ||
	     isnumeric!=((sw&RA_NUMERIC)!=0)) {
            error=ERROR_REQUIRED_ARG_MISSING;
            goto fail;
          }
	  ((Argtype *)array)[-1].ptr=lastmarg->ptr;
          (lastmarg--)->ptr=NULL;
          --multinum;
        }
      }
      CS_ReadChar(&rdargs->RDA_Source);
      return rdargs;

    case ITEM_UNQUOTED:
      argnum=FindArg(template, start);
      if(argnum<0)
	goto not_namedarg;
      sw=switches[argnum];
      if(sw&RA_SWITCH) {
	((Argtype *)array)[argnum].num=-1;
        break;
      }
      if(!(sw&RA_FORCE)) {
	LONG argt;
        if((argt=rareaditem(rdargs, &start, &end))==ITEM_EQUAL)
	  argt=rareaditem(rdargs, &start, &end);
        if(argt==ITEM_NOTHING) {
	  error=ERROR_KEY_NEEDS_ARG;
	  goto fail;
        } else if(argt<0) {
	  error=ERROR_LINE_TOO_LONG;
	  goto fail;
        } else
	  goto quoted;
      }
      munched=0;
force:
      if(end-start<=munched) {
	UBYTE *old;
	if(!(old=ra_realloc(rdargs, &start, &end)))
	  goto nomemfail;
	memcpy(start, old, munched);
      }
      if((ch=CS_ReadChar(&rdargs->RDA_Source))>=0 && ch!='\n') {
        start[munched++]=ch;
	goto force;
      }
      if(itemtype!=ITEM_QUOTED)
        while((start[munched-1]==' ' || start[munched-1]=='\t') && --munched>0);
      start[munched]='\0';
      goto quoted2;

not_namedarg:
      ch=CS_ReadChar(&rdargs->RDA_Source);
      if(ch<0)
	goto quoted;
      if(ch=='\n' && start[0]=='?' && start[1]=='\0' &&
	 !(rdargs->RDA_Flags & RDAF_NOPROMPT)) {
	UBYTE *helptext = template;
	if(!(rdargs->RDA_Flags & RDAF_PRIVATE1)) {
	  rdargs->RDA_Flags |= RDAF_PRIVATE1;
	  if(rdargs->RDA_ExtHelp)
	    helptext=rdargs->RDA_ExtHelp;
	}
	fprintf(stderr, "%s: ", (char *)helptext);
	fflush(stderr);
	break;
      }
quoted2:
      CS_UnReadChar((struct CSource *)rdargs, ch);

quoted:
    case ITEM_QUOTED:
      if(argnum<0) {
        swptr=switches;
        error=ERROR_TOO_MANY_ARGS;
        for(;;) {
          if(++argnum>=numargs)
            goto fail;
          if((*swptr)&RA_MULTI)
            break;
          if((sw=*swptr++)&RA_FOUND)
            continue;
          if(sw&RA_FORCE) {
	    munched=strlen((char *)start);
	    if(itemtype!=ITEM_QUOTED)
	      start[munched++]=' ';
	    goto force;
	  }
          if(!(sw&RA_KEYWORD))
	    if(!(sw&=(RA_TOGGLE|RA_SWITCH)))
	      break;
        }
      }
      sw=switches[argnum];
      switches[argnum]|=RA_FOUND;
      if(!rdargs) {
        ((Argtype *)array)[argnum].num=((LONG)start)>>2;
        start+=((*start)&~3)+4;
        continue;
      }
      if(sw&RA_MULTI) {
        if(multinum>=MAX_MULTIARGS-1) {
	  error=ERROR_LINE_TOO_LONG;
          goto fail;
        } if(sw&RA_NUMERIC)
          goto numeric;
        multiargs[multinum++]=start;
      } else if((sw&RA_FOUND)&&!(sw&RA_SWITCH)) {
        error=ERROR_TOO_MANY_ARGS;
        goto fail;
      } else if(sw&RA_TOGGLE) {
        if(Stricmp(start, (STRPTR)"yes"))
	  ((Argtype*)array)[argnum].num=-1;
        else if(Stricmp(start, (STRPTR)"no"))
	  ((Argtype*)array)[argnum].num=0;
        else if(Stricmp(start, (STRPTR)"on"))
	  ((Argtype*)array)[argnum].num=-1;
        else if(Stricmp(start, (STRPTR)"off"))
	  ((Argtype*)array)[argnum].num=0;
        else {
	  error=ERROR_KEY_NEEDS_ARG;
	  goto fail;
        }
      } else if(sw&RA_NUMERIC) {
        LONG n; int len;
numeric:
        if((len=StrToLong(start, &n))<=0 || len!=strlen((char *)start)) {
	  error=ERROR_BAD_NUMBER;
	  goto fail;
        }
	start+=sizeof(Argtype)-1;
	start-=((LONG)start)&(sizeof(Argtype)-1);
        if(end-start<sizeof(LONG))
	  if(!ra_realloc(rdargs, &start, &end))
	    goto nomemfail;
        *(LONG *)start=n;
        if(sw&RA_MULTI)
	  multiargs[multinum++]=start;
        else
	  ((Argtype*)array)[argnum].ptr=start;
        start+=sizeof(LONG);
        continue;
      } else
        ((Argtype*)array)[argnum].ptr=start;

      start+=strlen((char *)start)+1;
      break;

    default:
      error=ERROR_LINE_TOO_LONG;
      goto fail;
    }
  }

nomemfail:
  error=ERROR_NO_FREE_STORE;
fail:
  do
    ch=CS_ReadChar(&rdargs->RDA_Source);
  while(ch!='\n' && ch!=';' && ch>=0);
  if(rdargs)
    FreeArgs(rdargs);
  SetIoErr(error);
  return NULL;
}

#endif
