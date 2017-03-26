#ifndef STYLE_H
#define STYLE_H

#include <X11/Xlib.h>

#include "icon.h"

typedef struct _Style {
  struct _Style *next;
  char *style_class, *style_title, *style_icon_title;
  struct IconPixmaps icon_pms;
  unsigned int icon_pm_w, icon_pm_h;
  char *icon_name;
} Style;

#endif
