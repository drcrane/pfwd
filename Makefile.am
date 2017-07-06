# AUTOMAKE_OPTIONS = foreign
AC_LOCAL_AMFLAGS = -I m4 --install

bin_PROGRAMS = pfwd
pfwd_SOURCES = src/main.c src/application.c src/application.h src/myconfig.h
pfwd_CPPFLAGS = -DDEBUG -D_POSIX_C_SOURCE=201112L

# these are for including libraries found by pkgconfig
AM_CFLAGS = $(DEPS_CFLAGS)
AM_LIBS = $(DEPS_LIBS)


