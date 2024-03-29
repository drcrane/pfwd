#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "utilityfn.h"
#include "timefn.h"

/*
 * This programme will dump the stream saved from pfwd
 * compile like this:
 * gcc -Iplugins/writestream/ -o streaminspector plugins/writestream/timefn.c plugins/writestream/utilityfn.c plugins/writestream/streaminspector.c
 * the executable should be built with pfwd when using ./configure:
 * make
 * Create a stream dump with pfwd:
 * pfwd 0.0.0.0 8080 192.168.1.104 80
 * Then inspect the output logfile:
 * streaminspector 20220101T080023.log
 */

struct cmd_options_s {
	uint64_t start_time;
	uint64_t end_time;
	char * filename;
};

struct cmd_options_s opt_str;
struct cmd_options_s * options;

int parse_options(int argc, char *argv[]) {
	uint64_t parsed_time;
	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--startat") == 0) {
			parsed_time = timefn_parsetimetomillis(argv[i + 1]);
			i++;
			options->start_time = parsed_time;
		} else
		if (strcmp(argv[i], "--endat") == 0) {
			parsed_time = timefn_parsetimetomillis(argv[i + 1]);
			i++;
			options->end_time = parsed_time;
		} else {
			options->filename = argv[i];
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	FILE * file;
	options = &opt_str;
	options->start_time = 0;
	options->end_time = ~((uint64_t)0);
	parse_options(argc, argv);
	file = fopen(options->filename, "rb");
	uint64_t parsed_time;
	size_t block_size;
	char origin;
	size_t read_size;
	char * buf, * buf2;
	int res = 0;
	buf = malloc(4096);
	buf2 = malloc(32768);
	while (!feof(file)) {
		size_t bytes_read;
		bytes_read = fread(&origin, 1, 1, file);
		if (bytes_read != 1) {
			if (ftell(file) == 0) {
				fprintf(stderr, "seems like an empty file\n");
				res = 1;
			}
			break;
		}
		read_size = fread(&parsed_time, 1, sizeof(uint64_t), file);
		if (read_size != sizeof(uint64_t)) {
			fprintf(stderr, "error reading time, offset %ld\n", ftell(file));
			res = 1;
			break;
		}
		read_size = fread(&block_size, 1, sizeof(size_t), file);
		if (read_size != sizeof(uint64_t)) {
			fprintf(stderr, "error reading block size header, offset %ld\n", ftell(file));
			res = 1;
			break;
		}
		read_size = fread(buf, 1, block_size, file);
		if (read_size != block_size) {
			fprintf(stderr, "error reading data block, offset %ld\n", ftell(file));
			res = 1;
			break;
		}
		if (parsed_time >= options->start_time && parsed_time < options->end_time) {
			timefn_formattimefrommillis(buf2, parsed_time);
			fprintf(stdout, "\n%c %s %lu\n", origin, buf2, block_size);
			Utility_dumphex(buf2, buf, block_size);
			fprintf(stdout, "%s", buf2);
		}
	}
	free(buf);
	free(buf2);
	fclose(file);
	return res;
}

