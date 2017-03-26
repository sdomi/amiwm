#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#ifdef HAVE_X11_EXTENSIONS_SHAPE_H
#include <X11/extensions/shape.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drawinfo.h"
#include "screen.h"
#include "icon.h"
#include "client.h"
#include "icc.h"
#include "prefs.h"
#include "module.h"
#include "libami.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

#define mx(a,b) ((a)>(b)?(a):(b))

extern Display *dpy;
extern XContext client_context, screen_context;
extern Cursor wm_curs;
extern int shape_extn;
extern void redrawmenubar(Window);
void reshape_frame(Client *c);

Window creategadget(Client *c, Window p, int x, int y, int w, int h)
{
  XSetWindowAttributes attr;
  Window r;

  attr.override_redirect=True;
  attr.background_pixel=scr->dri.dri_Pens[BACKGROUNDPEN];
  r=XCreateWindow(dpy, p, x, y, w, h, 0, CopyFromParent, InputOutput,
		  CopyFromParent, CWOverrideRedirect|CWBackPixel, &attr);
  XSelectInput(dpy, r, ExposureMask|ButtonPressMask|ButtonReleaseMask|
	       EnterWindowMask|LeaveWindowMask);
  XSaveContext(dpy, r, client_context, (XPointer)c);
  return r;
}

void spread_top_gadgets(Client *c)
{
  int w=c->pwidth;
  w-=23; XMoveWindow(dpy, c->depth, w, 0);
  if(c->zoom) {
    w-=23; XMoveWindow(dpy, c->zoom, w, 0);
  }
  if(c->iconify) {
    w-=23; XMoveWindow(dpy, c->iconify, w, 0);
  }
  XResizeWindow(dpy, c->drag, c->dragw=mx(4, w-19), scr->bh);
}

void setclientborder(Client *c, int old, int new)
{
  int wc=0;
  int x=c->x, y=c->y, oldpw=c->pwidth, oldph=c->pheight;
  old&=(Psizeright|Psizebottom|Psizetrans);
  new&=(Psizeright|Psizebottom|Psizetrans);
  if(new!=old) {
    if((new & Psizeright)&&!(old & Psizeright))
      { c->pwidth+=14; wc++; }
    else if((old & Psizeright)&&!(new & Psizeright))
      { c->pwidth-=14; wc++; }
    if((new & Psizebottom)&&!(old & Psizebottom))
      c->pheight+=8;
    else if((old & Psizebottom)&&!(new & Psizebottom))
      c->pheight-=8;
    XResizeWindow(dpy, c->parent, c->pwidth, c->pheight);
    if(c->resize)
      XMoveWindow(dpy, c->resize, c->pwidth-18, c->pheight-10);
    if(wc) 
      spread_top_gadgets(c);
  }
  c->proto = (c->proto&~(Psizeright|Psizebottom|Psizetrans)) | new;
  c->framewidth=((new&Psizeright)?22:8);
  c->frameheight=scr->bh+((new&Psizebottom)?10:2);
  if(c->gravity==EastGravity || c->gravity==NorthEastGravity ||
     c->gravity==SouthEastGravity)
    x-=(c->pwidth-oldpw);
  if(c->gravity==SouthGravity || c->gravity==SouthEastGravity ||
     c->gravity==SouthWestGravity)
    y-=(c->pheight-oldph);
  if(x!=c->x || y!=c->y)
    XMoveWindow(dpy, c->parent, c->x=x, c->y=y);
}

void resizeclientwindow(Client *c, int width, int height)
{
  if(width!=c->pwidth || height!=c->pheight) {
    int old_width=c->pwidth;
    XResizeWindow(dpy, c->parent, c->pwidth=width, c->pheight=height);
    if(width!=old_width)
      spread_top_gadgets(c);
    if(c->resize)
      XMoveWindow(dpy, c->resize, c->pwidth-18, c->pheight-10);
    XResizeWindow(dpy, c->window, c->pwidth-c->framewidth, c->pheight-c->frameheight);
    if(c->shaped)
      reshape_frame(c);
    sendconfig(c);
  }
}

static int resizable(XSizeHints *xsh)
{
  int r=0;
  if(xsh->width_inc) {
    if(xsh->min_width+xsh->width_inc<=xsh->max_width)
      r++;
  }
  if(xsh->height_inc) {
    if(xsh->min_height+xsh->height_inc<=xsh->max_height)
      r+=2;
  }
  return r;
}

extern unsigned int meta_mask;

void reshape_frame(Client *c)
{
  XRectangle r;

#ifdef HAVE_XSHAPE
  if(!shape_extn)
    return;

  XShapeCombineShape(dpy, c->parent, ShapeBounding, 4, c->scr->bh, c->window,
		     ShapeBounding, ShapeSet);

  r.x = 0;
  r.y = 0;
  r.width = c->pwidth;
  r.height = c->scr->bh;

  XShapeCombineRectangles(dpy, c->parent, ShapeBounding,
			  0, 0, &r, 1, ShapeUnion, Unsorted);

  if(c->proto & Psizeright) {
    r.x = c->pwidth-18;
    r.y = 0;
    r.width = 18;
    r.height = c->pheight;
    XShapeCombineRectangles(dpy, c->parent, ShapeBounding,
			    0, 0, &r, 1, ShapeUnion, Unsorted);
  }
  if(c->proto & Psizebottom) {
    r.x = 0;
    r.y = c->pheight-10;
    r.width = c->pwidth;
    r.height = 10;
    XShapeCombineRectangles(dpy, c->parent, ShapeBounding,
			    0, 0, &r, 1, ShapeUnion, Unsorted);
  }
#endif
}

void redrawclient(Client *);

void reparent(Client *c)
{
  XWindowAttributes attr;
  XSetWindowAttributes attr2;
  XTextProperty screen_prop;
  Scrn *s=scr;
  int cb;
  Window leader;
  Client *lc;
  struct mcmd_keygrab *kg;
  extern struct mcmd_keygrab *keygrabs;
  char **cargv = NULL;
  int cargc;

  if(XGetTransientForHint(dpy, c->window, &leader) &&
     !XFindContext(dpy, leader, client_context, (XPointer *)&lc))
    c->scr = lc->scr;
  else if(XGetTextProperty(dpy, c->window, &screen_prop, amiwm_screen)) {
    do {
      if(s->root == scr->root &&
	 (!s->deftitle[screen_prop.nitems])&&!strncmp(s->deftitle,
		      (char *)screen_prop.value, screen_prop.nitems))
	break;
      s=s->behind;
    } while(s!=scr);
    XFree(screen_prop.value);
    c->scr=s;
  } else if(XGetCommand(dpy, c->window, &cargv, &cargc)) {
    XrmDatabase db = (XrmDatabase)NULL;
    XrmValue value;
    char *str_type;
    static XrmOptionDescRec table [] = {
      {"-xrm", NULL, XrmoptionResArg, (caddr_t) NULL},
    };
    XrmParseCommand(&db, table, 1, "amiwm", &cargc, cargv);
    if(XrmGetResource(db, "amiwm.screen", "Amiwm.Screen",
		      &str_type, &value) == True &&
       value.size != 0) {
      do {
	if(s->root == scr->root &&
	   (!s->deftitle[value.size])&&!strncmp(s->deftitle, value.addr,
						value.size))
	  break;
	s=s->behind;
      } while(s!=scr);
      c->scr=s;
    }
    XrmDestroyDatabase (db);
  }
  scr=c->scr;
  if(c->parent && c->parent != scr->root)
    return;
  getproto(c);
  getwflags(c);
  XAddToSaveSet(dpy, c->window);
  XGetWindowAttributes(dpy, c->window, &attr);
  c->colormap = attr.colormap;
  c->old_bw = attr.border_width;
  c->framewidth=8;
  c->frameheight=scr->bh+2;
  attr2.override_redirect=True;
  grav_map_win_to_frame(c, attr.x, attr.y, &c->x, &c->y);
  c->parent=XCreateWindow(dpy, scr->back, c->x, c->y,
			  c->pwidth=attr.width+8, c->pheight=attr.height+2+scr->bh,
			  0, CopyFromParent, InputOutput, CopyFromParent,
			  CWOverrideRedirect, &attr2);
  XSaveContext(dpy, c->parent, client_context, (XPointer)c);
  XSaveContext(dpy, c->parent, screen_context, (XPointer)c->scr);
  XSetWindowBackground(dpy, c->parent, scr->dri.dri_Pens[BACKGROUNDPEN]);
  XSetWindowBorderWidth(dpy, c->window, 0);
  XReparentWindow(dpy, c->window, c->parent, 4, scr->bh);
  XSelectInput(dpy, c->window, EnterWindowMask | LeaveWindowMask |
	       ColormapChangeMask | PropertyChangeMask |
	       (c->module? SubstructureNotifyMask:0));
#ifdef HAVE_XSHAPE
  if(shape_extn)
    XShapeSelectInput(dpy, c->window, ShapeNotifyMask);
#endif
  XSelectInput(dpy, c->parent, SubstructureRedirectMask |
	       SubstructureNotifyMask | StructureNotifyMask | ExposureMask |
	       EnterWindowMask | LeaveWindowMask | ButtonPressMask);
  for(kg=keygrabs; kg; kg=kg->next)
    XGrabKey(dpy, kg->keycode, kg->modifiers, c->window, False, GrabModeAsync,
	     GrabModeAsync);
  cb=(resizable(&c->sizehints)? prefs.sizeborder:0);
  c->close=creategadget(c, c->parent, 0, 0, 19, scr->bh);
  c->drag=creategadget(c, c->parent, 19, 0, 1, 1);
  if(c->wflags&WF_NOICONIFY)
    c->iconify=None;
  else
    c->iconify=creategadget(c, c->parent, 0, 0, 23, scr->bh);
  if(cb)
    c->zoom=creategadget(c, c->parent, 0, 0, 23, scr->bh);
  c->depth=creategadget(c, c->parent, 0, 0, 23, scr->bh);
  spread_top_gadgets(c);
  setclientborder(c, 0, cb);
  if(cb)
    c->resize=creategadget(c, c->parent, c->pwidth-18, c->pheight-10, 18, 10);
  c->shaped = 0;
#ifdef HAVE_XSHAPE
  if(shape_extn) {
    int xbs, ybs, cShaped, xcs, ycs;
    unsigned int wbs, hbs, wcs, hcs;
    XShapeQueryExtents (dpy, c->window, &c->shaped, &xbs, &ybs, &wbs, &hbs,
			&cShaped, &xcs, &ycs, &wcs, &hcs);
    if(c->shaped)
      reshape_frame(c);
  }
#endif
  XMapSubwindows(dpy, c->parent);
  sendconfig(c);
  setstringprop(c->window, amiwm_screen, scr->deftitle);
  if(prefs.focus == FOC_CLICKTOTYPE)
    XGrabButton(dpy, Button1, AnyModifier, c->parent, True,
		ButtonPressMask, GrabModeSync, GrabModeAsync, None, wm_curs);
}

void redraw(Client *c, Window w)
{
  if(w==c->parent) {
    int x, y;
    x=c->pwidth-((c->proto&Psizeright)?18:4);
    y=c->pheight-((c->proto&Psizebottom)?10:2);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHINEPEN]);
    XDrawLine(dpy, w, scr->gc, 0, 0, c->pwidth-1, 0);
    XDrawLine(dpy, w, scr->gc, 0, 0, 0, c->pheight-1);
    XDrawLine(dpy, w, scr->gc, 4, y, x, y);
    XDrawLine(dpy, w, scr->gc, x, scr->bh, x, y);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
    XDrawLine(dpy, w, scr->gc, 1, c->pheight-1, c->pwidth-1, c->pheight-1);
    XDrawLine(dpy, w, scr->gc, c->pwidth-1, 1, c->pwidth-1, c->pheight-1);
    XDrawLine(dpy, w, scr->gc, 3, scr->bh-1, 3, y);
    XDrawLine(dpy, w, scr->gc, 3, scr->bh-1, x, scr->bh-1);
  } else if(w==c->close) {
    if(c->active) {
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHINEPEN]);
      XFillRectangle(dpy, w, scr->gc, 7, scr->h3, 4, scr->h7-scr->h3);
    }
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
    XDrawRectangle(dpy, w, scr->gc, 7, scr->h3, 4, scr->h7-scr->h3);
    if(c->clicked==w) {
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
      XDrawLine(dpy, w, scr->gc, 0, 0, 18, 0);
      XDrawLine(dpy, w, scr->gc, 0, 0, 0, scr->bh-2);
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHINEPEN]);
      XDrawLine(dpy, w, scr->gc, 0, scr->bh-1, 18, scr->bh-1);
      XDrawLine(dpy, w, scr->gc, 18, 1, 18, scr->bh-1);
    } else {
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHINEPEN]);
      XDrawLine(dpy, w, scr->gc, 0, 0, 18, 0);
      XDrawLine(dpy, w, scr->gc, 0, 0, 0, scr->bh-1);
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
      XDrawLine(dpy, w, scr->gc, 1, scr->bh-1, 18, scr->bh-1);
      XDrawLine(dpy, w, scr->gc, 18, 1, 18, scr->bh-1);    
    }
  } else if(w==c->drag) {
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->active?FILLTEXTPEN:TEXTPEN]);
    XSetBackground(dpy, scr->gc, scr->dri.dri_Pens[c->active?FILLPEN:BACKGROUNDPEN]);
#ifdef USE_FONTSETS
    if(c->title)
      XmbDrawImageString(dpy, w, scr->dri.dri_FontSet, scr->gc,
			 11, 1+scr->dri.dri_Ascent,
			 c->title, strlen(c->title));
#else
    if(c->title.value)
      XDrawImageString(dpy, w, scr->gc, 11, 1+scr->dri.dri_Ascent,
		       (char *)c->title.value, c->title.nitems);
#endif
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHINEPEN]);
    XDrawLine(dpy, w, scr->gc, 0, 0, c->dragw-1, 0);
    XDrawLine(dpy, w, scr->gc, 0, 0, 0, scr->bh-2);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
    XDrawLine(dpy, w, scr->gc, 0, scr->bh-1, c->dragw-1, scr->bh-1);
    XDrawLine(dpy, w, scr->gc, c->dragw-1, 1, c->dragw-1, scr->bh-1);
  } else if(w==c->iconify) {
    if(c->active) {
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHINEPEN]);
      XFillRectangle(dpy, w, scr->gc, 7, scr->h8-4, 4, 2);
    }
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
    XDrawRectangle(dpy, w, scr->gc, 7, scr->h8-4, 4, 2);
    if(c->clicked==w) {
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->active? FILLPEN:BACKGROUNDPEN]);
      XDrawRectangle(dpy, w, scr->gc, 5, scr->h2, 12, scr->h8-scr->h2);
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
      XDrawRectangle(dpy, w, scr->gc, 5, scr->h8-(scr->bh>11? 7:6), 9, (scr->bh>11? 7:6));
    } else {
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->active? FILLPEN:BACKGROUNDPEN]);
      XDrawRectangle(dpy, w, scr->gc, 5, scr->h8-(scr->bh>11? 7:6), 9, (scr->bh>11? 7:6));
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
      XDrawRectangle(dpy, w, scr->gc, 5, scr->h2, 12, scr->h8-scr->h2);
    }
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->clicked==w?SHADOWPEN:SHINEPEN]);
    XDrawLine(dpy, w, scr->gc, 0, 0, 22, 0);
    XDrawLine(dpy, w, scr->gc, 0, 0, 0, scr->bh-2);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->clicked==w?SHINEPEN:SHADOWPEN]);
    XDrawLine(dpy, w, scr->gc, 0, scr->bh-1, 22, scr->bh-1);
    XDrawLine(dpy, w, scr->gc, 22, 0, 22, scr->bh-1);
  } else if(w==c->zoom) {
    if(c->active) {
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->clicked==w? SHINEPEN:FILLPEN]);
      XFillRectangle(dpy, w, scr->gc, 5, scr->h2, 12, scr->h8-scr->h2);
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->clicked==w? FILLPEN:SHINEPEN]);
      XFillRectangle(dpy, w, scr->gc, 6, scr->h2, 5, scr->h5-scr->h2);
    }
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
    XDrawRectangle(dpy, w, scr->gc, 6, scr->h2, 5, scr->h5-scr->h2);
    XDrawRectangle(dpy, w, scr->gc, 5, scr->h2, 7, scr->h5-scr->h2);
    XDrawRectangle(dpy, w, scr->gc, 5, scr->h2, 12, scr->h8-scr->h2);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->clicked==w? SHADOWPEN:SHINEPEN]);
    XDrawLine(dpy, w, scr->gc, 0, 0, 22, 0);
    XDrawLine(dpy, w, scr->gc, 0, 0, 0, scr->bh-2);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->clicked==w? SHINEPEN:SHADOWPEN]);
    XDrawLine(dpy, w, scr->gc, 0, scr->bh-1, 22, scr->bh-1);
    XDrawLine(dpy, w, scr->gc, 22, 0, 22, scr->bh-1);
  } else if(w==c->depth) {
    if(c->active) {
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[BACKGROUNDPEN]);
      XFillRectangle(dpy, w, scr->gc, 4, scr->h2, 10, scr->h6-scr->h2);
    }
    if(c->clicked!=w) {
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
      XDrawRectangle(dpy, w, scr->gc, 4, scr->h2, 10, scr->h6-scr->h2);
    }
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->active?SHINEPEN:BACKGROUNDPEN]);
    XFillRectangle(dpy, w, scr->gc, 8, scr->h4, 10, scr->h8-scr->h4);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
    XDrawRectangle(dpy, w, scr->gc, 8, scr->h4, 10, scr->h8-scr->h4);
    if(c->clicked==w)
      XDrawRectangle(dpy, w, scr->gc, 4, scr->h2, 10, scr->h6-scr->h2);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->clicked==w? SHADOWPEN:SHINEPEN]);
    XDrawLine(dpy, w, scr->gc, 0, 0, 22, 0);
    XDrawLine(dpy, w, scr->gc, 0, 0, 0, scr->bh-2);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[c->clicked==w? SHINEPEN:SHADOWPEN]);
    XDrawLine(dpy, w, scr->gc, 0, scr->bh-1, 22, scr->bh-1);
    XDrawLine(dpy, w, scr->gc, 22, 0, 22, scr->bh-1);
  } else if(w==c->resize) {
    static XPoint points[]={{4,6},{13,2},{14,2},{14,7},{4,7}};
    if(c->active) {
      XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHINEPEN]);
      XFillPolygon(dpy, w, scr->gc, points, sizeof(points)/sizeof(points[0]), Convex, CoordModeOrigin);
    }
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
    XDrawLines(dpy, w, scr->gc, points, sizeof(points)/sizeof(points[0]), CoordModeOrigin);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHINEPEN]);
    XDrawLine(dpy, w, scr->gc, 0, 0, 16, 0);
    XDrawLine(dpy, w, scr->gc, 0, 0, 0, 8);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
    XDrawLine(dpy, w, scr->gc, 0, 9, 17, 9);
    XDrawLine(dpy, w, scr->gc, 17, 0, 17, 9);
  }
}

void redrawclient(Client *c)
{
  unsigned long bgpix;

  scr=c->scr;
  if((!c->parent) || c->parent == scr->root)
    return;
  bgpix=scr->dri.dri_Pens[c->active?FILLPEN:BACKGROUNDPEN];
  XSetWindowBackground(dpy, c->parent, bgpix);
  XSetWindowBackground(dpy, c->close, bgpix);
  XSetWindowBackground(dpy, c->drag, bgpix);
  if(c->iconify)
    XSetWindowBackground(dpy, c->iconify, bgpix);
  if(c->zoom)
    XSetWindowBackground(dpy, c->zoom, bgpix);
  XSetWindowBackground(dpy, c->depth, bgpix);
  if(c->resize)
    XSetWindowBackground(dpy, c->resize, bgpix);
  XClearWindow(dpy, c->parent);
  XClearWindow(dpy, c->close);
  XClearWindow(dpy, c->drag);
  if(c->iconify)
    XClearWindow(dpy, c->iconify);
  if(c->zoom)
    XClearWindow(dpy, c->zoom);
  XClearWindow(dpy, c->depth);
  if(c->resize)
    XClearWindow(dpy, c->resize);
  redraw(c, c->parent);
  redraw(c, c->close);
  redraw(c, c->drag);
  if(c->iconify)
    redraw(c, c->iconify);
  if(c->zoom)
    redraw(c, c->zoom);
  redraw(c, c->depth);
  if(c->resize)
    redraw(c, c->resize);
}

extern Client *clickclient;
extern Window clickwindow;
extern Scrn *mbdclick, *mbdscr;

void clickenter()
{
  if((scr=mbdscr)&& clickwindow == scr->menubardepth) {
    mbdclick = scr;
    redrawmenubar(scr->menubardepth);
  } else {
    scr = clickclient->scr;
    redraw(clickclient, clickclient->clicked=clickwindow);
  }
}

void clickleave()
{
  if((scr=mbdscr)&& clickwindow == scr->menubardepth) {
    mbdclick = NULL;
    redrawmenubar(scr->menubardepth);
  } else {
    scr = clickclient->scr;
    clickclient->clicked=None;
    redraw(clickclient, clickwindow);
  }
}

void gadgetclicked(Client *c, Window w, XEvent *e)
{
  scr=c->scr;
  redraw(c, clickwindow=(clickclient=c)->clicked=w);
}

void gadgetaborted(Client *c)
{
  Window w;
  scr=c->scr;
  if((w=c->clicked)) {
    c->clicked=None;
    redraw(c, w);
  }
  clickwindow=None;
  clickclient=NULL;
}

static Client *topmostmappedclient(Window *children, unsigned int nchildren)
{
  int n;
  Client *c;
  for(n=nchildren-1; n>=0; --n)
    if((!XFindContext(dpy, children[n], client_context, (XPointer*)&c)) &&
       (children[n]==c->parent || children[n]==c->window) &&
       c->state==NormalState)
      return c;
  return NULL;
}

static Client *bottommostmappedclient(Window *children, unsigned int nchildren)
{
  int n;
  Client *c;
  for(n=0; n<nchildren; n++)
    if((!XFindContext(dpy, children[n], client_context, (XPointer*)&c)) &&
       children[n]==c->parent && c->state==NormalState)
      return c;
  return NULL;
}

void raiselowerclient(Client *c, int place)
{
  Window r,p,*children;
  unsigned int nchildren;
  if(place!=PlaceOnTop &&
     XQueryTree(dpy, scr->back, &r, &p, &children, &nchildren)) {
    if(place==PlaceOnBottom || topmostmappedclient(children, nchildren)==c) {
      Client *c2 = bottommostmappedclient(children, nchildren);
      if(c2 != NULL && c2 != c) {
	Window ws[2];
	ws[0]=c2->parent;
	ws[1]=c->parent;
	XRestackWindows(dpy, ws, 2);
      } else if(place!=PlaceOnBottom)
	XRaiseWindow(dpy, c->parent);
    } else
      XRaiseWindow(dpy, c->parent);
    if(children) XFree(children);
  } else XRaiseWindow(dpy, c->parent);
}

void gadgetunclicked(Client *c, XEvent *e)
{
  extern void adjusticon(Icon *);
  Window w;
  scr=c->scr;
  if((w=c->clicked)) {
    c->clicked=None;
    redraw(c, w);
    if(w==c->close) {
      if((c->proto & Pdelete)&&!(e->xbutton.state&ShiftMask))
	sendcmessage(c->window, wm_protocols, wm_delete);
      else
	XKillClient(dpy, c->window);
    } else if(w==c->depth)
      raiselowerclient(c, -1);
    else if(w==c->zoom) {
      XWindowAttributes xwa;
      XGetWindowAttributes(dpy, c->parent, &xwa);
      XMoveWindow(dpy, c->parent, c->x=c->zoomx, c->y=c->zoomy);
      resizeclientwindow(c, c->zoomw+c->framewidth, c->zoomh+c->frameheight);
      c->zoomx=xwa.x;
      c->zoomy=xwa.y;
      c->zoomw=xwa.width-c->framewidth;
      c->zoomh=xwa.height-c->frameheight;
/*      XWarpPointer(dpy, None, c->zoom, 0, 0, 0, 0, 23/2, scr->h5);  */
      sendconfig(c);
    } else if(w==c->iconify) {
      if(!(c->icon))
	createicon(c);
      XUnmapWindow(dpy, c->parent);
      /*      XUnmapWindow(dpy, c->window); */
      adjusticon(c->icon);
      XMapWindow(dpy, c->icon->window);
      if(c->icon->labelwidth)
	XMapWindow(dpy, c->icon->labelwin);
      c->icon->mapped=1;
      setclientstate(c, IconicState);
    }
  }
  clickwindow=None;
  clickclient=NULL;
}
