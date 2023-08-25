#include "gpio_utils.h"
#include <gpiod.h>
#include "logging.h"
#include "miner.h"

#define GPIO_CHIP_NAME "gpiochip1"
#define GPIO_LINE_OFFSET 28


/**
 * @brief Initialize a GPIO (General-Purpose Input/Output) line on a given GPIO chip.
 * 
 * This function opens the specified GPIO chip, retrieves the required GPIO line
 * based on the given line offset, and requests it for output. It will populate
 * the `S_GPIO_PORT` structure pointed to by `s_device` with the chip and line
 * information. If any step fails, the function will perform necessary cleanup
 * and return -1.
 * 
 * @param s_device Pointer to an S_GPIO_PORT struct to store GPIO chip and line information.
 * @param gpiod_chip_name Name of the GPIO chip to be opened (e.g., "gpiochip0").
 * @param gpio_line_offset Offset of the GPIO line within the chip to be initialized.
 * 
 * @return Returns 0 on successful initialization, -1 otherwise.
 */
void gpio_init(struct S_GPIO_PORT *s_device, char* gpiod_chip_name, unsigned int gpio_line_offset) {

    s_device->s_chip = gpiod_chip_open_by_name(gpiod_chip_name);
    if (!s_device->s_chip)
    {
        applog(LOG_ERR,"Failed to open GPIO chip");
        return -1;
    }
    // Get the GPIO line
    s_device->s_line = gpiod_chip_get_line(s_device->s_chip, gpio_line_offset);
    if (!s_device->s_line)
    {
        applog(LOG_ERR,"Failed to get GPIO line");
        gpiod_chip_close(s_device->s_chip);
        return -1;
    }

    // Request the GPIO line
    int ret = gpiod_line_request_output(s_device->s_line, "gpio-reset", 0);
    if (ret < 0)
    {
        applog(LOG_ERR,"Failed to request GPIO line");
        gpiod_line_release(s_device->s_line);
        gpiod_chip_close(s_device->s_chip);
        return -1;
    }
    return 0;
}

/**
 * @brief Toggles the state of a GPIO (General-Purpose Input/Output) line.
 * 
 * This function reads the current state of the GPIO line represented in the
 * S_GPIO_PORT structure pointed to by `s_device`. If the current state is 0,
 * the function will set it to 1, and vice versa.
 * 
 * @param s_device Pointer to an S_GPIO_PORT struct containing GPIO chip and line information.
 * 
 * @note This function assumes that `s_device` has been successfully initialized 
 *       by a prior call to `gpio_init()`.
 */
void gpio_toggle(struct S_GPIO_PORT *s_device)
{
    if (gpiod_line_get_value(s_device->s_line) == 0)
    {
        gpiod_line_set_value(s_device->s_line, 1);
    }
    else{
        gpiod_line_set_value(s_device->s_line, 0);
    }
}

/**
 * @brief Sets the state of a GPIO (General-Purpose Input/Output) line to a specified value.
 * 
 * This function sets the state of the GPIO line represented in the S_GPIO_PORT structure
 * pointed to by `s_device`. The state is set to 0 if the input `value` is 0, and set to 1
 * otherwise.
 * 
 * @param s_device Pointer to an S_GPIO_PORT struct containing GPIO chip and line information.
 * @param value The value to set the GPIO line to. If 0, the line is set to low (0); otherwise, it's set to high (1).
 * 
 * @note This function assumes that `s_device` has been successfully initialized 
 *       by a prior call to `gpio_init()`.
 */
void gpio_set(struct S_GPIO_PORT *s_device, unsigned int value)
{
    if (value == 0)
    {
        gpiod_line_set_value(s_device->s_line, 0);
    }
    else
    {
        gpiod_line_set_value(s_device->s_line, 1);
    }
}


/**
 * @brief Releases a GPIO (General-Purpose Input/Output) line and closes the GPIO chip.
 * 
 * This function releases the GPIO line and closes the GPIO chip represented in the
 * S_GPIO_PORT structure pointed to by `s_device`.
 * 
 * @param s_device Pointer to an S_GPIO_PORT struct containing GPIO chip and line information.
 * 
 * @note This function assumes that `s_device` has been successfully initialized 
 *       by a prior call to `gpio_init()`.
 * @note After calling this function, `s_device` should not be used for GPIO operations
 *       until it has been re-initialized.
 */
void gpio_release(struct S_GPIO_PORT *s_device)
{
    gpiod_line_release(s_device->s_line);
    gpiod_chip_close(s_device->s_chip);
}