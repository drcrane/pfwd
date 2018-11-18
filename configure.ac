AC_INIT([pfwd], [0.01], [http://www.bengreen.eu/])
AC_PREREQ([2.68])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_AUX_DIR([buildaux])
AC_CONFIG_MACRO_DIR([m4])

# information on these can be found in the autoconf manual:
# https://www.gnu.org/software/autoconf/manual/autoconf.html#Canonicalizing
# set the $host environment variable so we can make decisions on Windows or Linux
# this is the host system type (the system for which the project is built for)
AC_CANONICAL_HOST
# set the $target environment variable so we can make decisions on Windows or Linux
# this is actually used for toolchains, build and host would be the systems used
# to compile the sources and the system used to execute the generated executable.
#AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.11 subdir-objects foreign dist-bzip2 -Wall -Werror])
AC_CONFIG_FILES([Makefile])

# Check for C compiler
AC_PROG_CC
# C++
#AC_PROG_CXX 
# also see GNU autoconf archive
# needed for xxx_CPPFLAGS in Makefile.am though it seems to work without
#AC_PROG_CC_C_O

# Restrict to C99
#AC_PROG_CC_C99

# Add C11 Support
AX_CHECK_COMPILE_FLAG([-std=c11],
	[AX_APPEND_FLAG([-std=c11])],
	[AC_MSG_WARN([-std=c11 not supported, you may have to set CFLAGS to enable C11 support.])
])

# define the appropriate thing depending on the host OS.
case "${host_os}" in
mingw*)
	AC_DEFINE(COMPILING_FOR_WINDOWS, 1, [Compiling for Windows])
	;;
linux*)
	AX_APPEND_COMPILE_FLAGS([-D_POSIX_C_SOURCE=201112L])
	AC_DEFINE(COMPILING_FOR_LINUX, 1, [Compiling for Linux])
	;;
*)
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
       AC_DEFINE(PFWD_ENABLE_HEXDUMPS, 1, [to enable Hex Dumps])
       AC_SUBST(PFWD_ENABLE_HEXDUMPS, [1])
       ;;
  esac ],[
       AC_MSG_RESULT(yes) ]
)

AC_MSG_CHECKING([include support for plugins])
AC_ARG_ENABLE(plugins,
AC_HELP_STRING([--enable-plugins],[Enable plugin support])
AC_HELP_STRING([--disable-plugins],[Disable plugin support]),
[ case "$enableval" in
  no)
       AC_MSG_RESULT(no)
       AC_DEFINE(PFWD_DISABLE_PLUGINS, 1, [to disable Plugins])
       AC_SUBST(PFWD_DISABLE_PLUGINS, [1])
       ;;
  *)
       AC_MSG_RESULT(yes)
       AC_DEFINE(PFWD_ENABLE_PLUGINS, 1, [to enable Plugins])
       AC_SUBST(PFWD_ENABLE_PLUGINS, [1])
       ;;
  esac ],[
       AC_MSG_RESULT(yes) ]
)

# PKG_CHECK_MODULES([DEPS], [pthreads])

AC_OUTPUT
