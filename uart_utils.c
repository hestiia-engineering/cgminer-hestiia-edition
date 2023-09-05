#include "uart_utils.h"
#include <fcntl.h>
#include <unistd.h>
#include "util.h"

#define SRV_TIMEOUT 2

#define MAX_UART_DEVICES 3
#define UART_DEVICE_ENABLED 1

// transformer ca sous forme d'un tableau de char
const char *uart1_device_name = "/dev/ttyS1";
const char *uart2_device_name = "/dev/ttyS2";
const char *uart3_device_name = "/dev/ttyS3";

const char *uart_device_names[MAX_UART_DEVICES] = {
	"/dev/ttyS1",
	"/dev/ttyS2",
	"/dev/ttyS3",
};

const char *gpio_chip[MAX_UART_DEVICES] = {
	"gpiochip1",
	"gpiochip2",
	"gpiochip3",
};

const int gpio_line_offset[MAX_UART_DEVICES] = {
	28,
	29,
	30,
};

#define MAX_UART_DEVICES 3

int8_t __attribute__((optimize("O2")))uart_init(struct S_UART_DEVICE *s_device, char *uart_device_name, uint32_t speed)
{
	s_device->name = uart_device_name;
	s_device->fd = -1;
	s_device->speed = speed;

	int fd;
	if ((fd = open(s_device->name, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to open device [%s]: %s",
			   __func__, s_device->name, strerror(errno));
	}

	s_device->fd = fd;

	if (fcntl(fd, F_SETFL, 0) < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to set descriptor status flags [%s]: %s",
			   __func__, s_device->name, strerror(errno));
		close(fd);
		return -1;
	}

	if (tcgetattr(fd, &s_device->settings) < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to get device attributes [%s]: %s",
			   __func__, s_device->name, strerror(errno));
		close(fd);
		return -1;
	}

	s_device->settings.c_cflag &= ~PARENB; /* no parity */
	s_device->settings.c_cflag &= ~CSTOPB; /* 1 stop bit */
	s_device->settings.c_cflag &= ~CSIZE;
	s_device->settings.c_cflag |= CS8 | CLOCAL | CREAD; /* 8 bits */
	s_device->settings.c_cc[VMIN] = 1;
	s_device->settings.c_cc[VTIME] = 2;
	s_device->settings.c_lflag = ICANON;  /* canonical mode */
	s_device->settings.c_oflag &= ~OPOST; /* raw output */

	if (cfsetospeed(&s_device->settings, speed) < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to set device output speed [%s]: %s",
			   __func__, s_device->name, strerror(errno));
		close(fd);
		return -1;
	}

	if (cfsetispeed(&s_device->settings, speed) < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to set device input speed [%s]: %s",
			   __func__, s_device->name, strerror(errno));
		close(fd);
		return -1;
	}

	if (tcsetattr(fd, TCSANOW, &s_device->settings) < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to get device attributes [%s]: %s",
			   __func__, s_device->name, strerror(errno));
		close(fd);
		return -1;
	}

	if (tcflush(fd, TCOFLUSH) < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to flush device data [%s]: %s",
			   __func__, s_device->name, strerror(errno));
		close(fd);
		return -1;
	}

	return 0;
}

/**
 * @brief
 *
 * @param s_device
 * @param buf
 * @param bufsiz
 * @param processed
 */
void uart_write(struct S_UART_DEVICE *s_device, char *buf, size_t bufsiz, int *processed)
{
	*processed = 0;

	if (s_device->fd == -1)
	{
		applog(LOG_ERR, "BM1397: %s() device [%s] is not initialized",
			   __func__, s_device->name);
	}
	if (buf == NULL)
	{
		applog(LOG_ERR, "BM1397: %s() buffer is NULL",
			   __func__);
	}

	int sent = write(s_device->fd, buf, bufsiz);
	if (sent < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to write to device [%s]: %s",
			   __func__, s_device->name, strerror(errno));
	}
	applog(LOG_ERR,"[UART_WRITE] Sent %d bytes to device %s -> %s", sent, s_device->name, buf);
	*processed += sent;
}

/**
 * @brief
 *
 * @param s_device
 * @param buf
 * @param bufsiz
 * @param processed
 */
void uart_read(struct S_UART_DEVICE *s_device, char *buf, size_t bufsiz, int *processed)
{

	*processed = 0;
	if (s_device->fd == -1)
	{
		applog(LOG_ERR, "BM1397: %s() device [%s] is not initialized",
			   __func__, s_device->name);
	}
	if (buf == NULL)
	{
		applog(LOG_ERR, "BM1397: %s() buffer is NULL",
			   __func__);
	}

	int readed = read(s_device->fd, buf, bufsiz);
	if (readed < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to read from device [%s]: %s",
			   __func__, s_device->name, strerror(errno));
	}
	applog(LOG_ERR,"[UART_READ] Received %d bytes from device %s", readed, s_device->name);
	*processed += readed;
}

struct cgpu_info *uart_alloc_cgpu(struct device_drv *drv, int threads)
{
	struct cgpu_info *cgpu = cgcalloc(1, sizeof(*cgpu));

	cgpu->drv = drv;
	cgpu->deven = DEV_ENABLED;
	cgpu->threads = threads;
	// cglock_init(&cgpu->usbinfo.devlock);
	return cgpu;
}

/**
 * @brief
 *
 *
 * @param device_detect function pointer
 * @param single if set, will only initialise one driver
 */
void __uart_detect(struct cgpu_info *(*device_detect)(const char *uart_device_names, const char *gpio_chip, int gpio_line, int device_number), bool single)
{
	ssize_t count, i;
	struct cgpu_info *cgpu;

	for (i = 0; i < UART_DEVICE_ENABLED; i++)
	{
		bool new_dev = false;
		cgpu = device_detect(uart_device_names[i], gpio_chip[i], gpio_line_offset[i], (i + 1));
		if (!cgpu)
		{
			// TODO deals with errors and mutex if needed
			asm volatile("nop");
			// cgminer_usb_unlock(drv, list[i]);
		}
		else
		{
			new_dev = true;
			applog(LOG_ERR, "New BM1397: %d device on uart %s", i, uart_device_names[i]);
		}
		if (single && new_dev)
			break;
	}
}

void uart_release(struct S_UART_DEVICE *s_device)
{
	close(s_device->fd);
}


void uart_set_speed(struct S_UART_DEVICE *s_device,uint32_t speed ) {

	// check if fd is not initialized
	if (s_device->fd == -1)
	{
		applog(LOG_ERR, "BM1397: %s() device [%s] is not initialized",
			   __func__, s_device->name);
	}

	s_device->speed = speed;
	if (cfsetospeed(&s_device->settings, speed) < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to set device output speed [%s]: %s",
			   __func__, s_device->name, strerror(errno));
	}

	if (cfsetispeed(&s_device->settings, speed) < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to set device input speed [%s]: %s",
			   __func__, s_device->name, strerror(errno));
	}
}

void uart_flush(struct S_UART_DEVICE *s_device) {

	// check if fd is not initialized
	if (s_device->fd == -1)
	{
		applog(LOG_ERR, "BM1397: %s() device [%s] is not initialized",
			   __func__, s_device->name);
	}

	if (tcflush(s_device->fd, TCIFLUSH) < 0)
	{
		applog(LOG_ERR, "BM1397: %s() failed to flush device data [%s]: %s",
			   __func__, s_device->name, strerror(errno));
	}
}