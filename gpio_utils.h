#ifndef GPIO_UTILS
#define GPIO_UTILS
#include <gpiod.h>

struct S_GPIO_PORT {   
    struct gpiod_chip *s_chip;
    struct gpiod_line *s_line;
};

int gpio_init(struct S_GPIO_PORT *s_device, const char* gpiod_chip_name, unsigned int gpio_line_offset);

void gpio_toggle(struct S_GPIO_PORT *s_device);

void gpio_set(struct S_GPIO_PORT *s_device, unsigned int value);

void gpio_release(struct S_GPIO_PORT *s_device);

#endif // GPIO_UTILS