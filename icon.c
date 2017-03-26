#include <X11/Xlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "drawinfo.h"
#include "screen.h"
#include "icon.h"
#include "client.h"
#include "prefs.h"
#include "icc.h"
#include "style.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

#define NO_ICON_POSITION	(0x80000000)

extern Display *dpy;
extern char *progname;
extern XContext icon_context, client_context, screen_context;

extern void init_iconpalette();

#ifdef USE_FONTSETS
XFontSet labelfontset;
int labelfont_ascent;
#else
XFontStruct *labelfont;
#endif

char *label_font_name="-b&h-lucida-medium-r-normal-sans-10-*-*-*-*-*-iso8859-1"
#ifdef USE_FONTSETS
",-misc-fixed-medium-r-normal--10-*-*-*-*-*-iso10646-1"
#endif
;

void redrawicon(Icon *i, Window w)
{
  Pixmap pm;

  scr=i->scr;
  if(w==i->window) {
    pm=i->iconpm;
    if(i->selected && i->secondpm)
      pm=i->secondpm;
    if(pm) {
      XGCValues xgc;
      Window r;
      int x, y;
      unsigned int w, h, bw, d;
      XGetGeometry(dpy, pm, &r, &x, &y, &w, &h, &bw, &d);
      if(i->maskpm) {
	xgc.clip_mask = i->maskpm;
	xgc.clip_x_origin = xgc.clip_y_origin = 4;
	XChangeGC(dpy, scr->gc, GCClipXOrigin|GCClipYOrigin|GCClipMask, &xgc);
      }
      if(d!=scr->depth) {
	XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[SHADOWPEN]);
	XSetBackground(dpy, scr->gc, scr->dri.dri_Pens[BACKGROUNDPEN]);
	XCopyPlane(dpy, pm, i->window, scr->gc, 0, 0,
		i->width-8, i->height-8, 4, 4, 1);
      }
      else
	XCopyArea(dpy, pm, i->window, scr->gc, 0, 0,
		  i->width-8, i->height-8, 4, 4);
      if(i->maskpm) {
	xgc.clip_mask = None;
	XChangeGC(dpy, scr->gc, GCClipMask, &xgc);
      }
    }
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[i->selected? SHADOWPEN:SHINEPEN]);
    XDrawLine(dpy, w, scr->gc, 0, 0, i->width-1, 0);
    XDrawLine(dpy, w, scr->gc, 0, 0, 0, i->height-1);
    XSetForeground(dpy, scr->gc, scr->dri.dri_Pens[i->selected? SHINEPEN:SHADOWPEN]);
    XDrawLine(dpy, w, scr->gc, 1, i->height-1, i->width-1, i->height-1);
    XDrawLine(dpy, w, scr->gc, i->width-1, 1, i->width-1, i->height-1);
#ifdef USE_FONTSETS
  } else if(w==i->labelwin && i->label) {
    XmbDrawImageString(dpy, w, labelfontset, scr->icongc, 0, labelfont_ascent,
		       i->label, strlen(i->label));
#else
  } else if(w==i->labelwin && i->label.value) {
    XDrawImageString(dpy, w, scr->icongc, 0, labelfont->ascent,
		     (char *)i->label.value, i->label.nitems);
#endif
  }
}

void selecticon(Icon *i)
{
  if(!(i->selected)) {
    i->nextselected = i->scr->firstselected;
    i->scr->firstselected = i;
    i->selected = 1;
    if(i->mapped)
      redrawicon(i, i->window);
  }
}

void deselecticon(Icon *i)
{
  Icon *i2;
  if(i==i->scr->firstselected)
    i->scr->firstselected = i->nextselected;
  else for(i2=i->scr->firstselected; i2; i2=i2->nextselected)
    if(i2->nextselected==i) {
      i2->nextselected = i->nextselected;
      break;
    }
  i->nextselected = NULL;
  i->selected=0;
  if(i->mapped)
    redrawicon(i, i->window);
}

void deselect_all_icons(Scrn *scr)
{
  while(scr->firstselected)
    deselecticon(scr->firstselected);
}

void select_all_icons(Scrn *scr)
{
  Icon *i;
  for(i=scr->icons; i; i=i->next)
    if(i->window && i->mapped)
      selecticon(i);
}

void reparenticon(Icon *i, Scrn *s, int x, int y)
{
  Icon **ip;
  int os=i->selected;
  if(s==i->scr) {
    if(x!=i->x || y!=i->y)
      XMoveWindow(dpy, i->window, i->x=x, i->y=y);
    return;
  }
  if(os)
    deselecticon(i);
  for(ip=&i->scr->icons; *ip; ip=&(*ip)->next)
    if(*ip==i) {
      *ip=i->next;
      break;
    }
  XReparentWindow(dpy, i->window, s->back, i->x=x, i->y=y);
  if(i->labelwin) {
    XReparentWindow(dpy, i->labelwin, s->back,
		    x+(i->width>>1)-(i->labelwidth>>1),
		    y+i->height+1);
  }
  i->scr=s;
  i->next=s->icons;
  s->icons=i;
  if(i->client) {
    i->client->scr=s;
    if(i->client->parent != i->client->scr->root)
      XReparentWindow(dpy, i->client->parent, s->back,
		      i->client->x, i->client->y);
    setstringprop(i->client->window, amiwm_screen, s->deftitle);
    sendconfig(i->client);
  }
  if(os)
    selecticon(i);
}

void createdefaulticons()
{
  Window r;
  int x,y;
  unsigned int b,d;
  extern void load_do(const char *, struct IconPixmaps *);

  init_iconpalette();
  load_do(prefs.defaulticon, &scr->default_tool_pms);
  if(scr->default_tool_pms.pm == None) {
    fprintf(stderr, "%s: Cannot load default icon \"%s\".\n",
	    progname, prefs.defaulticon);
    exit(1);
  }
  XGetGeometry(dpy, scr->default_tool_pms.pm, &r, &x, &y,
	       &scr->default_tool_pm_w, &scr->default_tool_pm_h, &b, &d);
}

void adjusticon(Icon *i)
{
  Window w=i->window,lw=i->labelwin;
  int nx, ny;
  Window rt, ws[3];
  Icon *i2;
  int xx, yy;
  unsigned int maxx, maxy, bw, dd;

  scr=i->scr;
  maxx=scr->width; maxy=scr->height;
  if(i->parent!=scr->back)
    XGetGeometry(dpy, i->parent, &rt, &xx, &yy, &maxx, &maxy, &bw, &dd);
  nx=i->x; ny=i->y;
  for(i2=scr->icons; i2; i2=i2->next)
    if(i2->parent==i->parent && i2!=i && i2->mapped &&
       nx<i2->x+i2->width && nx+i->width>i2->x &&
       ny<i2->y+i2->height+scr->lh+2 && ny+i->height+scr->lh+2>i2->y) {
      ny=i2->y+i2->height+scr->lh+4;
      if(ny+i->height+scr->lh+1>maxy) {
	nx=i2->x+i2->width+5;
	if(i->parent==scr->back && nx+i->width>maxx) {
	  ny=maxy;
	  break;
	} else
	  ny=scr->bh+4;
      }
      i2=scr->icons;
      continue;
    }

  if(i->parent==scr->back) {
    if(nx+i->width>maxx)
      nx=maxx-i->width;
    if(ny+i->height>maxy)
      ny=maxy-i->height;
    if(nx<0) nx=0;
    if(ny<scr->bh) ny=scr->bh;
  }
  if(nx!=i->x || ny!=i->y)
    XMoveWindow(dpy, w, i->x=nx, i->y=ny);
  if(i->parent==scr->back) {
    ws[0]=scr->menubar;
    ws[1]=w;
    ws[2]=lw;
    XRestackWindows(dpy, ws, 3);
  } else {
    XRaiseWindow(dpy, lw);
    XRaiseWindow(dpy, w);
  }
  XMoveWindow(dpy, lw, nx+(i->width>>1)-(i->labelwidth>>1),
	      ny+i->height+1);
}

void destroyiconicon(Icon *i)
{
  if(i->innerwin) {
    XUnmapWindow(dpy, i->innerwin);
    XReparentWindow(dpy, i->innerwin, i->scr->root, i->x+4, i->y+4);
    XRemoveFromSaveSet(dpy, i->innerwin);
    i->innerwin=None;
  }
  i->iconpm = i->secondpm = i->maskpm = None;
}

static void setstdiconicon(Icon *i, unsigned int *w, unsigned int *h)
{
  Style *s;
  if(i->client && (s=i->client->style) && s->icon_name) {
    if(s->icon_pms.pm==None) {
      Window r;
      int x,y;
      unsigned int b,d;
      extern void load_do(const char *, struct IconPixmaps *);
      load_do(s->icon_name, &s->icon_pms);
      if(s->icon_pms.pm == None) {
	fprintf(stderr, "%s: Cannot load icon \"%s\".\n",
		progname, s->icon_name);
	s->icon_name = NULL;
      } else
	XGetGeometry(dpy, s->icon_pms.pm, &r, &x, &y,
		     &s->icon_pm_w, &s->icon_pm_h, &b, &d);
    }
    if(s->icon_pms.pm!=None) {
      i->iconpm=s->icon_pms.pm;
      i->secondpm=s->icon_pms.pm2;
      i->maskpm=None;
      *w=s->icon_pm_w+8;
      *h=s->icon_pm_h+8;
      return;
    }
  }
  i->iconpm=scr->default_tool_pms.pm;
  i->secondpm=scr->default_tool_pms.pm2;
  i->maskpm=None;
  *w=scr->default_tool_pm_w+8;
  *h=scr->default_tool_pm_h+8;
}

void createiconicon(Icon *i, XWMHints *wmhints)
{
  XSetWindowAttributes attr;
  int x=20, y=20;
  unsigned int w=40, h=40;
  Window win=None;
  void newicontitle(Client *);

  scr=i->scr;
  if(wmhints && !prefs.customiconsonly) {
    if((wmhints->flags&IconWindowHint) && wmhints->icon_window) {
      Window r;
      unsigned int b, d;
      i->innerwin=win=wmhints->icon_window;
      XGetGeometry(dpy, win, &r, &x, &y, &w, &h, &b, &d);
      x-=4; y-=4; w+=8; h+=8;
    } else if((wmhints->flags&IconPixmapHint) && wmhints->icon_pixmap) {
      Window r;
      int x, y;
      unsigned int b, d;
      i->iconpm=wmhints->icon_pixmap;
      i->secondpm=None;
      if(wmhints->flags&IconMaskHint)
	i->maskpm = wmhints->icon_mask;
      else
	i->maskpm = None;
      XGetGeometry(dpy, i->iconpm, &r, &x, &y, &w, &h, &b, &d);
      w+=8;
      h+=8;
    } else
      setstdiconicon(i, &w, &h);
    if(wmhints->flags&IconPositionHint) {
      x=wmhints->icon_x;
      y=wmhints->icon_y;
    } else if(i->window) {
      Window r;
      unsigned int w, h, bw, d;
      XGetGeometry(dpy, i->window, &r, &x, &y, &w, &h, &bw, &d);
    }
  } else
    setstdiconicon(i, &w, &h);

  if(!(i->window)) {  
    attr.override_redirect=True;
    attr.background_pixel=scr->dri.dri_Pens[BACKGROUNDPEN];
    i->window=XCreateWindow(dpy, scr->back, i->x=x, i->y=y,
			    i->width=w, i->height=h, 0, CopyFromParent,
			    InputOutput, CopyFromParent,
			    CWOverrideRedirect|CWBackPixel, &attr);
    i->mapped=0;
    XSaveContext(dpy, i->window, icon_context, (XPointer)i);
    XLowerWindow(dpy, i->window);
    XSelectInput(dpy, i->window,ExposureMask|ButtonPressMask|ButtonReleaseMask);
  } else {
    XMoveResizeWindow(dpy, i->window, i->x=x, i->y=y, i->width=w, i->height=h);
    if(i->mapped)
      redrawicon(i, i->window);
  }
  if(!(i->labelwin)) {
    i->labelwin=XCreateWindow(dpy, scr->back, 0, 0, 1, 1, 0,
			      CopyFromParent, InputOutput, CopyFromParent,
			      CWOverrideRedirect|CWBackPixel, &attr);
    XSaveContext(dpy, i->labelwin, icon_context, (XPointer)i);
    XLowerWindow(dpy, i->labelwin);
    XSelectInput(dpy, i->labelwin, ExposureMask);
  }
  if(i->client)
    newicontitle(i->client);
  if((win=i->innerwin)) {
    XAddToSaveSet(dpy, win);
    XReparentWindow(dpy, win, i->window, 4, 4);
    XSelectInput(dpy, win, SubstructureRedirectMask);
    XMapWindow(dpy, win);
  }
  adjusticon(i);
}

void createicon(Client *c)
{
  XWMHints *wmhints;
  Icon *i;

  c->icon=i=(Icon *)calloc(1, sizeof(Icon));
  i->scr=scr=c->scr;
  i->client=c;
  i->module=NULL;
  i->next=scr->icons;
  i->parent=scr->back;
  scr->icons=i;

  wmhints=XGetWMHints(dpy, c->window);
  createiconicon(i, wmhints);
  if(wmhints) XFree(wmhints);
}

Icon *createappicon(struct module *m, Window p, char *name,
		    Pixmap pm1, Pixmap pm2, Pixmap pmm, int x, int y)
{
  Icon *i;
  Client *c;
  int tx, ty, ap=0;
  unsigned int w, h, b, d;
  Window r, ws[3];
  XSetWindowAttributes attr;

  if(x==NO_ICON_POSITION || y==NO_ICON_POSITION) {
    x=0; y=0; ap++;
  }

  i=(Icon *)calloc(1, sizeof(Icon));
  if(!XFindContext(dpy, p, client_context, (XPointer*)&c)) {
    scr=c->scr;
  } else
    if(XFindContext(dpy, p, screen_context, (XPointer*)&scr))
      scr=front;
  if(p==scr->root) p=scr->back;

  i->scr=scr;
  i->client=NULL;
  i->module=m;
  i->next=scr->icons;
  i->parent=p;
  scr->icons=i;

  i->iconpm=pm1;
  i->secondpm=pm2;
  i->maskpm=pmm;
  XGetGeometry(dpy, i->iconpm, &r, &tx, &ty, &w, &h, &b, &d);
  w+=8;
  h+=8;

  attr.override_redirect=True;
  attr.background_pixel=scr->dri.dri_Pens[BACKGROUNDPEN];
  i->window=XCreateWindow(dpy, p, i->x=x, i->y=y,
			  i->width=w, i->height=h, 0, CopyFromParent,
			  InputOutput, CopyFromParent,
			  CWOverrideRedirect|CWBackPixel, &attr);
  XSaveContext(dpy, i->window, icon_context, (XPointer)i);
  XSelectInput(dpy, i->window,ExposureMask|ButtonPressMask|ButtonReleaseMask);

  i->labelwin=XCreateWindow(dpy, p, 0, 0, 1, 1, 0,
			    CopyFromParent, InputOutput, CopyFromParent,
			    CWOverrideRedirect|CWBackPixel, &attr);
  XSaveContext(dpy, i->labelwin, icon_context, (XPointer)i);
  XSelectInput(dpy, i->labelwin, ExposureMask);

#ifdef USE_FONTSETS
  if((i->label=malloc(strlen(name)+1))!=NULL) {
    strcpy(i->label, name);
    i->labelwidth=XmbTextEscapement(labelfontset, i->label,
				    strlen(i->label));
#else
  if((i->label.value=malloc((i->label.nitems=strlen(name))+1))!=NULL) {
    strcpy((char *)i->label.value, name);
    i->labelwidth=XTextWidth(labelfont, (char *)i->label.value,
			     i->label.nitems);
#endif
    if(i->labelwidth)
      XResizeWindow(dpy, i->labelwin, i->labelwidth, scr->lh);
    XMoveWindow(dpy, i->labelwin,
		i->x+(i->width>>1)-(i->labelwidth>>1),
		i->y+i->height+1);
  }

  if(i->parent==scr->back) {
    ws[0]=scr->menubar;
    ws[1]=i->window;
    ws[2]=i->labelwin;
    XRestackWindows(dpy, ws, 3);
  } else {
    XRaiseWindow(dpy, i->labelwin);
    XRaiseWindow(dpy, i->window);
  }

  if(ap) adjusticon(i);

  i->mapped=1;
  if(i->labelwidth)
    XMapWindow(dpy, i->labelwin);
  XMapWindow(dpy, i->window);

  return i;
}

void rmicon(Icon *i)
{
  Icon *ii;

  if (i->selected)
    deselecticon(i);

  if (i == i->scr->icons)
    i->scr->icons = i->next;
  else
    if((ii = i->scr->icons))
      for (; ii->next; ii = ii->next)
	if (ii->next == i) {
	  ii->next = i->next;
	  break;
	}

  destroyiconicon(i);
  XDestroyWindow(dpy, i->window);
  XDeleteContext(dpy, i->window, icon_context);
  if(i->labelwin) {
    XDestroyWindow(dpy, i->labelwin);
    XDeleteContext(dpy, i->labelwin, icon_context);
  }
#ifdef USE_FONTSETS
  if(i->label)
    free(i->label);
#else
  if(i->label.value)
    if(i->module)
      free(i->label.value);
    else
      XFree(i->label.value);
#endif
  free(i);
}

static int cmp_iconpos(const Icon **p1, const Icon **p2)
{
  int r;
  return((r=(*p2)->mapped-(*p1)->mapped)?r:((r=(*p1)->x-(*p2)->x)?r:
					    (*p1)->y-(*p2)->y));
}

static void placeicons(Icon **p, int n, int x, int w)
{
  int i;

  x+=w>>1;
  for(i=0; i<n; i++) {
    Icon *i=*p++;
    XMoveWindow(dpy, i->window, i->x=x-(i->width>>1), i->y);
    XMoveWindow(dpy, i->labelwin, x-(i->labelwidth>>1),
		i->y+i->height+1);
  }
}

void cleanupicons()
{
  Icon *i, **icons;
  int nicons=0, maxicons=16;

  if((icons=calloc(maxicons, sizeof(Icon*)))) {
    for(i=scr->icons; i; i=i->next)
      if(i->window) {
	if(nicons>=maxicons)
	  if(!(icons=realloc(icons, sizeof(Icon*)*(maxicons<<=1))))
	    return;
	icons[nicons]=i;
	i->x+=i->width>>1;
	nicons++;
      }
    if(nicons) {
      int i0=0, i, x=5, y=scr->bh+4, w, mw=0;
      qsort(icons, nicons, sizeof(*icons),
	    (int (*)(const void *, const void *))cmp_iconpos);
      for(i=0; i<nicons; i++) {
	if(i>i0 && y+icons[i]->height>scr->height-4-scr->lh) {
	  placeicons(icons+i0, i-i0, x, mw);
	  x+=mw+5;
	  y=scr->bh+4;
	  mw=0;
	  i0=i;
	}
	icons[i]->y=y;
	w=icons[i]->width;
	if(icons[i]->mapped && icons[i]->labelwidth>w)
	  w=icons[i]->labelwidth;
	if(w>mw)
	  mw=w;
	y+=icons[i]->height+4+scr->lh;
      }
      placeicons(icons+i0, nicons-i0, x, mw);
    }
    free(icons);
  }
}

void newicontitle(Client *c)
{
  Icon *i=c->icon;
#ifdef USE_FONTSETS
  XTextProperty prop;
  if(i->label) {
    free(i->label);
    i->label = NULL;
  }
  if(XGetWMIconName(dpy, c->window, &prop) && prop.value) {
    char **list;
    int n;
    if(XmbTextPropertyToTextList(dpy, &prop, &list, &n) >= Success) {
      if(n > 0) {
	if( prefs.shortlabelicons )
	{
	  if (strlen(list[0])>10){
	    char *str1=list[0];
	    char str2[11];
	    strncpy (str2,str1,8);
	    str2[8]='.';
	    str2[9]='.';
	    str2[10]='\0';
	    i->label = strdup(str2);
	  } else {
	    i->label = strdup(list[0]);
	  }
	} else {
	  i->label = strdup(list[0]);
	}
	i->labelwidth=XmbTextEscapement(labelfontset, i->label,
					strlen(i->label));
      }
      XFreeStringList(list);
    }
    XFree(prop.value);
  }
  if(!i->label)
    i->labelwidth = 0;
#else
  if(i->label.value)
    XFree(i->label.value);
  if(!(XGetWMIconName(dpy, c->window, &i->label))) {
    i->label.value=NULL;
    i->labelwidth=0;
  } else
    i->labelwidth=XTextWidth(labelfont, (char *)i->label.value,
			     i->label.nitems);
#endif
  if(i->labelwidth)
    XResizeWindow(dpy, i->labelwin, i->labelwidth, c->scr->lh);
  if(i->mapped && i->labelwidth>0)
    XMapWindow(dpy, i->labelwin);
  else
    XUnmapWindow(dpy, i->labelwin);
  XMoveWindow(dpy, i->labelwin,
	      i->x+(i->width>>1)-(i->labelwidth>>1),
	      i->y+i->height+1);
  if(i->mapped)
    redrawicon(i, i->labelwin);
}

void free_icon_pms(struct IconPixmaps *pms)
{
  if(pms->pm!=None) XFreePixmap(dpy, pms->pm);
  if(pms->pm2!=None) XFreePixmap(dpy, pms->pm2);
  free_color_store(dpy, &pms->cs);
  free_color_store(dpy, &pms->cs2);
}
