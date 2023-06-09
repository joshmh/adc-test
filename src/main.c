/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include "read_adc.h"
#include "lora.h"
#include "util.h"

LOG_MODULE_REGISTER(lora_fence, LOG_LEVEL_DBG);

int main(void) {
    int res;

	LOG_MODULE_DECLARE(lora_fence);
	printk("version 1.1\n");

	init_analog();
    res = lora_init();
	if (res < 0) {
        LOG_ERR("Couldn't initialize lora.");
        return -1;
    }

	while (1) {
		int32_t val_mv = 0;
        uint8_t data[2];

		printk("Reading channel ...\n");
		int err = read_analog(&val_mv);
		if (err < 0) {
			printk("Could not read channel (%d)\n", err);
			return 1;
		} else {
            printk("\n%d mV\n", val_mv);
        }

		convert_uint16_to_uint8_array(val_mv, data);
        lora_send(data);

		k_sleep(K_MSEC(20000));
	}
}