/* Doing this means that timefn.c and timefn.h
 * do not need to be altered from the original
 * in libgreen */
#ifdef HAVE_CONFIG_H
#include "myconfig.h"
#endif
#define USE_UTILITY_FN
#include "timefn.c"
