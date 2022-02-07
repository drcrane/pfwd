# AUTOMAKE_OPTIONS = foreign
AC_LOCAL_AMFLAGS = -I m4 --install

bin_PROGRAMS = pfwd streaminspector
pfwd_SOURCES = src/main.c src/application.c src/application.h src/myconfig.h plugins/writestream/timefn.c plugins/writestream/utilityfn.c plugins/writestream/writestream.c
pfwd_CPPFLAGS = -DDEBUG -I src/ -I plugins/writestream/

streaminspector_SOURCES = plugins/writestream/timefn.c plugins/writestream/utilityfn.c plugins/writestream/streaminspector.c
streaminspector_CPPFLAGS = -DDEBUG -I src/ -I plugins/writestream/

# these are for including libraries found by pkgconfig
AM_CFLAGS = $(DEPS_CFLAGS)
AM_LIBS = $(DEPS_LIBS)


