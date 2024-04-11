/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include "gpio.h"
#include "volt_measure.h"

#define THREAD_PRIORITY 5

LOG_MODULE_REGISTER(adc_test, 0);

void led_on() {
	gpio_pin_set_dt(&gpio_led, 1);
}

void led_off() {
	gpio_pin_set_dt(&gpio_led, 0);
}

int main(void) {

	LOG_MODULE_DECLARE(adc_test);
	printk("version 1.1\n");

	if (gpio_init() < 0) {
		LOG_ERR("Couldn't initialize GPIOs.");
		return -10;
	}

	// Boot led indication
	for (int i = 0; i < 7; i++) {
		led_on();
		k_sleep(K_MSEC(100));
		led_off();
		k_sleep(K_MSEC(500));
	}

	if (volt_measure_init() < 0) {
		LOG_ERR("Couldn't initialize ADC.");
		return -11;	
	}
	
	while (true) {
		uint16_t val;
		val = volt_measure(0);
		printk("Voltage 0: %d mV\n\n", val);

		k_sleep(K_MSEC(100));

		val = volt_measure(1);
		printk("Voltage 1: %d mV\n\n\n", val);

		k_sleep(K_MSEC(5000));
	}

	printk("Done\n");
	return 0;
}