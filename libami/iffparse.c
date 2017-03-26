#include "libami.h"
#include "alloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifndef AMIGAOS

static LONG callIFFHook(struct IFFHandle *iff, LONG cmd, APTR buf, LONG nby)
{
  struct IFFStreamCmd c;
  c.sc_Command = cmd;
  c.sc_Buf = buf;
  c.sc_NBytes = nby;
  if(iff->iff_Hook == NULL)
    return IFFERR_NOHOOK;
  else
    return iff->iff_Hook->h_SubEntry(iff->iff_Hook, iff, &c);
}

LONG ReadChunkBytes( struct IFFHandle *iff, APTR buf, long numBytes );
LONG WriteChunkBytes( struct IFFHandle *iff, APTR buf, long numBytes );
LONG ReadChunkRecords( struct IFFHandle *iff, APTR buf, long bytesPerRecord,
	long numRecords );
LONG WriteChunkRecords( struct IFFHandle *iff, APTR buf, long bytesPerRecord,
	long numRecords );

LONG PushChunk( struct IFFHandle *iff, long type, long id, long size );
LONG PopChunk( struct IFFHandle *iff );

LONG EntryHandler( struct IFFHandle *iff, long type, long id, long position,
	struct Hook *handler, APTR object );
LONG ExitHandler( struct IFFHandle *iff, long type, long id, long position,
	struct Hook *handler, APTR object );

LONG PropChunk( struct IFFHandle *iff, long type, long id );
LONG PropChunks( struct IFFHandle *iff, LONG *propArray, long numPairs );
LONG StopChunk( struct IFFHandle *iff, long type, long id );
LONG StopChunks( struct IFFHandle *iff, LONG *propArray, long numPairs );
LONG CollectionChunk( struct IFFHandle *iff, long type, long id );
LONG CollectionChunks( struct IFFHandle *iff, LONG *propArray,
	long numPairs );
LONG StopOnExit( struct IFFHandle *iff, long type, long id );

struct StoredProperty *FindProp( struct IFFHandle *iff, long type, long id );
struct CollectionItem *FindCollection( struct IFFHandle *iff, long type,
	long id );
struct ContextNode *FindPropContext( struct IFFHandle *iff );
struct ContextNode *CurrentChunk( struct IFFHandle *iff );
struct ContextNode *ParentChunk( struct ContextNode *contextNode );

struct LocalContextItem *AllocLocalItem( long type, long id, long ident,
	long dataSize );
APTR LocalItemData( struct LocalContextItem *localItem );
void SetLocalItemPurge( struct LocalContextItem *localItem,
	struct Hook *purgeHook );
void FreeLocalItem( struct LocalContextItem *localItem );
struct LocalContextItem *FindLocalItem( struct IFFHandle *iff, long type,
	long id, long ident );
LONG StoreLocalItem( struct IFFHandle *iff, struct LocalContextItem *localItem,
	long position );
void StoreItemInContext( struct IFFHandle *iff,
	struct LocalContextItem *localItem,
	struct ContextNode *contextNode );

LONG GoodID( long id );
LONG GoodType( long type );
STRPTR IDtoStr( long id, STRPTR buf );


void InitIFF( struct IFFHandle *iff, long flags, struct Hook *streamHook )
{
  iff->iff_Flags = flags;
  iff->iff_Hook = streamHook;
}

struct IFFHandle *AllocIFF( void )
{
  return (struct IFFHandle *)calloc(1, sizeof(struct IFFHandle));
}

LONG OpenIFF( struct IFFHandle *iff, long rwMode )
{
  return callIFFHook(iff, IFFCMD_INIT, NULL, 0);
}

LONG ParseIFF( struct IFFHandle *iff, long control )
{
  return IFFERR_NOHOOK;
}

void CloseIFF( struct IFFHandle *iff )
{
  callIFFHook(iff, IFFCMD_CLEANUP, NULL, 0);
}

void FreeIFF( struct IFFHandle *iff )
{
  free(iff);
}



#else

static ULONG __saveds __asm hllDispatch(register __a0 struct Hook *hook,
					register __a2 APTR object,
					register __a1 APTR message)
{
  return ((ULONG (*)(struct Hook *, APTR, APTR))hook->h_SubEntry)
    (hook, object, message);
}

#endif

static ULONG iffFileHook(struct Hook *hook, APTR object, APTR message)
{
  struct IFFStreamCmd *cmd = (struct IFFStreamCmd *)message;
  switch(cmd->sc_Command) {
  case IFFCMD_INIT:
    return 0;
  case IFFCMD_CLEANUP:
    close(((struct IFFHandle *)object)->iff_Stream);
    return 0;
  case IFFCMD_READ:
    return IFFERR_READ;
  case IFFCMD_WRITE:
    return IFFERR_WRITE;
  case IFFCMD_SEEK:
    return IFFERR_SEEK;
  }
  return 0;
}

LONG OpenIFFasFile( struct IFFHandle *iff, char *fn, char *mode )
{
  static struct Hook hook =
#ifdef AMIGAOS
  { { 0, 0 }, hllDispatch, iffFileHook, 0 };
#else
  { { 0, 0 }, 0, iffFileHook, 0 };
#endif
  int m, fd;
  LONG r;

  if((fd = open(fn, (strchr(mode, 'w')==NULL? O_RDONLY :
		     (O_WRONLY|O_TRUNC|O_CREAT)), 0666))<0)
    return IFFERR_READ;
  iff->iff_Stream = fd;
  InitIFF( iff, (strchr(mode, 'w')==NULL? (m=IFFF_READ|IFFF_RSEEK) :
		 (m=IFFF_WRITE|IFFF_RSEEK)), &hook );
  if((r=OpenIFF( iff, m ))!=0) {
    close(fd);
    return r;
  }
  return 0;
}
