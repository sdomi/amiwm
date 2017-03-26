#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#include <signal.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "libami.h"
#include "module.h"
#include "alloc.h"

static int md_in_fd=-1, md_out_fd=-1;

static char *md_name = "";

char *amiwm_version;

Window md_root = None;

static int md_int_len=0;
static char *md_int_buf=NULL;
void (*md_broker_func)(XEvent *, unsigned long);

void md_exit(int signal)
{
  if(md_in_fd>=0)
    close(md_in_fd);
  if(md_out_fd>=0)
    close(md_out_fd);
  exit(0);
}

static int md_write(void *ptr, int len)
{
  char *p=ptr;
  int r, tot=0;
  while(len>0) {
    if((r=write(md_out_fd, p, len))<0)
      if(errno==EINTR)
	continue;
      else
	return r;
    if(!r)
      return tot;
    tot+=r;
    p+=r;
    len-=r;
  }
  return tot;
}

static int md_read(void *ptr, int len)
{
  char *p=ptr;
  int r, tot=0;
  while(len>0) {
    if((r=read(md_in_fd, p, len))<0)
      if(errno==EINTR)
	continue;
      else
	return r;
    if(!r)
      if(tot)
	return tot;
      else
	md_exit(0);
    tot+=r;
    p+=r;
    len-=r;
  }
  return tot;
}

static int md_int_load(int len)
{
  if(len>=md_int_len)
    if(md_int_buf!=NULL)
      md_int_buf=realloc(md_int_buf, md_int_len=len+1);
    else
      md_int_buf=malloc(md_int_len=len+1);
  md_int_buf[len]='\0';
  return md_read(md_int_buf, len);
}

static struct md_queued_event {
  struct md_queued_event *next;
  struct mcmd_event e;
} *event_head=NULL, *event_tail=NULL;

void md_process_queued_events()
{
  struct md_queued_event *e;

  while((e=event_head)) {
    event_head=e->next;
    md_broker_func(&e->e.event, e->e.mask);
    free(e);
  }
}

static void md_enqueue(struct mcmd_event *e)
{
  struct md_queued_event *qe=malloc(sizeof(struct md_queued_event));
  if(qe) {
    qe->e=*e;
    qe->next=NULL;
    if(event_head) {
      event_tail->next=qe;
      event_tail=qe;
    } else {
      event_head=event_tail=qe;
    }
  }
}

static int md_get_async(int len)
{
  if(md_int_load(len)!=len)
    return -1;
  if(md_broker_func)
    md_enqueue((struct mcmd_event *)md_int_buf);
  return 1;
}

int md_handle_input()
{
  int res;

  if(md_read(&res, sizeof(res))!=sizeof(res))
    return -1;
  if(res>=0) {
    if(!res)
      return 0;
    md_int_load(res);
    return 0;
  } else {
    res=~res;
    if(!res)
      return 0;
    return md_get_async(res);
  }
}

int md_command(XID id, int cmd, void *data, int data_len, char **buffer)
{
  int res;
  struct mcmd_header mcmd;

  *buffer=NULL;

  mcmd.id = id;
  mcmd.cmd = cmd;
  mcmd.len = data_len;

  if(md_write(&mcmd, sizeof(mcmd))!=sizeof(mcmd) ||
     md_write(data, data_len)!=data_len ||
     md_read(&res, sizeof(res))!=sizeof(res))
    return -1;

  while(res<-1) {
    md_get_async(~res);
    if(md_read(&res, sizeof(res))!=sizeof(res))
      return -1;
  }
  if(res>0) {
    *buffer=malloc(res);
    if(md_read(*buffer, res)!=res)
      return -1;
  }
  return res;
}

int md_command0(XID id, int cmd, void *data, int data_len)
{
  char *ptr=NULL;
  int res=md_command(id, cmd, data, data_len, &ptr);
  if(ptr) free(ptr);
  return res;
}

int md_command00(XID id, int cmd)
{
  return md_command0(id, cmd, NULL, 0);
}

static void md_fail()
{
  fprintf(stderr, "%s: cannot establish connection to amiwm\n", md_name);
  exit(1);
}

Display *md_display()
{
  static Display *dpy=NULL;
  if(!dpy)
    dpy=XOpenDisplay(NULL);
  return dpy;
}

char *md_init(int argc, char *argv[])
{
  if(argc>0)
    md_name=argv[0];
  if(argc>2) {
    md_in_fd=strtol(argv[1], NULL, 0);
    md_out_fd=strtol(argv[2], NULL, 0);
  } else
    md_fail();

  signal(SIGHUP, md_exit);
  signal(SIGPIPE, md_exit);

  if(argc>3)
    md_root=strtol(argv[3], NULL, 0);
  else
    md_root=None;

  if(md_command(None, MCMD_GET_VERSION, NULL, 0, &amiwm_version)<=0)
    md_fail();

  return (argc>4? argv[4]:NULL);
}

void md_main_loop()
{
  do md_process_queued_events(); while(md_handle_input()>=0);
}

int md_connection_number()
{
  return md_in_fd;
}
