/*
 * Copyright (c) 2020 Libre Solar Technologies GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include "read_adc.h"

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

/* Data of ADC io-channel specified in devicetree. */
static const struct adc_dt_spec adc_channel_1 = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 1);
static const struct adc_dt_spec adc_channel_2 = ADC_DT_SPEC_GET_BY_IDX(DT_PATH(zephyr_user), 2);

int err;

int read_adc_init() {
	if (!device_is_ready(adc_channel_1.dev)) {
		printk("ADC controller device not ready\n");
		return -1;
	}

	err = adc_channel_setup_dt(&adc_channel_1);
	if (err < 0) {
		printk("Could not setup channel (%d)\n", err);
		return -2;
	}

	if (!device_is_ready(adc_channel_2.dev)) {
		printk("ADC controller device not ready\n");
		return -1;
	}

	err = adc_channel_setup_dt(&adc_channel_2);
	if (err < 0) {
		printk("Could not setup channel (%d)\n", err);
		return -2;
	}

	return 0;
}

int read_adc(int32_t *val_mv, int idx) {
	int16_t buf;
	struct adc_dt_spec adc_channel = idx == 0 ? adc_channel_1 : adc_channel_2;
	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};

	printk("ADC reading:\n");

	printk("- %s, channel %d: ",
			adc_channel.dev->name,
			adc_channel.channel_id);

	(void)adc_sequence_init_dt(&adc_channel, &sequence);

	err = adc_read(adc_channel.dev, &sequence);
	if (err < 0) { return -1; }

	printk(" %d ", buf);

	/* conversion to mV may not be supported, skip if not */
	*val_mv = (int32_t)buf;
	err = adc_raw_to_millivolts_dt(&adc_channel, val_mv);

	if (err < 0) {
		return -2;
	} 

	return 0;
}
