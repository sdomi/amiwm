#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "drawinfo.h"
#include "screen.h"
#include "icon.h"
#include "client.h"
#include "prefs.h"
#include "icc.h"

extern Display *dpy;
extern Cursor wm_curs;
#ifdef USE_FONTSETS
extern XFontSet labelfontset;
extern int labelfont_ascent;
#else
extern XFontStruct *labelfont;
#endif
extern char *progname;
extern XContext screen_context, client_context, vroot_context;

extern void createmenubar();
extern void reparent(Client *);

Scrn *front = NULL, *scr = NULL;

static Scrn *getvroot(Window root)
{
  Scrn *s;
  if(!XFindContext(dpy, root, vroot_context, (XPointer *)&s))
    return s;
  return NULL;
}

static void setvroot(Window root, Scrn *s)
{
  XSaveContext(dpy, root, vroot_context, (XPointer)s);
}

void setvirtualroot(Scrn *s)
{
  if(s) {
    Scrn *old_vroot = getvroot(s->root);

    if(s==old_vroot) return;
    if(old_vroot)
      XDeleteProperty(dpy, old_vroot->back, swm_vroot);
    setvroot(s->root, s);
    XChangeProperty(dpy, s->back, swm_vroot, XA_WINDOW, 32, PropModeReplace,
		    (unsigned char *)&(s->back), 1);
  }
}

Scrn *getscreenbyroot(Window w);

void screentoback(void)
{
  Scrn *f;
  if((!scr)||(scr->back==scr->root)) return;
  if(scr==front) {
    XLowerWindow(dpy, scr->back);
    front=scr->behind;
  } else if(scr==getscreenbyroot(scr->root)) {
    XLowerWindow(dpy, scr->back);
    scr->upfront->behind=scr->behind;
    scr->behind->upfront=scr->upfront;
    scr->upfront=front->upfront;
    scr->behind=front;
    front->upfront->behind=scr;
    front->upfront=scr;
  } else if(scr->behind==front) {
    XRaiseWindow(dpy, scr->back);
    front=scr;
  } else {
    XRaiseWindow(dpy, scr->back);
    scr->upfront->behind=scr->behind;
    scr->behind->upfront=scr->upfront;
    scr->upfront=front->upfront;
    scr->behind=front;
    front->upfront->behind=scr;
    front->upfront=scr;
    front=scr;
  }
  if((f = getscreenbyroot(scr->root))) {
    init_dri(&f->dri, dpy, f->root, f->cmap, True);
    setvirtualroot(f);
  }
}

void assimilate(Window w, int x, int y)
{
#ifdef ASSIMILATE_WINDOWS
  XSetWindowAttributes xsa;

  XAddToSaveSet(dpy, w);
  XReparentWindow(dpy, w, scr->back, x, (y<scr->y? 0:y-scr->y));
  XSaveContext(dpy, w, screen_context, (XPointer)scr);
  xsa.override_redirect = False;
  XChangeWindowAttributes(dpy, w, CWOverrideRedirect, &xsa);
#endif
}

static void scanwins()
{
  unsigned int i, nwins;
  Client *c;
  Window dw1, dw2, *wins;
  XWindowAttributes *pattr=NULL;
  XPointer dummy;
  Scrn *s=scr;

  XQueryTree(dpy, scr->root, &dw1, &dw2, &wins, &nwins);
  if(nwins && (pattr=calloc(nwins, sizeof(XWindowAttributes)))) {
    for (i = 0; i < nwins; i++)
      XGetWindowAttributes(dpy, wins[i], pattr+i);
    for (i = 0; i < nwins; i++) {
      if (!XFindContext(dpy, wins[i], client_context, &dummy))
	continue;
      if (pattr[i].override_redirect) {
	if(scr->back!=scr->root && XFindContext(dpy, wins[i], screen_context, &dummy))
	  assimilate(wins[i], pattr[i].x, pattr[i].y);
	continue;
      }
      c = createclient(wins[i]);
      if (c != 0 && c->window == wins[i]) {
	if (pattr[i].map_state == IsViewable) {
	  c->state=NormalState;
	  getstate(c);
	  reparent(c);
	  if(c->state==IconicState) {
	    createicon(c);
	    adjusticon(c->icon);
	    XMapWindow(dpy, c->icon->window);
	    if(c->icon->labelwidth)
	      XMapWindow(dpy, c->icon->labelwin);
	    c->icon->mapped=1;
	  } else if(c->state==NormalState)
	    XMapRaised(dpy, c->parent);
	  else
	    XRaiseWindow(dpy, c->parent);
	  c->reparenting=1;
	  scr=s;
	}
      }
    }
    free(pattr);
  }
  XFree((void *) wins);
  cleanupicons();
}

void closescreen(void)
{
  Scrn *dummy;

  if(scr->behind == scr)
    scr->behind = NULL;
  else {
    scr->upfront->behind=scr->behind;
    scr->behind->upfront=scr->upfront;
  }

  XDeleteContext(dpy,scr->menubardepth,screen_context);
  XDestroyWindow(dpy,scr->menubardepth);
  XDeleteContext(dpy,scr->menubarparent,screen_context);
  XDestroyWindow(dpy,scr->menubarparent);
  XDeleteContext(dpy,scr->menubar,screen_context);
  XDestroyWindow(dpy,scr->menubar);
  if(scr->inputbox != None) {
    XDeleteContext(dpy,scr->inputbox,screen_context);
    XDestroyWindow(dpy,scr->inputbox);
  }

  XFreeGC(dpy,scr->rubbergc);
  XFreeGC(dpy,scr->icongc);
  XFreeGC(dpy,scr->gc);
  XDeleteContext(dpy,scr->back,screen_context);
  XDestroyWindow(dpy,scr->back);
  free_icon_pms(&scr->default_tool_pms);
  term_dri(&scr->dri, dpy, scr->cmap);
  if(scr->iconcolorsallocated)
    XFreeColors(dpy, scr->cmap, scr->iconcolor, scr->iconcolorsallocated, 0);
  if(front==scr)
    front=scr->behind;
  dummy=scr->behind;
  free(scr);
  scr=dummy;
}

Scrn *openscreen(char *deftitle, Window root)
{
  Scrn *s;
  XWindowAttributes attr;
  XSetWindowAttributes swa;
  XGCValues gcv;
  extern char *label_font_name;

  if(!root)
    root = DefaultRootWindow(dpy);

  XGetWindowAttributes(dpy, root, &attr);

  s = (Scrn *)calloc(1, sizeof(Scrn));
  s->root = root;
  s->cmap = attr.colormap;
  s->depth = attr.depth;
  s->visual = attr.visual;
  s->number = XScreenNumberOfScreen(attr.screen);
  s->inputbox = None;

  init_dri(&s->dri, dpy, s->root, s->cmap, True);

  swa.background_pixel = s->dri.dri_Pens[BACKGROUNDPEN];
  swa.override_redirect = True;
  swa.colormap = attr.colormap;
  swa.cursor = wm_curs;
  swa.border_pixel = BlackPixel(dpy, DefaultScreen(dpy));

  s->back = XCreateWindow(dpy, root,
			  -prefs.borderwidth, (s->y=0)-prefs.borderwidth,
			  s->width = attr.width, s->height = attr.height,
			  s->bw=prefs.borderwidth, CopyFromParent,
			  InputOutput, CopyFromParent,
			  CWBackPixel|CWOverrideRedirect|CWColormap|CWCursor|
			  CWBorderPixel,
			  &swa);

  XSaveContext(dpy, s->back, screen_context, (XPointer)s);

  gcv.background = s->dri.dri_Pens[BACKGROUNDPEN];

#ifdef USE_FONTSETS
  s->gc = XCreateGC(dpy, s->back, GCBackground, &gcv);

  if(!labelfontset) {
    char **missing_charsets = NULL;
    int n_missing_charsets = 0;
    labelfontset =  XCreateFontSet(dpy, label_font_name,
				   &missing_charsets,
				   &n_missing_charsets, NULL);
    if(missing_charsets)
      XFreeStringList(missing_charsets);
    if(!labelfontset) { 
      fprintf(stderr, "%s: cannot open font %s\n", progname, label_font_name);
      labelfontset = s->dri.dri_FontSet;
    }
  }
  {
    XFontStruct **fsl;
    char **fnl;
    if(XFontsOfFontSet(labelfontset, &fsl, &fnl) < 1) {
      fprintf(stderr, "%s: fontset %s is empty\n", progname,
	      label_font_name);
      exit(1);
    }
    labelfont_ascent = fsl[0]->ascent;
    s->lh = fsl[0]->ascent+fsl[0]->descent;
  }
  gcv.foreground = s->dri.dri_Pens[TEXTPEN];

  s->icongc = XCreateGC(dpy, s->back, GCForeground|GCBackground, &gcv);
#else  
  gcv.font = s->dri.dri_Font->fid;

  s->gc = XCreateGC(dpy, s->back, GCBackground|GCFont, &gcv);
  
  if(!labelfont)
    if(!(labelfont = XLoadQueryFont(dpy, label_font_name))) {
      fprintf(stderr, "%s: cannot open font %s\n", progname, label_font_name);
      labelfont = s->dri.dri_Font;
    }

  s->lh = labelfont->ascent+labelfont->descent;
  gcv.font = labelfont->fid;
  gcv.foreground = s->dri.dri_Pens[TEXTPEN];

  s->icongc = XCreateGC(dpy, s->back, GCForeground|GCBackground|GCFont, &gcv);
#endif

  gcv.function = GXinvert;
  gcv.subwindow_mode = IncludeInferiors;

  s->rubbergc = XCreateGC(dpy, s->back, GCFunction|GCSubwindowMode, &gcv);

  s->title = s->deftitle = deftitle;

  s->default_tool_pms.pm = None;
  s->default_tool_pms.pm2 = None;
  s->default_tool_pms.cs.colors = NULL;
  s->default_tool_pms.cs2.colors = NULL;
  s->default_tool_pm_w=0;
  s->default_tool_pm_h=0;

  if(front) {
    s->behind=front;
    s->upfront=front->upfront;
    front->upfront->behind=s;
    front->upfront=s;
  } else {
    s->behind = s->upfront = s;
    front = s;
  }

  scr=s;

  return s;
}

void realizescreens(void)
{
  scr = front;

  do {
    if(!scr->realized) {
      scr->fh = scr->dri.dri_Ascent+scr->dri.dri_Descent;
      scr->bh=scr->fh+3;
      scr->h2=(2*scr->bh)/10; scr->h3=(3*scr->bh)/10;
      scr->h4=(4*scr->bh)/10; scr->h5=(5*scr->bh)/10;
      scr->h6=(6*scr->bh)/10; scr->h7=(7*scr->bh)/10;
      scr->h8=(8*scr->bh)/10;
      createmenubar();
      createdefaulticons();

      if (scr->inputbox == None && prefs.focus == FOC_CLICKTOTYPE) {
	scr->inputbox = XCreateWindow(dpy, scr->back, 0, 0,
				      scr->width, scr->height, 0, 0,
				      InputOnly, CopyFromParent, 0, NULL);
	XSaveContext(dpy, scr->inputbox, screen_context, (XPointer)scr);
	XSelectInput(dpy, scr->inputbox, KeyPressMask|KeyReleaseMask);
	XLowerWindow(dpy, scr->inputbox);
	XMapWindow(dpy, scr->inputbox);
      }

      XSelectInput(dpy, scr->root,
		   SubstructureNotifyMask|SubstructureRedirectMask|
		   KeyPressMask|KeyReleaseMask|
		   ButtonPressMask|ButtonReleaseMask|FocusChangeMask);
      if(scr->back != scr->root)
	XSelectInput(dpy, scr->back,
		     SubstructureNotifyMask|SubstructureRedirectMask|
		     KeyPressMask|KeyReleaseMask|
		     ButtonPressMask|ButtonReleaseMask);

      XStoreName(dpy, scr->back, scr->title);
      XLowerWindow(dpy, scr->back);
      XMapWindow(dpy, scr->back);
    }
    scr=scr->behind;
  } while(scr!=front);
  do {  
    if(!scr->realized) {
      scanwins();
      if(!getvroot(scr->root)) {
	init_dri(&scr->dri, dpy, scr->root, scr->cmap, True);
	setvirtualroot(scr);
      }
      scr->realized=1;
    }
    scr=scr->behind;
  } while(scr!=front);
}

Scrn *getscreen(Window w)
{
  Scrn *s=front;
  if(w && s)
    do {
      if(s->back == w || s->root == w)
	return s;
      s=s->behind;
    } while(s!=front);
  return front;
}

Scrn *getscreenbyroot(Window w)
{
  Scrn *s=front;
  if(s)
    do {
      if(s->root == w)
	return s;
      s=s->behind;
    } while(s!=front);
  return NULL;
}
