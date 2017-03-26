#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "libami.h"
#include "drawinfo.h"

char *progname;

Display *dpy;
XContext launchercontext;
struct DrawInfo dri;
Atom appiconmsg;
char *icondir;

struct launcher {
  struct ColorStore colorstore1, colorstore2;
  char cmdline[1];
};

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
#ifdef HAVE_ALLOCA
  char *line=alloca(strlen(cmd)+4);
#else
  char *line=malloc(strlen(cmd)+4);
  if(line) {
#endif
  sprintf(line, "%s &", cmd);
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

static void broker_cb(XEvent *evt, unsigned long mask)
{
  XPointer ret;

  if(evt->type != ClientMessage || evt->xclient.message_type != appiconmsg)
    return;

  if(!XFindContext(dpy, evt->xclient.window, launchercontext, &ret)) {
    struct launcher *l = (struct launcher*)ret;
    spawn(l->cmdline);
  }
}

static void create_broker()
{
  cx_broker(0, broker_cb);
}

static void create_launcher(char *label, char *icon, char *cmdline)
{
  struct DiskObject *icon_do = NULL;
  Pixmap icon_icon1, icon_icon2;
  Window win;

  struct launcher *l = malloc(sizeof(struct launcher)+strlen(cmdline));
  if (l == NULL) {
    fprintf(stderr, "%s: out of memory!\n", progname);
    exit(1);
  }

  memset(l, 0, sizeof(*l));
  strcpy(l->cmdline, cmdline);

  if (icon != NULL && *icon != 0) {
#ifdef AMIGAOS
    char fn[256];
    strncpy(fn, icondir, sizeof(fn)-1);
    fn[sizeof(fn)-1]='\0';
    AddPart(fn,icon,sizeof(fn));
#else
    int rl=strlen(icon)+strlen(icondir)+2;
#ifdef HAVE_ALLOCA
    char *fn=alloca(rl);
#else
    char fn[1024];
#endif
    sprintf(fn, "%s/%s", icondir, icon);
#endif
    fn[strlen(fn)-5]=0;
    icon_do = GetDiskObject(fn);
  }

  if(icon_do == NULL)
    icon_do = GetDefDiskObject(3/*WBTOOL*/);

  if(icon_do == NULL) {
    md_errormsg(md_root, "Failed to load icon");
    md_exit(0);
  }

  icon_icon1 =
    md_image_to_pixmap(md_root, dri.dri_Pens[BACKGROUNDPEN],
		       (struct Image *)icon_do->do_Gadget.GadgetRender,
		       icon_do->do_Gadget.Width, icon_do->do_Gadget.Height,
		       &l->colorstore1);

  icon_icon2 =
    md_image_to_pixmap(md_root, dri.dri_Pens[BACKGROUNDPEN],
		       (struct Image *)icon_do->do_Gadget.SelectRender,
		       icon_do->do_Gadget.Width, icon_do->do_Gadget.Height,
		       &l->colorstore2);

  FreeDiskObject(icon_do);
  XSync(dpy, False);
  win = md_create_appicon(md_root, 0x80000000, 0x80000000,
			  label, icon_icon1, icon_icon2, None);
  XSaveContext(dpy, win, launchercontext, (XPointer)l);
}

static int skip_ws(char *arg, int len)
{
  int n = 0;
  while(len > 0 && isspace(*(unsigned char *)arg)) { n++; arg++; --len; }
  return n;
}

static int get_string(char *arg, int len, int lineno)
{
  int n;
  if (len < 1 || *arg != '(') {
    fprintf(stderr, "%s: missing '(' on line %d\n", progname, lineno);
    return 0;
  }
  for (n=1; n<len; n++)
    if (arg[n] == ')')
      break;
  if (n >= len) {
    fprintf(stderr, "%s: missing ')' on line %d\n", progname, lineno);
    return 0;
  }
  return n+1;
}

static char *create_string(char *arg, int l)
{
  char *p = malloc(l+1);
  if (!p) {
    fprintf(stderr, "%s: out of memory!\n", progname);
    exit(1);
  }
  if(l>0) memcpy(p, arg, l);
  p[l] = 0;
  return p;
}

static void create_appicons_line(char *arg, int len, int lineno)
{
  int l;
  char *label, *icon, *cmdline;
  if (*arg == '#')
    return;
  l = skip_ws(arg, len);
  arg += l; len -= l;
  if (!len) return;
  l = get_string(arg, len, lineno);
  if (!l) return;
  label = create_string(arg+1, l-2);
  arg += l; len -= l;
  l = skip_ws(arg, len);
  arg += l; len -= l;
  l = get_string(arg, len, lineno);
  if (!l) { free(label); return; }
  icon = create_string(arg+1, l-2);
  arg += l; len -= l;
  l = skip_ws(arg, len);
  arg += l; len -= l;
  l = get_string(arg, len, lineno);
  if (!l) { free(icon); free(label); return; }
  cmdline = create_string(arg+1, l-2);
  arg += l; len -= l;
  l = skip_ws(arg, len);
  arg += l; len -= l;
  if (len) fprintf(stderr, "%s: junk at end of line %d\n", progname, lineno);
  create_launcher(label, icon, cmdline);
  free(cmdline);
  free(icon);
  free(label);
}

static void create_appicons(char *arg)
{
  char *p;
  int l = 1;
  while((p = strchr(arg, '\n'))) {
    create_appicons_line(arg, p-arg, l++);
    arg = p+1;
  }
  create_appicons_line(arg, strlen(arg), l);
}

static void setup()
{
  XWindowAttributes attr;
  launchercontext = XUniqueContext();
  XGetWindowAttributes(dpy, md_root, &attr);
  init_dri(&dri, dpy, md_root, attr.colormap, False);
  appiconmsg = XInternAtom(dpy, "AMIWM_APPICONMSG", False);
  icondir = get_current_icondir();
  if(!icondir) icondir="";
}

int main(int argc, char *argv[])
{
  char *arg=md_init(argc, argv);
  progname=argv[0];

  if(!(dpy = md_display())) {
    fprintf(stderr, "%s: cannot connect to X server %s\n", progname,
	    XDisplayName(NULL));
    exit(1);
  }
  if(arg) {
    setup();
    create_broker();
    create_appicons(arg);
    md_main_loop();
  }
  return 0;
}
