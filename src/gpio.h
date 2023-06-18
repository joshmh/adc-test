#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

extern const struct gpio_dt_spec gpio_relay_set;        
extern const struct gpio_dt_spec gpio_relay_reset;
extern const struct gpio_dt_spec gpio_led;

int gpio_init(void);
