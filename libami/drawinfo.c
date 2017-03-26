#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"
#include "drawinfo.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

#define TAG_DRI_VERSION 1
#define TAG_DRI_PENS    2
#define TAG_DRI_FONT    3
#define TAG_DRI_FONTSET 4

extern char *progname;

static char black[] = "#000000";
static char white[] = "#ffffff";
static char grey[] = "#aaaaaa";
static char blue[] = "#6688bb";

char *default_colors[NUMDRIPENS] = {
  black, white, black, white, black, blue, black, grey, white,
  black, white, black
};

char *default_screenfont =
"-b&h-lucida-medium-r-normal-sans-12-*-*-*-*-*-iso8859-1"
#ifdef USE_FONTSETS
",-b&h-lucida-medium-r-normal-sans-12-*-*-*-*-*-iso10646-1"
",-misc-fixed-medium-r-normal--14-*-*-*-*-*-jisx0208.1983-0"
#endif
;

static void setdriprop(Display *dpy, Atom atom, Atom typ, Window win,
		       struct DrawInfo *dri)
{
  CARD32 prop[/* NUMDRIPENS+6 */ 100], *p=prop;
  CARD16 i;
  *p++=TAG_DRI_VERSION;
  *p++=dri->dri_Version;
#ifdef USE_FONTSETS
  *p++=TAG_DRI_FONTSET;
  *p++=dri->dri_FontSetAtom;
#else
  *p++=TAG_DRI_FONT;
  *p++=dri->dri_Font->fid;
#endif
  *p++=TAG_DRI_PENS;
  *p++=dri->dri_NumPens;
  for(i=0; i<dri->dri_NumPens; i++)
    *p++=dri->dri_Pens[i];
  XChangeProperty(dpy, win, atom, typ, 32, PropModeReplace,
		  (unsigned char *)prop, p-prop);
}

static void getdriprop(Display *dpy, Atom atom, Atom typ, Window win,
		       struct DrawInfo *dri)
{
  Atom atyp;
  int afmt;
  unsigned long nitems, extra;
  int i,j,n;
  CARD32 *prop=NULL;
  if(XGetWindowProperty(dpy, win, atom, 0l, NUMDRIPENS+6, False, typ,
			&atyp, &afmt, &nitems, &extra,
			(unsigned char **)&prop) == Success &&
     atyp==typ && afmt==32) {
    for(i=0; i<nitems; )
      switch(prop[i++]) {
      case TAG_DRI_VERSION:
	if(i<nitems)
	  dri->dri_Version=prop[i++];
	break;
#ifdef USE_FONTSETS
      case TAG_DRI_FONTSET:
	if(i<nitems)
	  dri->dri_FontSetAtom=prop[i++];
	break;
#else
      case TAG_DRI_FONT:
	if(i<nitems)
	  dri->dri_Font=XQueryFont(dpy, prop[i++]);
	break;
#endif
      case TAG_DRI_PENS:
	if(i<nitems) {
	  n=prop[i++];
	  if(n>nitems-i)
	    n=nitems-i;
	  if(n>0) {
	    dri->dri_NumPens=n;
	    if((dri->dri_Pens=calloc(sizeof(unsigned long), n)))
	      for(j=0; j<n; j++)
		dri->dri_Pens[j]=prop[i++];
	  }
	}
	break;
      }
  }
  if(prop)
    XFree(prop);
}

Status myXAllocNamedColor(Display *dpy, Colormap cmap, char *color_name,
			  XColor *screen_def, XColor *exact_def)
{
  Status s;

  if((s=XAllocNamedColor(dpy, cmap, color_name, screen_def, exact_def)))
    return s;
  if((s=XLookupColor(dpy, cmap, color_name, screen_def, exact_def))||
     (s=XParseColor(dpy, cmap, color_name, exact_def))) {
    *screen_def = *exact_def;
    screen_def->pixel = 0;
    screen_def->flags = DoRed|DoGreen|DoBlue;
    s=XAllocColor(dpy, cmap, screen_def);
  }
  return s;
}

unsigned long allocdripen(Display *dpy, int n, Colormap cm)
{
  XColor screen, exact;
  char *name;

  if(!myXAllocNamedColor(dpy, cm, name = default_colors[n],
			 &screen, &exact)) {
    fprintf(stderr, "%s: cannot allocate color %s\n", progname, name);
    exit(1);
  }
  return screen.pixel;
}

void term_dri(struct DrawInfo *dri, Display *dpy, Colormap cm)
{
  if(dri->dri_Pens) {
    XFreeColors(dpy, cm, dri->dri_Pens, dri->dri_NumPens, 0);
    free(dri->dri_Pens);
    dri->dri_Pens = NULL;
  }
#ifdef USE_FONTSETS
  if(dri->dri_FontSet) {
    XFreeFontSet(dpy, dri->dri_FontSet);
    dri->dri_FontSet = NULL;
  }
#else
  if(dri->dri_Font) {
    XFreeFont(dpy, dri->dri_Font);
    dri->dri_Font = NULL;
  }
#endif
}

void init_dri(struct DrawInfo *dri, Display *dpy, Window root, Colormap cm,
	      int override)
{
  int i;
  Atom driatom, dritypatom;

  driatom=XInternAtom(dpy, "AMIWM_DRAWINFO", False);
  dritypatom=XInternAtom(dpy, "DRAWINFO", False);
  if(!override) {
    memset(dri, 0, sizeof(*dri));
    getdriprop(dpy, driatom, dritypatom, root, dri);
  }
  if(!dri->dri_Version)
    dri->dri_Version = DRI_VERSION;
  if(!dri->dri_Pens) {
    if(!(dri->dri_Pens = calloc(sizeof(unsigned long), NUMDRIPENS))) {
      fprintf(stderr, "%s: out of memory\n", progname);
      exit(1);
    }
    for(i=0; i<NUMDRIPENS; i++)
      dri->dri_Pens[i] = allocdripen(dpy, i, cm);
    dri->dri_NumPens = NUMDRIPENS;
  }
#ifdef USE_FONTSETS
  if(!dri->dri_FontSet)
    {
      char *fn;
      XFontStruct **fsl;
      char **fnl;
      char **missing_charsets = NULL;
      int n_missing_charsets = 0;

      if(dri->dri_FontSetAtom) {
	fn = XGetAtomName(dpy, dri->dri_FontSetAtom);
      } else {
	dri->dri_FontSetAtom = XInternAtom(dpy, fn=default_screenfont, False);
      }

      dri->dri_FontSet = XCreateFontSet(dpy, fn,
					&missing_charsets,
					&n_missing_charsets, NULL);
      if(missing_charsets)
	XFreeStringList(missing_charsets);
      if(!dri->dri_FontSet) {
	fprintf(stderr, "%s: cannot open font %s\n", progname, fn);
	exit(1);
      }
      if(XFontsOfFontSet(dri->dri_FontSet, &fsl, &fnl) < 1) {
	fprintf(stderr, "%s: fontset %s is empty\n", progname, fn);
	exit(1);
      }
      if(fn != default_screenfont)
	XFree(fn);
#if 0
      //dri->dri_Font = fsl[0];
      dri->dri_Font = malloc(sizeof(XFontStruct));
      memcpy(dri->dri_Font, fsl[0], sizeof(XFontStruct));
#else
      dri->dri_Ascent = fsl[0]->ascent;
      dri->dri_Descent = fsl[0]->descent;
      dri->dri_MaxBoundsWidth = fsl[0]->max_bounds.width;
#endif
    }
#else
  if(!dri->dri_Font)
    if(!(dri->dri_Font = XLoadQueryFont(dpy, default_screenfont))) {
      fprintf(stderr, "%s: cannot open font %s\n", progname,
	      default_screenfont);
      exit(1);
    }
  dri->dri_Ascent = dri->dri_Font->ascent;
  dri->dri_Descent = dri->dri_Font->descent;
  dri->dri_MaxBoundsWidth = dri->dri_Font->max_bounds.width;
#endif
  if(override)
    setdriprop(dpy, driatom, dritypatom, root, dri);
}
