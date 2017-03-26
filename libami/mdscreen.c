#include "libami.h"
#include "module.h"
#include <string.h>

int md_rotate_screen(XID id)
{
  return md_command00(id, MCMD_ROTATE_SCREEN);
}

int md_front(XID id)
{
  return md_command00(id, MCMD_FRONT);
}

int md_back(XID id)
{
  return md_command00(id, MCMD_BACK);
}

int md_iconify(XID id)
{
  return md_command00(id, MCMD_ICONIFY);
}

int md_errormsg(Window id, char *str)
{
  return md_command0(id, MCMD_ERRORMSG, str, strlen(str));
}

int md_managemenu(Window id, int menu, int item, int subitem)
{
  int data[3];
  data[0]=menu; data[1]=item; data[2]=subitem;
  return md_command0(id, MCMD_MANAGEMENU, data, sizeof(data));
}
