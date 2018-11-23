#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "utilityfn.h"
#include "timefn.h"

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
	char * buf, * buf2;
	buf = malloc(4096);
	buf2 = malloc(32768);
	while (!feof(file)) {
		size_t bytes_read;
		bytes_read = fread(&origin, 1, 1, file);
		if (bytes_read != 1) {
			break;
		}
		fread(&parsed_time, sizeof(uint64_t), 1, file);
		fread(&block_size, sizeof(size_t), 1, file);
		fread(buf, block_size, 1, file);
		if (parsed_time >= options->start_time && parsed_time < options->end_time) {
			timefn_formattimefrommillis(buf2, parsed_time);
			fprintf(stdout, "\n%c %s %lld\n", origin, buf2, block_size);
			Utility_dumphex(buf2, buf, block_size);
			fprintf(stdout, "%s", buf2);
		}
	}
	free(buf);
	free(buf2);
	fclose(file);
	return 0;
}

