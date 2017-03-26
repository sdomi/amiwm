/* GNOME Window Manager Compliance support for amiwm
   by Peter Bortas february 2000 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>

#include <X11/Xutil.h>


extern void gnome_setup()
{
  /* Section 1 - Detection of a GNOME compliant Window Manager

     There is a single unambiguous way to detect if there currently is
     a GNOME compliant Window Manager running. It is the job of the
     Window Manager to set up a few things to make this
     possible. Using the following method it is also possible for
     applications to detect compliance by receiving an event when the
     Window Manager exits.

     To do this the Window Manager should create a Window, that is a
     child of the root window. There is no need to map it, just create
     it. The Window Manager may reuse ANY window it has for this
     purpose - even if it is mapped, just as long as the window is
     never destroyed while the Window Manager is running.

     Once the Window is created the Window Manager should set a
     property on the root window of the name _WIN_SUPPORTING_WM_CHECK,
     and type CARDINAL. The atom's data would be a CARDINAL that is
     the Window ID of the window that was created above. The window
     that was created would ALSO have this property set on it with the
     same values and type.*/

  Display            *disp;
  Window              root_window;
  Atom                atom_set;
  CARD32              val;
  Window              win;

  Atom                list[10];

  atom_set = XInternAtom(disp, "_WIN_SUPPORTING_WM_CHECK", False);
  win = XCreateSimpleWindow(disp, root_window, -200, -200, 5, 5, 0, 0, 0);
  val = win;
  XChangeProperty(disp, root_window, atom_set, XA_CARDINAL, 32, 
		  PropModeReplace, (unsigned char *)&val, 1);
  XChangeProperty(disp, win, atom_set, XA_CARDINAL, 32, PropModeReplace,
		  (unsigned char *)&val, 1); 

  /* Section 2 - Listing GNOME Window Manager Compliance

     It is important to list which parts of GNOME Window Manager
     compliance are supported. This is done fairly easily by doing the
     following:

     Create a property on the root window of the atom name
     _WIN_PROTOCOLS. This property contains a list(array)of atoms that
     are all the properties the Window Manager supports. These atoms
     are any number of the following:

     _WIN_LAYER 
     _WIN_STATE 
     _WIN_HINTS 
     _WIN_APP_STATE 
     _WIN_EXPANDED_SIZE 
     _WIN_ICONS 
     _WIN_WORKSPACE 
     _WIN_WORKSPACE_COUNT 
     _WIN_WORKSPACE_NAMES 
     _WIN_CLIENT_LIST

     If you list one of these properties then you support it and
     applications can expect information provided by, or accepted by
     the Window Manager to work. */

  atom_set = XInternAtom(disp, "_WIN_PROTOCOLS", False);
  list[0] = XInternAtom(disp, "_WIN_LAYER", False);
  list[1] = XInternAtom(disp, "_WIN_STATE", False);
  list[2] = XInternAtom(disp, "_WIN_HINTS", False);
  list[3] = XInternAtom(disp, "_WIN_APP_STATE", False);
  list[4] = XInternAtom(disp, "_WIN_EXPANDED_SIZE", False);
  list[5] = XInternAtom(disp, "_WIN_ICONS", False);
  list[6] = XInternAtom(disp, "_WIN_WORKSPACE", False);
  list[7] = XInternAtom(disp, "_WIN_WORKSPACE_COUNT", False);
  list[8] = XInternAtom(disp, "_WIN_WORKSPACE_NAMES", False);
  list[9] = XInternAtom(disp, "_WIN_CLIENT_LIST", False);
  XChangeProperty(disp, root_window, atom_set, XA_ATOM, 32, PropModeReplace,
		  (unsigned char *)list, 10);  
}

extern void gnome_managed_clients()
{
  /* Section 3 - Providing Shortcuts Managed Clients

     As an aide in having external applications be able to list and
     access clients being managed by the Window Manager, a property
     should be set on the root window of the name _WIN_CLIENT_LIST
     which is an array of type CARDINAL. Each entry is the Window ID
     of a managed client. If the list of managed clients changes,
     clients are added or deleted, this list should be updated. */

  Display            *disp;
  Window              root_window;
  Atom                atom_set;
  Window             *wl;
  int                 num;
  
  fprintf(stderr, "FIXME: snome_managed_clients is a stub\n");

  atom_set = XInternAtom(disp, "_WIN_CLIENT_LIST", False);
  num = 0;  /* FIXME: number of clients goes here */
  wl = malloc(sizeof(Window) * num);
  /* FIXME: Fill in array of window ID's */
  XChangeProperty(disp, root_window, atom_set, XA_CARDINAL, 32, 
		  PropModeReplace, (unsigned char *)wl, num);
  if (wl)
    free(wl);
}

extern void gnome_multiple_desktops()
{
  /* Section 3 - Providing Multiple/Virtual Desktop Information.

     If your Window Manager supports the concept of Multiple/Virtual
     Desktops or Workspaces then you will definitely want to include
     it. This involves your Window Manager setting several properties
     on the root window.

     First you should advertise how many Desktops your Window Manager
     supports. This is done by setting a property on the root window
     with the atom name _WIN_WORKSPACE_COUNT of type CARDINAL. The
     properties data is a 32-bit integer that is the number of
     Desktops your Window Manager currently supports. If you can add
     and delete desktops while running, you may change this property
     and its value whenever required. You should also set a property
     of the atom _WIN_WORKSPACE of type CARDINAL that contains the
     number of the currently active desktop (which is a number between
     0 and the number advertised by _WIN_WORKSPACE_COUNT -
     1). Whenever the active desktop changes, change this property.

     Lastly you should set a property that is a list of strings called
     _WIN_WORKSPACE_NAMES that contains names for the desktops (the
     first string is the name of the first desktop, the second string
     is the second desktop, etc.). This will allow applications toq
     know what the name of the desktop is too, possibly to display it. */

  Display            *disp;
  Window              root_window;
  Atom                atom_set;
  XTextProperty       text;
  int                 i, current_desk, number_of_desks;
  char              **names, s[1024];
  CARD32              val;
  
  /* FIXME: set current_desk, number_of_desks  names */

  atom_set = XInternAtom(disp, "_WIN_WORKSPACE", False);
  val = (CARD32) current_desk;
  XChangeProperty(disp, root_window, atom_set, XA_CARDINAL, 32, 
		  PropModeReplace, (unsigned char *)&val, 1);
  atom_set = XInternAtom(disp, "_WIN_WORKSPACE_COUNT", False);

  val = (CARD32) number_of_desks;
  XChangeProperty(disp, root_window, atom_set, XA_CARDINAL, 32, 
		  PropModeReplace, (unsigned char *)&val, 1);
  atom_set = XInternAtom(disp, "_WIN_WORKSPACE_NAMES", False);
  names = malloc(sizeof(char *) * number_of_desks);
  for (i = 0; i < number_of_desks; i++)
  {
    snprintf(s, sizeof(s), "Desktop %i", i);
    names[i] = malloc(strlen(s) + 1);
    strcpy(names[i], s);
  }
  if (XStringListToTextProperty(names, mode.numdesktops,  ))
  {
    XSetTextProperty(disp, root_window, &val, atom_set);
    XFree(text.value);
  }
  for (i = 0; i < number_of_desks; i++)
    free(names[i]);
  free(names);
}
