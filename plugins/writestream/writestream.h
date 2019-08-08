#ifndef WRITESTREAM_H
#define WRITESTREAM_H

#include "myconfig.h"

#ifdef __COMPILE_FOR_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ws2tcpip.h>
#endif

#include "pfwd.h"

#include <stddef.h>

void * writestream_connectionstarted(pfwd_context_t * ctx);
void * writestream_datafromserver(pfwd_context_t * ctx, void * data, size_t data_len);
void * writestream_datafromclient(pfwd_context_t * ctx, void * data, size_t data_len);
void * writestream_connectionclosed(pfwd_context_t * ctx);

#endif // WRITESTREAM_H

