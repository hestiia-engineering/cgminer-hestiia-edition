#ifndef UART_UTILS_H
#define UART_UTILS_H
#include <stdbool.h>
#include <stdint.h>
#include <termios.h>
#include <errno.h>


#define UART_BUFFER_SIZE 256
struct S_UART_DEVICE {
	char *name;
	int *fd;
	uint32_t speed;
	struct termios settings;
};

int8_t uart_init(struct S_UART_DEVICE *s_device, char* uart_device_name, uint32_t speed)
int8_t uart_transfer(S_UART_DEVICE *attr);
void uart_release(S_UART_DEVICE *attr);


struct cgpu_info *uart_alloc_cgpu(struct device_drv *drv, int threads);
void __uart_detect(struct cgpu_info *(*device_detect)(), bool single)

#define uart_detect(cgpu) __uart_detect(cgpu, false)
#define uart_detect_one(cgpu) __uart_detect(cgpu, true)
#endif