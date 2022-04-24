#ifndef SCREEN_H
#define SCREEN_H

#include "icon.h"

/*
 * Struct _Scrn - virtual desktop screen
 *
 * @root: root window for virtual desktop
 * @back: background window
 * @colormap: current colourmap
 * @fh: font height for menu/window, including ascender/descender.
 * @bh: title bar / menu bar height; fh + fixed padding.
 * @icons: list of icons on this desktop.
 * @firstselected: first selected icon on this desktop.
 */

typedef struct _Scrn {
  struct _Scrn *behind, *upfront;
  Window root, back, inputbox;
  Colormap cmap;
  Visual *visual;
  GC gc, icongc, rubbergc, menubargc;
  char *title;
  char *deftitle;
  struct DrawInfo dri;
  int fh,bh,h2,h3,h4,h5,h6,h7,h8;
  int width, height, depth, y, bw;
  struct IconPixmaps default_tool_pms;
  Pixmap disabled_stipple;
  unsigned int default_tool_pm_w, default_tool_pm_h, lh;
  Window menubar, menubarparent, menubardepth;
  int hotkeyspace, checkmarkspace, subspace, menuleft;
  struct _Icon *icons, *firstselected;
  struct Menu *firstmenu;
  int number, realized, iconcolorsallocated;
  unsigned long iconcolor[256];
} Scrn;

extern Scrn *scr, *front;

extern void closescreen();
extern Scrn * openscreen(char *, Window);
extern void realizescreens();
extern void screentoback();

#endif
