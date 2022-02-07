#ifndef MYCONFIG_H_
#define MYCONFIG_H_

#include "config.h"

#ifdef COMPILING_FOR_WINDOWS
#define __COMPILE_FOR_WIN32
#endif

#ifdef COMPILING_FOR_LINUX
#define __COMPILE_FOR_LINUX
#endif

#ifndef __COMPILE_FOR_WIN32
#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif

#endif /* MYCONFIG_H_ */
