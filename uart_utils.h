#ifndef UART_UTILS_H
#define UART_UTILS_H
#include <stdbool.h>
#include <stdint.h>
#include <termios.h>
#include "miner.h"


#define UART_BUFFER_SIZE 256
struct S_UART_DEVICE {
	char *name;
	int fd;
	uint32_t speed;
	struct termios settings;
	int device_number;
};

int8_t uart_init(struct S_UART_DEVICE *s_device, char* uart_device_name, uint32_t speed);
int8_t uart_transfer(struct S_UART_DEVICE *attr);
void uart_release(struct S_UART_DEVICE *attr);

void uart_write(struct S_UART_DEVICE *s_device, char *buf, size_t bufsiz, int *processed);
void uart_read(struct S_UART_DEVICE *s_device, char *buf, size_t bufsiz, int *processed);

struct cgpu_info *uart_alloc_cgpu(struct device_drv *drv, int threads);
void __uart_detect(struct cgpu_info *(*device_detect)(const char *uart_device_names, const char *gpio_chip, int gpio_line, int device_number), bool single);

#define uart_detect(fct) __uart_detect(fct, false)
#define uart_detect_one(fct) __uart_detect(fct, true)
#endif