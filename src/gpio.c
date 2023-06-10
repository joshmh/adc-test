#include <zephyr/logging/log.h>
#include "gpio.h"

#define ZEPHYR_USER_NODE DT_PATH(zephyr_user)
#define LED0_NODE DT_ALIAS(led0)

const struct gpio_dt_spec gpio_relay_set =
        GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, relay_set_gpios);

const struct gpio_dt_spec gpio_relay_reset =
        GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, relay_reset_gpios);

const struct gpio_dt_spec gpio_enable_measure =
        GPIO_DT_SPEC_GET(ZEPHYR_USER_NODE, enable_measure_gpios);

const struct gpio_dt_spec gpio_led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

LOG_MODULE_DECLARE(lora_fence);

int init_gpio(const struct gpio_dt_spec *gpio) {
    LOG_MODULE_DECLARE(lora_fence);

	if (!gpio_is_ready_dt(gpio)) {
        LOG_ERR("GPIO %s is not ready.", gpio->port->name);
		return -1;
	}

	if (gpio_pin_configure_dt(gpio, GPIO_OUTPUT_INACTIVE) < 0) {
        LOG_ERR("Couldn't configure GPIO %s.", gpio->port->name);
        return -2;
    }

    return 0;
}

int gpio_init(void) {
    if (init_gpio(&gpio_relay_set) < 0) { return -1; }
    if (init_gpio(&gpio_relay_reset) < 0) { return -2; }
    if (init_gpio(&gpio_enable_measure) < 0) { return -3; }
    if (init_gpio(&gpio_led) < 0) { return -4; }

    return 0;
}