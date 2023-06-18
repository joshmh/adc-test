#include "read_adc.h"
#include "volt_measure.h"
#include "gpio.h"

const uint16_t volt_div_factor = 56668; // voltage divider factor (* 10_000)

int volt_measure_init(void) {
    return read_adc_init();
}

uint16_t volt_measure(void) {
    int32_t val_mv = 0;
    
	printk("Reading channel...\n");
	int err = read_adc(&val_mv);

	if (err < 0) {
		printk("Could not read channel (%d)\n", err);
		return -2;
	} 

	uint16_t adj_val_mv = (uint16_t)((val_mv * volt_div_factor + 5000) / 10000);

	printk("\n%d mV; %d mV [adj]\n", val_mv, adj_val_mv);

    return adj_val_mv;
}
