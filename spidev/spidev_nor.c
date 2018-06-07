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

/*
 * This is an example of reading the id of a spi nor flash by full-duplex
 * spi operations. However, in fact, the spi nor flash is a half-duplex
 * device, so this example is just for reference.
 */
int spidev_read_id(int fd)
{
	struct spi_ioc_transfer transfer[2];
	unsigned char first_buf[1];
	unsigned char second_buf[5];

	printf("%s()\n", __FUNCTION__);
	memset(transfer, 0, sizeof(transfer));
	memset(first_buf, 0, 1);
	memset(second_buf, 0, 5);
	first_buf[0] = 0x9f; /* read id command */
	/* read id command to a spi nor flash */
	transfer[0].tx_buf = (__u64)first_buf;
	/*
	 * while sending read id command to spi, it will receive a datum
	 * at the same time. the received datum is meaningless because
	 * the spi nor flash is a half-duplex device. if we don't assign
	 * a rx_buf to first transfer here, kernel will assign a dummy
	 * buffer to itself.
	 */
	transfer[0].rx_buf = (__u64)first_buf;
	transfer[0].len = 1;

	/*
	 * to receive the flash id data, we must send dummy tx to spi.
	 * if we don't assign tx_buf to second transfer here, kernel will
	 * assign a dummy buffer to itself.
	 */
	transfer[1].tx_buf = (__u64)second_buf;
	/* receive flash id data from spi */
	transfer[1].rx_buf = (__u64)second_buf;
	transfer[1].len = 5;

	if (ioctl(fd, SPI_IOC_MESSAGE(2), &transfer) < 0) {
		perror("can't send spi message");
		return -errno;
	}

	printf("this is received data of first transfer\n");
	printf("first_buf[0]: %#x\n", first_buf[0]);

	printf("this is received data of second transfer\n");
		printf("flash id: %02x %02x %02x %02x %02x\n", second_buf[0], second_buf[1], second_buf[2], second_buf[3], second_buf[4]);

	return 0;
}

/*
 * This is an example of reading the id of a spi nor flash by full-duplex
 * spi operations which integrates tx/rx into one transfer. However, in fact,
 * the spi nor flash is a half-duplex device, so this example is just for
 * reference.
 */
int spidev_read_id_2(int fd)
{
	struct spi_ioc_transfer transfer[1];
	unsigned char buf[6];

	printf("%s()\n", __FUNCTION__);
	memset(transfer, 0, sizeof(transfer));
	memset(buf, 0, 6);
	buf[0] = 0x9f; /* read id command */
	/* read id command to a spi nor flash */
	transfer[0].tx_buf = (__u64)buf;
	/* integrate tx/rx into one transfer */
	transfer[0].rx_buf = (__u64)buf;
	transfer[0].len = 6;

	if (ioctl(fd, SPI_IOC_MESSAGE(1), &transfer) < 0) {
		perror("can't send spi message");
		return -errno;
	}

	printf("flash id: %02x %02x %02x %02x %02x\n", buf[1], buf[2], buf[3], buf[4], buf[5]);

	return 0;
}

/*
 * This is an example for reading data from a spi nor flash.
 */
int spidev_read_3k(int fd)
{
	struct spi_ioc_transfer transfer[2];
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

	file = fopen("/tmp/spi_3k", "w");

	if (file) {
		fwrite(buf, SZ_3K, 1, file);
		fclose(file);
		printf("read 3k succeeded\n");
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

	if (!spidev_read_id(fd)) {
		if (!spidev_read_id_2(fd))
			ret = spidev_read_3k(fd);
	}

	close(fd);

	return ret;
}
