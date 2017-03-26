#ifndef ICC_H
#define ICC_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "client.h"

extern void init_atoms(void);
extern void sendcmessage(Window, Atom, long);
extern void getproto(Client *c);
extern void setstringprop(Window, Atom, char *);
extern void propertychange(Client *, Atom);
extern long _getprop(Window, Atom, Atom, long, char **);
extern void getwflags(Client *);

extern Atom wm_state, wm_change_state, wm_protocols, wm_delete, wm_take_focus, wm_colormaps, wm_hints, amiwm_screen, swm_vroot;

#define Pdelete 1
#define Ptakefocus 2
#define Psizebottom 4
#define Psizeright 8
#define Psizetrans 16

#endif
