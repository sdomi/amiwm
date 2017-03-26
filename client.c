#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drawinfo.h"
#include "screen.h"
#include "icon.h"
#include "client.h"
#include "icc.h"
#include "prefs.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

extern Display *dpy;
extern XContext client_context, screen_context;
extern Client *activeclient;
extern Scrn *menuactive;
extern void setfocus(Window);

Client *clients=NULL;

void grav_map_frame_to_win(Client *c, int x0, int y0, int *x, int *y)
{
  switch(c->gravity) {
  case EastGravity:
  case NorthEastGravity:
  case SouthEastGravity:
    *x=x0+c->framewidth-c->old_bw-c->old_bw; break;
  case CenterGravity:
  case NorthGravity:
  case SouthGravity:
    *x=x0+4-c->old_bw; break;
  case WestGravity:
  case NorthWestGravity:
  case SouthWestGravity:
  default:
    *x=x0; break;
  }
  switch(c->gravity) {
  case SouthGravity:
  case SouthEastGravity:
  case SouthWestGravity:
    *y=y0+c->frameheight-c->old_bw-c->old_bw; break;
  case CenterGravity:
  case EastGravity:
  case WestGravity:
    *y=y0+c->scr->bh-c->old_bw; break;
  case NorthGravity:
  case NorthEastGravity:
  case NorthWestGravity:
  default:
    *y=y0; break;
  }
}

void grav_map_win_to_frame(Client *c, int x0, int y0, int *x, int *y)
{
  switch(c->gravity) {
  case EastGravity:
  case NorthEastGravity:
  case SouthEastGravity:
    *x=x0-c->framewidth+c->old_bw+c->old_bw; break;
  case CenterGravity:
  case NorthGravity:
  case SouthGravity:
    *x=x0-4+c->old_bw; break;
  case WestGravity:
  case NorthWestGravity:
  case SouthWestGravity:
  default:
    *x=x0; break;
  }
  switch(c->gravity) {
  case SouthGravity:
  case SouthEastGravity:
  case SouthWestGravity:
    *y=y0-c->frameheight+c->old_bw+c->old_bw; break;
  case CenterGravity:
  case EastGravity:
  case WestGravity:
    *y=y0-c->scr->bh+c->old_bw; break;
  case NorthGravity:
  case NorthEastGravity:
  case NorthWestGravity:
  default:
    *y=y0; break;
  }
}

void sendconfig(Client *c)
{
  XConfigureEvent ce;

  ce.type = ConfigureNotify;
  ce.event = c->window;
  ce.window = c->window;
/*
   grav_map_frame_to_win(c, c->x, c->y, &ce.x, &ce.y);
*/
  ce.x = c->x+4;
#ifndef ASSIMILATE_WINDOWS
  ce.y = c->y+c->scr->bh+c->scr->y;
#else
  ce.y = c->y+c->scr->bh;
#endif
  ce.width = c->pwidth-c->framewidth;
  ce.height = c->pheight-c->frameheight;
  ce.border_width = c->old_bw;
  ce.above = None;
  ce.override_redirect = 0;
  XSendEvent(dpy, c->window, False, StructureNotifyMask, (XEvent*)&ce);
  XSync(dpy, False);
}

void scrsendconfig(Scrn *s)
{
  Client *c;
  for (c=clients; c; c = c->next)
    if(c->scr == s)
      sendconfig(c);
}

void checksizehints(Client *c)
{
  long supplied;

  XGetWMNormalHints(dpy, c->window, &c->sizehints, &supplied);
  if(!(c->sizehints.flags&PMinSize))
    c->sizehints.min_width=c->sizehints.min_height=0;
  if(!(c->sizehints.flags&PMaxSize))
    c->sizehints.max_width=c->sizehints.max_height=1<<30;
  if(!(c->sizehints.flags&PResizeInc))
    c->sizehints.width_inc=c->sizehints.height_inc=1;
  if(c->sizehints.flags&PBaseSize) {
    c->sizehints.min_width=c->sizehints.base_width;
    c->sizehints.min_height=c->sizehints.base_height;
  }
  if(c->sizehints.min_width<1) c->sizehints.min_width=1;
  if(c->sizehints.min_height<1) c->sizehints.min_height=1;
  c->sizehints.base_width=c->sizehints.min_width;
  c->sizehints.base_height=c->sizehints.min_height;
  if(c->sizehints.flags&PWinGravity) c->gravity=c->sizehints.win_gravity;
}

void setclientstate(Client *c, int state)
{
  long data[2];

  data[0] = (long) state;
  data[1] = (long) None;

  c->state = state;
  XChangeProperty(dpy, c->window, wm_state, wm_state, 32,
		  PropModeReplace, (unsigned char *)data, 2);
}

void getstate(Client *c)
{
  long *data=NULL;

  if(_getprop(c->window, wm_state, wm_state, 2l, (char **)&data)>0) {
    c->state=*data;
    XFree((char *)data);
  }
}

Client *createclient(Window w)
{
  extern void checkstyle(Client *c);

  XWindowAttributes attr;
  Client *c;

  if(w==0) return 0;
  if(!XFindContext(dpy, w, client_context, (XPointer*)&c)) return c;

  XGetWindowAttributes(dpy, w, &attr);

  c = (Client *)calloc(1, sizeof(Client));
  c->scr = scr;
  c->window = w;
  c->parent = scr->root;
  c->old_bw = attr.border_width;
  c->next = clients;
  c->state = WithdrawnState;
  c->gravity = NorthWestGravity;
  c->reparenting = 0;
  XSelectInput(dpy, c->window, PropertyChangeMask);
#ifdef USE_FONTSETS
  {
    XTextProperty prop;
    c->title = NULL;
    if(XGetWMName(dpy, c->window, &prop) && prop.value) {
      char **list;
      int n;
      if(XmbTextPropertyToTextList(dpy, &prop, &list, &n) >= Success) {
	if(n > 0)
	  c->title = strdup(list[0]);
	XFreeStringList(list);
      }
      XFree(prop.value);
    }
  }
#else
  XGetWMName(dpy, c->window, &c->title);
#endif
  c->style = NULL;
  checkstyle(c);
  checksizehints(c);
  c->zoomx=0;
  c->zoomy=scr->bh;
  if(c->sizehints.width_inc) {
    c->zoomw=scr->width-c->sizehints.base_width-22;
    c->zoomw-=c->zoomw%c->sizehints.width_inc;
    c->zoomw+=c->sizehints.base_width;
    if(c->zoomw>c->sizehints.max_width)
      c->zoomw=c->sizehints.max_width;
    if(c->zoomw<c->sizehints.min_width)
      c->zoomw=c->sizehints.min_width;
  } else
    c->zoomw=attr.width;
  if(c->sizehints.height_inc) {
    c->zoomh=scr->height-c->sizehints.base_height-scr->bh-c->zoomy-2;
    c->zoomh-=c->zoomh%c->sizehints.height_inc;
    c->zoomh+=c->sizehints.base_height;
    if(c->zoomh>c->sizehints.max_height)
      c->zoomh=c->sizehints.max_height;
    if(c->zoomh<c->sizehints.min_height)
      c->zoomh=c->sizehints.min_height;
  } else
    c->zoomh=attr.height;
  XSaveContext(dpy, w, client_context, (XPointer)c);
  return clients = c;
}

void rmclient(Client *c)
{
    Client *cc;

    if (c == clients)
      clients = c->next;
    else
      if((cc = clients))
	for (; cc->next; cc = cc->next)
	  if (cc->next == c) {
            cc->next = cc->next->next;
	    break;
	  }

    if(c->active) {
      if(!menuactive)
	setfocus(None);
      c->active=False;
      activeclient = NULL;
      XInstallColormap(dpy, scr->cmap);
    } else if(prefs.focus==FOC_CLICKTOTYPE)
      XUngrabButton(dpy, Button1, AnyModifier, c->parent);
#ifdef USE_FONTSETS
    if(c->title)
      free(c->title);
#else
    if(c->title.value)
      XFree(c->title.value);
#endif
    if(c->parent != c->scr->root) {
      XDestroyWindow(dpy, c->parent);
      XDeleteContext(dpy, c->parent, client_context);
    }
    if(c->close)
      XDeleteContext(dpy, c->close, client_context);
    if(c->drag)
      XDeleteContext(dpy, c->drag, client_context);
    if(c->iconify)
      XDeleteContext(dpy, c->iconify, client_context);
    if(c->zoom)
      XDeleteContext(dpy, c->zoom, client_context);
    if(c->depth)
      XDeleteContext(dpy, c->depth, client_context);
    if(c->resize)
      XDeleteContext(dpy, c->resize, client_context);
    if(c->icon)
      rmicon(c->icon);
    if(c->window)
      XDeleteContext(dpy, c->window, client_context);
    free(c);
}

int screen_has_clients()
{
  int n = 0;
  Client *c = clients;
  while(c) {
    if(c->scr == scr)
      n++;
    c = c->next;
  }
  return n;
}

void flushclients()
{
  unsigned int i, nwins;
  Window dw1, dw2, *wins;
  Client *c;
#ifdef ASSIMILATE_WINDOWS
  Scrn *scr2;
#endif

  if((scr = front)) do {
    scr = scr->upfront;
    XQueryTree(dpy, scr->back, &dw1, &dw2, &wins, &nwins);
    for(i=0; i<nwins; i++)
      if((!XFindContext(dpy, wins[i], client_context, (XPointer *)&c))&&wins[i]==c->parent) {
	int x,y;
	grav_map_frame_to_win(c, c->x, c->y, &x, &y);
	XReparentWindow(dpy, c->window, scr->root, x, y);
	XSetWindowBorderWidth(dpy, c->window, c->old_bw);
	XRemoveFromSaveSet(dpy, c->window);
	wins[i]=c->window;
	rmclient(c);
      }
#ifdef ASSIMILATE_WINDOWS
      else if((!XFindContext(dpy, wins[i], screen_context, (XPointer *)&scr2)) && scr2==scr) {
	XWindowAttributes attr;
	XSetWindowAttributes xsa;
	XGetWindowAttributes(dpy, wins[i], &attr);
	XReparentWindow(dpy, wins[i], scr->root, attr.x, attr.y);
	xsa.override_redirect = True;
	XChangeWindowAttributes(dpy, wins[i], CWOverrideRedirect, &xsa);
	XDeleteContext(dpy, wins[i], screen_context);
	XRemoveFromSaveSet(dpy, wins[i]);
      }
#endif
    /*
      if(nwins) {
      for(i=0; i<(nwins>>1); i++) {
      Window w=wins[i];
      wins[i]=wins[nwins-1-i];
      wins[nwins-1-i]=w;
      }
      XRestackWindows(dpy, wins, nwins);
      }
      */
    XFree((void *) wins);
  } while(scr!=front);
  while((c=clients)) {
    if(c->parent != c->scr->root) {
      int x,y;
      grav_map_frame_to_win(c, c->x, c->y, &x, &y);
      XReparentWindow(dpy, c->window, c->scr->root, x, y);
      XSetWindowBorderWidth(dpy, c->window, c->old_bw);
      XRemoveFromSaveSet(dpy, c->window);
    }
    rmclient(c);
  }
}
