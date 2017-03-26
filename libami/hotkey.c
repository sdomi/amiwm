#include "libami.h"
#include <X11/keysym.h>
#include <stdlib.h>
#include <string.h>

static struct hotkey {
  struct hotkey *next;
  KeySym keysym;
  unsigned int mod0;
  int keycode;
  unsigned int modifiers;
  int meta, position;
  int grab;
  void (*callback)(XEvent *, void *);
  void *client_data;
} *hotkeys = NULL;

static XModifierKeymap *modmap=NULL;
static KeySym *keymap=NULL;
static int kbd_mincode, kbd_maxcode, kbd_maxsym;
static unsigned int meta_mask, switch_mask;
static Display *dpy=NULL;

static void lookup_meta()
{
  int i, j, k;
  KeySym *kp;
  unsigned int alt_mask = 0;

  meta_mask = switch_mask = 0;
  for(i=3; i<8; i++)
    for(j=0; j<modmap->max_keypermod; j++)
      for(kp=keymap+(modmap->modifiermap[i*modmap->max_keypermod+j]-
		     kbd_mincode)*kbd_maxsym, k=0; k<kbd_maxsym; k++)
	switch(*kp++) {
	case XK_Meta_L:
	case XK_Meta_R:
	  meta_mask|=1<<i;
	  break;
	case XK_Mode_switch:
	  switch_mask|=1<<i;
	  break;
	case XK_Alt_L:
	case XK_Alt_R:
	  alt_mask|=1<<i;
	  break;
	}
  if(meta_mask == 0)
    meta_mask = (alt_mask? alt_mask :
		 (switch_mask? switch_mask : Mod1Mask));
}

static void lookup_hotkey(struct hotkey *hk)
{
  if(!keymap) {
    XDisplayKeycodes(dpy, &kbd_mincode, &kbd_maxcode);
    keymap=XGetKeyboardMapping(dpy, kbd_mincode, kbd_maxcode-kbd_mincode+1,
			       &kbd_maxsym);
  }
  if(!modmap) {
    modmap=XGetModifierMapping(dpy);
    if(keymap)
      lookup_meta();
  }
  hk->modifiers=hk->mod0|(hk->meta? meta_mask:0);
  if(!(hk->keycode=XKeysymToKeycode(dpy, hk->keysym)))
    if((hk->keysym&~0x20)>=XK_A && (hk->keysym&~0x20)<=XK_Z)
      hk->keycode=XKeysymToKeycode(dpy, hk->keysym^0x20);

  if(hk->keycode && keymap)
    if(keymap[(hk->keycode-kbd_mincode)*kbd_maxsym])
      ;
    else if(kbd_maxsym>1 && keymap[(hk->keycode-kbd_mincode)*kbd_maxsym+1])
      hk->modifiers|=ShiftMask;
    else if(kbd_maxsym>2 && keymap[(hk->keycode-kbd_mincode)*kbd_maxsym+2])
      hk->modifiers|=switch_mask;
    else if(kbd_maxsym>3 && keymap[(hk->keycode-kbd_mincode)*kbd_maxsym+3])
      hk->modifiers|=ShiftMask|switch_mask;
}

static void rethink_keymap()
{
  struct hotkey *hk;

  if(keymap) {
    XFree(keymap);
    keymap=NULL;
  }
  if(modmap) {
    XFreeModifiermap(modmap);
    modmap=NULL;
  }
  for(hk=hotkeys; hk; hk=hk->next) {
    int oldkc=hk->keycode;
    unsigned oldmd=hk->modifiers;
    lookup_hotkey(hk);
    if(hk->grab>=0 && (hk->keycode!=oldkc || hk->modifiers!=oldmd)) {
      md_ungrabkey(hk->grab);
      hk->grab=-1;
    }
    if((hk->position & IN_WINDOW_MASK) && hk->grab<0)
      hk->grab=md_grabkey(hk->keycode, hk->modifiers);
  }
}

static int mappingfunc(XEvent *e)
{
  if(e->xmapping.request==MappingKeyboard ||
     e->xmapping.request==MappingModifier) {
    e->xmapping.display=dpy;
    XRefreshKeyboardMapping(&e->xmapping);
  }
  rethink_keymap();
  return 0;
}

static int keypressfunc(XEvent *e)
{
  struct hotkey *hk;

  for(hk=hotkeys; hk; hk=hk->next)
    if(hk->keycode==e->xkey.keycode &&
       (hk->modifiers==e->xkey.state || (hk->modifiers & AnyModifier))) {
      hk->callback(e, hk->client_data);
      return 1;
    }
  
  return 0;
}

void cx_hotkey(KeySym ks, unsigned int mod, int m, int pos,
	       void (*cb)(XEvent *, void *), void *cd)
{
  struct hotkey *hk=malloc(sizeof(struct hotkey));
  if(hk) {
    if(!dpy) {
      dpy=md_display();
      cx_event_broker(KeyPress, KeyPressMask, keypressfunc);
      cx_event_broker(MappingNotify, 0, mappingfunc);
    }
    hk->next=hotkeys;
    hk->keysym=ks;
    hk->mod0=mod;
    hk->meta=m;
    hk->position=pos;
    hk->callback=cb;
    hk->client_data=cd;
    lookup_hotkey(hk);
    hk->grab=((pos&IN_WINDOW_MASK)? md_grabkey(hk->keycode, hk->modifiers):-1);
    hotkeys=hk;
  }
}
