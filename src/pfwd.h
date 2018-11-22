#ifndef PFWD_H
#define PFWD_H

#include "myconfig.h"

typedef struct pfwd_context_s pfwd_context_t;

struct pfwd_context_s {
	SOCKET svrSock, cliSock;
	int threadId;
	void * userdata;
	char * svrBuf;
	char * cliBuf;
	void * (*onconnect)(pfwd_context_t * ctx);
	void * (*onclientdata)(pfwd_context_t * ctx, void * buf, size_t len);
	void * (*onserverdata)(pfwd_context_t * ctx, void * buf, size_t len);
	void * (*ondisconnect)(pfwd_context_t * ctx);
};

#endif // PFWD_H

