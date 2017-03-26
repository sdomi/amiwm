#ifndef CLIENT_H
#define CLIENT_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

struct _Icon;
struct _Scrn;
typedef struct _Client {
  struct _Client *next;
  struct _Scrn *scr;
  struct _Icon *icon;
  struct module *module;
  int wflags;
  Window window, parent;
  Window close, drag, iconify, zoom, depth, resize;
  Window clicked;
  Colormap colormap;
  int x, y, pwidth, pheight, dragw, framewidth, frameheight;
  int zoomx, zoomy, zoomw, zoomh;
  int old_bw, proto, state, gravity, reparenting;
  int active, shaped;
#ifdef USE_FONTSETS
  char *title;
#else
  XTextProperty title;
#endif
  XSizeHints sizehints;
  struct _Style *style;
} Client;

extern Client *clients;

extern Client *getclient(Window);
extern Client *getclientbyicon(Window);
extern Client *createclient(Window);
extern void rmclient(Client *);
extern void flushclients(void);
extern void scrsendconfig(struct _Scrn *);
extern void sendconfig(Client *);
extern void getstate(Client *);
extern void grav_map_frame_to_win(Client *, int, int, int *, int *);
extern void grav_map_win_to_frame(Client *, int, int, int *, int *);
extern void setclientstate(Client *, int);

#endif
