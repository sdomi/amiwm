#include <stdlib.h>
#include <string.h>

#include "libami.h"
#include "module.h"
#include "alloc.h"

Window md_create_appicon(Window p, int x, int y, char *name,
			 Pixmap pm1, Pixmap pm2, Pixmap pmm)
{
  char *data;
  Window w;
  int res, l=strlen(name);
#ifdef HAVE_ALLOCA
  struct NewAppIcon *nai=alloca(sizeof(struct NewAppIcon)+l);
#else
  struct NewAppIcon *nai=malloc(sizeof(struct NewAppIcon)+l);
  if(nai==NULL) return None;
#endif
  nai->x=x; nai->y=y;
  nai->pm1=pm1; nai->pm2=pm2; nai->pmm=pmm;
  strcpy(nai->name, name);
  res=md_command(p, MCMD_CREATEAPPICON, nai, sizeof(struct NewAppIcon)+l,
		 &data);
  if(res<sizeof(w)) {
    if(data) free(data);
#ifndef HAVE_ALLOCA
    free(nai);
#endif
    return None;
  }
  memcpy(&w, data, sizeof(w));
  free(data);
#ifndef HAVE_ALLOCA
  free(nai);
#endif
  return w;
}

Pixmap md_image_to_pixmap(Window w, unsigned long bgcolor, struct Image *i,
			  int width, int height, struct ColorStore *cs)
{
  Display *dpy = md_display();
  static GC gc = None;
  Pixmap pm;
  static int iconcolormask;
  static unsigned long *iconcolor = NULL;
  if(gc == None && w != None)
    gc = XCreateGC(dpy, w, 0, NULL);
  if(iconcolor == NULL) {
    char *p;
    int res = md_command(w, MCMD_GETICONPALETTE, NULL, 0, &p);
    if(res<0)
      return None;
    iconcolor = (unsigned long *)(void *)p;
    iconcolormask = (res/sizeof(unsigned long))-1;
  }
  pm = image_to_pixmap(md_display(), w, gc, bgcolor, iconcolor, iconcolormask,
		       i, width, height, cs);
  return pm;
}

char *get_current_icondir()
{
  char *p;
  if(md_command(None, MCMD_GETICONDIR, NULL, 0, &p)>=0 && p)
    return p;
  if(p) free(p);
  return NULL;
}

