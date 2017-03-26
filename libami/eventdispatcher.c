#include "libami.h"

static int (*eventfunc[LASTEvent])(XEvent*);

static unsigned long eventmask=0;

static void eb_dispatch(XEvent *e, unsigned long mask)
{
  if(e->type<0 || e->type>=LASTEvent || (!eventfunc[e->type]) ||
     (!eventfunc[e->type](e)))
    cx_send_event(mask, e);
}

void cx_event_broker(int type, unsigned long mask, int (*callback)(XEvent*))
{
  if(type>=0 && type<LASTEvent) {
    eventfunc[type]=callback;
    cx_broker(eventmask|=mask, eb_dispatch);
  }
}

