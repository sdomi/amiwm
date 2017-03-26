#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <errno.h>
#ifdef USE_FONTSETS
#include <locale.h>
#include <wchar.h>
#endif

#include "drawinfo.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

#define MAX_CMD_CHARS 256
#define VISIBLE_CMD_CHARS 35

#define BOT_SPACE 4
#define TEXT_SIDE 8
#define BUT_SIDE 12
#define TOP_SPACE 4
#define INT_SPACE 7
#define BUT_VSPACE 2
#define BUT_HSPACE 8

static const char ok_txt[]="Ok", cancel_txt[]="Cancel";
static const char enter_txt[]="Enter Command and its Arguments:";
static const char cmd_txt[]="Command:";

static int selected=0, depressed=0, stractive=1;
static Window button[3];
static const char *buttxt[]={ NULL, ok_txt, cancel_txt };

char cmdline[MAX_CMD_CHARS+1];
int buf_len=0;
int cur_pos=0;
int left_pos=0;
int cur_x=6;

char *progname;

Display *dpy;

struct DrawInfo dri;

Window root, mainwin, strwin;
GC gc;

int strgadw, strgadh, fh, mainw, mainh, butw;

#ifdef USE_FONTSETS
static XIM xim = (XIM) NULL;
static XIC xic = (XIC) NULL;
#endif

int getchoice(Window w)
{
  int i;
  for(i=1; i<3; i++)
    if(button[i]==w)
      return i;
  return 0;
}

void refresh_button(Window w, const char *txt, int idx)
{
  int h=fh+2*BUT_VSPACE, l=strlen(txt);
#ifdef USE_FONTSETS
  int tw=XmbTextEscapement(dri.dri_FontSet, txt, l);
#else
  int tw=XTextWidth(dri.dri_Font, txt, l);
#endif
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
#ifdef USE_FONTSETS
  XmbDrawString(dpy, w, dri.dri_FontSet, gc, (butw-tw)>>1,
		dri.dri_Ascent+BUT_VSPACE, txt, l);
#else
  XDrawString(dpy, w, gc, (butw-tw)>>1,
	      dri.dri_Ascent+BUT_VSPACE, txt, l);
#endif
  XSetForeground(dpy, gc, dri.dri_Pens[(selected==idx && depressed)?
				     SHADOWPEN:SHINEPEN]);
  XDrawLine(dpy, w, gc, 0, 0, butw-2, 0);
  XDrawLine(dpy, w, gc, 0, 0, 0, h-2);
  XSetForeground(dpy, gc, dri.dri_Pens[(selected==idx && depressed)?
				     SHINEPEN:SHADOWPEN]);
  XDrawLine(dpy, w, gc, 1, h-1, butw-1, h-1);
  XDrawLine(dpy, w, gc, butw-1, 1, butw-1, h-1);  
  XSetForeground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
  XDrawPoint(dpy, w, gc, butw-1, 0);
  XDrawPoint(dpy, w, gc, 0, h-1);
}

void refresh_main(void)
{
  int w;

  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
#ifdef USE_FONTSETS
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, TEXT_SIDE,
		TOP_SPACE+dri.dri_Ascent, enter_txt, strlen(enter_txt));
#else
  XDrawString(dpy, mainwin, gc, TEXT_SIDE, TOP_SPACE+dri.dri_Ascent,
	      enter_txt, strlen(enter_txt));
#endif
  XSetForeground(dpy, gc, dri.dri_Pens[HIGHLIGHTTEXTPEN]);
#ifdef USE_FONTSETS
  w=XmbTextEscapement(dri.dri_FontSet, cmd_txt, strlen(cmd_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc,
		mainw-strgadw-w-TEXT_SIDE-BUT_SIDE,
		TOP_SPACE+fh+INT_SPACE+dri.dri_Ascent,
		cmd_txt, strlen(cmd_txt));
#else
  w=XTextWidth(dri.dri_Font, cmd_txt, strlen(cmd_txt));
  XDrawString(dpy, mainwin, gc, mainw-strgadw-w-TEXT_SIDE-BUT_SIDE,
	      TOP_SPACE+fh+INT_SPACE+dri.dri_Ascent,
	      cmd_txt, strlen(cmd_txt));
#endif
}

void refresh_str_text(void)
{
  int l, mx=6;
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  if(buf_len>left_pos) {
#ifdef USE_FONTSETS
    int w, c;
    for(l=0; l<buf_len-left_pos; ) {
      c=mbrlen(cmdline+left_pos+l, buf_len-left_pos-l, NULL);
      w=6+XmbTextEscapement(dri.dri_FontSet, cmdline+left_pos, l+c);
      if(w>strgadw-6)
	break;
      mx=w;
      l+=c;
    }
    XmbDrawImageString(dpy, strwin, dri.dri_FontSet, gc, 6, 3+dri.dri_Ascent,
		       cmdline+left_pos, l);
#else
    mx+=XTextWidth(dri.dri_Font, cmdline+left_pos, l=buf_len-left_pos);
    while(mx>strgadw-6)
      mx-=XTextWidth(dri.dri_Font, cmdline+left_pos+--l, 1);
    XDrawImageString(dpy, strwin, gc, 6, 3+dri.dri_Ascent,
		     cmdline+left_pos, l);
#endif
  }
  XSetForeground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
  XFillRectangle(dpy, strwin, gc, mx, 3, strgadw-mx-6, fh);
  if(stractive) {
    if(cur_pos<buf_len) {
      XSetBackground(dpy, gc, ~0);
#ifdef USE_FONTSETS
      l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
      XmbDrawImageString(dpy, strwin, dri.dri_FontSet, gc, cur_x,
			 3+dri.dri_Ascent, cmdline+cur_pos, l);
#else
      XDrawImageString(dpy, strwin, gc, cur_x, 3+dri.dri_Ascent,
		       cmdline+cur_pos, 1);
#endif
      XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
    } else {
      XSetForeground(dpy, gc, ~0);
#ifdef USE_FONTSETS
      XFillRectangle(dpy, strwin, gc, cur_x, 3,
		     XExtentsOfFontSet(dri.dri_FontSet)->
		     max_logical_extent.width, fh);
#else
      XFillRectangle(dpy, strwin, gc, cur_x, 3,
		     dri.dri_Font->max_bounds.width, fh);
#endif
    }
  }
}

void refresh_str(void)
{
  refresh_str_text();
  XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
  XDrawLine(dpy, strwin, gc, 0, strgadh-1, 0, 0);
  XDrawLine(dpy, strwin, gc, 0, 0, strgadw-2, 0);
  XDrawLine(dpy, strwin, gc, 3, strgadh-2, strgadw-4, strgadh-2);
  XDrawLine(dpy, strwin, gc, strgadw-4, strgadh-2, strgadw-4, 2);
  XDrawLine(dpy, strwin, gc, 1, 1, 1, strgadh-2);
  XDrawLine(dpy, strwin, gc, strgadw-3, 1, strgadw-3, strgadh-2);
  XSetForeground(dpy, gc, dri.dri_Pens[SHADOWPEN]);
  XDrawLine(dpy, strwin, gc, 1, strgadh-1, strgadw-1, strgadh-1);
  XDrawLine(dpy, strwin, gc, strgadw-1, strgadh-1, strgadw-1, 0);
  XDrawLine(dpy, strwin, gc, 3, strgadh-3, 3, 1);
  XDrawLine(dpy, strwin, gc, 3, 1, strgadw-4, 1);
  XDrawLine(dpy, strwin, gc, 2, 1, 2, strgadh-2);
  XDrawLine(dpy, strwin, gc, strgadw-2, 1, strgadw-2, strgadh-2);
}

void strkey(XKeyEvent *e)
{
  void endchoice(void);
#ifdef USE_FONTSETS
  Status stat;
#else
  static XComposeStatus stat;
#endif
  KeySym ks;
  char buf[256];
  int x, i, n;
#ifndef USE_FONTSETS
  n=XLookupString(e, buf, sizeof(buf), &ks, &stat);
#else
  n=XmbLookupString(xic, e, buf, sizeof(buf), &ks, &stat);
  if(stat == XLookupKeySym || stat == XLookupBoth)
#endif
  switch(ks) {
  case XK_Return:
  case XK_Linefeed:
    selected=1;
    endchoice();
    break;
  case XK_Left:
    if(cur_pos) {
#ifdef USE_FONTSETS
      int p=cur_pos;
      int z;
      while(p>0) {
	--p;
	if(((int)mbrlen(cmdline+p, cur_pos-p, NULL))>0) {
	  cur_pos=p;
	  break;
	}
      }
#else
      --cur_pos;
#endif
    }
    break;
  case XK_Right:
    if(cur_pos<buf_len) {
#ifdef USE_FONTSETS
      int l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
      if(l>0)
	cur_pos+=l;
#else
      cur_pos++;
#endif
    }
    break;
  case XK_Begin:
    cur_pos=0;
    break;
  case XK_End:
    cur_pos=buf_len;
    break;
  case XK_Delete:
    if(cur_pos<buf_len) {
      int l=1;
#ifdef USE_FONTSETS
      l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
      if(l<=0)
	break;
#endif
      buf_len-=l;
      for(x=cur_pos; x<buf_len; x++)
	cmdline[x]=cmdline[x+l];
      cmdline[x] = 0;
    } else XBell(dpy, 100);
    break;
  case XK_BackSpace:
    if(cur_pos>0) {
      int l=1;
#ifdef USE_FONTSETS
      int p=cur_pos;
      while(p>0) {
	--p;
	if(((int)mbrlen(cmdline+p, cur_pos-p, NULL))>0) {
	  l=cur_pos-p;
	  break;
	}
      }
#endif
      buf_len-=l;
      for(x=(cur_pos-=l); x<buf_len; x++)
	cmdline[x]=cmdline[x+l];
      cmdline[x] = 0;
    } else XBell(dpy, 100);
    break;
#ifdef USE_FONTSETS
  default:
    if(stat == XLookupBoth)
      stat = XLookupChars;
  }
  if(stat == XLookupChars) {
#else
  default:
#endif
    for(i=0; i<n && buf_len<MAX_CMD_CHARS; i++) {
      for(x=buf_len; x>cur_pos; --x)
	cmdline[x]=cmdline[x-1];
      cmdline[cur_pos++]=buf[i];
      buf_len++;
    }
    if(i<n)
      XBell(dpy, 100);
  }
  if(cur_pos<left_pos)
    left_pos=cur_pos;
  cur_x=6;
#ifdef USE_FONTSETS
  if(cur_pos>left_pos)
    cur_x+=XmbTextEscapement(dri.dri_FontSet, cmdline+left_pos, cur_pos-left_pos);
  if(cur_pos<buf_len) {
    int l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
    x=XmbTextEscapement(dri.dri_FontSet, cmdline+cur_pos, l);
  } else
    x=XExtentsOfFontSet(dri.dri_FontSet)->max_logical_extent.width;
#else
  if(cur_pos>left_pos)
    cur_x+=XTextWidth(dri.dri_Font, cmdline+left_pos, cur_pos-left_pos);
  if(cur_pos<buf_len)
    x=XTextWidth(dri.dri_Font, cmdline+cur_pos, 1);
  else
    x=dri.dri_Font->max_bounds.width;
#endif
  if((x+=cur_x-(strgadw-6))>0) {
    cur_x-=x;
    while(x>0) {
#ifdef USE_FONTSETS
      int l=mbrlen(cmdline+left_pos, buf_len-left_pos, NULL);
      x-=XmbTextEscapement(dri.dri_FontSet, cmdline+left_pos, l);
      left_pos += l;
#else
      x-=XTextWidth(dri.dri_Font, cmdline+left_pos++, 1);
#endif
    }
    cur_x+=x;
  }
  refresh_str_text();
}

void strbutton(XButtonEvent *e)
{
  int w, l=1;
  stractive=1;
  cur_pos=left_pos;
  cur_x=6;
  while(cur_x<e->x && cur_pos<buf_len) {
#ifdef USE_FONTSETS
    l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
    if(l<=0)
      break;
    w=XmbTextEscapement(dri.dri_FontSet, cmdline+cur_pos, l);
#else
    w=XTextWidth(dri.dri_Font, cmdline+cur_pos, 1);
#endif
    if(cur_x+w>e->x)
      break;
    cur_x+=w;
    cur_pos+=l;
  }
  refresh_str();
}

void toggle(int c)
{
  XSetWindowBackground(dpy, button[c], dri.dri_Pens[(depressed&&c==selected)?
						  FILLPEN:BACKGROUNDPEN]);
  XClearWindow(dpy, button[c]);
  refresh_button(button[c], buttxt[c], c);
}

void abortchoice()
{
  if(depressed) {
    depressed=0;
    toggle(selected);
  }
  selected=0;
}

void endchoice()
{
  int c=selected;

  abortchoice();
  XCloseDisplay(dpy);
  if(c==1)
    system(cmdline);
  exit(0);
}

int main(int argc, char *argv[])
{
  XWindowAttributes attr;
  static XSizeHints size_hints;
  static XTextProperty txtprop1, txtprop2;
  Window ok, cancel;
  int w2, c;
#ifdef USE_FONTSETS
  char *p;

  setlocale(LC_CTYPE, "");
#endif
  progname=argv[0];
  if(!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "%s: cannot connect to X server %s\n", progname,
	    XDisplayName(NULL));
    exit(1);
  }
  root = RootWindow(dpy, DefaultScreen(dpy));
  XGetWindowAttributes(dpy, root, &attr);
  init_dri(&dri, dpy, root, attr.colormap, False);

#ifdef USE_FONTSETS
  strgadw=VISIBLE_CMD_CHARS*XExtentsOfFontSet(dri.dri_FontSet)->
    max_logical_extent.width+12;
#else
  strgadw=VISIBLE_CMD_CHARS*dri.dri_Font->max_bounds.width+12;
#endif
  strgadh=(fh=dri.dri_Ascent+dri.dri_Descent)+6;

#ifdef USE_FONTSETS
  butw=XmbTextEscapement(dri.dri_FontSet, ok_txt, strlen(ok_txt))+2*BUT_HSPACE;
  w2=XmbTextEscapement(dri.dri_FontSet, cancel_txt, strlen(cancel_txt))+2*BUT_HSPACE;
#else
  butw=XTextWidth(dri.dri_Font, ok_txt, strlen(ok_txt))+2*BUT_HSPACE;
  w2=XTextWidth(dri.dri_Font, cancel_txt, strlen(cancel_txt))+2*BUT_HSPACE;
#endif
  if(w2>butw)
    butw=w2;

  mainw=2*(BUT_SIDE+butw)+BUT_SIDE;
#ifdef USE_FONTSETS
  w2=XmbTextEscapement(dri.dri_FontSet, enter_txt, strlen(enter_txt))+2*TEXT_SIDE;
#else
  w2=XTextWidth(dri.dri_Font, enter_txt, strlen(enter_txt))+2*TEXT_SIDE;
#endif
  if(w2>mainw)
    mainw=w2;
#ifdef USE_FONTSETS
  w2=strgadw+XmbTextEscapement(dri.dri_FontSet, cmd_txt, strlen(cmd_txt))+
    2*TEXT_SIDE+2*BUT_SIDE+butw;
#else
  w2=strgadw+XTextWidth(dri.dri_Font, cmd_txt, strlen(cmd_txt))+
    2*TEXT_SIDE+2*BUT_SIDE+butw;
#endif
  if(w2>mainw)
    mainw=w2;

  mainh=3*fh+TOP_SPACE+BOT_SPACE+2*INT_SPACE+2*BUT_VSPACE;
  
  mainwin=XCreateSimpleWindow(dpy, root, 20, 20, mainw, mainh, 1,
			      dri.dri_Pens[SHADOWPEN],
			      dri.dri_Pens[BACKGROUNDPEN]);
  strwin=XCreateSimpleWindow(dpy, mainwin, mainw-BUT_SIDE-strgadw,
			     TOP_SPACE+fh+INT_SPACE-3,
			     strgadw, strgadh, 0,
			     dri.dri_Pens[SHADOWPEN],
			     dri.dri_Pens[BACKGROUNDPEN]);
  ok=XCreateSimpleWindow(dpy, mainwin, BUT_SIDE,
			 mainh-BOT_SPACE-2*BUT_VSPACE-fh,
			 butw, fh+2*BUT_VSPACE, 0,
			 dri.dri_Pens[SHADOWPEN],
			 dri.dri_Pens[BACKGROUNDPEN]);
  cancel=XCreateSimpleWindow(dpy, mainwin, mainw-butw-BUT_SIDE,
			     mainh-BOT_SPACE-2*BUT_VSPACE-fh,
			     butw, fh+2*BUT_VSPACE, 0,
			     dri.dri_Pens[SHADOWPEN],
			     dri.dri_Pens[BACKGROUNDPEN]);
  button[0]=None;
  button[1]=ok;
  button[2]=cancel;
  XSelectInput(dpy, mainwin, ExposureMask|KeyPressMask|ButtonPressMask);
  XSelectInput(dpy, strwin, ExposureMask|ButtonPressMask);
  XSelectInput(dpy, ok, ExposureMask|ButtonPressMask|ButtonReleaseMask|
		 EnterWindowMask|LeaveWindowMask);
  XSelectInput(dpy, cancel, ExposureMask|ButtonPressMask|ButtonReleaseMask|
		 EnterWindowMask|LeaveWindowMask);
  gc=XCreateGC(dpy, mainwin, 0, NULL);
  XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
#ifndef USE_FONTSETS
  XSetFont(dpy, gc, dri.dri_Font->fid);
#endif

#ifdef USE_FONTSETS
  if ((p = XSetLocaleModifiers("@im=none")) != NULL && *p)
    xim = XOpenIM(dpy, NULL, NULL, NULL);
  if (!xim)
    fprintf(stderr, "Failed to open input method.\n");
  else {
    xic = XCreateIC(xim,
		    XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
		    XNClientWindow, mainwin,
		    XNFocusWindow, mainwin,
		    NULL);
    if (!xic)
      fprintf(stderr, "Failed to create input context.\n");
  }
  if (!xic)
    exit(1);
#endif

  size_hints.flags = PResizeInc;
  txtprop1.value=(unsigned char *)"Execute a File";
  txtprop2.value=(unsigned char *)"ExecuteCmd";
  txtprop2.encoding=txtprop1.encoding=XA_STRING;
  txtprop2.format=txtprop1.format=8;
  txtprop1.nitems=strlen((char *)txtprop1.value);
  txtprop2.nitems=strlen((char *)txtprop2.value);
  XSetWMProperties(dpy, mainwin, &txtprop1, &txtprop2, argv, argc,
		   &size_hints, NULL, NULL);
  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);
  for(;;) {
    XEvent event;
    XNextEvent(dpy, &event);
#ifdef USE_FONTSETS
    if(!XFilterEvent(&event, mainwin))
#endif
    switch(event.type) {
    case Expose:
      if(!event.xexpose.count)
	if(event.xexpose.window == mainwin)
	  refresh_main();	
	else if(event.xexpose.window == strwin)
	  refresh_str();
	else if(event.xexpose.window == ok)
	  refresh_button(ok, ok_txt, 1);
	else if(event.xexpose.window == cancel)
	  refresh_button(cancel, cancel_txt, 2);
    case LeaveNotify:
      if(depressed && event.xcrossing.window==button[selected]) {
	depressed=0;
	toggle(selected);
      }
      break;
    case EnterNotify:
      if((!depressed) && selected && event.xcrossing.window==button[selected]) {
	depressed=1;
	toggle(selected);
      }
      break;
    case ButtonPress:
      if(event.xbutton.button==Button1) {
	if(stractive && event.xbutton.window!=strwin) {
	  stractive=0;
	  refresh_str();
	}
	if((c=getchoice(event.xbutton.window))) {
	  abortchoice();
	  depressed=1;
	  toggle(selected=c);
	} else if(event.xbutton.window==strwin)
	  strbutton(&event.xbutton);
      }
      break;
    case ButtonRelease:
      if(event.xbutton.button==Button1 && selected)
	if(depressed)
	  endchoice();
	else
	  abortchoice();
      break;
    case KeyPress:
      if(stractive)
	strkey(&event.xkey);
    }
  }
}
