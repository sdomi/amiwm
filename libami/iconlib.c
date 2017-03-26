#include "libami.h"
#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifndef AMIGAOS

#define	WBDISK		1
#define	WBDRAWER	2
#define	WBTOOL		3
#define	WBPROJECT	4
#define	WBGARBAGE	5
#define	WBDEVICE	6
#define	WBKICK		7
#define WBAPPICON	8

#define WB_DISKMAGIC    0xe310


char *BumpRevision(char *newbuf, char *oldname)
{
  char tmpbuf[32];
  int n;
  if(!strncmp(oldname, "copy_of_", 8)) {
    sprintf(newbuf, "copy_2_of_%.*s", 20, oldname);
  } else if(2==sscanf(oldname, "copy_%d_of_%30s", &n, tmpbuf)) {
    sprintf(newbuf, "copy_%d_of_", n+1);
    tmpbuf[30-strlen(newbuf)]='\0';
    strcat(newbuf, tmpbuf);
  } else {
    sprintf(newbuf, "copy_of_%.*s", 22, oldname);
  }
  return newbuf;
}

BOOL DeleteDiskObject(char *name)
{
  int res;
#ifdef HAVE_ALLOCA
  char *infoname=alloca(strlen(name)+8);
#else
  char *infoname=malloc(strlen(name)+8);
  if(infoname==NULL)
    return FALSE;
#endif
  sprintf(infoname, "%s.info", name);
  res=unlink(infoname)>=0;
#ifndef HAVE_ALLOCA
  free(infoname);
#endif
  return res;
}

char *FindToolType(char **toolTypeArray, char *typeName)
{
  char *p;
  while((p=*toolTypeArray++)) {
    char *p2=typeName;
    while(*p2)
      if(ToUpper(*p2)!=ToUpper(*p))
	break;
      else { p2++; p++; }
    if(!*p2) {
      if(!*p) return p;
      if(*p=='=') return p+1;
    }
  }
  return NULL;
}

void FreeDiskObject(struct DiskObject *diskobj)
{
  if(diskobj->do_Gadget.GadgetRender) free(diskobj->do_Gadget.GadgetRender);
  if(diskobj->do_Gadget.SelectRender) free(diskobj->do_Gadget.SelectRender);
  if(diskobj->do_DefaultTool) free(diskobj->do_DefaultTool);
  if(diskobj->do_ToolTypes) {
    char *p, **pp=diskobj->do_ToolTypes;
    while((p=*pp++)) free(p);
    free(diskobj->do_ToolTypes);
  }
  if(diskobj->do_DrawerData) free(diskobj->do_DrawerData);
  if(diskobj->do_ToolWindow) free(diskobj->do_ToolWindow);
  free(diskobj);
}

static UWORD getu16(char **p)
{
  union { UWORD n; char b[2]; } v;

#ifdef LAME_ENDIAN
  v.b[1]=*(*p)++;
  v.b[0]=*(*p)++;
#else
  v.b[0]=*(*p)++;
  v.b[1]=*(*p)++;
#endif
  return v.n;
}

static WORD get16(char **p)
{
  union { WORD n; char b[2]; } v;

#ifdef LAME_ENDIAN
  v.b[1]=*(*p)++;
  v.b[0]=*(*p)++;
#else
  v.b[0]=*(*p)++;
  v.b[1]=*(*p)++;
#endif
  return v.n;
}

static ULONG getu32(char **p)
{
  union { ULONG n; char b[4]; } v;

#ifdef LAME_ENDIAN
  v.b[3]=*(*p)++;
  v.b[2]=*(*p)++;
  v.b[1]=*(*p)++;
  v.b[0]=*(*p)++;
#else
  v.b[0]=*(*p)++;
  v.b[1]=*(*p)++;
  v.b[2]=*(*p)++;
  v.b[3]=*(*p)++;
#endif
  return v.n;
}

static LONG get32(char **p)
{
  union { LONG n; char b[4]; } v;

#ifdef LAME_ENDIAN
  v.b[3]=*(*p)++;
  v.b[2]=*(*p)++;
  v.b[1]=*(*p)++;
  v.b[0]=*(*p)++;
#else
  v.b[0]=*(*p)++;
  v.b[1]=*(*p)++;
  v.b[2]=*(*p)++;
  v.b[3]=*(*p)++;
#endif
  return v.n;
}


#define SANITYMAXLEN (1<<20)

static char *loadstring(FILE *f)
{
  ULONG l;
  char buf[4], *p=buf;
  if(1!=fread(buf, 4, 1, f) || (l=getu32(&p))>SANITYMAXLEN || !(p=malloc(l)))
    return NULL;
  fread(p, 1, l, f);
  return p;
}

static char **loadtooltypes(FILE *f)
{
  LONG i, n;
  char **p, buf[4], *tp=buf;
  if(1!=fread(buf, 4, 1, f) || (n=(getu32(&tp)>>2))>SANITYMAXLEN ||
     !(p=calloc(n, sizeof(char *))))
    return NULL;
  --n;
  for(i=0; i<n; i++)
    if(!(p[i]=loadstring(f))) {
      while(--i>=0)
	free(p[i]);
      free(p);
      return NULL;
    }
  return p;
}

#define MAXICONSIZE 2000

static struct Image *loadimage(FILE *f)
{
  char buf[20], *p=buf;
  WORD le, te, w, h, d;
  int imgsz;
  struct Image *im;
  if(1!=fread(buf, 20, 1, f))
    return NULL;
  le=get16(&p); te=get16(&p); w=get16(&p); h=get16(&p); d=get16(&p);
  if(w<=0 || w>MAXICONSIZE || h<=0 || h>MAXICONSIZE || d<1 || d>8)
    return NULL;
  imgsz=2*((w+15)>>4)*h*d;
  if(!(im=malloc(imgsz+sizeof(struct Image))))
    return NULL;
  im->LeftEdge=le; im->TopEdge=te; im->Width=w; im->Height=h;
  im->Depth=d; im->ImageData=(UWORD*)getu32(&p);
  im->PlanePick=*p++; im->PlaneOnOff=*p++;
  im->NextImage=(struct Image *)getu32(&p);
  if(im->ImageData) {
    im->ImageData=(UWORD *)(im+1);
    fread(im->ImageData, 1, imgsz, f);
  }
  return im;
}

static struct Image *backfillimage(struct Image *im)
{
  return NULL;
}

#define GETBITS(v,n) do{if(bits<(n)){if(!srclen--)return dst-dst0;data=(data\
<<8)|*src++;bits+=8;};v=(data>>(bits-(n)))&((1<<(n))-1);bits-=(n);}while(0)

static LONG unpack_rle(unsigned char *src, LONG srclen, char *dst, LONG dstlen,
		       int bpp, int comp)
{
  char *dst0 = dst;
  unsigned char *srcn = src+srclen;
  int bits=0, data=0;
  if(!comp) {
    if(srclen>dstlen)
      srclen = dstlen;
    memcpy(dst, src, srclen);
    return srclen;
  }
  while(dstlen>0) {
    int code;
    GETBITS(code, 8);
    if(code&0x80) {
      char rpt;
      GETBITS(rpt, bpp);
      code -= 0x100;
      if((dstlen-=(1-code))<0)
	break;
      do { *dst++ = rpt; } while(code++);
    } else {
      if((dstlen-=(code+1))<0)
	break;
      do GETBITS(*dst++, bpp); while(code--);
    }
  }
  if(src != srcn)
    fprintf(stderr, "Warning:  Left %d bytes unused.\n", srcn-src);
  return dst-dst0;
}

static void decode_IMAG(unsigned char *buf, LONG len, int width, int height,
			APTR *im)
{
  struct {
    int transp, ncolors, flags, bodycomp, palcomp, bpp;
    int bodybytes, palbytes;
  } hdr;
  char *dbuf;
  struct Image *ii;

  if(len<10)
    return;

  hdr.transp = *buf++;
  hdr.ncolors = (*buf++)+1;
  hdr.flags = *buf++;
  hdr.bodycomp = *buf++;
  hdr.palcomp = *buf++;
  hdr.bpp = *buf++;
  hdr.bodybytes = ((buf[0]<<8)|(buf[1]))+1;
  hdr.palbytes = ((buf[2]<<8)|(buf[3]))+1;
  buf+=4;
  len -= 10;
  if(!(hdr.flags&1))
    hdr.transp = -1;
  if(!(hdr.flags&2))
    hdr.ncolors = 0;

  if(hdr.bodybytes > len)
    hdr.bodybytes = len;
  if(hdr.palbytes+hdr.bodybytes > len)
    hdr.palbytes = len-hdr.bodybytes;

  if(hdr.bodycomp>1 || hdr.palcomp>1)
    return;

  if((dbuf = malloc(sizeof(struct Image)+width*height+3*hdr.ncolors+3))==NULL)
    return;

  /* body */
  if(unpack_rle(buf, hdr.bodybytes, dbuf+sizeof(struct Image), width*height,
		hdr.bpp, hdr.bodycomp) != width*height) {
    free(dbuf);
    return;
  }
  /* palette */
  if(unpack_rle(buf+hdr.bodybytes, hdr.palbytes,
		dbuf+sizeof(struct Image)+width*height+3, 3*hdr.ncolors,
		8, hdr.palcomp) != 3*hdr.ncolors) {
    free(dbuf);
    return;
  }

  if(*im != NULL)
    free(*im);
  *im = (APTR) (ii = (struct Image *)dbuf);
  dbuf += sizeof(struct Image);
  ii->ImageData = (UWORD *)dbuf;
  dbuf += width*height;

  *dbuf++ = hdr.transp;
  *dbuf++ = hdr.ncolors-1;
  *dbuf++ = hdr.flags;

  ii->LeftEdge = ii->TopEdge = 0;
  ii->Width = width;
  ii->Height = height;
  ii->Depth = -1;
  ii->PlanePick = ii->PlaneOnOff = 0;
  ii->NextImage = NULL;

  return;
}

static void decode_glowicon(struct DiskObject *diskobj, char *glow, LONG len)
{
  int imgno=0, gotface=0;
  struct { char width, height; UWORD dunno1, dunno2; } face;
  while(len>=8) {
    ULONG id = getu32(&glow);
    LONG clen = get32(&glow);
    char *chunk = glow;
    len -= 8;
    if(clen<0)
      clen = len;
    if(clen>len)
      break;
    switch(id) {
     case 0x46414345: /* FACE */
       if(clen>=6) {
	 face.width = 1+*chunk++;
	 face.height = 1+*chunk++;
	 face.dunno1 = getu16(&chunk);
	 face.dunno2 = getu16(&chunk);
	 gotface = 1;
	 diskobj->do_Gadget.Width = face.width;
	 diskobj->do_Gadget.Height = face.height;
       break;
       }
       break;
     case 0x494d4147: /* IMAG */
       if(!gotface || imgno>1)
	 break;
       decode_IMAG((unsigned char *)chunk, clen, face.width, face.height,
		   (imgno++? &diskobj->do_Gadget.SelectRender :
		    &diskobj->do_Gadget.GadgetRender));
       break;
    }
    if(clen&1)
      clen++;
    len -= clen;
    glow += clen;
  }
}

static struct DiskObject *int_load_do(char *filename)
{
  FILE *f;
  struct DiskObject *diskobj;
  char buf[78], *p=buf;
  int error=0;
  if((f=fopen(filename, "r"))) {
    if(1==fread(buf, 78, 1, f) &&
       (diskobj=calloc(1, sizeof(struct DiskObject)))) {
      diskobj->do_Magic=getu16(&p); diskobj->do_Version=getu16(&p);
      if(diskobj->do_Magic!=WB_DISKMAGIC) {
	free(diskobj);
	return NULL;
      }
      diskobj->do_Gadget.NextGadget=(struct Gadget *)getu32(&p);
      diskobj->do_Gadget.LeftEdge=get16(&p);
      diskobj->do_Gadget.TopEdge=get16(&p);
      diskobj->do_Gadget.Width=get16(&p); diskobj->do_Gadget.Height=get16(&p);
      diskobj->do_Gadget.Flags=getu16(&p);
      diskobj->do_Gadget.Activation=getu16(&p);
      diskobj->do_Gadget.GadgetType=getu16(&p);
      diskobj->do_Gadget.GadgetRender=(APTR)getu32(&p);
      diskobj->do_Gadget.SelectRender=(APTR)getu32(&p);
      diskobj->do_Gadget.GadgetText=(struct IntuiText *)getu32(&p);
      diskobj->do_Gadget.MutualExclude=get32(&p);
      diskobj->do_Gadget.SpecialInfo=(APTR)getu32(&p);
      diskobj->do_Gadget.GadgetID=getu16(&p);
      diskobj->do_Gadget.UserData=(APTR)getu32(&p);
      diskobj->do_Type=*p; p+=2;
      diskobj->do_DefaultTool=(char *)getu32(&p);
      diskobj->do_ToolTypes=(char **)getu32(&p);
      diskobj->do_CurrentX=get32(&p);
      diskobj->do_CurrentY=get32(&p);
      diskobj->do_DrawerData=(struct DrawerData *)getu32(&p);
      diskobj->do_ToolWindow=(char *)getu32(&p);
      diskobj->do_StackSize=get32(&p);
      
      if(diskobj->do_DrawerData) {
	struct DrawerData *dd;
	if(1==fread(buf, 56, 1, f) &&
	   (diskobj->do_DrawerData=dd=calloc(1, sizeof(struct DrawerData)))) {
	  p=buf;
	  dd->dd_NewWindow.LeftEdge=get16(&p);
	  dd->dd_NewWindow.TopEdge=get16(&p);
	  dd->dd_NewWindow.Width=get16(&p);
	  dd->dd_NewWindow.Height=get16(&p);
	  dd->dd_NewWindow.DetailPen=*p++;
	  dd->dd_NewWindow.BlockPen=*p++;
	  dd->dd_NewWindow.IDCMPFlags=getu32(&p);
	  dd->dd_NewWindow.Flags=getu32(&p);
	  dd->dd_NewWindow.FirstGadget=(struct Gadget *)getu32(&p);
	  dd->dd_NewWindow.CheckMark=(struct Image *)getu32(&p);
	  dd->dd_NewWindow.Title=(UBYTE *)getu32(&p);
	  dd->dd_NewWindow.Screen=(struct Screen *)getu32(&p);
	  dd->dd_NewWindow.BitMap=(struct BitMap *)getu32(&p);
	  dd->dd_NewWindow.MinWidth=get16(&p);
	  dd->dd_NewWindow.MinHeight=get16(&p);
	  dd->dd_NewWindow.MaxWidth=getu16(&p);
	  dd->dd_NewWindow.MaxHeight=getu16(&p);
	  dd->dd_NewWindow.Type=getu16(&p);
	  dd->dd_CurrentX=get32(&p);
	  dd->dd_CurrentY=get32(&p);
	} else error++;
      }

      if(!(diskobj->do_Gadget.GadgetRender=loadimage(f)))
	error++;
	
      if(diskobj->do_Gadget.Flags&2)
	if(!(diskobj->do_Gadget.SelectRender=loadimage(f)))
	  error++;
	else ;
      else if(diskobj->do_Gadget.Flags&1)
	if(!(diskobj->do_Gadget.SelectRender=
	     backfillimage((struct Image *)diskobj->do_Gadget.GadgetRender)))
	  error++;
	else ;
      else diskobj->do_Gadget.SelectRender=NULL;
      
      if(diskobj->do_DefaultTool)
	if(!(diskobj->do_DefaultTool=loadstring(f)))
	  error++;

      if(diskobj->do_ToolTypes)
	if(!(diskobj->do_ToolTypes=loadtooltypes(f)))
	  error++;

      if(diskobj->do_ToolWindow)
	if(!(diskobj->do_ToolWindow=loadstring(f)))
	  error++;

      if(diskobj->do_DrawerData && diskobj->do_Version) {
	char buf[6], *p=buf;
	if(1==fread(buf, 6, 1, f)) {
	  diskobj->do_DrawerData->dd_Flags=getu32(&p);
	  diskobj->do_DrawerData->dd_ViewModes=getu16(&p);
	}
      }

      if(diskobj->do_Version) {
	/* Check for GlowIcon */
	char buf[8], *p = buf, *glowicon;
	LONG len;
	if(1==fread(buf, 4, 1, f) && !strncmp(buf, "FORM", 4) &&
	   1==fread(buf, 8, 1, f) && !strncmp(buf+4, "ICON", 4) &&
	   (len = get32(&p))>4 && (glowicon = malloc(len))!=NULL) {
	  if(1==fread(glowicon, len-4, 1, f))
	    decode_glowicon(diskobj, glowicon, len-4);
	  free(glowicon);
	}
      }

      if(!error) {
	fclose(f);
	return diskobj;
      }

      FreeDiskObject(diskobj);
    }
    fclose(f);
    }
  return NULL;
}

struct DiskObject *GetDefDiskObject(LONG def_type)
{
  static char *defnames[]= {
    "disk", "drawer", "tool", "project",
    "garbage", "device", "kick", "appicon"
  };
  static char *icondir=NULL;
  static int l;
  char *buf;
  struct DiskObject *diskobj;

  if(def_type<WBDISK || def_type>WBAPPICON)
    return NULL;
  if(!icondir)
    if(!(icondir = get_current_icondir()))
      return NULL;
    else
      l = strlen(icondir);

#ifdef HAVE_ALLOCA
  buf = alloca(l+18);
#else
  buf = malloc(l+18);
  if(buf==NULL) return NULL;
#endif
  sprintf(buf, "%s/def_%s.info", icondir, defnames[def_type-WBDISK]);
  diskobj=int_load_do(buf);
#ifndef HAVE_ALLOCA
  free(buf);
#endif
  return diskobj;
}

struct DiskObject *GetDiskObject(char *name)
{
  struct DiskObject *diskobj;
#ifdef HAVE_ALLOCA
  char *buf = alloca(strlen(name)+6);
#else
  char *buf = malloc(strlen(name)+6);
  if(buf==NULL) return NULL;
#endif
  sprintf(buf, "%s.info", name);
  diskobj=int_load_do(buf);
#ifndef HAVE_ALLOCA
  free(buf);
#endif
  return diskobj;
}

struct DiskObject *GetDiskObjectNew(char *name)
{
  struct DiskObject *d;
  struct stat st;

  if((d=GetDiskObject(name))) return d;
  if(stat(name, &st)<0) return NULL;
  if(S_ISREG(st.st_mode))
    if(st.st_mode&0555)
      return GetDefDiskObject(WBTOOL);
    else
      return GetDefDiskObject(WBPROJECT);
  else if(S_ISDIR(st.st_mode))
    return GetDefDiskObject(WBDRAWER);
  else if(S_ISBLK(st.st_mode) || S_ISCHR(st.st_mode))
    return GetDefDiskObject(WBDEVICE);
  else
    return NULL;
}

#ifdef notdef

BOOL MatchToolValue(char *typeString, char *value)
{

}

BOOL PutDiskObject(char *name, struct DiskObject *diskobj)
{

}

#endif

#endif
