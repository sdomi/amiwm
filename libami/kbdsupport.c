#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <string.h>
#include "libami.h"
#include "module.h"


int md_grabkey(int keycode, unsigned int modifiers)
{
  int pkt[2], res=-1;
  char *ptr=NULL;
  pkt[0]=keycode; pkt[1]=modifiers;
  if(md_command(None, MCMD_ADD_KEYGRAB, pkt, sizeof(pkt), &ptr)>=sizeof(int))
    res=*((int*)ptr);
  if(ptr) free(ptr);
  return res;
}

int md_ungrabkey(int id)
{
  return md_command0(None, MCMD_DEL_KEYGRAB, &id, sizeof(id));
}
