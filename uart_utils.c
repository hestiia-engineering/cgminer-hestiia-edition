#include "uart_utils.h"
#include <fcntl.h>
#include <unistd.h>
#include "miner.h"

#define SRV_TIMEOUT 2

#define MAX_UART_DEVICES 3

// transformer ca sous forme d'un tableau de char
const char *uart1_device_name = "/dev/ttyS1";
const char *uart2_device_name = "/dev/ttyS2";
const char *uart3_device_name = "/dev/ttyS3";

const char *uart_device_names[MAX_UART_DEVICES] = {
	"/dev/ttyS1",
	"/dev/ttyS2",
	"/dev/ttyS3",
};

#define MAX_UART_DEVICES 3

int8_t uart_init(struct S_UART_DEVICE *s_device, char* uart_device_name, uint32_t speed)
{
	s_device->name = uart_device_name;
	s_device->speed = speed;

	int fd;
	if ((fd = open(s_device->name, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
		quit(1, "BM1397: %s() failed to open device [%s]: %s",
			__func__, s_device->name, strerror(errno));

	s_device->fd = fd;

	if (fcntl(fd, F_SETFL, 0) < 0)
		quit(1, "BM1397: %s() failed to set descriptor status flags [%s]: %s",
				__func__, s_device->name, strerror(errno));

	if (tcgetattr(fd, &s_device->settings) < 0)
		quit(1, "BM1397: %s() failed to get device attributes [%s]: %s",
				__func__, s_device->name, strerror(errno));

	s_device->settings.c_cflag &= ~PARENB; /* no parity */
	s_device->settings.c_cflag &= ~CSTOPB; /* 1 stop bit */
	s_device->settings.c_cflag &= ~CSIZE;
	s_device->settings.c_cflag |= CS8 | CLOCAL | CREAD; /* 8 bits */
	s_device->settings.c_cc[VMIN] = 1;
	s_device->settings.c_cc[VTIME] = 2;
	s_device->settings.c_lflag = ICANON; /* canonical mode */
	s_device->settings.c_oflag &= ~OPOST; /* raw output */

	if (cfsetospeed(&s_device->settings, speed) < 0)
		quit(1, "BM1397: %s() failed to set device output speed [%s]: %s",
				__func__, s_device->name, strerror(errno));

	if (cfsetispeed(&s_device->settings, speed) < 0)
		quit(1, "BM1397: %s() failed to set device input speed [%s]: %s",
				__func__, s_device->name, strerror(errno));

	if (tcsetattr(fd, TCSANOW, &s_device->settings) < 0)
		quit(1, "BM1397: %s() failed to get device attributes [%s]: %s",
				__func__, s_device->name, strerror(errno));

	if (tcflush(fd, TCOFLUSH) < 0)
		quit(1, "BM1397: %s() failed to flush device data [%s]: %s",
				__func__, s_device->name, strerror(errno));

	return 0;
}

/* TODO: add errors processing */
static int read_to(int fd, uint8_t* buffer, int len, int timeout)
{
	fd_set readset;
	int result;
	struct timeval tv;

	FD_ZERO(&readset);
	FD_SET(fd, &readset);

	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	result = select(fd + 1, &readset, NULL, NULL, &tv);
 
	if (result < 0)
		return result;
	else if (result > 0 && FD_ISSET(fd, &readset)) {
		result = read(fd, buffer, len);
		return result;
	}
	return -2;
}


void uart_write(struct cgpu_info *cgpu, char *buf, size_t bufsiz, int *processed) {
	S_UART_DEVICE *s_device = cgpu_info->;

}
static int write_to(int fd, uint8_t* buffer, int len, int timeout)
{
	fd_set writeset;
	int result;
	struct timeval tv;

	FD_ZERO(&writeset);
	FD_SET(fd, &writeset);

	tv.tv_sec = timeout;
	tv.tv_usec = 0;
	result = select(fd + 1, NULL, &writeset, NULL, &tv);

	if (result < 0)
		return result;
	else if (result > 0 && FD_ISSET(fd, &writeset)) {
		result = write(fd, buffer, len);
		return result;
	}

	return -2;
}

int8_t uart_transfer(S_UART_DEVICE *attr)
{
	int ret;

	if ((ret = write_to(attr->fd, attr->tx, attr->datalen, SRV_TIMEOUT)) < 1) {
		applog(LOG_ERR, "BM1397: %s() failed to send UART message to [%s]: %s",
				__func__, attr->device, strerror(errno));

		attr->datalen = 0;
		return -1;
	}

	if ((ret = read_to(attr->fd, attr->rx, attr->size, SRV_TIMEOUT)) < 1) {
		applog(LOG_ERR, "BM1397: %s() failed to read UART message from [%s]: %s",
				__func__, attr->device, strerror(errno));

		attr->datalen = 0;
		return -1;
	}

	attr->datalen = ret;
	return 0;
}

struct cgpu_info *uart_alloc_cgpu(struct device_drv *drv, int threads){
	struct cgpu_info *cgpu = cgcalloc(1, sizeof(*cgpu));

	cgpu->drv = drv;
	cgpu->deven = DEV_ENABLED;
	cgpu->threads = threads;
	//cglock_init(&cgpu->usbinfo.devlock);
	return cgpu;
}

/**
 * @brief 
 * 
 * 
 * @param device_detect 
 * @param single 
 */
void __uart_detect(struct cgpu_info *(*device_detect)(struct uart_device_t *uart_device), bool single)
{
	ssize_t count, i;
	struct cgpu_info *cgpu;

	for (i = 0; i < MAX_UART_DEVICES; i++) {
		bool new_dev = false;
		cgpu = device_detect(&uart_devices[i]);
		if (!cgpu) {
			//TODO deals with errors and mutex if needed
			asm volatile("nop");
			//cgminer_usb_unlock(drv, list[i]);
		}
		else {
			new_dev = true;
		}
		if (single && new_dev)
			break;
	}
}

void uart_release(S_UART_DEVICE *attr)
{
	free(attr->rx);
	free(attr->tx);
	close(attr->fd);
}
