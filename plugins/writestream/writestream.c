#include "writestream.h"

#include "timefn.h"
#include "utilityfn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void * writestream_connectionstarted(pfwd_context_t * ctx) {
	char buf[128];
	FILE * file;
	uint64_t current_time_millis;
	current_time_millis = timefn_getcurrentunixtimemillis();
	timefn_formattimefrommillis(buf, current_time_millis);
	fprintf(stdout, "DATETIME: %ld %s\n", current_time_millis, buf);
	Utility_removeCharacters(buf, ": -");
	strcat(buf, ".log");
	file = fopen(buf, "wb");
	ctx->userdata = (void *)file;
	return NULL;
}

static void * writetofile(FILE * file, char origin, void * data, size_t data_len) {
	uint64_t current_time_millis;
	current_time_millis = timefn_getcurrentunixtimemillis();
	fwrite(&origin, 1, 1, file);
	fwrite(&current_time_millis, sizeof(uint64_t), 1, file);
	fwrite(&data_len, sizeof(size_t), 1, file);
	fwrite(data, data_len, 1, file);
	return NULL;
}

void * writestream_datafromserver(pfwd_context_t * ctx, void * data, size_t data_len) {
	return writetofile((FILE *)ctx->userdata, 'S', data, data_len);
}

void * writestream_datafromclient(pfwd_context_t * ctx, void * data, size_t data_len) {
	return writetofile((FILE *)ctx->userdata, 'C', data, data_len);
}

void * writestream_connectionclosed(pfwd_context_t * ctx) {
	FILE * file;
	file = (FILE *)ctx->userdata;
	fclose(file);
	ctx->userdata = NULL;
	return NULL;
}

