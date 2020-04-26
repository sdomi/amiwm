AC_INIT(diskobject.c)

AC_ENABLE(fontsets,
	  [use_fontsets="$enableval"], [use_fontsets=auto])

AC_PROG_CC
AC_ISC_POSIX
AC_PROG_AWK
AC_PROG_YACC
AC_PROG_LEX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

AC_PATH_XTRA

AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h limits.h sys/time.h sys/types.h unistd.h sys/select.h sys/resource.h sys/stat.h termio.h)
ac_save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
AC_CHECK_HEADERS(X11/extensions/shape.h)
CPPFLAGS="$ac_save_CPPFLAGS"

AC_C_CONST
AC_C_CHAR_UNSIGNED
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

AC_FUNC_ALLOCA
AC_TYPE_SIGNAL
AC_FUNC_WAIT3
AC_CHECK_FUNCS(select strdup waitpid)


dnl  On SunOS4, libXmu seems to be all in one unit forcing references
dnl  to libXt into the linking.  Only fix I can think of is to link with
dnl  libXt too, so that's what we'll do.

AC_MSG_CHECKING([for broken libXmu])
old_LIBS="$LIBS"
LIBS="$X_LIBS $X_PRE_LIBS -lXext -lXmu -lX11 $X_EXTRA_LIBS $LIBS"
AC_TRY_LINK([],[
  extern void XmuCopyISOLatin1Lowered(char *,char *);
  char foo[]="FOO";
  XmuCopyISOLatin1Lowered (foo, foo);
], AC_MSG_RESULT([no])
XT_LIB_KLUDGE="", AC_MSG_RESULT([yes])
XT_LIB_KLUDGE="-lXt")
LIBS="$old_LIBS"
AC_SUBST(XT_LIB_KLUDGE)


if test "x$ac_cv_header_X11_extensions_shape_h" = xyes; then
  ac_save_LIBS="$LIBS"
  LIBS="$X_LIBS $X_PRE_LIBS $XT_LIB_KLUDGE -lXext -lXmu -lX11 $X_EXTRA_LIBS $LIBS"
  AC_CHECK_FUNC(XShapeQueryExtension, AC_DEFINE([HAVE_XSHAPE],1,[Define this if your Xlib supports the SHAPE X server extension.]))
  LIBS="$ac_save_LIBS"
else
  :
fi

AC_MSG_CHECKING([number of arguments to gettimeofday])
AC_TRY_LINK([
#include <stdlib.h>
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
],[
  struct timeval tp;
  struct timezone tz;
  gettimeofday(&tp, &tz);
], AC_DEFINE(BSD_STYLE_GETTIMEOFDAY,1,[Define this if gettimeofday() takes two arguments.])
AC_MSG_RESULT([2]), AC_MSG_RESULT([1]))

AC_CHECK_TYPE(caddr_t, char *)
AC_MSG_CHECKING([for XPointer])
AC_EGREP_HEADER([XPointer[^a-zA-Z_]], [$]x_includes[$]{x_includes:+/}X11/Xlib.h, AC_MSG_RESULT([yes]), AC_DEFINE(XPointer, caddr_t,[Define this to a pointer type if XPointer is not a predefined type])
AC_MSG_RESULT([no]))

ac_save_LIBS="$LIBS"
LIBS="$X_LIBS $X_PRE_LIBS $XT_LIB_KLUDGE -lXext -lXmu -lX11 $X_EXTRA_LIBS $LIBS"
AC_CHECK_FUNCS(XCreateFontSet XmbTextEscapement Xutf8DrawImageString)
LIBS="$ac_save_LIBS"

if test "x$ac_cv_func_XCreateFontSet:$ac_cv_func_XmbTextEscapement:$use_fontsets" = xyes:yes:auto; then
  use_fontsets=yes
else
  :
fi

if test "x$use_fontsets" = xyes; then
  AC_DEFINE([USE_FONTSETS])
else
  :
fi

AC_MSG_CHECKING([byteorder])
AC_TRY_RUN([
int main()
{
  union { int i; char c[sizeof(int)]; } v;
  v.i=0;
  v.c[0]=1;
  if(v.i==1) exit(0); else exit(1);
  return 0;
}
], AC_DEFINE([LAME_ENDIAN],1,[Define this is your achitecture stores integers backwards in memory])
AC_MSG_RESULT([backward]), AC_MSG_RESULT([normal]), AC_MSG_RESULT([no idea]))

AC_SUBST(x_includes)
AC_SUBST(x_libraries)
AC_OUTPUT(Makefile libami/Makefile)
