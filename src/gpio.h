#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)

const struct gpio_dt_spec relay_set =
        GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, relay_set_gpios);

const struct gpio_dt_spec relay_reset =
        GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, relay_reset_gpios);
