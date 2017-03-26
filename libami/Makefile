srcdir = .

SHELL = /bin/sh
CC = gcc
LEX = flex
YACC = bison -y
RANLIB = ranlib
CFLAGS = -g -O2
YFLAGS = -d
DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DYYTEXT_POINTER=1 -DSTDC_HEADERS=1 -DHAVE_SYS_WAIT_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_FCNTL_H=1 -DHAVE_LIMITS_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_SYS_RESOURCE_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_TERMIO_H=1 -DHAVE_X11_EXTENSIONS_SHAPE_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1 -DRETSIGTYPE=void -DHAVE_WAIT3=1 -DHAVE_SELECT=1 -DHAVE_STRDUP=1 -DHAVE_WAITPID=1 -DHAVE_XSHAPE=1 -DBSD_STYLE_GETTIMEOFDAY=1 -DHAVE_XCREATEFONTSET=1 -DHAVE_XMBTEXTESCAPEMENT=1 -DHAVE_XUTF8DRAWIMAGESTRING=1 -DUSE_FONTSETS=1 -DLAME_ENDIAN=1
ALL_CFLAGS = -I. -I.. $(DEFS) $(CFLAGS) 
LIBS =   -lSM -lICE -lXext -lXmu -lX11  
prefix = /usr/local
exec_prefix = ${prefix}
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
INSTALL = /usr/bin/install -c
LN_S = ln -s

OBJS  = drawinfo.o module.o broker.o eventdispatcher.o mdscreen.o \
	mdicon.o mdwindow.o kbdsupport.o hotkey.o \
	lists.o readargs.o iconlib.o iconutil.o error.o strutil.o \
	iffparse.o

SRCS = drawinfo.c module.c broker.c eventdispatcher.c mdscreen.c \
	mdicon.c mdwindow.c kbdsupport.c hotkey.c \
	lists.c readargs.c iconlib.c iconutil.c error.c strutil.c \
	iffparse.c

all : libami.a

libami.a : $(OBJS)
	ar rc libami.a $(OBJS)
	$(RANLIB) libami.a

.c.o:
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) $<

depend: $(SRCS)
	-@rm Makefile.bak
	mv Makefile Makefile.bak
	( sed '/^#--DO NOT REMOVE THIS LINE--$$/q' < Makefile.bak ; \
	  $(CC) -MM $(ALL_CFLAGS) $(SRCS) ) > Makefile

#--DO NOT REMOVE THIS LINE--
