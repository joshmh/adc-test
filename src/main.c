/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include "read_adc.h"

int main(void) {
	printk("version 1.1\n");

	init_analog();
	
	while (1) {
		int32_t val_mv;
		int err = read_analog(&val_mv);
		if (err < 0) {
			printk("Could not read channel (%d)\n", err);
			return 1;
		} else {
			printk("%d mV\n", val_mv);
		}
		k_sleep(K_MSEC(1000));
	}
}