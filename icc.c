#include "drawinfo.h"
#include "screen.h"
#include "icc.h"
#include "icon.h"
#include "style.h"
#include "prefs.h"

#include <string.h>

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

extern void redraw(Client *, Window);

Atom wm_state, wm_change_state, wm_protocols, wm_delete, wm_take_focus;
Atom wm_colormaps, wm_name, wm_normal_hints, wm_hints, wm_icon_name, wm_class;
Atom amiwm_screen, swm_vroot, amiwm_wflags, amiwm_appiconmsg, amiwm_appwindowmsg;

extern Display *dpy;

void init_atoms()
{
  wm_state = XInternAtom(dpy, "WM_STATE", False);
  wm_change_state = XInternAtom(dpy, "WM_CHANGE_STATE", False);
  wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
  wm_delete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
  wm_take_focus = XInternAtom(dpy, "WM_TAKE_FOCUS", False);
  wm_colormaps = XInternAtom(dpy, "WM_COLORMAP_WINDOWS", False);
  wm_name = XInternAtom(dpy, "WM_NAME", False);
  wm_normal_hints = XInternAtom(dpy, "WM_NORMAL_HINTS", False);
  wm_hints = XInternAtom(dpy, "WM_HINTS", False);
  wm_icon_name = XInternAtom(dpy, "WM_ICON_NAME", False);
  wm_class = XInternAtom(dpy, "WM_CLASS", False);
  amiwm_screen = XInternAtom(dpy, "AMIWM_SCREEN", False);
  swm_vroot = XInternAtom(dpy, "__SWM_VROOT", False);
  amiwm_wflags = XInternAtom(dpy, "AMIWM_WFLAGS", False);
  amiwm_appiconmsg = XInternAtom(dpy, "AMIWM_APPICONMSG", False);
  amiwm_appwindowmsg = XInternAtom(dpy, "AMIWM_APPWINDOWMSG", False);
}

void setstringprop(Window w, Atom a, char *str)
{
    XTextProperty txtp;

    txtp.value=(unsigned char *)str;
    txtp.encoding=XA_STRING;
    txtp.format=8;
    txtp.nitems=strlen(str);
    XSetTextProperty(dpy, w, &txtp, a);
}

XEvent *mkcmessage(Window w, Atom a, long x)
{
  static XEvent ev;

  memset(&ev, 0, sizeof(ev));
  ev.xclient.type = ClientMessage;
  ev.xclient.window = w;
  ev.xclient.message_type = a;
  ev.xclient.format = 32;
  ev.xclient.data.l[0] = x;
  ev.xclient.data.l[1] = CurrentTime;
  return &ev;
}

void sendcmessage(Window w, Atom a, long x)
{
  if(!(XSendEvent(dpy, w, False, 0L, mkcmessage(w, a, x))))
    XBell(dpy, 100);
}

long _getprop(Window w, Atom a, Atom type, long len, char **p)
{
  Atom real_type;
  int format;
  unsigned long n, extra;
  int status;
  
  status = XGetWindowProperty(dpy, w, a, 0L, len, False, type, &real_type, &format,
			      &n, &extra, (unsigned char **)p);
  if (status != Success || *p == 0)
    return -1;
  if (n == 0)
    XFree((void*) *p);
  return n;
}

void getwflags(Client *c)
{
  BITS32 *p;
  long n;

  c->wflags = 0;
  if ((n = _getprop(c->window, amiwm_wflags, amiwm_wflags, 1L, (char**)&p)) <= 0)
    return;

  c->wflags = p[0];
  
  XFree((char *) p);
}

void getproto(Client *c)
{
  Atom *p;
  int i;
  long n;
  Window w;

  w = c->window;
  c->proto &= ~(Pdelete|Ptakefocus);
  if ((n = _getprop(w, wm_protocols, XA_ATOM, 20L, (char**)&p)) <= 0)
    return;

  for (i = 0; i < n; i++)
    if (p[i] == wm_delete)
      c->proto |= Pdelete;
    else if (p[i] == wm_take_focus)
      c->proto |= Ptakefocus;
  
  XFree((char *) p);
}

static int stylematch_low(char *p, int l, char *m)
{
  char *lf;
  int ml;
  --m;
  do {
    lf = strchr(++m, '\n');
    ml = (lf? lf-m:strlen(m));
    if(ml == l && !strncmp(m, p, ml))
      return 1;
  } while((m=lf));
  return 0;
}

static int stylematch_tprop(XTextProperty *p, char *m)
{
  return stylematch_low((char *)p->value, p->nitems, m);
}

#ifdef USE_FONTSETS
static int stylematch_str(char *p, char *m)
{
  return stylematch_low(p, strlen(p), m);
}
#endif

void checkstyle(Client *c)
{
  XTextProperty icon_name, class_name;
  Style *style;

  if(prefs.firststyle==NULL)
    return;

  if(!XGetTextProperty(dpy, c->window, &class_name, wm_class))
    class_name.value=NULL;
  else
    /* This value seems to be 2x it's correct value always... */
    class_name.nitems=strlen((char *)class_name.value);
  if(!XGetWMIconName(dpy, c->window, &icon_name))
    icon_name.value=NULL;

  for(style=prefs.firststyle; style!=NULL; style=style->next)
    if((class_name.value!=NULL && style->style_class!=NULL &&
	stylematch_tprop(&class_name, style->style_class)) ||
#ifdef USE_FONTSETS
       (c->title!=NULL && style->style_title!=NULL &&
	stylematch_str(c->title, style->style_title)) ||
#else
       (c->title.value!=NULL && style->style_title!=NULL &&
	stylematch_tprop(&c->title, style->style_title)) ||
#endif
       (icon_name.value!=NULL && style->style_icon_title!=NULL &&
	stylematch_tprop(&icon_name, style->style_icon_title))) {
      c->style = style;
      break;
    }

  if(icon_name.value)
    XFree(icon_name.value);
  if(class_name.value)
    XFree(class_name.value);
}

void propertychange(Client *c, Atom a)
{
  extern void checksizehints(Client *);
  extern void newicontitle(Client *);

  if(a==wm_name) {
#ifdef USE_FONTSETS
    XTextProperty prop;
    if(c->title) {
      free(c->title);
      c->title = NULL;
    }
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
#else
    if(c->title.value)
      XFree(c->title.value);
    XGetWMName(dpy, c->window, &c->title);
#endif
    if(c->style==NULL)
      checkstyle(c);
    if(c->drag) {
      XClearWindow(dpy, c->drag);
      redraw(c, c->drag);
    }
  } else if(a==wm_normal_hints) {
    checksizehints(c);
  } else if(a==wm_hints) {
    XWMHints *xwmh;
    if((xwmh=XGetWMHints(dpy, c->window))) {
      if((xwmh->flags&(IconWindowHint|IconPixmapHint))&&c->icon) {
	destroyiconicon(c->icon);
	createiconicon(c->icon, xwmh);
      }
      if((xwmh->flags&IconPositionHint)&&c->icon&&c->icon->window) {
	XMoveWindow(dpy, c->icon->window, c->icon->x=xwmh->icon_x,
		    c->icon->y=xwmh->icon_y);
	adjusticon(c->icon);
      }
      XFree(xwmh);
    }
  } else if(a==wm_protocols) {
    getproto(c);
  } else if(a==wm_icon_name) {
    if(c->style==NULL)
      checkstyle(c);
    if(c->icon) newicontitle(c);
  } else if(a==wm_state) {
    if(c->parent==c->scr->root) {
      getstate(c);
      if(c->state==NormalState)
	c->state=WithdrawnState;
    }
  } else if(a==wm_class && c->style==NULL)
    checkstyle(c);
}

void handle_client_message(Client *c, XClientMessageEvent *xcme)
{
  if(xcme->message_type == wm_change_state) {
    int state=xcme->data.l[0];
    if(state==IconicState)
      if(c->state!=IconicState) {
	if(!(c->icon))
	  createicon(c);
	XUnmapWindow(dpy, c->parent);
	/*	XUnmapWindow(dpy, c->window); */
	adjusticon(c->icon);
	XMapWindow(dpy, c->icon->window);
	if(c->icon->labelwidth)
	  XMapWindow(dpy, c->icon->labelwin);
	c->icon->mapped=1;
	setclientstate(c, IconicState);
      } else ;
    else
      if(c->state==IconicState && c->icon) {
	Icon *i=c->icon;
	if(i->labelwin)
	  XUnmapWindow(dpy, i->labelwin);
	if(i->window)
	  XUnmapWindow(dpy, i->window);
	i->mapped=0;
	deselecticon(i);
	XMapWindow(dpy, c->window);
	if(c->parent!=c->scr->root)
	  XMapRaised(dpy, c->parent);
	setclientstate(c, NormalState);
      }
  }
}
