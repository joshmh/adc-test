#include "read_adc.h"
#include "volt_measure.h"
#include "gpio.h"

const uint16_t volt_div_factor = 5; // voltage divider factor

int volt_measure_init(void) {
    return read_adc_init();
}

uint16_t volt_measure(void) {
    int32_t val_mv = 0;

    if (gpio_pin_set_dt(&gpio_enable_measure, 1) < 0) {
        printk("Error setting enable pin\n");
        return -1;
    }
    
    //  Wait for voltage to stabilize
    k_sleep(K_MSEC(1000));

	printk("Reading channel...\n");
	int err = read_adc(&val_mv);

    if (gpio_pin_set_dt(&gpio_enable_measure, 0) < 0) {
        printk("Error setting enable pin\n");
    }

	if (err < 0) {
		printk("Could not read channel (%d)\n", err);
		return -2;
	} 

	uint16_t adj_val_mv = val_mv * volt_div_factor;

	printk("\n%d mV; %d mV [adj]\n", val_mv, adj_val_mv);

    return adj_val_mv;
}
