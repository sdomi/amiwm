#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "libami.h"
#include "drawinfo.h"

#ifdef BSD_STYLE_GETTIMEOFDAY
#define GETTIMEOFDAY(tp) gettimeofday(tp, NULL)
#else
#define GETTIMEOFDAY(tp) gettimeofday(tp)
#endif

#define FIXUPTV(tv) { \
    while((tv).tv_usec<0) { (tv).tv_usec+=1000000; (tv).tv_sec--; } \
    while((tv).tv_usec>=1000000) { (tv).tv_usec-=1000000; (tv).tv_sec++; } \
}

#define BIN_PREFIX AMIWM_HOME"/"

#define NFMT 2

char fmt[NFMT+2][80];
int fmtw[NFMT];


Display *dpy;
char *progname;
unsigned long color_scheme[2];
int scr, coloralloc, state=0;
Window root, win, win2;
Colormap cmap;
GC gc;
XFontStruct *font;
int gh, gw, ascent;
char *rcpath = BIN_PREFIX"requestchoice";
char *rcmsg = "Notice";
Pixmap disk_icon1, disk_icon2;
struct ColorStore colorstore1, colorstore2;
struct DrawInfo dri;
Atom appiconmsg;
char *currentdir = NULL;
XContext dircontext;
int my_in_fd, my_out_fd;

void load_diskicon()
{
  struct DiskObject *disk_do;

  disk_do = GetDefDiskObject(1/*WBDISK*/);
  if(disk_do == NULL)
    disk_do = GetDefDiskObject(2/*WBDRAWER*/);
  if(disk_do == NULL)
    disk_do = GetDefDiskObject(3/*WBTOOL*/);
  if(disk_do == NULL) {
    md_errormsg(md_root, "Failed to load disk icon");
    md_exit(0);
  }
  disk_icon1 =
    md_image_to_pixmap(md_root, dri.dri_Pens[BACKGROUNDPEN],
		       (struct Image *)disk_do->do_Gadget.GadgetRender,
		       disk_do->do_Gadget.Width, disk_do->do_Gadget.Height,
		       &colorstore1);

  disk_icon2 =
    md_image_to_pixmap(md_root, dri.dri_Pens[BACKGROUNDPEN],
		       (struct Image *)disk_do->do_Gadget.SelectRender,
		       disk_do->do_Gadget.Width, disk_do->do_Gadget.Height,
		       &colorstore2);

  FreeDiskObject(disk_do);
  XSync(dpy, False);
}

void broker_cb(XEvent *evt, unsigned long mask)
{
  XPointer ret;

  if(evt->type != ClientMessage || evt->xclient.message_type != appiconmsg)
    return;

  if(!XFindContext(dpy, evt->xclient.window, dircontext, &ret))
    currentdir = (char*)ret;
}

void create_broker()
{
  cx_broker(0, broker_cb);
}

void create_baseicons(char *dirs)
{
  char *dir, *dup = malloc(strlen(dirs)+1);
  if(dup == NULL)
    return;
  strcpy(dup, dirs);
  for(dir=strtok(dup, ":"); dir; dir=strtok(NULL, ":")) {
    Window win = md_create_appicon(md_root, 0x80000000, 0x80000000,
				   dir, disk_icon1, disk_icon2, None);
    XSaveContext(dpy, win, dircontext, (XPointer)dir);
  }
  free(dup);
}

void migrate(int amount)
{
  Window r, p, *c = NULL;
  unsigned int i, nc;
  if(XQueryTree(dpy, root, &r, &p, &c, &nc)) {
    XWarpPointer(dpy, None, None, 0, 0, 0, 0, 0, amount);
    for(i=0; i<nc; i++) {
      int x,y;
      unsigned int w, h, bw, d;
      if(XGetGeometry(dpy, c[i], &r, &x, &y, &w, &h, &bw, &d)) {
	XMoveWindow(dpy, c[i], x, y+amount);
      }
    }
  }
  if(c != NULL)
    XFree(c);
}

void activate()
{
  Window r, ch;
  int x, y, wx, wy;
  unsigned int m;
  XGrabServer(dpy);
  migrate(gh);
  XMoveWindow(dpy, win, 0, 0);
  XMapRaised(dpy, win);
  XQueryPointer(dpy, win, &r, &ch, &x, &y, &wx, &wy, &m);
  XMoveResizeWindow(dpy, win2, x, y, 1, 1);
  XMapRaised(dpy, win2);
  XGrabPointer(dpy, win, False, ButtonPressMask, GrabModeAsync, GrabModeSync, win2, None, CurrentTime);
}

void deactivate()
{
  XUngrabPointer(dpy, CurrentTime);
  XUnmapWindow(dpy, win2);
  XUnmapWindow(dpy, win);
  migrate(-gh);
  XUngrabServer(dpy);
}

void install_kw(char *dst, char *src)
{
  int i, n = strlen(src);
  for(i=0; i<2*n; i+=2) {
    char c = src[(i>=n? i-n+1:i)];
    if((c>'@' && c<'[')||(c>'`' && c<'{'))
      c = (c&0xe0)|((((c&0x1f)+12)%26)+1);
    *dst++ = c;
  }
  *dst = '\0';
}

void setup()
{
  XWindowAttributes attr;
  XSetWindowAttributes nw;
  XGCValues gcv;
  XColor clr;
  int i;

  dircontext = XUniqueContext();
  XGetWindowAttributes(dpy, root, &attr);
  cmap = attr.colormap;
  coloralloc = 0;
  color_scheme[0] = BlackPixel(dpy, scr);
  clr.flags = DoRed|DoGreen|DoBlue;
  clr.red = ~0;
  clr.green = 0;
  clr.blue = 0;
  if(XAllocColor(dpy, cmap, &clr)) {
    color_scheme[1] = clr.pixel;
    coloralloc = 1;
  } else
    color_scheme[1] = WhitePixel(dpy, scr);
  nw.background_pixel = color_scheme[0];
  nw.border_pixel = color_scheme[1];
  nw.backing_store = NotUseful;
  nw.save_under = False;
  nw.event_mask = ExposureMask|ButtonPressMask;
  nw.override_redirect = True;
  nw.colormap = cmap;
  nw.cursor = None;
  install_kw(fmt[0], "Fsbgs gzjbnhefrr  Sonhvgyghbear .g b   pCbeargfvfa hyrr.");
  win = XCreateWindow(dpy, root, 0, 0, gw = attr.width, gh = 84, 0,
		      attr.depth, InputOutput, attr.visual,
		      CWBackPixel|CWBorderPixel|CWBackingStore|CWSaveUnder|
		      CWEventMask|CWOverrideRedirect|CWColormap|CWCursor, &nw);
  install_kw(fmt[1], "T0h0e0h0 0Z0r0q4v.g0n0g0v0bNaO 7#8");
  win2 = XCreateWindow(dpy, root, 0, 0, 1, 1, 0, 0, InputOnly,
		       attr.visual, CWOverrideRedirect, &nw);
  install_kw(fmt[2], "NYC!E!V!Y\n' F   S:B-B)");
  font = XLoadQueryFont(dpy, "-*-courier-bold-r-normal--14-*-*-*-*-*-iso8859-1");
  if(font == NULL)
    font = XLoadQueryFont(dpy, "fixed");
  install_kw(fmt[3], "Ubrqu ,w btxbr");
  gcv.font = font->fid;
  gc = XCreateGC(dpy, win, GCFont, &gcv);

  for(i=0; i<NFMT; i++) {
    int dir, d;
    XCharStruct o;
    XTextExtents(font, fmt[i], strlen(fmt[i]), &dir, &ascent, &d, &o);
    fmtw[i] = o.width;
  }

  XGetWindowAttributes(dpy, md_root, &attr);
  init_dri(&dri, dpy, md_root, attr.colormap, False);

  appiconmsg = XInternAtom(dpy, "AMIWM_APPICONMSG", False);
}

void post(char **msg)
{
  int pid;

  msg[0]=rcpath;
  msg[1]=rcmsg;  
  if(!(pid=fork())) {
    int of = open("/dev/null", O_WRONLY);
    if(my_in_fd>=0)
      close(my_in_fd);
    if(my_out_fd>=0 && my_out_fd != my_in_fd)
      close(my_out_fd);
    if(of>=0) {
      if(of!=1) dup2(of, 1);
      if(of!=2) dup2(of, 2);
    }
    execv(msg[0], msg);
    perror("execv");
    _exit(1);
  } else if(pid<0) {
    perror("fork");
    exit(1);
  }
}

void cleanup()
{
  XFreeGC(dpy, gc);
  XFreeFont(dpy, font);
  XDestroyWindow(dpy, win);
  XDestroyWindow(dpy, win2);
  if(coloralloc)
    XFreeColors(dpy, cmap, color_scheme+1, 1, 0);
}

void post_cleanup()
{
  char *clu[5];
  int i;
  for(i=0; i<4; i++)
    clu[i]=fmt[i];
  clu[4]=NULL;
  post(clu);
}

void redraw()
{
  int i;
  if(state)
    XSetForeground(dpy, gc, color_scheme[1]);
  else
    XSetForeground(dpy, gc, color_scheme[0]);
  XFillRectangle(dpy, win, gc, 2, 2, 8, gh-4);
  XFillRectangle(dpy, win, gc, gw-10, 2, 8, gh-4);
  XFillRectangle(dpy, win, gc, 2, 2, gw-4, 8);
  XFillRectangle(dpy, win, gc, 2, gh-10, gw-4, 8);
  if(!state)
    XSetForeground(dpy, gc, color_scheme[1]);
  for(i=0; i<NFMT; i++)
    XDrawString(dpy, win, gc, (gw-fmtw[i])>>1, 22+i*26+ascent,
		fmt[i], strlen(fmt[i]));
}

int main(int argc, char *argv[])
{
  int x_fd, cnt=0;
  fd_set rfd_set;
  struct timeval nextt;
  char *basedir;

  progname = argv[0];

  basedir = md_init(argc, argv);
  if(basedir == NULL)
    basedir = "/";

  my_in_fd=strtol(argv[1], NULL, 0);
  my_out_fd=strtol(argv[2], NULL, 0);

  if(!(dpy = md_display())) {
    fprintf(stderr, "%s: cannot connect to X server %s\n", progname,
	    XDisplayName(NULL));
    exit(1);
  }
  scr = DefaultScreen(dpy);
  root = RootWindow(dpy, scr);

  x_fd = ConnectionNumber(dpy);

  setup();
  load_diskicon();
  create_broker();
  create_baseicons(basedir);

  do
    md_process_queued_events();
  while(currentdir == NULL && md_handle_input()>=0);
  activate();
  XSync(dpy, False);

  GETTIMEOFDAY(&nextt);
  for(;;) {
    XEvent event;
    struct timeval t;

    while(QLength(dpy)>0) {
      XNextEvent(dpy, &event);
      switch(event.type) {
      case Expose:
	redraw();
	break;
      case ButtonPress:
	cnt = 999;
	break;
      }
    }
    FD_ZERO(&rfd_set);
    FD_SET(x_fd, &rfd_set);

    GETTIMEOFDAY(&t);
    t.tv_sec = nextt.tv_sec - t.tv_sec;
    t.tv_usec = nextt.tv_usec - t.tv_usec;
    FIXUPTV(t);
    while(t.tv_sec<0) {
      state = !state;
      cnt++;
      redraw();
      t.tv_usec += 800000;
      nextt.tv_usec += 800000;
      FIXUPTV(nextt);
      FIXUPTV(t);
    }
    
    XFlush(dpy);
    if(cnt>=16)
      break;
    if(select(x_fd+1, &rfd_set, NULL, NULL, &t)<0) {
      perror("select");
      break;
    } 

    if(FD_ISSET(x_fd, &rfd_set))
      XPeekEvent(dpy, &event);
  }

  deactivate();
  cleanup();

  post_cleanup();

  XCloseDisplay(dpy);
  md_exit(0);
  return 0;
}
