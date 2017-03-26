#include <stdio.h>
#include <string.h>
#include "libami.h"

#ifndef AMIGAOS

static int amiga_errno=0;

#define MIN_ERRNO 103

static const char *syserrmsg[] = {
  "not enough memory available", /* 103 */
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
  "bad template", /* 114 */
  "bad number", /* 115 */
  "required argument missing", /* 116 */
  "value after keyword missing", /* 117 */
  "wrong number of arguments", /* 118 */
  NULL,
  "argument line invalid or too long" /* 120 */
};

BOOL Fault(LONG code, UBYTE *header, UBYTE *buffer, LONG len)
{
  amiga_errno=code;
  if(header) {
    int hdlen=strlen((char *)header);
    if(hdlen+2>len)
      return FALSE;
    strcpy((char *)buffer, (char *)header);
    buffer+=hdlen;
    *buffer++=':';
    *buffer++=' ';
    len-=hdlen+2;
  }
  if(code>=MIN_ERRNO && code<MIN_ERRNO+sizeof(syserrmsg)/sizeof(syserrmsg[0])
     && syserrmsg[code-MIN_ERRNO]) {
    if(len<strlen(syserrmsg[code-MIN_ERRNO])+1)
      return FALSE;
    strcpy((char *)buffer, syserrmsg[code-MIN_ERRNO]);
  } else {
    char number[6+4*sizeof(LONG)];
    sprintf(number, "Error %ld", code);
    if(len<strlen(number)+1)
      return FALSE;
    strcpy((char *)buffer, number);
  }
  return TRUE;
}

BOOL PrintFault(LONG code, UBYTE *header)
{
  UBYTE buf[128];
  if(Fault(code, header, buf, sizeof(buf))) {
    fprintf(stderr, "%s\n", (char *)buf);
    return TRUE;
  } else return FALSE;
}

LONG IoErr()
{
  return amiga_errno;
}

LONG SetIoErr(LONG code)
{
  LONG old_errno=amiga_errno;
  amiga_errno=code;
  return old_errno;
}

#endif
