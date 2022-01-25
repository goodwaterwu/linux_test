#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/limits.h>

#define DEFAULT_PATTERN_FILE "hex_pattern"
#define DEFAULT_LOWER 0
#define DEFAULT_UPPER 255
#define DEFAULT_FILE_SIZE 1024

static struct option long_options[] = {
	{"add",	 required_argument, 0, 0},
	{"append", no_argument, 0, 0},
	{"delete", required_argument, 0, 0},
	{"verbose", no_argument, 0, 0},
	{"create", required_argument, 0, 'c'},
	{"file", required_argument, 0, 0},
	{0, 0, 0, 0}
};

void show_help(char *name)
{
	printf("\nGenerate a pattern file\n");
	printf("\nUsage: %s [-f file] "
			"[-l lower] [-u upper] [-s size] [-h]\n", name);
	printf("\t-f\tfile path (Cannot overwrite an exist file)\n");
	printf("\t-l\tlowest hexadecimal number (0-255)\n");
	printf("\t-u\tlargest hexadecimal number (0-255)\n");
	printf("\t-s\tfile size in bytes\n");
	printf("\t-h\tshow this help\n\n");
}

int main(int argc, char *argv[])
{
	char pattern[PATH_MAX] = DEFAULT_PATTERN_FILE;
	int opt = 0;
	unsigned long lower = DEFAULT_LOWER;
	unsigned long upper = DEFAULT_UPPER;
	unsigned long file_size = DEFAULT_FILE_SIZE;
	FILE *file = NULL;

	while (1) {
		int option_index = 0;

		opt = getopt_long(argc, argv, "f:l:s:u:h",
				long_options, &option_index);
		if (opt == -1)
			break;

		switch (opt) {
		case 'f':
			memset(pattern, 0, sizeof(pattern));
			memcpy(pattern, optarg, strlen(optarg));
			break;

		case 'l':
			lower = strtoul(optarg, NULL, 10);
			break;

		case 'u':
			upper = strtoul(optarg, NULL, 10);
			break;

		case 's':
			file_size = strtoul(optarg, NULL, 10);
			break;

		default:
			show_help(argv[0]);
		}
	}

	if (file_size < 1 || lower > upper ||
			(lower < 0 || lower > 255) ||
			(upper < 0 || upper > 255) ||
			!access(pattern, F_OK)) {
		show_help(argv[0]);
		exit(EXIT_FAILURE);
	}

	file = fopen(pattern, "wb");
	if (!file) {
		perror("Open file failed");
		exit(EXIT_FAILURE);
	}

	for (int i = 0, c = lower; i != file_size; i++, c++) {
		if (!(c % (upper + 1)))
			c = lower;

		fputc(c, file);
	}

	fclose(file);
	file = NULL;
	exit(EXIT_SUCCESS);

	return 0;
}
