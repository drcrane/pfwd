AC_INIT([pfwd], [0.01], [http://www.bengreen.eu/])
AC_PREREQ([2.68])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR([buildaux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 subdir-objects foreign dist-bzip2 -Wall -Werror])
AC_CONFIG_FILES([Makefile])

# Check for C compiler
AC_PROG_CC
# C++
#AC_PROG_CXX 
# also see GNU autoconf archive
# needed for xxx_CPPFLAGS in Makefile.am though it seems to work without
#AC_PROG_CC_C_O

# set the $host environment variable so we can make decisions on Windows or Linux
AC_CANONICAL_HOST

# These should not be used, it is silly to look at the current system as you may not be
# compiling for the system the configure script is running on.
if test "x${OS}" = "xWindows_NT" ; then
	AC_DEFINE(COMPILING_ON_WINDOWS, 1, [Are we compiling on Windows?])
fi
if test "x${OS}" != "xWindows_NT" && $(uname -s) = "Linux" ; then
	AC_DEFINE(COMPILING_ON_LINUX, 1, ["Are we compiling on Linux"])
fi

# Determine what the host system is, that is the system we want the resulting executable to run on.
case "${host}" in
i[3456]86-*-linux-*)
	AC_DEFINE(COMPILING_FOR_LINUX, 1, ["Are we compiling for Linux"])
	;;
x86_64-*-linux-*)
	AC_DEFINE(COMPILING_FOR_LINUX, 1, ["Are we compiling for Linux"])
	;;
x86_64-*-mingw*)
	AC_DEFINE(COMPILING_FOR_WINDOWS, 1, [Are we compiling for Windows?])
	;;
esac

have_ws2_32=no
AC_SEARCH_LIBS([socket], [ws2_32], [have_ws2_32=yes])

if test "x${have_ws2_32}" = xyes; then
  AC_CHECK_HEADERS([winsock2.h], [],
    [have_ws2_32=no])
fi

have_pthreads=no
AC_SEARCH_LIBS([pthread_create], [pthread], [have_pthreads=yes])

if test "x${have_pthreads}" = xyes; then
  AC_CHECK_HEADERS([pthread.h], [],
    [have_pthreads=no])
fi

AC_MSG_CHECKING([whether to include hex dumping])
AC_ARG_ENABLE(hexdumps,
AC_HELP_STRING([--enable-hexdumps],[Enable Hex Dumps])
AC_HELP_STRING([--disable-hexdumps],[Disable Hex Dumps]),
[ case "$enableval" in
  no)
       AC_MSG_RESULT(no)
       AC_DEFINE(PFWD_DISABLE_HEXDUMPS, 1, [to disable Hex Dumps])
       AC_SUBST(PFWD_DISABLE_HEXDUMPS, [1])
       ;;
  *)
       AC_MSG_RESULT(yes)
       ;;
  esac ],[
       AC_MSG_RESULT(yes) ]
)

# PKG_CHECK_MODULES([DEPS], [pthreads])

AC_OUTPUT
