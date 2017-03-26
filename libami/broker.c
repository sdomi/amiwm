#include <stdio.h>
#include <stdlib.h>

#include "libami.h"
#include "module.h"

extern void (*md_broker_func)(XEvent *, unsigned long);

int cx_broker(unsigned long mask, void (*callback)(XEvent *, unsigned long))
{
  md_broker_func=callback;
  return md_command0(md_root, MCMD_SET_BROKER, &mask, sizeof(mask));
}

int cx_send_event(unsigned long mask, XEvent *e)
{
  struct mcmd_event me;
  me.mask=mask;
  me.event=*e;
  return md_command0(md_root, MCMD_SEND_EVENT, &me, sizeof(me));
}
