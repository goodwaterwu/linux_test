#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <linux/limits.h>
#include <asm-generic/errno-base.h>

#define DEFAULT_SPIDEV "/dev/spidev0.1"

#define SPI_IOC_RD_MODE			0x80016b01
#define SPI_IOC_RD_LSB_FIRST		0x80016b02
#define SPI_IOC_RD_BITS_PER_WORD	0x80016b03
#define SPI_IOC_RD_MAX_SPEED_HZ		0x80046b04
#define SPI_IOC_WR_MODE			0x40016b01
#define SPI_IOC_WR_LSB_FIRST		0x40016b02
#define SPI_IOC_WR_BITS_PER_WORD	0x40016b03
#define SPI_IOC_WR_MAX_SPEED_HZ		0x40046b04
#define SPI_IOC_EEPROM_READ		0xc0046b05
#define SPI_IOC_FLASHID_READ		0xc0046b06
#define SPI_IOC_EEPROM_WRITE		0xc0046b07

enum SPIDEV_MENU_ITEM {
	SPIDEV_MENU_ITEM_RD_MODE = 0x0,
	SPIDEV_MENU_ITEM_RD_LSB_FIRST,
	SPIDEV_MENU_ITEM_RD_BITS_PER_WORD,
	SPIDEV_MENU_ITEM_RD_MAX_SPEED_HZ,
	SPIDEV_MENU_ITEM_WR_MODE,
	SPIDEV_MENU_ITEM_WR_LSB_FIRST,
	SPIDEV_MENU_ITEM_WR_BITS_PER_WORD,
	SPIDEV_MENU_ITEM_WR_MAX_SPEED_HZ,
	SPIDEV_MENU_ITEM_EEPROM_READ,
	SPIDEV_MENU_ITEM_FLASHID_READ,
	SPIDEV_MENU_ITEM_EEPROM_WRITE,
	SPIDEV_MENU_ITEM_EXIT,
	SPIDEV_MENU_ITEM_NUMBER
};

#define SHOW_HELP(name)\
{\
	printf("Usage: %s [OPTIONS]\n", name);\
	printf("Test spidev.\n");\
	printf("\t-n <device>, --name=<device>\tspidev device\n");\
	printf("\t-h, --help\tshow this help\n\n");\
	exit(EXIT_SUCCESS);\
}

struct option long_options[] = {
	{"name", required_argument, 0, 'n'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

static unsigned int show_menu(void)
{
	unsigned int select;

	while (1) {
		printf("%d) read spi mode\n", SPIDEV_MENU_ITEM_RD_MODE);
		printf("%d) read lsb first\n", SPIDEV_MENU_ITEM_RD_LSB_FIRST);
		printf("%d) read bits per word\n", SPIDEV_MENU_ITEM_RD_BITS_PER_WORD);
		printf("%d) read max speed (Hz)\n", SPIDEV_MENU_ITEM_RD_MAX_SPEED_HZ);
		printf("%d) write spi mode\n", SPIDEV_MENU_ITEM_WR_MODE);
		printf("%d) write lsb first\n", SPIDEV_MENU_ITEM_WR_LSB_FIRST);
		printf("%d) write bits per word\n", SPIDEV_MENU_ITEM_WR_BITS_PER_WORD);
		printf("%d) write max speed (Hz)\n", SPIDEV_MENU_ITEM_WR_MAX_SPEED_HZ);
		printf("%d) eeprom read\n", SPIDEV_MENU_ITEM_EEPROM_READ);
		printf("%d) read flash id\n", SPIDEV_MENU_ITEM_FLASHID_READ);
		printf("%d) eeprom write\n", SPIDEV_MENU_ITEM_EEPROM_WRITE);
		printf("%d) exit\n", SPIDEV_MENU_ITEM_EXIT);
		printf("select: ");
		scanf("%u", &select);
		fflush(stdin);

		if (select >= 0 && select < SPIDEV_MENU_ITEM_NUMBER)
			break;
	}

	return select;
}

static int spidev_read_mode(int fd, unsigned char *mode)
{
	return ioctl(fd, SPI_IOC_RD_MODE, mode);
}

static int spidev_read_lsb_first(int fd, unsigned char *first)
{
	return ioctl(fd, SPI_IOC_RD_LSB_FIRST, first);
}

static int spidev_read_bits_per_word(int fd, unsigned char *bits_per_word)
{
	return ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, bits_per_word);
}

static int spidev_read_max_speed_hz(int fd, unsigned int *hz)
{
	return ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, hz);
}

static int spidev_write_mode(int fd)
{
	unsigned int mode;

	printf("spi mode: \n");
	scanf("%u", &mode);

	if (mode >= 0 && mode <= 3) {
		printf("set spi mode to %u\n", mode);
	} else {
		printf("unavailable spi mode\n");
		return -EINVAL;
	}

	return ioctl(fd, SPI_IOC_WR_MODE, &mode);
}

static int spidev_write_lsb_first(int fd)
{
	unsigned int first;

	printf("lsb first: \n");
	scanf("%u", &first);

	if (first == 0 || first == 1) {
		printf("set lsb first to %u\n", first);
	} else {
		printf("unavailable lsb first\n");
		return -EINVAL;
	}

	return ioctl(fd, SPI_IOC_WR_LSB_FIRST, &first);
}

static int spidev_write_bits_per_word(int fd)
{
	unsigned int bits_per_word;

	printf("bits per word: \n");
	scanf("%u", &bits_per_word);

	if (bits_per_word == 8 || bits_per_word == 16) {
		printf("set bits per word to %u\n", bits_per_word);
	} else {
		printf("unavailable bits per word\n");
		return -EINVAL;
	}

	return ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
}

static int spidev_write_max_speed_hz(int fd)
{
	unsigned int hz;

	printf("max speed (Hz): \n");
	scanf("%u", &hz);

	if (!(hz % 1000000)) {
		printf("set max speed hz to %u\n", hz);
	} else {
		printf("unavailable max speed hz\n");
		return -EINVAL;
	}

	return ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &hz);
}

static int spidev_eeprom_read(int fd, unsigned char *rx)
{
	return ioctl(fd, SPI_IOC_EEPROM_READ, rx);
}

static int spidev_flashid_read(int fd, unsigned char *id)
{
	return ioctl(fd, SPI_IOC_FLASHID_READ, id);
}

static int spidev_eeprom_write(int fd, unsigned char *tx)
{
	return ioctl(fd, SPI_IOC_EEPROM_WRITE, tx);
}

int main(int argc, char *const *argv)
{
	int ret = 0;
	int fd = 0;
	int opt = 0;
	char device[PATH_MAX] = DEFAULT_SPIDEV;

	while (1) {
		int option_index = 0;

		opt = getopt_long(argc, argv, "hn:", long_options, &option_index);
		if (opt == -1)
			break;

		switch (opt) {
		case 'n':
			memset(device, 0, PATH_MAX);
			memcpy(device, optarg, strlen(optarg));
			break;
		case 'h':
		case '?':
		default:
			SHOW_HELP(argv[0]);
		}
	}

	fd = open(device, O_WRONLY);

	if (fd == -1) {
		perror("spidev");
		exit(EXIT_FAILURE);
	}

	while (1) {
		unsigned int select = show_menu();
		unsigned int hz = 0;
		unsigned char buffer[5];

		memset(buffer, 0, sizeof(buffer));
		switch (select) {
		case SPIDEV_MENU_ITEM_RD_MODE:
			ret = spidev_read_mode(fd, &buffer[0]);
			if (!ret)
				printf("##### spi mode: %u #####\n", buffer[0]);
			break;
		case SPIDEV_MENU_ITEM_RD_LSB_FIRST:
			ret = spidev_read_lsb_first(fd, &buffer[0]);
			if (!ret)
				printf("##### lsb first: %u #####\n", buffer[0]);
			break;
		case SPIDEV_MENU_ITEM_RD_BITS_PER_WORD:
			ret = spidev_read_bits_per_word(fd, &buffer[0]);
			if (!ret)
				printf("##### bits per word: %u ######\n", buffer[0]);
			break;
		case SPIDEV_MENU_ITEM_RD_MAX_SPEED_HZ:
			ret = spidev_read_max_speed_hz(fd, &hz);
			if (!ret)
				printf("##### max speed (Hz): %u #####\n", hz);
			break;
		case SPIDEV_MENU_ITEM_WR_MODE:
			ret = spidev_write_mode(fd);
			break;
		case SPIDEV_MENU_ITEM_WR_LSB_FIRST:
			ret = spidev_write_lsb_first(fd);
			break;
		case SPIDEV_MENU_ITEM_WR_BITS_PER_WORD:
			ret = spidev_write_bits_per_word(fd);
			break;
		case SPIDEV_MENU_ITEM_WR_MAX_SPEED_HZ:
			ret = spidev_write_max_speed_hz(fd);
			break;
		case SPIDEV_MENU_ITEM_EEPROM_READ:
			buffer[0] = 0x5;
			ret = spidev_eeprom_read(fd, buffer);
			if (!ret)
				printf("##### sr: %#x #####\n", buffer[0]);
			break;
		case SPIDEV_MENU_ITEM_FLASHID_READ:
			buffer[0] = 0x9f;
			ret = spidev_flashid_read(fd, buffer);
			if (!ret)
				printf("##### id: %#x %#x %#x #####\n", buffer[0], buffer[1], buffer[2]);
			break;
		case SPIDEV_MENU_ITEM_EEPROM_WRITE:
			printf("write enable\n");
			buffer[0] = 0x6;
			ret = spidev_eeprom_write(fd, buffer);
			break;
		case SPIDEV_MENU_ITEM_EXIT:
			goto exit;
			break;
		}

		if (ret)
			break;
	}

exit:
	close(fd);

	return ret;
}
