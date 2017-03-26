#include <X11/Xlib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "alloc.h"
#include "drawinfo.h"
#include "prefs.h"
#include "screen.h"
#include "client.h"
#include "icon.h"
#include "version.h"

#define ABOUT_STRING(LF) \
     " " LF "version "VERSION LF "by Marcus Comstedt" LF \
     "<marcus@lysator.liu.se>" LF " " LF \
     "Contributions by Scott Lawrence" LF "<amiwm10@umlautllama.com>" LF \
     "and Nicolas Sipieter " LF "<freeeaks@gmail.com>" LF " " LF

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

#ifdef AMIGAOS
#define BIN_PREFIX AMIWM_HOME
#else
#define BIN_PREFIX AMIWM_HOME"/"
#endif

#define CHECKIT 1
#define CHECKED 2
#define DISABLED 4

extern Display *dpy;
extern Cursor wm_curs;
extern XContext screen_context, client_context;
extern Client *activeclient;

extern void select_all_icons(Scrn *i);
extern void mod_menuselect(struct module *, int, int, int);
extern void setfocus(Window);
extern void flushmodules();

Scrn *mbdclick=NULL, *mbdscr=NULL;

static struct ToolItem {
  struct ToolItem *next;
  const char *name, *cmd;
  char hotkey;
  int level;
  struct Menu *submenu;
} *firsttoolitem=NULL, *lasttoolitem=NULL;

static struct Item {
  struct Item *next;
  Window win;
  const char *text;
  int textlen;
  char hotkey, flags;
  struct Menu *menu, *sub;
  struct module *owner;
  struct Item *mod_chain;
} *activeitem=NULL, *activesubitem=NULL;

static struct Menu {
  struct Menu *next;
  struct Item *item;
  Window win, parent;
  const char *title;
  int titlelen;
  int width, height;
  char flags;
  struct Item *firstitem, *lastitem;
} *activemenu=NULL, *activesubmenu=NULL;

#ifdef AMIGAOS
void spawn(const char *cmd)
{
  char *line=malloc(strlen(cmd)+12);
  if(line) {
    sprintf(line, "RUN <>NIL: %s", cmd);
    system(line);
    free(line);
  }
}
#else
void spawn(const char *cmd)
{
  extern char *x_server;
#ifdef HAVE_ALLOCA
  char *line=alloca(strlen(x_server)+strlen(cmd)+28);
#else
  char *line=malloc(strlen(x_server)+strlen(cmd)+28);
  if(line) {
#endif
  char *dot;
  sprintf(line, "DISPLAY='%s", x_server);
  if(!(dot=strrchr(line, '.')) || strchr(dot, ':'))
    dot=line+strlen(line);
  sprintf(dot, ".%d' %s &", scr->number, cmd);
#ifdef __ultrix
  {
    int pid, status;
    if ((pid = fork ()) == 0) {
      (void) setsid();
      execl ("/bin/sh", "sh", "-c", line, 0);
    } else
      waitpid (pid, &status, 0);
  }
#else
  system(line);
#endif
#ifndef HAVE_ALLOCA
    free(line);
  }
#endif
}
#endif

void add_toolitem(const char *n, const char *c, const char *k, int l)
{
  struct ToolItem *ti;
  if((ti=malloc(sizeof(struct ToolItem)))) {
    ti->name=n;
    ti->cmd=c;
    if(k)
      ti->hotkey=k[0];
    else
      ti->hotkey=0;
    ti->level=l;
    ti->next=NULL;
    if(lasttoolitem)
      lasttoolitem->next=ti;
    else
      firsttoolitem=ti;
    lasttoolitem=ti;
  }
}

static void menu_layout(struct Menu *menu)
{
  XSetWindowAttributes attr;
  XWindowAttributes attr2;
  int w, x, y;
  struct Item *item;
  if(menu->win) {
    XGetWindowAttributes(dpy, menu->win, &attr2);
    x=attr2.x; y=scr->bh-2;
  } else {
    XGetWindowAttributes(dpy, menu->item->win, &attr2);
    x=attr2.x+attr2.width-scr->hotkeyspace+9; y=attr2.y-2;
    XGetWindowAttributes(dpy, menu->item->menu->parent, &attr2);
    x+=attr2.x; y+=attr2.y;
  }
  menu->width=menu->height=0;
  for(item=menu->firstitem; item; item=item->next) {
    if(item->text)
      menu->height+=scr->dri.dri_Ascent+scr->dri.dri_Descent+1;
    else
      menu->height+=6;
#ifdef USE_FONTSETS
    w=XmbTextEscapement(scr->dri.dri_FontSet, item->text, item->textlen)+2;
#else
    w=XTextWidth(scr->dri.dri_Font, item->text, item->textlen)+2;
#endif
    if(item->hotkey)
      w+=scr->hotkeyspace;
    if(item->flags&CHECKIT)
      w+=scr->checkmarkspace;
    if(item->sub)
      w+=scr->subspace;
    if(w>menu->width)
      menu->width=w;
  }
  menu->width+=6;
  menu->height+=2;
  attr.override_redirect=True;
  attr.background_pixel=scr->dri.dri_Pens[BARBLOCKPEN];
  attr.border_pixel=scr->dri.dri_Pens[BARDETAILPEN];
  menu->parent=XCreateWindow(dpy, scr->back, x, y,
			     menu->width, menu->height, 1,
			     CopyFromParent, InputOutput, CopyFromParent,
			     CWOverrideRedirect|CWBackPixel|CWBorderPixel,
			     &attr);
  XSaveContext(dpy, menu->parent, screen_context, (XPointer)scr);
  XSelectInput(dpy, menu->parent, ExposureMask);
  w=1;
  for(item=menu->firstitem; item; item=item->next) {
    int h=(item->text? scr->dri.dri_Ascent+scr->dri.dri_Descent+1:6);
    item->win=XCreateWindow(dpy, menu->parent, 3, w, menu->width-6, h, 0,
			    CopyFromParent, InputOutput, CopyFromParent,
			    CWOverrideRedirect|CWBackPixel, &attr);
    XSaveContext(dpy, item->win, screen_context, (XPointer)scr);
    w+=h;
    XSelectInput(dpy, item->win, ExposureMask|ButtonReleaseMask|
		 EnterWindowMask|LeaveWindowMask);
  }
  XMapSubwindows(dpy, menu->parent);
}

static struct Menu *add_menu(const char *name, char flags)
{
  struct Menu *menu=calloc(1, sizeof(struct Menu));
  XSetWindowAttributes attr;
  int w;

  if(menu) {
    attr.override_redirect=True;
    attr.background_pixel=scr->dri.dri_Pens[BARBLOCKPEN];
#ifdef USE_FONTSETS
    w=XmbTextEscapement(scr->dri.dri_FontSet, name, menu->titlelen=strlen(name))+8;
#else
    w=XTextWidth(scr->dri.dri_Font, name, menu->titlelen=strlen(name))+8;
#endif
    menu->win=XCreateWindow(dpy, scr->menubarparent, scr->menuleft, 0, w, scr->bh-1, 0,
			    CopyFromParent, InputOutput, CopyFromParent,
			    CWOverrideRedirect|CWBackPixel, &attr);
    XSaveContext(dpy, menu->win, screen_context, (XPointer)scr);
    XMapWindow(dpy, menu->win);
    XSelectInput(dpy, menu->win, ExposureMask|EnterWindowMask|LeaveWindowMask);
    scr->menuleft+=w+6;
    menu->flags=flags;
    menu->title=name;
    menu->item=NULL;
    menu->firstitem=menu->lastitem=NULL;
    menu->next=scr->firstmenu;
    scr->firstmenu=menu;
  }
  return menu;
}

static struct Item *add_item(struct Menu *m, const char *name, char key,
			     char flags)
{
  struct Item *item=calloc(1, sizeof(struct Item));

  if(item) {
    if(name) {
      item->text=name;
      item->textlen=strlen(name);
    } else item->text=NULL;
    item->hotkey=key;
    item->flags=flags;
    item->menu=m;
    item->sub=NULL;
    item->next=NULL;
    if(m->lastitem)
      m->lastitem->next=item;
    else
      m->firstitem=item;
    m->lastitem=item;
  }
  return item;  
}

static struct Menu *sub_menu(struct Item *i, char flags)
{
  struct Menu *menu=calloc(1, sizeof(struct Menu));

  if(menu) {
    menu->flags=flags;
    menu->title=NULL;
    menu->firstitem=menu->lastitem=NULL;
    menu->next=NULL;
    menu->item=i;
    i->sub=menu;
  }
  return menu;
}

void redraw_menu(struct Menu *m, Window w)
{
  int active=(m==activemenu && !(m->flags & DISABLED));
  XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[active?BARBLOCKPEN:
					      BARDETAILPEN]);
  XSetBackground(dpy, scr->menubargc, scr->dri.dri_Pens[active?BARDETAILPEN:
					      BARBLOCKPEN]);
#ifdef USE_FONTSETS
  XmbDrawImageString(dpy, w, scr->dri.dri_FontSet,
		     scr->menubargc, 4, 1+scr->dri.dri_Ascent,
		     m->title, m->titlelen);
#else
  XDrawImageString(dpy, w, scr->menubargc, 4, 1+scr->dri.dri_Ascent,
		   m->title, m->titlelen);
#endif
}

void redraw_item(struct Item *i, Window w)
{
  struct Menu *m=i->menu;
  int s=scr->dri.dri_Ascent>>1;
  if((i==activeitem || i==activesubitem) && !(i->flags&DISABLED)) {
    XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[BARBLOCKPEN]);
    XSetBackground(dpy, scr->menubargc, scr->dri.dri_Pens[BARDETAILPEN]);
  } else {
    XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[BARDETAILPEN]);
    XSetBackground(dpy, scr->menubargc, scr->dri.dri_Pens[BARBLOCKPEN]);
  }
  if(i->text)
#ifdef USE_FONTSETS
    XmbDrawImageString(dpy, w, scr->dri.dri_FontSet,
		       scr->menubargc, (i->flags&CHECKIT)?1+scr->checkmarkspace:1,
		       scr->dri.dri_Ascent+1, i->text, i->textlen);
#else
    XDrawImageString(dpy, w, scr->menubargc, (i->flags&CHECKIT)?1+scr->checkmarkspace:1,
		     scr->dri.dri_Ascent+1, i->text, i->textlen);
#endif
  else
    XFillRectangle(dpy, w, scr->menubargc, 2, 2, m->width-10, 2);
  if(i->sub) {
    int x=m->width-6-scr->hotkeyspace-1+8;
#ifdef USE_FONTSETS
#ifdef HAVE_XUTF8DRAWIMAGESTRING
    Xutf8DrawImageString(dpy, w, scr->dri.dri_FontSet,
			 scr->menubargc, x+scr->dri.dri_Ascent+1,
			 1+scr->dri.dri_Ascent, "\302\273", 2);
#else
    XmbDrawImageString(dpy, w, scr->dri.dri_FontSet,
		       scr->menubargc, x+scr->dri.dri_Ascent+1,
		       1+scr->dri.dri_Ascent, "»", 1);
#endif
#else
    XDrawImageString(dpy, w, scr->menubargc, x+scr->dri.dri_Ascent+1,
		     1+scr->dri.dri_Ascent, "»", 1);
#endif
  } else if(i->hotkey) {
    int x=m->width-6-scr->hotkeyspace-1+8;
    XDrawLine(dpy, w, scr->menubargc, x, 1+s, x+s, 1);
    XDrawLine(dpy, w, scr->menubargc, x+s, 1, x+s+s, 1+s);
    XDrawLine(dpy, w, scr->menubargc, x+s+s, 1+s, x+s, 1+s+s);
    XDrawLine(dpy, w, scr->menubargc, x+s, 1+s+s, x, 1+s);
#ifdef USE_FONTSETS
    XmbDrawImageString(dpy, w, scr->dri.dri_FontSet,
		       scr->menubargc, x+scr->dri.dri_Ascent+1,
		       1+scr->dri.dri_Ascent, &i->hotkey, 1);
#else
    XDrawImageString(dpy, w, scr->menubargc, x+scr->dri.dri_Ascent+1,
		     1+scr->dri.dri_Ascent, &i->hotkey, 1);
#endif
  }
  if(i->flags&CHECKED) {
    XDrawLine(dpy, w, scr->menubargc, 0, s, s, scr->dri.dri_Ascent);
    XDrawLine(dpy, w, scr->menubargc, s, scr->dri.dri_Ascent, s+s, 0);
  }
  if(i->flags&DISABLED) {
    XSetStipple(dpy, scr->menubargc, scr->disabled_stipple);
    XSetFillStyle(dpy, scr->menubargc, FillStippled);
    XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[BARBLOCKPEN]);
    XFillRectangle(dpy, w, scr->menubargc, 0, 0,
		   m->width-6, scr->dri.dri_Ascent+scr->dri.dri_Descent+1);
    XSetFillStyle(dpy, scr->menubargc, FillSolid);
  }
}

void createmenubar()
{
  XSetWindowAttributes attr;
  struct Menu *m, *sm1, *sm2, *sm3;
  struct ToolItem *ti;
  GC gc;

  scr->firstmenu = NULL;
  attr.override_redirect=True;
  attr.background_pixel=scr->dri.dri_Pens[BARBLOCKPEN];
  scr->menubar=XCreateWindow(dpy, scr->back, 0, 0, scr->width, scr->bh, 0,
			     CopyFromParent,
			     InputOutput, CopyFromParent,
			     CWOverrideRedirect|CWBackPixel,
			     &attr);
  XSaveContext(dpy, scr->menubar, screen_context, (XPointer)scr);
  scr->menubarparent=XCreateWindow(dpy, scr->menubar, 0, 0, scr->width,
				   scr->bh-1, 0,
				   CopyFromParent, InputOutput, CopyFromParent,
				   CWOverrideRedirect|CWBackPixel, &attr);
  XSaveContext(dpy, scr->menubarparent, screen_context, (XPointer)scr);
  attr.background_pixel=scr->dri.dri_Pens[BACKGROUNDPEN];
  scr->menubardepth=XCreateWindow(dpy, scr->menubar, scr->width-23,
				  0, 23, scr->bh, 0,
				  CopyFromParent, InputOutput, CopyFromParent,
				  CWOverrideRedirect|CWBackPixel, &attr);
  XSaveContext(dpy, scr->menubardepth, screen_context, (XPointer)scr);
  scr->disabled_stipple=XCreatePixmap(dpy, scr->back, 6, 2, 1);
  gc=XCreateGC(dpy, scr->disabled_stipple, 0, NULL);
  XSetForeground(dpy, gc, 0);
  XFillRectangle(dpy, scr->disabled_stipple, gc, 0, 0, 6, 2);
  XSetForeground(dpy, gc, 1);
  XDrawPoint(dpy, scr->disabled_stipple, gc, 0, 0);
  XDrawPoint(dpy, scr->disabled_stipple, gc, 3, 1);
  XFreeGC(dpy, gc);
  scr->menubargc=XCreateGC(dpy, scr->menubar, 0, NULL); 
#ifndef USE_FONTSETS
  XSetFont(dpy, scr->menubargc, scr->dri.dri_Font->fid);
#endif
  XSetBackground(dpy, scr->menubargc, scr->dri.dri_Pens[BARBLOCKPEN]);
  XSelectInput(dpy, scr->menubar, ExposureMask|ButtonPressMask|ButtonReleaseMask);
  XSelectInput(dpy, scr->menubardepth, ExposureMask|ButtonPressMask|
	       ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  XMapWindow(dpy, scr->menubardepth);
  XMapWindow(dpy, scr->menubar);
  scr->hotkeyspace=8+1+scr->dri.dri_MaxBoundsWidth+
    scr->dri.dri_Ascent;
  scr->checkmarkspace=4+scr->dri.dri_Ascent;
  scr->subspace=scr->hotkeyspace-scr->dri.dri_Ascent;
  scr->menuleft=4;
  m=add_menu("Workbench", 0);
  add_item(m,"Backdrop",'B',CHECKIT|CHECKED|DISABLED);
  add_item(m,"Execute Command...",'E',0);
  add_item(m,"Redraw All",0,0);
  add_item(m,"Update All",0,DISABLED);
  add_item(m,"Last Message",0,DISABLED);
  add_item(m,"About...",'?',0);
  add_item(m,"Quit...",'Q',0);
  menu_layout(m);
  m=add_menu("Window", 0);
  add_item(m,"New Drawer",'N',DISABLED);
  add_item(m,"Open Parent",0,DISABLED);
  add_item(m,"Close",'K',DISABLED);
  add_item(m,"Update",0,DISABLED);
  add_item(m,"Select Contents",'A',0);
  add_item(m,"Clean Up",'.',0);
  sm1=sub_menu(add_item(m,"Snapshot",0,DISABLED),0);
  add_item(sm1, "Window",0,DISABLED);
  add_item(sm1, "All",0,DISABLED);
  sm2=sub_menu(add_item(m,"Show",0,DISABLED),0);
  add_item(sm2, "Only Icons",0,CHECKIT|CHECKED|DISABLED);
  add_item(sm2, "All Files",'V',CHECKIT|DISABLED);
  sm3=sub_menu(add_item(m,"View By",0,DISABLED),0);
  add_item(sm3, "Icon",0,CHECKIT|CHECKED|DISABLED);
  add_item(sm3, "Name",0,CHECKIT|DISABLED);
  add_item(sm3, "Date",0,CHECKIT|DISABLED);
  add_item(sm3, "Size",0,CHECKIT|DISABLED);
  menu_layout(m);
  menu_layout(sm1);
  menu_layout(sm2);
  menu_layout(sm3);
  m=add_menu("Icons", DISABLED);
  add_item(m,"Open",'O',DISABLED);
  add_item(m,"Copy",'C',DISABLED);
  add_item(m,"Rename...",'R',DISABLED);
  add_item(m,"Information...",'I',DISABLED);
  add_item(m,"Snapshot",'S',DISABLED);
  add_item(m,"UnSnapshot",'U',DISABLED);
  add_item(m,"Leave Out",'L',DISABLED);
  add_item(m,"Put Away",'P',DISABLED);
  add_item(m,NULL,0,DISABLED);
  add_item(m,"Delete...",'D',DISABLED);
  add_item(m,"Format Disk...",0,DISABLED);
  add_item(m,"Empty Trash",0,DISABLED);
  menu_layout(m);
  m=add_menu("Tools", 0);
#ifdef AMIGAOS
  add_item(m,"ResetWB",0,DISABLED);
#else
  add_item(m,"ResetWB",0,0);
#endif

  for(ti=firsttoolitem; ti; ti=ti->next)
    if(ti->level<0)
      ti->submenu=sm1=sub_menu(add_item(m,ti->name,ti->hotkey,0),0);
    else {
      ti->submenu=NULL;
      add_item((ti->level? sm1:m), ti->name,ti->hotkey,(ti->cmd? 0:DISABLED));
    }
  menu_layout(m);
  for(ti=firsttoolitem; ti; ti=ti->next)
    if(ti->submenu) {
      menu_layout(ti->submenu);
      ti->submenu=NULL;
    }
  if(prefs.screenmenu) {
    m=add_menu("Screens",0);
    add_item(m,"New Screen",0,0);
    add_item(m,"Delete Screen",0,0);
    menu_layout(m);
  }
}

void redrawmenubar(Window w)
{
  static const char defaultTimeFormat[] = "%c";

  struct Menu *m;
  struct Item *item;

  if(!w)
    return;
  if(w==scr->menubar) {
    XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[BARDETAILPEN]);
    XSetBackground(dpy, scr->menubargc, scr->dri.dri_Pens[BARBLOCKPEN]);
#ifdef USE_FONTSETS
    XmbDrawImageString(dpy, w, scr->dri.dri_FontSet,
		       scr->menubargc, 4, 1+scr->dri.dri_Ascent,
		       scr->title, strlen(scr->title));
#else
    XDrawImageString(dpy, w, scr->menubargc, 4, 1+scr->dri.dri_Ascent,
		     scr->title, strlen(scr->title));
#endif
    XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[BARTRIMPEN]);  
    XDrawLine(dpy, w, scr->menubargc, 0, scr->bh-1, scr->width-1, scr->bh-1);
    if( prefs.titlebarclock )
    {
      char clockbuf[512];
      const char * fmt = defaultTimeFormat;
      time_t the_time;
      int l;
      if( prefs.titleclockformat ) fmt = prefs.titleclockformat;
      time( &the_time );
      strftime( clockbuf, 512, fmt, localtime( &the_time ) );
#ifdef USE_FONTSETS
      l = XmbTextEscapement(scr->dri.dri_FontSet, clockbuf, strlen(clockbuf));
      XmbDrawImageString(dpy, w, scr->dri.dri_FontSet, scr->menubargc,
			 (scr->width-30-l), 1+scr->dri.dri_Ascent,
			 clockbuf, strlen(clockbuf));
#else
      l = XTextWidth(scr->dri.dri_Font, clockbuf, strlen(clockbuf));
      XDrawImageString( dpy, w, scr->menubargc,(scr->width-30-l),
			1+scr->dri.dri_Ascent, clockbuf, strlen(clockbuf));
#endif
    }    
  } else if(w==scr->menubardepth) {
    if(!mbdclick) {
      XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[SHADOWPEN]);
      XDrawRectangle(dpy, w, scr->menubargc, 4, scr->h2, 10, scr->h6-scr->h2);
    }
    XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[SHINEPEN]);
    XFillRectangle(dpy, w, scr->menubargc, 8, scr->h4, 10, scr->h8-scr->h4);
    XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[SHADOWPEN]);
    XDrawRectangle(dpy, w, scr->menubargc, 8, scr->h4, 10, scr->h8-scr->h4);
    if(mbdclick)
      XDrawRectangle(dpy, w, scr->menubargc, 4, scr->h2, 10, scr->h6-scr->h2);
    XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[mbdclick?SHADOWPEN:SHINEPEN]);
    XDrawLine(dpy, w, scr->menubargc, 0, 0, 22, 0);
    XDrawLine(dpy, w, scr->menubargc, 0, 0, 0, scr->bh-2);
    XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[mbdclick?SHINEPEN:SHADOWPEN]);
    XDrawLine(dpy, w, scr->menubargc, 0, scr->bh-1, 22, scr->bh-1);
    XDrawLine(dpy, w, scr->menubargc, 22, 0, 22, scr->bh-1);
  } else {
    for(m=scr->firstmenu; m; m=m->next)
      if(m->win==w)
	redraw_menu(m, w);
    if(activemenu) {
      for(item=activemenu->firstitem; item; item=item->next)
	if(item->win==w)
	  redraw_item(item, w);
      if(w==activemenu->parent) {
	XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[BARDETAILPEN]);
	XDrawLine(dpy, w, scr->menubargc, 0, 0, 0, activemenu->height-1);
	XDrawLine(dpy, w, scr->menubargc, activemenu->width-1, 0,
		  activemenu->width-1, activemenu->height-1);
      }
    }
    if(activesubmenu) {
      for(item=activesubmenu->firstitem; item; item=item->next)
	if(item->win==w)
	  redraw_item(item, w);
      if(w==activesubmenu->parent) {
	XSetForeground(dpy, scr->menubargc, scr->dri.dri_Pens[BARDETAILPEN]);
	XDrawLine(dpy, w, scr->menubargc, 0, 0, 0, activesubmenu->height-1);
	XDrawLine(dpy, w, scr->menubargc, activesubmenu->width-1, 0,
		  activesubmenu->width-1, activesubmenu->height-1);
      }      
    }
  }
}

static void leave_item(struct Item *i, Window w)
{
  if(i==activesubitem)
    activesubitem=NULL;
  if(i==activeitem)
    if(activesubmenu && i->sub==activesubmenu)
      return;
    else
      activeitem=NULL;
  XSetWindowBackground(dpy, i->win, scr->dri.dri_Pens[BARBLOCKPEN]);
  XClearWindow(dpy, i->win);
  redraw_item(i, i->win);    
}

static void enter_item(struct Item *i, Window w)
{
  if(activesubitem)
    leave_item(activesubitem, activesubitem->win);
  if(activesubmenu!=i->sub && i->menu!=activesubmenu) {
    if(activesubmenu)
      XUnmapWindow(dpy, activesubmenu->parent);
    if(i->sub)
      XMapRaised(dpy, i->sub->parent);
    activesubmenu=i->sub;
  }
  if(activeitem && !(activeitem->sub && i->menu==activeitem->sub))
    leave_item(activeitem, activeitem->win);
  if(!(i->flags&DISABLED)) {
    if(activeitem)
      activesubitem=i;
    else
      activeitem=i;
    XSetWindowBackground(dpy, i->win, scr->dri.dri_Pens[BARDETAILPEN]);
    XClearWindow(dpy, i->win);
    redraw_item(i, i->win);    
  }
}

static void enter_menu(struct Menu *m, Window w)
{
  if(m!=activemenu) {
    struct Menu *oa=activemenu;
    if(activesubitem)
      leave_item(activeitem, activesubitem->win);
    if(activesubmenu) {
      XUnmapWindow(dpy, activesubmenu->parent);
      activesubmenu=NULL;
    }
    if(activeitem)
      leave_item(activeitem, activeitem->win);
    if(!(m->flags & DISABLED))
      XSetWindowBackground(dpy, w, scr->dri.dri_Pens[BARDETAILPEN]);
    XClearWindow(dpy, w);
    redraw_menu(activemenu=m, w);
    if(m->parent)
      XMapRaised(dpy, m->parent);
    if(oa) {
      if(oa->parent)
	XUnmapWindow(dpy, oa->parent);
      XSetWindowBackground(dpy, oa->win, scr->dri.dri_Pens[BARBLOCKPEN]);
      XClearWindow(dpy, oa->win);
      redraw_menu(oa, oa->win);
    }
  }
}

void menubar_enter(Window w)
{
  struct Menu *m;
  struct Item *i;

  for(m=scr->firstmenu; m; m=m->next)
    if(m->win==w) {
      enter_menu(m, w);
      return;
    }
  if((m=activemenu))
    for(i=m->firstitem; i; i=i->next)
      if(w==i->win) {
	enter_item(i, w);
	return;
      }
  if((m=activesubmenu))
    for(i=m->firstitem; i; i=i->next)
      if(w==i->win) {
	enter_item(i, w);
	return;
      }
}

void menubar_leave(Window w)
{
  if(activesubitem && activesubitem->win==w)
    leave_item(activesubitem, w);
  if(activeitem && activeitem->win==w)
    leave_item(activeitem, w);
}

void menu_on()
{
  Window r, c;
  int rx, ry, x, y;
  unsigned int m;

  if(scr->menubarparent) {
    XMapRaised(dpy, scr->menubarparent);
    XRaiseWindow(dpy, scr->menubar);
    XGrabPointer(dpy, scr->back, True, ButtonPressMask|ButtonReleaseMask|
		EnterWindowMask|LeaveWindowMask, GrabModeAsync, GrabModeAsync,
		scr->back, wm_curs, CurrentTime);
    XSetInputFocus(dpy, scr->menubar, RevertToParent, CurrentTime);
    if(XQueryPointer(dpy, scr->menubarparent, &r, &c, &rx, &ry, &x, &y, &m))
      menubar_enter(c);
  }
}

void menuaction(struct Item *i, struct Item *si)
{
  extern void restart_amiwm(void);
  extern int screen_has_clients(void);
  struct Menu *m;
  struct Item *mi;
  struct ToolItem *ti;
  int menu=0, item=0, sub=0;

  for(m=i->menu->next; m; m=m->next) menu++;
  for(mi=i->menu->firstitem; mi&&mi!=i; mi=mi->next) item++;
  if(i->sub) {
    for(mi=i->sub->firstitem; mi&&mi!=si; mi=mi->next) sub++;
    if(!mi)
      sub=-1;
  } else
    --sub;
  mi=(sub>=0? si:i);
  if(mi->flags & DISABLED)
    return;
  if(mi->owner) {
    mod_menuselect(mi->owner, menu, item, sub);
    return;
  }
  switch(menu) {
  case 0: /* Workbench */
    switch(item) {
    case 1:
      spawn(BIN_PREFIX"executecmd");
      break;
    case 2:
      {
	XSetWindowAttributes xswa;
	unsigned long mask;
	Window win;
	xswa.background_pixmap = None;
	xswa.override_redirect = True;
	xswa.backing_store = NotUseful;
	xswa.save_under = False;
	mask = CWBackPixmap|CWOverrideRedirect|CWBackingStore|CWSaveUnder;
	win = XCreateWindow(dpy, scr->back, 0, 0, scr->width, scr->height,
			    0, scr->depth, InputOutput,
			    scr->visual, mask, &xswa);
	XMapWindow(dpy, win);
	XDestroyWindow(dpy, win);
      }
      break;
    case 5:
#ifdef AMIGAOS
      spawn(BIN_PREFIX"requestchoice >NIL: amiwm \""
	    ABOUT_STRING("*N") "\" Ok");
#else
      spawn(BIN_PREFIX"requestchoice >/dev/null amiwm '"
	    ABOUT_STRING("\n") "' Ok");
#endif
      break;      
    case 6:
#ifndef AMIGAOS
      if(prefs.fastquit) {
#endif
	exit(0);
#ifndef AMIGAOS
      } else {
#ifdef HAVE_ALLOCA
	char *buf=alloca(256);
#else
	char buf[256];
#endif
	sprintf(buf, "; export DISPLAY; ( if [ `"BIN_PREFIX"requestchoice "
		"Workbench 'Do you really want\nto quit workbench?' Ok Cancel`"
		" = 1 ]; then kill %d; fi; )", (int)getpid());
	spawn(buf);
      }
#endif
      break;
    }
    break;
  case 1: /* Window */
    switch(item) {
    case 4:
      select_all_icons(scr);
      break;
    case 5:
      cleanupicons();
      break;
    }
    break;
  case 2: /* Icons */
    break;
  case 3: /* Tools */
#ifndef AMIGAOS
    if(item==0)
      restart_amiwm();
#endif
    if(item>0) {
      int it=0, si=-1;
      for(ti=firsttoolitem; ti; ti=ti->next) {
	if(ti->level>0)
	  si++;
	else {
	  it++;
	  si=-1;
	}
	if(it==item && si==sub) break;
      }
      if(ti && ti->cmd) spawn(ti->cmd);
    }
    break;
  case 4: /* Screens */
    if(item==0) {
      openscreen("New Screen", DefaultRootWindow(dpy));
      realizescreens();
      scr=front->upfront;
      screentoback();
    }
    if(item==1) {
      if(scr->behind == scr)
	wberror(scr,"Cannot close last Screen");
      else if(screen_has_clients())
	wberror(scr,"Can't close a screen with running programs on it");
      else
	closescreen();
    }
    break;
  }
}

void menu_off()
{
  struct Menu *oa;
  struct Item *oi, *osi;

  if(scr->menubarparent) {
    Window r,p,*children;
    unsigned int nchildren;
    XUngrabPointer(dpy, CurrentTime);
    setfocus((activeclient && activeclient->state==NormalState?
	      activeclient->window:None));
    XUnmapWindow(dpy, scr->menubarparent);
    if(XQueryTree(dpy, scr->back, &r, &p, &children, &nchildren)) {
      int n;
      Client *c2;
      for(n=0; n<nchildren; n++)
	if((!XFindContext(dpy, children[n], client_context, (XPointer*)&c2)) &&
	   children[n]==c2->parent)
	  break;
      if(n<nchildren) {
	Window ws[2];
	ws[0]=children[n];
	ws[1]=scr->menubar;
	XRestackWindows(dpy, ws, 2);
      }
      if(children) XFree(children);
    }
  }
  if((osi=activesubitem))
    leave_item(osi, osi->win);
  if((oi=activeitem))
    leave_item(oi, oi->win);
  if((oa=activesubmenu)) {
    activesubmenu=NULL;
    if(oa->parent)
      XUnmapWindow(dpy, oa->parent);
  }
  if((oa=activemenu)) {
    activemenu=NULL;
    if(oa->parent)
      XUnmapWindow(dpy, oa->parent);
    XSetWindowBackground(dpy, oa->win, scr->dri.dri_Pens[BARBLOCKPEN]);
    XClearWindow(dpy, oa->win);
    redraw_menu(oa, oa->win);
  }
  if(oi) {
    XSync(dpy, False);
    menuaction(oi, osi);
  }
}

struct Item *getitembyhotkey(KeySym key)
{
  struct Menu *m;
  struct Item *i;

  if(key) {
    if(key>='a' && key<='z')
      key-=0x20;
    for(m=scr->firstmenu; m; m=m->next)
      for(i=m->firstitem; i; i=i->next)
	if(i->hotkey==key)
	  return i;
  }
  return NULL;
}

static struct Item *own_item(struct module *m, struct Item *i, struct Item *c)
{
  if(i->owner)
    return NULL;
  i->owner = m;
  i->mod_chain = c;
  i->flags &= ~DISABLED;
  redraw_item(i, i->win);
  return i;
}

void disown_item_chain(struct module *m, struct Item *i)
{
  while(i)
    if(i->owner == m) {
      struct Item *c = i->mod_chain;
      i->owner = NULL;
      i->mod_chain = NULL;
      i->flags |= DISABLED;
      redraw_item(i, i->win);
      i = c;
    } else
      i=i->mod_chain;
}

struct Item *own_items(struct module *m, Scrn *s,
		       int menu, int item, int sub, struct Item *c)
{
  struct Item *cl, *chain = NULL, *endlink = NULL;
  int m0, m1, mn;
  struct Menu *mm;
  if(!s) return NULL;
  if(menu<0) {
    m0 = 0; m1 = 65535;
  } else m0 = m1 = menu;
  for(mn=0, mm=s->firstmenu; mm && mn <= m1; mm=mm->next, mn++)
    if(mn>=m0) {
      int i0, i1, in;
      struct Item *ii;
      if(item<0) {
	i0 = 0; i1 = 65535;
      } else i0 = i1 = item;
      for(in=0, ii=mm->firstitem; ii && in <= i1; ii=ii->next, in++)
	if(in>=i0) {
	  int s0, s1, sn;
	  struct Item *ss;
	  if(!(cl=own_item(m, ii, chain))) {
	    disown_item_chain(m, chain);
	    return NULL;
	  } else chain=cl;
	  if(!endlink) endlink=chain;
	  if(ii->sub) {
	    if(sub<0) {
	      s0 = 0; s1 = 65535;
	    } else s0 = s1 = sub;
	    for(sn=0, ss=ii->sub->firstitem; ss && sn <= i1; ss=ss->next, sn++)
	      if(sn>=s0) {
		if(!(cl=own_item(m, ss, chain))) {
		  disown_item_chain(m, chain);
		  return NULL;
		} else chain=cl;
	      }
	  }
	}
    }
  if(endlink)
    endlink->next = c;
  return chain;
}
