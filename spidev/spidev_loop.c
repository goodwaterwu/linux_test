#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/limits.h>
#include <linux/types.h>
#include <asm-generic/errno-base.h>
#include <linux/spi/spidev.h>
#include <linux/version.h>
#include <linux/types.h>

#define DEFAULT_SPIDEV "/dev/spidev0.1"
#define SZ_3K 0xc00

int spidev_read_3k(int fd)
{
	struct spi_ioc_transfer transfer[2];
	static int odd = 0;
	FILE *file;
	unsigned char buf[SZ_3K];

	memset(transfer, 0, sizeof(transfer));
	memset(buf, 0, SZ_3K);
	buf[0] = 0xb; /* fast read command */
	buf[1] = 0x0; /* 1st byte of the flash offset */
	buf[2] = 0x0; /* 2nd byte of the flash offset */
	buf[3] = 0x0; /* 3rd byte of the flash offset */
	buf[4] = 0x0; /* dummy byte according to the flash spec. */
	transfer[0].tx_buf = (__u64)buf;
	transfer[0].len = 5;

	transfer[1].rx_buf = (__u64)buf;
	transfer[1].len = SZ_3K;

	if (ioctl(fd, SPI_IOC_MESSAGE(2), &transfer) < 0) {
		perror("can't send spi message");
		return -errno;
	}

	if (odd)
		file = fopen("/tmp/spi_3k_1", "w");
	else
		file = fopen("/tmp/spi_3k_2", "w");

	odd = (odd + 1) % 2;

	if (file) {
		fwrite(buf, SZ_3K, 1, file);
		fclose(file);
		printf("read 3k succeeded\n");
		if (!system("cmp /tmp/spi_3k_1 /tmp/spi_3k_2 > /dev/tty"))
			exit(EXIT_FAILURE);
	} else {
		perror("read 3k failed");
		return -errno;
	}

	return 0;
}

int main(int argc, char *const *argv)
{
	int ret = 0;
	int fd = 0;
	char device[PATH_MAX] = DEFAULT_SPIDEV;

	fd = open(device, O_WRONLY);

	if (fd == -1) {
		perror("spidev");
		exit(EXIT_FAILURE);
	}

	while (1) {
		ret = spidev_read_3k(fd);
		if (ret)
			break;
		sleep(1);
	}

	close(fd);

	return ret;
}
