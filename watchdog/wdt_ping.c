#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/limits.h>

#define DEFAULT_WATCHDOG "/dev/watchdog"
#define DEFAULT_PINGTIME 10

#define SHOW_HELP(name)\
{\
	printf("Usage: %s [OPTIONS]\n", name);\
	printf("Ping watchdog.\n");\
	printf("\t-c <count>, --count=<count>\tPing times\n");\
	printf("\t-n <device>, --name=<device>\tWatchdog device\n");\
	printf("\t-t <time>, --time=<time>\tPing watchdog every <time> second(s)\n");\
	printf("\t-h, --help\tShow this help\n\n");\
	exit(EXIT_SUCCESS);\
}

static struct option long_options[] = {
	{"count", required_argument, 0, 'c'},
	{"name", required_argument, 0, 'n'},
	{"time", required_argument, 0, 't'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

int main(int argc, char *const *argv)
{
	int fd = 0;
	int opt = 0;
	int count = 0;
	int pingtimes = 0;
	int pingtime = DEFAULT_PINGTIME;
	char device[PATH_MAX] = DEFAULT_WATCHDOG;

	while (1) {
		int option_index = 0;

		opt =
		    getopt_long(argc, argv, "c:hn:t:", long_options,
				&option_index);

		if (opt == -1)
			break;

		switch (opt) {
		case 'c':
			pingtimes = atoi(optarg);
			break;
		case 'n':
			memset(device, 0, PATH_MAX);
			memcpy(device, optarg, strlen(optarg));
			break;
		case 't':
			pingtime = atoi(optarg);
			break;
		case 'h':
		case '?':
		default:
			SHOW_HELP(argv[0]);
		}
	}

	fd = open(device, O_WRONLY);

	if (fd == -1) {
		perror("watchdog");
		exit(EXIT_FAILURE);
	}

	while (1) {
		if (write(fd, "\0", 1) != -1)
			printf("Succeed to ping watchdog device: %s\n", device);
		else
			printf("Fail to ping watchdog device: %s\n\n", device);

		if (pingtimes > 0 && ++count >= pingtimes)
			break;

		sleep(pingtime);
	}

	if (write(fd, "V", 1) != -1)
		close(fd);

	return 0;
}
