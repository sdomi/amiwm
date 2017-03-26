srcdir = .

SHELL = /bin/sh
CC = gcc
LEX = flex
YACC = bison -y
CFLAGS = -g -O2
YFLAGS = -d
DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DYYTEXT_POINTER=1 -DSTDC_HEADERS=1 -DHAVE_SYS_WAIT_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_FCNTL_H=1 -DHAVE_LIMITS_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_UNISTD_H=1 -DHAVE_SYS_SELECT_H=1 -DHAVE_SYS_RESOURCE_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_TERMIO_H=1 -DHAVE_X11_EXTENSIONS_SHAPE_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_ALLOCA_H=1 -DHAVE_ALLOCA=1 -DRETSIGTYPE=void -DHAVE_WAIT3=1 -DHAVE_SELECT=1 -DHAVE_STRDUP=1 -DHAVE_WAITPID=1 -DHAVE_XSHAPE=1 -DBSD_STYLE_GETTIMEOFDAY=1 -DHAVE_XCREATEFONTSET=1 -DHAVE_XMBTEXTESCAPEMENT=1 -DHAVE_XUTF8DRAWIMAGESTRING=1 -DUSE_FONTSETS=1 -DLAME_ENDIAN=1
ALL_CFLAGS = -I. -I. -I./libami $(DEFS) $(CFLAGS) 
LIBS =   -lSM -lICE -Llibami -lami  -lXext -lXmu \
	-lX11  
prefix = /usr/local
exec_prefix = ${prefix}
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
mandir = $(prefix)/man
INSTALL = /usr/bin/install -c
LN_S = ln -s
RM = -rm -f
LIBAMI = libami/libami.a
DT_DIR = /etc/dt
STRIPFLAG = -s

PROGS = amiwm requestchoice executecmd ppmtoinfo

MODULES = $(srcdir)/Background Keyboard Launcher

OBJS  = main.o screen.o client.o frame.o icc.o \
		icon.o menu.o diskobject.o gram.o lex.o rc.o \
		module.o

SRCS = main.c screen.c client.c frame.c icc.c \
		icon.c menu.c diskobject.c gram.c lex.c rc.c \
		module.c \
		requestchoice.c executecmd.c kbdmodule.c kbdlexer.c \
		launchermodule.c

DISTFILES = README README.modules INSTALL LICENSE amiwm.1 \
	configure configure.in Makefile.in install-sh smakefile scoptions \
	*.[chly] system.amiwmrc def_*.info *.map \
	Background Xresources.amiwm.in Amilogo.?m Xsession*.in Xinitrc.in \
	amiwm-init libami/Makefile.in libami/*.[chly]

PACKAGENAME = amiwm

AMIWM_HOME = $(libdir)/amiwm

all : lib_all
	@$(MAKE) local_all

local_all : $(PROGS) $(MODULES)

lib_all :
	@( cd libami; $(MAKE) all )

.c.o:
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) $<

menu.o : menu.c
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

rc.o : rc.c
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

diskobject.o : diskobject.c
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

filesystem.o : joke_fs.c
	$(CC) -o $@ -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

ppmtoinfo.o : ppmtoinfo.c
	$(CC) -c $(CPPFLAGS) $(ALL_CFLAGS) -DAMIWM_HOME=\"$(AMIWM_HOME)\" $<

gram.h gram.c: gram.y
	$(YACC) $(YFLAGS) $<
	mv y.tab.c gram.c
	mv y.tab.h gram.h

lex.c : lex.l
	$(LEX) -t $< > lex.c

kbdmodule.h kbdmodule.c: kbdmodule.y
	$(YACC) $(YFLAGS) $<
	mv y.tab.c kbdmodule.c
	mv y.tab.h kbdmodule.h

kbdlexer.c : kbdlexer.l
	$(LEX) -t $< > kbdlexer.c

install : $(PROGS) $(MODULES) Xsession Xsession2 Xinitrc amiwm-init
	-mkdir -p $(AMIWM_HOME)
	$(INSTALL) $(STRIPFLAG) requestchoice $(AMIWM_HOME)/requestchoice
	$(INSTALL) $(STRIPFLAG) executecmd $(AMIWM_HOME)/executecmd
	for module in $(MODULES); do \
	  if [ "$$module" = "$(srcdir)/Background" ]; then \
	    $(INSTALL) $$module $(AMIWM_HOME)/Background; \
	  else \
	    $(INSTALL) $(STRIPFLAG) $$module $(AMIWM_HOME)/$$module; \
	  fi; \
	done
	$(INSTALL) -m 644 $(srcdir)/system.amiwmrc $(AMIWM_HOME)/system.amiwmrc
	$(INSTALL) -m 644 $(srcdir)/def_tool.info $(AMIWM_HOME)/def_tool.info
	$(INSTALL) -m 644 $(srcdir)/system.map $(AMIWM_HOME)/system.map
	$(INSTALL) -m 644 $(srcdir)/magicwb.map $(AMIWM_HOME)/magicwb.map
	$(INSTALL) -m 644 $(srcdir)/schwartz.map $(AMIWM_HOME)/schwartz.map
	$(INSTALL) -m 755 Xsession $(AMIWM_HOME)/Xsession
	$(INSTALL) -m 755 Xsession2 $(AMIWM_HOME)/Xsession2
	$(INSTALL) -m 755 Xinitrc $(AMIWM_HOME)/Xinitrc
	$(INSTALL) -m 755 $(srcdir)/amiwm-init $(AMIWM_HOME)/amiwm-init
	-mkdir -p $(bindir)
	$(INSTALL) $(STRIPFLAG) amiwm $(bindir)/amiwm
	$(INSTALL) $(STRIPFLAG) ppmtoinfo $(bindir)/ppmtoinfo
	$(RM) $(bindir)/requestchoice
	$(LN_S) $(AMIWM_HOME)/requestchoice $(bindir)/requestchoice
	-mkdir -p $(mandir)/man1
	$(INSTALL) -m 644 $(srcdir)/amiwm.1 $(mandir)/man1/amiwm.1

fs-install : Filesystem
	$(INSTALL) $(STRIPFLAG) Filesystem $(AMIWM_HOME)/Filesystem
	$(INSTALL) -m 644 def_disk.info $(AMIWM_HOME)/def_disk.info
	$(INSTALL) -m 644 def_drawer.info $(AMIWM_HOME)/def_disk.info

cde-install : Xresources.amiwm Amilogo.bm Amilogo.pm
	-mkdir -p $(DT_DIR)/config/C/Xresources.d
	$(INSTALL) -m 644 Xresources.amiwm $(DT_DIR)/config/C/Xresources.d/Xresources.amiwm
	-mkdir -p $(DT_DIR)/appconfig/icons/C
	$(INSTALL) -m 644 Amilogo.bm $(DT_DIR)/appconfig/icons/C/Amilogo.bm
	$(INSTALL) -m 644 Amilogo.pm $(DT_DIR)/appconfig/icons/C/Amilogo.pm


amiwm : $(OBJS) $(LIBAMI)
	$(CC) -o amiwm $(OBJS) $(LIBS)

requestchoice : requestchoice.o $(LIBAMI)
	$(CC) -o requestchoice requestchoice.o $(LIBS)

executecmd : executecmd.o $(LIBAMI)
	$(CC) -o executecmd executecmd.o $(LIBS)

Filesystem : filesystem.o $(LIBAMI)
	$(CC) -o Filesystem filesystem.o $(LIBS)

Keyboard : kbdmodule.o kbdlexer.o $(LIBAMI)
	$(CC) -o Keyboard kbdmodule.o kbdlexer.o $(LIBS)

Launcher : launchermodule.o $(LIBAMI)
	$(CC) -o Launcher launchermodule.o $(LIBS)

ppmtoinfo : ppmtoinfo.o
	$(CC) -o ppmtoinfo ppmtoinfo.o -lm

localetest : localetest.o $(LIBAMI)
	$(CC) -o localetest localetest.o $(LIBS)

clean :
	$(RM) core $(PROGS) $(LIBAMI) Keyboard Launcher *.o libami/*.o
	$(RM) lex.yy.c lex.c y.tab.c y.tab.h gram.h gram.c
	$(RM) kbdlexer.c kbdmodule.h kbdmodule.c
	$(RM) config.log

distclean : clean
	$(RM) config.status config.cache *~
	$(RM) Makefile libami/Makefile Xresources.amiwm Xsession Xsession2 Xinitrc

spotless : distclean

TAGS:
	etags *.[ch] libami/*.[ch]

patch :
	mv version.h old_version.h
	sed < old_version.h 's/l/"/' | awk '-F"' '{ printf "%s\"%sl%s\"\n",$$1,$$2,1+$$3 }' > version.h
	rm old_version.h

dist : version.h clean
	( version=`sed < version.h -e 's/^[^"]*"//' -e 's/"[^"]*$$//'`; \
	  tarname="$(PACKAGENAME)$$version.tar"; \
	  mkdir $(PACKAGENAME)$$version; \
	  tar cf - $(DISTFILES) | (cd $(PACKAGENAME)$$version/; tar xf - ); \
	  rm -f $$tarname $$tarname.gz; \
	  tar cf $$tarname $(PACKAGENAME)$$version; \
	  rm -rf $(PACKAGENAME)$$version; \
	  gzip $$tarname; \
	)

$(srcdir)/configure: $(srcdir)/configure.in
	autoconf $(srcdir)/configure.in > $(srcdir)/configure

config.status: $(srcdir)/configure
	$(srcdir)/configure

Makefile: $(srcdir)/Makefile.in config.status
	./config.status

libami/Makefile: $(srcdir)/libami/Makefile.in config.status
	./config.status

Xresources.amiwm: $(srcdir)/Xresources.amiwm.in
	sed -e 's:[@]bindir[@]:$(bindir):g' -e 's:[@]AMIWM_HOME[@]:$(AMIWM_HOME):g' < $(srcdir)/Xresources.amiwm.in > Xresources.amiwm

Xsession: $(srcdir)/Xsession.in config.status
	sed -e 's:[@]bindir[@]:$(bindir):g' -e 's:[@]AMIWM_HOME[@]:$(AMIWM_HOME):g' < $(srcdir)/Xsession.in > Xsession

Xsession2: $(srcdir)/Xsession2.in config.status
	sed -e 's:[@]bindir[@]:$(bindir):g' -e 's:[@]AMIWM_HOME[@]:$(AMIWM_HOME):g' < $(srcdir)/Xsession2.in > Xsession2

Xinitrc: $(srcdir)/Xinitrc.in config.status
	sed -e 's:[@]bindir[@]:$(bindir):g' -e 's:[@]AMIWM_HOME[@]:$(AMIWM_HOME):g' < $(srcdir)/Xinitrc.in > Xinitrc


depend:: $(SRCS)
	-@rm Makefile.bak
	mv Makefile Makefile.bak
	( sed '/^#--DO NOT REMOVE THIS LINE--$$/q' < Makefile.bak ; \
	  $(CC) -MM $(ALL_CFLAGS) $(SRCS) ) > Makefile

depend::
	@( cd libami; $(MAKE) depend )

#--DO NOT REMOVE THIS LINE--
