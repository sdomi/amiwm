#include "libami.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

#ifndef AMIGAOS

UBYTE ToUpper(UBYTE ch)
{
  static int firstcall=1;
  if(firstcall) {
    setlocale(LC_CTYPE, "");
    firstcall=0;
  }
  return toupper(ch);
}

LONG StrToLong(STRPTR str, LONG *n)
{
  STRPTR end;

  *n=strtol((char *)str, (char **)&end, 0);
  return end-str;
}

LONG Stricmp(STRPTR a, STRPTR b)
{
  while(*a && *b)
    if(ToUpper(*a++)!=ToUpper(*b++))
      return FALSE;
  return !(*a || *b);
}

#endif
