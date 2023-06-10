/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>

#include "lora.h"
#include "fence.h"
#include "util.h"
#include "gpio.h"
#include "volt_measure.h"

LOG_MODULE_REGISTER(lora_fence, LOG_LEVEL_DBG);

static enum fence_cmd fCmd = FENCE_NONE;
K_SEM_DEFINE(lora_sem, 0, 1); // Initial count 0, max count 1

void cb(uint8_t port, const uint8_t* data, size_t size) {
	printk("Yo! (in main) Received %d bytes, port %d\n", size, port);

	if (port != 3) {
		return;
	}

	if (data[0] == 0x07) {
		printk("Energize\n");
		fCmd = FENCE_ENERGIZE;
		k_sem_give(&lora_sem);
	} else if (data[0] == 0x08) {
		printk("De-energize\n");
		fCmd = FENCE_DE_ENERGIZE;
		k_sem_give(&lora_sem);
	} else {
		printk("Unknown command\n");
	}
}


// TODO: break up loop stuff into sub-functions, define slow loop and fast loop
int main_loop(int do_handle_cmd) {
	uint8_t data[3];

	uint16_t adj_val_mv = volt_measure();
	if (adj_val_mv < 0) {
		LOG_ERR("Couldn't read voltage.");
		return -1;
	}

	if (do_handle_cmd) {
		// Received command
		fence_handle_cmd(adj_val_mv, fCmd);
		return 0;
	} 
	
	convert_uint16_to_uint8_array(adj_val_mv, data);
	data[2] = fence_is_energized(adj_val_mv);

	lora_send(data, 3);

	return 0;
}

void led_on() {
	gpio_pin_set_dt(&gpio_led, 1);
}

void led_off() {
	gpio_pin_set_dt(&gpio_led, 0);
}

int main(void) {
    int res;

	LOG_MODULE_DECLARE(lora_fence);
	printk("version 1.1\n");

	if (gpio_init() < 0) {
		LOG_ERR("Couldn't initialize GPIOs.");
		return -10;
	}

    res = lora_init(cb);
	if (res < 0) {
        LOG_ERR("Couldn't initialize lora.");
        return -1;
    }

	if (volt_measure_init() < 0) {
		LOG_ERR("Couldn't initialize voltage measurement.");
		return -11;
	}	

	int loop_delay_ms = 30000;
	int short_loop_count = 0;
	enum loop_state_t { LONG_LOOP, SHORT_LOOP, SHORTER_LOOP };
	enum loop_state_t loop_state = LONG_LOOP;

	while (1) {
		if (loop_state == SHORTER_LOOP) {
			LOG_MSG_DBG("Shorter loop");
			loop_delay_ms = 100;
			loop_state = SHORT_LOOP;
		} else if (loop_state == SHORT_LOOP && short_loop_count < 4) {
			LOG_MSG_DBG("Short loop: %d", short_loop_count);
			loop_delay_ms = 3000;
			short_loop_count++;
		} else {
			LOG_MSG_DBG("Long loop");
			loop_delay_ms = 30000;
			short_loop_count = 0;
			loop_state = LONG_LOOP;
		}

		res = k_sem_take(&lora_sem, K_MSEC(loop_delay_ms));

		gpio_pin_set_dt(&gpio_led, 1);

		if (res == 0) {
			printk("Received command\n");
			main_loop(1);
			loop_state = SHORTER_LOOP;
		} else {
			printk("No command received\n");
			main_loop(0);
		}

		gpio_pin_set_dt(&gpio_led, 0);
	}

	printk("Done\n");
	return 0;
}