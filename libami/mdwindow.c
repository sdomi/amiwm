#include "libami.h"
#include "module.h"

int md_set_appwindow(Window w)
{
  return md_command00(w, MCMD_SETAPPWINDOW);
}
