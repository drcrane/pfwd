#include "myconfig.h"
#include "application.h"

#ifdef __COMPILE_FOR_WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#endif
#ifdef __COMPILE_FOR_LINUX
#include <sys/socket.h>
#include <sys/select.h>
#endif

int app_initialise() {
	int retval;
#ifdef __COMPILE_FOR_WIN32
	WSADATA wsaData;
#endif
	retval = 0;

#ifdef __COMPILE_FOR_WIN32
	if ((retval = WSAStartup(0x202, &wsaData)) != 0) {
		goto error;
	}
#endif

	return retval;

error:
#ifdef __COMPILE_FOR_WIN32
	WSACleanup();
#endif
	return retval;
}

int app_shutdown() {
#ifdef __COMPILE_FOR_WIN32
	WSACleanup();
#endif
	return 0;
}
