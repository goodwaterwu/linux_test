#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/limits.h>
#include <linux/watchdog.h>

#define CHAR_TO_INT(x) (x - '0')

#define DEFAULT_WATCHDOG "/dev/watchdog"
#define DEFAULT_PINGMODE 0
#define DEFAULT_PINGTIME 10
#define DEFAULT_PINGTIMES 10

#define SHOW_HELP(name)\
{\
	printf("Usage: %s [OPTIONS]\n", name);\
	printf("Test watchdog.\n");\
	printf("\t-h, --help\tShow this help\n\n");\
	exit(EXIT_SUCCESS);\
}

void wdt_disablewatchdog(void);
void wdt_enablewatchdog(void);
void wdt_get_support(void);
void wdt_get_bootstatus(void);
void wdt_get_timeleft(void);
void wdt_get_timeout(void);
void wdt_ping(void);
void wdt_set_watchdogdevice(char *path);
void wdt_set_timeout(int timeout);
void wdt_set_pingmode(void);
void wdt_set_pingtime(int second);
void wdt_set_pingtimes(int count);
void wdt_exit(void);

struct menu
{
	int id;
	void (*func)();
	char desc[64];
};

struct menu main_menu[] = 
{
	{ 0, wdt_disablewatchdog, "%d) Disable watchdog device\n" },
	{ 1, wdt_enablewatchdog, "%d) Enable watchdog device\n" },
	{ 2, wdt_get_support, "%d) Get watchdog support\n" },
	{ 3, wdt_get_bootstatus, "%d) Get boot status\n" },
	{ 4, wdt_get_timeleft, "%d) Get watchdog timeleft\n" },
	{ 5, wdt_get_timeout, "%d) Get watchdog timeout\n" },
	{ 6, wdt_ping, "%d) Ping watchdog\n" },
	{ 7, wdt_set_watchdogdevice, "%d) Set watchdog device (%s)\n" },
	{ 8, wdt_set_timeout, "%d) Set watchdog timeout\n" },
	{ 9, wdt_set_pingmode, "%d) Set ping mode (%d)\n" },
	{ 10, wdt_set_pingtime, "%d) Set ping time (%d)\n" },
	{ 11, wdt_set_pingtimes, "%d) Set ping times (%d)\n" },
	{ 12, wdt_exit, "%d) Exit\n" }
};

static int items = sizeof(main_menu) / sizeof(struct menu);
static int ping_items = 0;

int fd = 0;
int opt = 0;
int pingmode = DEFAULT_PINGMODE;
int pingtime = DEFAULT_PINGTIME;
int pingtimes = DEFAULT_PINGTIMES;
int exit_flag = 0;
char device[PATH_MAX] = DEFAULT_WATCHDOG;

#define ITEM(fmt, args...) { printf(fmt, ##args); }

#define MENU()\
{\
	int index = 0;\
	for (index = 0; index != items ; index++)\
	{\
		if (main_menu[index].func == wdt_set_watchdogdevice)\
			ITEM(main_menu[index].desc, index, device)\
		else if (main_menu[index].func == wdt_set_pingmode)\
			ITEM(main_menu[index].desc, index, pingmode)\
		else if (main_menu[index].func == wdt_set_pingtime)\
			ITEM(main_menu[index].desc, index, pingtime)\
		else if (main_menu[index].func == wdt_set_pingtimes)\
			ITEM(main_menu[index].desc, index, pingtimes)\
		else\
			ITEM(main_menu[index].desc, index)\
	}\
	printf("\nSelect: ");\
}

#define PING_MODE_MENU()\
{\
	int index = 0;\
	printf("%d) Use write to ping watchdog\n", index);\
	printf("%d) Use ioctl to ping watchdog\n", ++index);\
	printf("%d) Use write to ping watchdog without closing device after pinging\n", ++index);\
	printf("%d) Use ioctl to ping watchdog without closing device after pinging\n", ++index);\
	ping_items = ++index; \
}

enum PING_MODE
{
	PING_MODE_WRITE = 0,
	PING_MODE_IOCTL,
	PING_MODE_WRITE_WITHOUT_CLOSE,
	PING_MODE_IOCTL_WITHOUT_CLOSE
};

static struct option long_options[] =
{
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

static void wdt_opendevice(void)
{
	fd = open(device, O_WRONLY);

	if (fd == -1)
		perror("watchdog");
}

static void wdt_closedevice(void)
{
	if (write(fd, "V", 1) != -1)
	{
		close(fd);
		fd = 0;
	}
}

void wdt_disablewatchdog(void)
{
	int flags = WDIOS_DISABLECARD;

	if (ioctl(fd, WDIOC_SETOPTIONS, &flags))
		printf("Fail to disable watchdog\n\n");
	else
		printf("Succeed to disable watchdog\n\n");
}

void wdt_enablewatchdog(void)
{
	int flags = WDIOS_ENABLECARD;

	if (ioctl(fd, WDIOC_SETOPTIONS, &flags))
		printf("Fail to enable watchdog\n\n");
	else
		printf("Succeed to enable watchdog\n\n");
}

void wdt_get_support(void)
{
	struct watchdog_info wdt_info;

	wdt_opendevice();

	memset(&wdt_info, 0, sizeof(struct watchdog_info));

	if (ioctl(fd, WDIOC_GETSUPPORT, &wdt_info))
	{
		printf("Fail to get support on watchdog device: %s\n", device);
	}
	else
	{
		printf("identity: %s\n", wdt_info.identity);
		printf("identity: %d\n", wdt_info.firmware_version);
		printf("options: %d\n\n", wdt_info.options);

		if (wdt_info.options & WDIOF_OVERHEAT)
			printf("\tOVERHEAT\n");

		if (wdt_info.options & WDIOF_FANFAULT)
			printf("\tFANFAULT\n");

		if (wdt_info.options & WDIOF_EXTERN1)
			printf("\tEXTERN1\n");

		if (wdt_info.options & WDIOF_EXTERN2)
			printf("\tEXTERN2\n");

		if (wdt_info.options & WDIOF_POWERUNDER)
			printf("\tPOWERUNDER\n");

		if (wdt_info.options & WDIOF_CARDRESET)
			printf("\tCARDRESET\n");

		if (wdt_info.options & WDIOF_POWEROVER)
			printf("\tPOWEROVER\n");

		if (wdt_info.options & WDIOF_SETTIMEOUT)
			printf("\tSETTIMEOUT\n");

		if (wdt_info.options & WDIOF_MAGICCLOSE)
			printf("\tMAGICCLOSE\n");

		if (wdt_info.options & WDIOF_PRETIMEOUT)
			printf("\tPRETIMEOUT\n");

		if (wdt_info.options & WDIOF_ALARMONLY)
			printf("\tALARMONLY\n");

		if (wdt_info.options & WDIOF_KEEPALIVEPING)
			printf("\tKEEPALIVEPING\n");

		printf("\n");
	}

	wdt_closedevice();
}

void wdt_get_bootstatus(void)
{
	int status = 0;

	wdt_opendevice();

	if (ioctl(fd, WDIOC_GETBOOTSTATUS, &status))
	{
		printf("Fail to get boot status\n\n");
	}
	else
	{
		printf("Succeed to get boot status: %d\n\n", status);

		if (status & WDIOF_OVERHEAT)
			printf("\tOVERHEAT\n");

		if (status & WDIOF_FANFAULT)
			printf("\tFANFAULT\n");

		if (status & WDIOF_EXTERN1)
			printf("\tEXTERN1\n");

		if (status & WDIOF_EXTERN2)
			printf("\tEXTERN2\n");

		if (status & WDIOF_POWERUNDER)
			printf("\tPOWERUNDER\n");

		if (status & WDIOF_CARDRESET)
			printf("\tCARDRESET\n");

		if (status & WDIOF_POWEROVER)
			printf("\tPOWEROVER\n");

		if (status & WDIOF_SETTIMEOUT)
			printf("\tSETTIMEOUT\n");

		if (status & WDIOF_MAGICCLOSE)
			printf("\tMAGICCLOSE\n");

		if (status & WDIOF_PRETIMEOUT)
			printf("\tPRETIMEOUT\n");

		if (status & WDIOF_ALARMONLY)
			printf("\tALARMONLY\n");

		if (status & WDIOF_KEEPALIVEPING)
			printf("\tKEEPALIVEPING\n");

		printf("\n");
	}

	wdt_closedevice();
}

void wdt_get_timeleft(void)
{
	int timeleft = 0;

	if (ioctl(fd, WDIOC_GETTIMELEFT, &timeleft))
		printf("Fail to get timeleft\n\n");
	else
		printf("Succeed to get timeleft: %d\n\n", timeleft);
}

void wdt_get_timeout(void)
{
	int timeout = 0;

	wdt_opendevice();

	if (ioctl(fd, WDIOC_GETTIMEOUT, &timeout))
		printf("Fail to get timeout\n\n");
	else
		printf("Succeed to get timeout: %d\n\n", timeout);

	wdt_closedevice();
}

void wdt_ping(void)
{
	int dummy = 0;
	int count = 0;
	int ret = 0;

	wdt_opendevice();

	while (1)
	{
		switch (pingmode)
		{
			case PING_MODE_WRITE:
			case PING_MODE_WRITE_WITHOUT_CLOSE:
				ret = write(fd, "\0", 1);
				break;
			case PING_MODE_IOCTL:
			case PING_MODE_IOCTL_WITHOUT_CLOSE:
				ret = ioctl(fd, WDIOC_KEEPALIVE, &dummy);
				break;
		}

		if (ret != -1)
			printf("Succeed to ping watchdog device: %s\n\n", device);
		else
			printf("Fail to ping watchdog device: %s\n\n", device);

		if (pingtimes > 0 && ++count >= pingtimes)
			break;

		sleep(pingtime);
	}

	if (pingmode == PING_MODE_WRITE || pingmode == PING_MODE_IOCTL)
		wdt_closedevice();
	else
		printf("*** WARNING: System will reboot after watchdog timeout ***\n\n");
}

void wdt_set_watchdogdevice(char *path)
{
	memset(device, 0, PATH_MAX);
	memcpy(device, path, strlen(path));
	printf("Set watchdog device: %s\n\n", path);
}

void wdt_set_timeout(int timeout)
{
	wdt_opendevice();

	if (ioctl(fd, WDIOC_SETTIMEOUT, &timeout))
		printf("Fail to set timeout\n\n");
	else
		printf("Succeed to set timeout\n\n");

	wdt_closedevice();
}

void wdt_set_pingmode(void)
{
	char input[4] = {0};
	int item = 0;

	PING_MODE_MENU();
	fflush(stdin);
	printf("\nNew ping mode: ");

	if (scanf("%s", input))
		item = atoi(input);

	if (item >= 0 && item < ping_items)
	{
		pingmode = item;
		printf("Set ping mode: %d\n\n", item);
	}

	fflush(stdin);
}

void wdt_set_pingtime(int second)
{
	pingtime = second;
	printf("Set ping time: %d\n\n", pingtime);
}

void wdt_set_pingtimes(int count)
{
	pingtimes = count;
	printf("Set ping times: %d\n\n", pingtimes);
}

void wdt_exit(void)
{
	if (fd > 0)
		wdt_closedevice();

	exit_flag = 1;
}

int main(int argc, char *const *argv)
{
	while (1)
	{
		int option_index = 0;

		opt = getopt_long(argc, argv, "h", long_options, &option_index);

		if (opt == -1)
			break;

		switch (opt)
		{
			case 'h':
			case '?':
			default:
				SHOW_HELP(argv[0]);
		}
	}

	while (!exit_flag)
	{
		char input[4] = {0};
		int item = 0;

		MENU();
		fflush(stdin);

		if (scanf("%s", input))
			item = atoi(input);
			
		fflush(stdin);

		if (item >= 0 && item < items)
		{
			char param[PATH_MAX] = {0};

			if (main_menu[item].func == wdt_set_watchdogdevice)
			{
				printf("New device: ");

				if (scanf("%s", param))
					main_menu[item].func(param);
			}
			else if (main_menu[item].func == wdt_set_timeout)
			{
				printf("New timeout: ");

				if (scanf("%s", param))
					main_menu[item].func(atoi(param));
			}
			else if (main_menu[item].func == wdt_set_pingtime)
			{
				printf("New ping time: ");

				if (scanf("%s", param))
					main_menu[item].func(atoi(param));
			}
			else if (main_menu[item].func == wdt_set_pingtimes)
			{
				printf("New ping times: ");

				if (scanf("%s", param))
					main_menu[item].func(atoi(param));
			}
			else
			{
				main_menu[item].func();
			}
		}

		fflush(stdin);
	}

	return 0;
}
