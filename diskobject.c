#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xmd.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>

#include "alloc.h"
#include "drawinfo.h"
#include "prefs.h"
#include "screen.h"
#include "libami.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

extern Display *dpy;
extern char *progname;

char *iconcolorname[256];
int iconcolormask;

static char *sysiconcolorname[]={
  "#aaaaaa", "#000000", "#ffffff", "#6688bb",
  "#ee4444", "#55dd55", "#0044dd", "#ee9e00"
};

static char *magicwbcolorname[]={
  "#aaaaaa", "#000000", "#ffffff", "#6688bb",
  "#999999", "#bbbbbb", "#bbaa99", "#ffbbaa"  
};

static char *schwartziconcolorname[]={
  "#aaa0aa", "#000000", "#ffffff", "#5b78f5",
  "#999099", "#bbb0bb", "#bbaa99", "#ffbbaa",
  "#d922f1", "#915a46", "#3e3e41", "#6e6973",
  "#ff1919", "#ffff00", "#727d92", "#00c800"
};

Pixmap image_to_pixmap_scr(Scrn *scr, struct Image *im, int width, int height,
			   struct ColorStore *cs)
{
  return image_to_pixmap(dpy, scr->back, scr->gc,
			 scr->dri.dri_Pens[BACKGROUNDPEN],
			 scr->iconcolor, iconcolormask, im, width, height, cs);
}

void load_do(const char *filename, struct IconPixmaps *ip)
{
  struct DiskObject *dobj;
#ifdef AMIGAOS
  char fn[256];
  strncpy(fn, prefs.icondir, sizeof(fn)-1);
  fn[sizeof(fn)-1]='\0';
  AddPart(fn,filename,sizeof(fn));
#else
  int rl=strlen(filename)+strlen(prefs.icondir)+2;
#ifdef HAVE_ALLOCA
  char *fn=alloca(rl);
#else
  char fn[1024];
#endif
  sprintf(fn, "%s/%s", prefs.icondir, filename);
#endif
  fn[strlen(fn)-5]=0;
  if((dobj=GetDiskObject(fn))) {
    ip->pm=image_to_pixmap_scr(scr, (struct Image *)dobj->do_Gadget.GadgetRender,
			       dobj->do_Gadget.Width, dobj->do_Gadget.Height, &ip->cs);
    ip->pm2=image_to_pixmap_scr(scr, (struct Image *)dobj->do_Gadget.SelectRender,
				dobj->do_Gadget.Width, dobj->do_Gadget.Height, &ip->cs2);
    FreeDiskObject(dobj);
  } else ip->pm=ip->pm2=None;
}

void init_iconpalette()
{
  extern Status myXAllocNamedColor(Display *, Colormap, char *, XColor *, XColor *);
  XColor scrp, xact;
  char *name;
  int i;

  for(i=0; i<=iconcolormask; i++) {
    if(!myXAllocNamedColor(dpy, scr->cmap, name = iconcolorname[i], &scrp, &xact)) {
      fprintf(stderr, "%s: cannot allocate color %s\n", progname, name);
      exit(1);
    }
    scr->iconcolor[i]=scrp.pixel;
    scr->iconcolorsallocated=i+1;
  }
}

void set_mwb_palette()
{
  iconcolormask=7;
  memcpy(iconcolorname, magicwbcolorname, sizeof(magicwbcolorname));
}

void set_sys_palette()
{
  iconcolormask=7;
  memcpy(iconcolorname, sysiconcolorname, sizeof(sysiconcolorname));
}

void set_schwartz_palette()
{
  iconcolormask=15;
  memcpy(iconcolorname, schwartziconcolorname, sizeof(schwartziconcolorname));
}

#ifndef HAVE_STRDUP
/* defined in gram.y */
extern char *strdup(char *);
#endif

static int custom_palette_from_file(char *fn, int ignorenofile)
{
  FILE *file;
  int rows, cols, pixels, pixel;
  int format, mv;

  if(!(file=fopen(fn, "r"))) {
    if(!ignorenofile)
      perror(fn);
    return 0;
  }

  if(4!=fscanf(file, "P%d %d %d %d", &format, &cols, &rows, &mv) ||
     (format!=3 && format!=6)) {
    fprintf(stderr, "%s: bad magic number - not a ppm file\n", fn);
    fclose(file);
    return 0;
  }
  if(format==6)
    getc(file);
  pixels = rows*cols;
  if(pixels>256)
    pixels = 256;
  for(pixel=0; pixel<pixels; ++pixel) {
    unsigned int r, g, b;
    char nam[16];
    if(format==3)
      fscanf(file, "%u %u %u", &r, &g, &b);
    else {
      r = getc(file);
      g = getc(file);
      b = getc(file);
    }
    if(mv==255)
      sprintf(nam, "#%02x%02x%02x", r&0xff, g&0xff, b&0xff);
    else if(mv==65536)
      sprintf(nam, "#%04x%04x%04x", r&0xffff, g&0xffff, b&0xffff);
    else if(mv<255)
      sprintf(nam, "#%02x%02x%02x", (r*0xff/mv)&0xff,
	      (g*0xff/mv)&0xff, (b*0xff/mv)&0xff);
    else
      sprintf(nam, "#%04x%04x%04x", (r*0xffff/mv)&0xffff,
	      (g*0xffff/mv)&0xffff, (b*0xffff/mv)&0xffff);
    iconcolorname[pixel] = strdup(nam);
  }
  fclose(file);
  while(pixels&(pixels-1))
    iconcolorname[pixels++]="black";
  iconcolormask=pixels-1;
  return 1;
}

void set_custom_palette(char *fn)
{
  if(*fn!='/') {
    char *fn2;
    int r;
#ifdef HAVE_ALLOCA
    fn2 = alloca(strlen(fn)+strlen(AMIWM_HOME)+2);
#else
    fn2 = malloc(strlen(fn)+strlen(AMIWM_HOME)+2);
#endif
    sprintf(fn2, "%s/%s", AMIWM_HOME, fn);
    r=custom_palette_from_file(fn2, 1);
#ifndef HAVE_ALLOCA
    free(fn2);
#endif
    if(r)
      return;
  }
  custom_palette_from_file(fn, 0);
}

char *get_current_icondir()
{
  return prefs.icondir;
}
