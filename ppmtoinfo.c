#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#define WB_DISKMAGIC	0xe310
#define WB_DISKVERSION	1
#define WB_DISKREVISION	1
#define WB_DISKREVISIONMASK	255

#define NO_ICON_POSITION	(0x80000000)

#define	WBDISK		1
#define	WBDRAWER	2
#define	WBTOOL		3
#define	WBPROJECT	4
#define	WBGARBAGE	5
#define	WBDEVICE	6
#define	WBKICK		7
#define WBAPPICON	8

#define NONNULL 0x123456


struct { char *name; int minl, code; } icontype[] = {
  { "appicon", 1, WBAPPICON },
  { "device", 2, WBDEVICE },
  { "disk", 2, WBDISK },
  { "drawer", 2, WBDRAWER },
  { "garbage", 1, WBGARBAGE },
  { "kick", 1, WBKICK },
  { "project", 1, WBPROJECT },
  { "tool", 1, WBTOOL },
};
#define NUM_ICONTYPES (sizeof(icontype)/sizeof(icontype[0]))

typedef unsigned short pixval;
typedef struct {
  pixval r, g, b;
} pixel;

void usage()
{
  fprintf(stderr, "usage:  ppmtoinfo [-type icontype][-floyd|-fs] [-map mapfile] [ppmfile] [ppmfile]\n");
  exit(1);
}

void w16(FILE *file, int v)
{
  putc((v>>8)&0xff, file);
  putc(v&0xff, file);
}

void w32(FILE *file, int v)
{
  w16(file, v>>16);
  w16(file, v);
}

char *myalloc(int size)
{
  char *p=malloc(size);
  if(!p) {
    fprintf(stderr, "out of memory!\n");
    exit(1);
  }
  return p;
}

pixel **readppm(FILE *file, int *colsP, int *rowsP, pixval *maxvalP)
{
  pixel** pixels;
  int row, col;
  int format, mv;

  if(4!=fscanf(file, "P%d %d %d %d", &format, colsP, rowsP, &mv) ||
     (format!=3 && format!=6)) {
    fprintf(stderr, "bad magic number - not a ppm file\n");
    exit(1);
  }
  if(format==6)
    getc(file);
  *maxvalP=mv;
  pixels = (pixel **)myalloc(*rowsP * sizeof(pixel*));
  pixels[0] = (pixel *)myalloc(*rowsP * *colsP * sizeof(pixel));
  for(row=1; row<*rowsP; row++)
    pixels[row] = &(pixels[0][row * *colsP]);
  for ( row = 0; row < *rowsP; ++row )
    if(format==3)
      for( col = 0; col < *colsP; ++col) {
	int r, g, b;
	fscanf(file, "%d %d %d", &r, &g, &b);
	pixels[row][col].r=r;
	pixels[row][col].g=g;
	pixels[row][col].b=b;
      }
    else
      for( col = 0; col < *colsP; ++col) {
	pixels[row][col].r=getc(file);
	pixels[row][col].g=getc(file);
	pixels[row][col].b=getc(file);
      }  
  return pixels;
}

char *makelibfilename(char *oldname)
{
  char *newname;
  if(*oldname=='/') return oldname;
  newname=myalloc(strlen(oldname)+strlen(AMIWM_HOME)+2);
  sprintf(newname, AMIWM_HOME"/%s", oldname);
  return newname;
}

#define HSTYLE_COMPL    0
#define HSTYLE_BACKFILL 1
#define HSTYLE_IMAGE    2

void writeiconheader(FILE *file, int type, int hstyle, int cols, int rows,
		     char *deftool, char **tooltypes, char *toolwin,
		     int stksize)
{
  int drw=0;

  if(type==1 || type==2 || type==5 || type==6)
    drw=1;
  w16(file, WB_DISKMAGIC); w16(file, WB_DISKVERSION);
  w32(file, 0);
  w16(file, 0); w16(file, 0); w16(file, cols); w16(file, rows);
  w16(file, 4|hstyle); w16(file, 3); w16(file, 1);
  w32(file, NONNULL); w32(file, (hstyle==HSTYLE_IMAGE? NONNULL:0));
  w32(file, 0); w32(file, 0); w32(file, 0);
  w16(file, 0); w32(file, WB_DISKREVISION);
  putc(type, file); putc(0, file);
  w32(file, (deftool!=NULL? NONNULL:0));
  w32(file, (tooltypes!=NULL? NONNULL:0));
  w32(file, NO_ICON_POSITION); w32(file, NO_ICON_POSITION);
  w32(file, (drw? NONNULL:0)); w32(file, (toolwin? NONNULL:0));
  w32(file, stksize);
  if(drw) {
    w16(file, 50); w16(file, 50); w16(file, 400); w16(file, 100);
    w16(file, ~0); w32(file, 0); w32(file, 0x0240027f);
    w32(file, 0); w32(file, 0); w32(file, NONNULL);
    w32(file, 0); w32(file, 0);
    w16(file, 90); w16(file, 40); w16(file, ~0); w16(file, ~0);
    w16(file, 1);
    w32(file, 0); w32(file, 0);
  }
}

void writeiconinit(FILE *file, int cols, int rows, int planes)
{
  w16(file, 0); w16(file, 0);
  w16(file, cols); w16(file, rows); w16(file, planes);
  w32(file, NONNULL);
  putc((1<<planes)-1, file); putc(0, file);
  w32(file, 0);
}

void writeiconimage(FILE *file, unsigned char *pixels,
		    int cols, int rows, int planes)
{
  register int i, j, k, l;
  register unsigned char pat;
  for(i=0; i<planes; i++)
    for(j=0; j<rows; j++)
      for(k=0; k<cols; k+=8) {
	pat=0;
	for(l=0; l<8; l++)
	  if(pixels[j*cols+k+l]&(1<<i))
	    pat|=0x80>>l;
	putc(pat, file);
      }
}

void writeiconstr(FILE *file, char *str)
{
  int l=strlen(str)+1;
  w32(file, l);
  fwrite(str, 1, l, file);
}

void writeiconend(FILE *file, int type,
		  char *deftool, char **tooltypes, char *toolwin)
{
  if(deftool) writeiconstr(file, deftool);
  if(tooltypes) {
    int n;
    for(n=0; tooltypes[n]!=NULL; n++);
    w32(file, 4*(n+1));
    for(n=0; tooltypes[n]!=NULL; n++)
      writeiconstr(file, tooltypes[n]);
  }
  if(toolwin) writeiconstr(file, toolwin);
  if(type==1 || type==2 || type==5 || type==6) {
    w32(file, 0); w16(file, 0);
  }
}

#define mx(a,b) ((a)>(b)?(a):(b))

unsigned char *processicon(FILE *ifp, int floyd, pixel **mappixels,
			   int maprows, int mapcols, pixval *mapmaxval,
			   int *rowsP, int *colsP, int *planesP)
{
  int rows, cols, planes, bpr, imgsz;
  pixval maxval;
  pixel **pixels, *colormap;
  unsigned char *outimg, *oip;
  register int row, col, limitcol;
  register pixel *pP;
  int newcolors;
  register int ind;
  long* thisrerr;
  long* nextrerr;
  long* thisgerr;
  long* nextgerr;
  long* thisberr;
  long* nextberr;
  long* temperr;
  register long sr, sg, sb, err;
  int fs_direction, dscale;

  pixels = readppm( ifp, &cols, &rows, &maxval );
  if(ifp != stdin)
    fclose( ifp );

  if ( *mapmaxval != maxval ) {
    if ( *mapmaxval > maxval )
      fprintf(stderr, "rescaling colormap colors\n");
    for ( row = 0; row < maprows; ++row )
      for ( col = 0, pP = mappixels[row]; col < mapcols; ++col, ++pP ) {
	pP->r=((int)pP->r*maxval+*mapmaxval/2)/ *mapmaxval;
        pP->g=((int)pP->g*maxval+*mapmaxval/2)/ *mapmaxval;
	pP->b=((int)pP->b*maxval+*mapmaxval/2)/ *mapmaxval;
      }
    *mapmaxval = maxval;
  }

  newcolors=mapcols*maprows;
  colormap=mappixels[0];
  if(newcolors>256) newcolors=256;
  for(planes=1; (1<<planes)<newcolors; planes++);

  bpr=2*((cols+15)>>4);
  imgsz=rows*bpr*8*sizeof(unsigned char);
  outimg = (unsigned char *)myalloc(imgsz);
  memset(outimg, 0, imgsz);
  oip = outimg;

  dscale = 0;
  if(maxval>=16384)
    while(maxval>=(16384<<dscale))
      dscale++;

  if ( floyd ) {
    /* Initialize Floyd-Steinberg error vectors. */
    thisrerr = (long*) myalloc( (cols + 2) * sizeof(long) );
    nextrerr = (long*) myalloc( (cols + 2) * sizeof(long) );
    thisgerr = (long*) myalloc( (cols + 2) * sizeof(long) );
    nextgerr = (long*) myalloc( (cols + 2) * sizeof(long) );
    thisberr = (long*) myalloc( (cols + 2) * sizeof(long) );
    nextberr = (long*) myalloc( (cols + 2) * sizeof(long) );
    srand( (int) ( time( 0 ) ^ getpid( ) ) );
    for ( col = 0; col < cols + 2; ++col ) {
      thisrerr[col] = rand( ) % ( 1024 * 2 ) - 1024;
      thisgerr[col] = rand( ) % ( 1024 * 2 ) - 1024;
      thisberr[col] = rand( ) % ( 1024 * 2 ) - 1024;
      /* (random errors in [-1 .. 1]) */
    }
    fs_direction = 1;
  }
  for ( row = 0; row < rows; ++row ) {
    if ( floyd )
      for ( col = 0; col < cols + 2; ++col )
	nextrerr[col] = nextgerr[col] = nextberr[col] = 0;
    if ( ( ! floyd ) || fs_direction ) {
      col = 0;
      limitcol = cols;
      pP = pixels[row];
    } else {
      col = cols - 1;
      limitcol = -1;
      pP = &(pixels[row][col]);
    }
    do {
      register int i, r2, g2, b2;
      register long dist, newdist;

      if ( floyd ) {
	/* Use Floyd-Steinberg errors to adjust actual color. */
	sr = pP->r + thisrerr[col + 1] / 1024;
	sg = pP->g + thisgerr[col + 1] / 1024;
	sb = pP->b + thisberr[col + 1] / 1024;
	if ( sr < 0 ) sr = 0;
	else if ( sr > maxval ) sr = maxval;
	if ( sg < 0 ) sg = 0;
	else if ( sg > maxval ) sg = maxval;
	if ( sb < 0 ) sb = 0;
	else if ( sb > maxval ) sb = maxval;
      } else {
	sr = pP->r;
	sg = pP->g;
	sb = pP->b;
      }

      for ( i = 0; i < newcolors; ++i ) {
	r2 = sr - colormap[i].r;
	g2 = sg - colormap[i].g;
	b2 = sb - colormap[i].b;
	if (dscale) {
	  r2 >>= dscale;
	  g2 >>= dscale;
	  b2 >>= dscale;
	}

	newdist = r2 * r2 + g2 * g2 + b2 * b2;
	if ( i==0 || newdist < dist ) {
	  ind = i;
	  dist = newdist;
	}
      }
      
      if ( floyd ) {
	/* Propagate Floyd-Steinberg error terms. */
	if ( fs_direction ) {
	  err = ( sr - (long) colormap[ind].r ) * 1024;
	  thisrerr[col + 2] += ( err * 7 ) / 16;
	  nextrerr[col    ] += ( err * 3 ) / 16;
	  nextrerr[col + 1] += ( err * 5 ) / 16;
	  nextrerr[col + 2] += ( err     ) / 16;
	  err = ( sg - (long) colormap[ind].g ) * 1024;
	  thisgerr[col + 2] += ( err * 7 ) / 16;
	  nextgerr[col    ] += ( err * 3 ) / 16;
	  nextgerr[col + 1] += ( err * 5 ) / 16;
	  nextgerr[col + 2] += ( err     ) / 16;
	  err = ( sb - (long) colormap[ind].b ) * 1024;
	  thisberr[col + 2] += ( err * 7 ) / 16;
	  nextberr[col    ] += ( err * 3 ) / 16;
	  nextberr[col + 1] += ( err * 5 ) / 16;
	  nextberr[col + 2] += ( err     ) / 16;
	} else {
	  err = ( sr - (long) colormap[ind].r ) * 1024;
	  thisrerr[col    ] += ( err * 7 ) / 16;
	  nextrerr[col + 2] += ( err * 3 ) / 16;
	  nextrerr[col + 1] += ( err * 5 ) / 16;
	  nextrerr[col    ] += ( err     ) / 16;
	  err = ( sg - (long) colormap[ind].g ) * 1024;
	  thisgerr[col    ] += ( err * 7 ) / 16;
	  nextgerr[col + 2] += ( err * 3 ) / 16;
	  nextgerr[col + 1] += ( err * 5 ) / 16;
	  nextgerr[col    ] += ( err     ) / 16;
	  err = ( sb - (long) colormap[ind].b ) * 1024;
	  thisberr[col    ] += ( err * 7 ) / 16;
	  nextberr[col + 2] += ( err * 3 ) / 16;
	  nextberr[col + 1] += ( err * 5 ) / 16;
	  nextberr[col    ] += ( err     ) / 16;
	}
      }
      
      oip[col] = ind;
      
      if ( ( ! floyd ) || fs_direction ) {
	++col;
	++pP;
      } else {
	--col;
	--pP;
      }
    }
    while ( col != limitcol );

    oip += bpr*8;

    if ( floyd ) {
      temperr = thisrerr;
      thisrerr = nextrerr;
      nextrerr = temperr;
      temperr = thisgerr;
      thisgerr = nextgerr;
      nextgerr = temperr;
      temperr = thisberr;
      thisberr = nextberr;
      nextberr = temperr;
      fs_direction = ! fs_direction;
    }

  }

  if(floyd) {
    free(thisrerr); free(nextrerr);
    free(thisgerr); free(nextgerr);
    free(thisberr); free(nextberr);
  }
  free(pixels[0]); free(pixels);
  *rowsP=rows; *colsP=cols; *planesP=planes;
  return outimg;
}


int main(int argc, char *argv[])
{
  char *mapname = "system.map";
  int argn, maprows, mapcols;
  int floyd;
  pixel **mappixels;
  pixval mapmaxval;
  FILE* ifp;
  FILE *ifp2=NULL;
  unsigned char *outimg;
  int planes, rows, cols;
  char *deftool=NULL, **tooltypes=NULL, *toolwin=NULL;
  int stksize=0;
  int type=WBPROJECT;

  argn = 1;
  floyd = 0;

  while ( argn < argc && argv[argn][0] == '-' && argv[argn][1] != '\0' ) {
    int l=strlen(argv[argn]);
    if ( !strncmp( argv[argn], "-fs", mx(l, 2) ) ||
	 !strncmp( argv[argn], "-floyd", mx(l, 2) ) )
      floyd = 1;
    else if ( !strncmp( argv[argn], "-nofs", mx(l, 2) ) ||
	      !strncmp( argv[argn], "-nofloyd", mx(l, 2) ) )
      floyd = 0;
    else if ( !strncmp( argv[argn], "-map", mx(l, 2) ) ) {
      ++argn;
      if ( argn == argc )
	usage();
      mapname = argv[argn];
    }
    else if ( !strncmp( argv[argn], "-type", mx(l, 2) ) ) {
      int i;
      ++argn;
      if ( argn == argc )
	usage();      
      l=strlen(argv[argn]);
      for(i=0; i<NUM_ICONTYPES; i++)
	if( !strncmp( argv[argn], icontype[i].name, mx(l, icontype[i].minl))) {
	  type = icontype[i].code;
	  break;
	}
      if(i>=NUM_ICONTYPES) {
	fprintf(stderr, "Unknown icon type.  Supported types:\n");
	for(i=0; i<NUM_ICONTYPES; i++)
	  fprintf(stderr, "  %s\n", icontype[i].name);
	exit(1);
      }
    } else
      usage();
    ++argn;
  }
  
  if(!strcmp(mapname, "-"))
    ifp = stdin;
  else
    if((ifp = fopen(mapname, "r"))==NULL)
      ifp = fopen(makelibfilename(mapname), "r");
  if(!ifp) {
    perror(mapname);
    exit(1);
  }
  mappixels = readppm( ifp, &mapcols, &maprows, &mapmaxval );
  if(ifp != stdin)
    fclose( ifp );
  if ( mapcols == 0 || maprows == 0 ) {
    fprintf(stderr, "null colormap??\n");
    exit(1);
  }

  if ( argn != argc ) {
    if(!strcmp(argv[argn], "-"))
      ifp = stdin;
    else
      ifp = fopen(argv[argn], "r");
    if(!ifp) {
      perror(argv[argn]);
      exit(1);
    }
    ++argn;
  }
  else
    ifp = stdin;

  if ( argn != argc ) {
    if(!strcmp(argv[argn], "-"))
      ifp2 = stdin;
    else
      ifp2 = fopen(argv[argn], "r");
    if(!ifp) {
      perror(argv[argn]);
      exit(1);
    }
    ++argn;
  }

  if ( argn != argc )
    usage();

  outimg = processicon(ifp, floyd, mappixels, maprows, mapcols, &mapmaxval,
		       &rows, &cols, &planes);
  writeiconheader(stdout, type,
		  (ifp2==NULL? HSTYLE_BACKFILL:HSTYLE_IMAGE),
		  cols, rows, deftool, tooltypes, toolwin, stksize);
  writeiconinit(stdout, cols, rows, planes);
  writeiconimage(stdout, outimg, 16*((cols+15)>>4), rows, planes);
  free(outimg);

  if(ifp2) {
    outimg = processicon(ifp2, floyd, mappixels, maprows, mapcols, &mapmaxval,
			 &rows, &cols, &planes);
    writeiconinit(stdout, cols, rows, planes);
    writeiconimage(stdout, outimg, 16*((cols+15)>>4), rows, planes);
    free(outimg);
  }

  writeiconend(stdout, type, deftool, tooltypes, toolwin);

  return 0;
}
