#include "myconfig.h"

#include <string.h>
#include <stdlib.h>

#ifdef __COMPILE_FOR_WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#ifdef __COMPILE_FOR_LINUX
/*
 * for linux compile with these flags:
 * -D_REENTRANT -lpthread
 */
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#define SOCKADDR_IN struct sockaddr_in
#define closesocket close
#include <pthread.h>
/* for getaddrinfo, freeaddrinfo and other useful stuff */
#include <sys/types.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <string.h>

#include "application.h"
#include "pfwd.h"
#include "writestream.h"

#define LINE_LENGTH 16

static void dump_bytes(FILE * file, char * prfx, char * bytes, int len) {
	int i, j;
	int count = 0;

	for (i = 0; i < len; i++) {
		if (!count) {
			fprintf(file, "%s", prfx);
		}
		fprintf(file, "%02x ", bytes[i] & 0xff);
		count ++;
		if (count == LINE_LENGTH || (i == len - 1)) {
			fprintf(file, " ");
			for (j = (i - (count - 1)); j <= i; j++) {
				/* Filter unprintable characters */
				if ((bytes[j] & 0xff) >= 32 && (bytes[j] & 0xff) <= 126) {
					fprintf(file, "%c", bytes[j] & 0xff);
				} else {
					fprintf(file, ".");
				}
			}
			fprintf(file, "\r\n");
			count = 0;
		}
	}
	fflush(file);
}

SOCKET configure_server(char * localipaddress, unsigned short port) {
	SOCKET sock;
	SOCKADDR_IN sin;
	int ret;
	int on = 1;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		return sock;
	}
	ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (ret < 0) {
		goto error;
	}
	memset(&sin, 0, sizeof(SOCKADDR_IN));
	sin.sin_family = PF_INET;
	sin.sin_port = htons(port);
	//sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_addr.s_addr = inet_addr(localipaddress);
	ret = bind(sock, (struct sockaddr *)&sin, sizeof(SOCKADDR_IN));
	if (ret == SOCKET_ERROR) {
		goto error;
	}
	ret = listen(sock, 10);
	if (ret == SOCKET_ERROR) goto error;

	return sock;

error:
	if (sock != INVALID_SOCKET) { closesocket(sock); }
	return INVALID_SOCKET;
}

SOCKET connect_client(char * hostname_str, unsigned short port) {
	int rc;
	struct addrinfo hints, *res = NULL;
	SOCKET sock = INVALID_SOCKET;

	memset(&hints, 0, sizeof(struct addrinfo));
	//hints.ai_family = AF_UNSPEC;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	rc = getaddrinfo(hostname_str, NULL, &hints, &res);
	if (rc != 0) {
		goto error;
	}
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock == INVALID_SOCKET) {
		goto error;
	}
	((struct sockaddr_in *)res->ai_addr)->sin_port = htons(port);
	rc = connect(sock, res->ai_addr, (int)res->ai_addrlen);
	if (rc != 0) {
		goto error;
	}
	freeaddrinfo(res);
	return sock;
error:
	if (res != NULL) {
		freeaddrinfo(res);
	}
	if (sock != INVALID_SOCKET) {
		closesocket(sock);
	}
	return INVALID_SOCKET;
}

#define BUFFER_SIZE 4096

void * service_thread(void * argsPtr) {
	SOCKET svcSock = INVALID_SOCKET, cliSock = INVALID_SOCKET, highSock;
	fd_set socks;
	pfwd_context_t * ctx;

	int readsocks;

	char * svcBuf;
	char * cliBuf;
	int res;

	size_t svcBytes;
	size_t cliBytes;

	struct timeval s_timeout;

	ctx = (pfwd_context_t *)argsPtr;
	svcSock = ((pfwd_context_t *)argsPtr)->svrSock;
	cliSock = ((pfwd_context_t *)argsPtr)->cliSock;

	svcBuf = malloc(BUFFER_SIZE);
	cliBuf = malloc(BUFFER_SIZE);

	ctx->svrBuf = svcBuf;
	ctx->cliBuf = cliBuf;

	if (svcBuf == NULL || cliBuf == NULL) { goto service_thread_error; }

	#ifdef PFWD_ENABLE_PLUGINS
	if (ctx->onconnect != NULL) {
		ctx->onconnect(ctx);
	}
	#endif
	#ifdef PFWD_ENABLE_HEXDUMPS
	fprintf(stdout, "HEXDUMP ENABLED\n");
	#endif

	while (1) {
		FD_ZERO(&socks);
		FD_SET(svcSock, &socks);
		FD_SET(cliSock, &socks);
		s_timeout.tv_sec = 2500 / 1000;
		s_timeout.tv_usec = (2500 % 1000) * 1000;
		highSock = cliSock;
		if (highSock < svcSock) { highSock = svcSock; }
		readsocks = select(highSock+1, &socks, (fd_set *)0, (fd_set *)0, &s_timeout);

		if (readsocks == -1) {
			break;
		}

		if (readsocks == 0) {
			//fprintf(stdout, "."); fflush(stdout);
			continue;
		}

		if (FD_ISSET(svcSock, &socks)) {
			svcBytes = recv(svcSock, svcBuf, 4096, 0);
			if (svcBytes == 0) { break; }
			if (svcBytes > 0) {
				res = send(cliSock, svcBuf, svcBytes, 0);
				#ifdef PFWD_ENABLE_PLUGINS
				if (ctx->onserverdata != NULL) {
					ctx->onserverdata(ctx, svcBuf, svcBytes);
				}
				#endif
				#ifdef PFWD_ENABLE_HEXDUMPS
				dump_bytes(stdout, "C ", svcBuf, svcBytes);
				#endif
				if (res <= 0) { break; }
			}
			svcBytes = 0;
		}

		if (FD_ISSET(cliSock, &socks)) {
			cliBytes = recv(cliSock, cliBuf, 4096, 0);
			if (cliBytes == 0) { break; }
			if (cliBytes > 0) {
				res = send(svcSock, cliBuf, cliBytes, 0);
				#ifdef PFWD_ENABLE_PLUGINS
				if (ctx->onclientdata != NULL) {
					ctx->onclientdata(ctx, cliBuf, cliBytes);
				}
				#endif
				#ifdef PFWD_ENABLE_HEXDUMPS
				dump_bytes(stdout, "S ", cliBuf, cliBytes);
				#endif
				if (res <= 0) { break; }
			}
			cliBytes = 0;
		}

		if (cliBytes == -1 || svcBytes == -1) {
			break;
		}
	}
	#ifdef PFWD_ENABLE_PLUGINS
	if (ctx->ondisconnect != NULL) {
		ctx->ondisconnect(ctx);
	}
	#endif
	goto service_thread_return;

service_thread_error:


service_thread_return:

	if (svcSock != INVALID_SOCKET) { closesocket(svcSock); }
	if (cliSock != INVALID_SOCKET) { closesocket(cliSock); }

	if (svcBuf != NULL) { free(svcBuf); }
	if (cliBuf != NULL) { free(cliBuf); }

	free(argsPtr);

	fprintf(stdout, "TRM\r\n"); fflush(stdout);

#ifdef __COMPILE_FOR_LINUX
	pthread_exit(NULL);
#endif

	return NULL;
}

int main(int argc, char *argv[]) {
	SOCKET svrSock;
	SOCKET cliSock;
	SOCKET svcSock;
	pfwd_context_t * svc_thr;

#ifdef __COMPILE_FOR_WIN32
	DWORD tid;
	HANDLE threadHandle;
#endif
#ifdef __COMPILE_FOR_LINUX
	pthread_t tid;
	pthread_attr_t * thattr;
#endif

	char * listenIp;
	unsigned short listenPort;
	char * destIp;
	unsigned short destPort;

	if (argc != 5) {
		fprintf(stderr, "Usage:\n   %s <listenIp> <listenPort> <destIp> <destPort>\n   %s 0.0.0.0 5000 192.168.1.104 2030\n", "pfwd", "pfwd");
		return 1;
	}
	
	listenIp = argv[1];
	listenPort = atoi(argv[2]);
	destIp = argv[3];
	destPort = atoi(argv[4]);

	app_initialise();

	svrSock = configure_server(listenIp, listenPort);
	if (svrSock == INVALID_SOCKET) {
		// maybe this port is taken?
		fprintf(stderr, "Cannot bind server socket, quitting now\n");
		goto error;
	}

#ifdef __COMPILE_FOR_LINUX
	signal(SIGPIPE, SIG_IGN);
#endif

	while (1 == 1) {
		svcSock = accept(svrSock, NULL, NULL);
		fprintf(stdout, "CLIENT CONNECTED\n"); fflush(stdout);
		
		cliSock = connect_client(destIp, destPort);
		fprintf(stdout, "SOCKETS %d %d\n", (int)cliSock, (int)svcSock); fflush(stdout);
		if (cliSock == INVALID_SOCKET) {
			fprintf(stdout, "Could not complete client connection\n");
			closesocket(svcSock);
			continue;
		}
		
		svc_thr = (pfwd_context_t *)malloc(sizeof(pfwd_context_t));
		memset(svc_thr, 0, sizeof(pfwd_context_t));
		svc_thr->svrSock = svcSock;
		svc_thr->cliSock = cliSock;
		svc_thr->onconnect = writestream_connectionstarted;
		svc_thr->onclientdata = writestream_datafromserver;
		svc_thr->onserverdata = writestream_datafromclient;
		svc_thr->ondisconnect = writestream_connectionclosed;

#ifdef __COMPILE_FOR_WIN32
		threadHandle = CreateThread((LPSECURITY_ATTRIBUTES)NULL, (SIZE_T)65536, (LPTHREAD_START_ROUTINE)service_thread, (LPVOID)svc_thr, 0, (LPDWORD)&tid);
		CloseHandle(threadHandle);
#endif
#ifdef __COMPILE_FOR_LINUX
		thattr = NULL;
		pthread_create(&tid, thattr, service_thread, (void *)svc_thr);
#endif
	}
error:
	app_shutdown();
	return 0;
}
