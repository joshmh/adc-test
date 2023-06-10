/*
 * Class A LoRaWAN sample application
 *
 * Copyright (c) 2020 Manivannan Sadhasivam <mani@kernel.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
DR0: SF = 12
DR1: SF = 11
DR2: SF = 10
DR3: SF = 9
DR4: SF = 8
DR5: SF = 7
*/

#include <stddef.h>
#include <stdint.h>

#include <zephyr/devicetree.h>
#include <zephyr/sys/util.h>

#include <zephyr/device.h>
#include <zephyr/lorawan/lorawan.h>
#include <zephyr/kernel.h>

#include <zephyr/sys/printk.h>

#include <zephyr/logging/log.h>

#include "lora.h"
#include "counter_storage.h"

/* Customize based on network configuration */
//70B3D57ED005AC1A
#define LORAWAN_DEV_EUI			{ 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x05, 0xAC, 0x1A }
#define LORAWAN_JOIN_EUI		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
					  0x00, 0x00 }
#define LORAWAN_APP_KEY			{ 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE,\
					  0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88,\
					  0x09, 0xCF, 0x4F, 0x3C }

static const int port = 2;

static lora_callback_t g_callback = NULL;

LOG_MODULE_DECLARE(lora_fence);

static void dl_callback(uint8_t port, bool data_pending,
			int16_t rssi, int8_t snr,
			uint8_t len, const uint8_t *data)
{
	LOG_MODULE_DECLARE(lora_fence);

	LOG_INF("Port %d, Pending %d, RSSI %ddB, SNR %ddBm", port, data_pending, rssi, snr);
	if (data) {
		LOG_HEXDUMP_INF(data, len, "Payload: ");
	}

	if (g_callback != NULL) {
        g_callback(port, data, len);
    }
	LOG_MSG_DBG("Done callback");
}

static void lorawan_datarate_changed(enum lorawan_datarate dr)
{
	uint8_t unused, max_size;

	LOG_MODULE_DECLARE(lora_fence);

	lorawan_get_payload_sizes(&unused, &max_size);
	LOG_INF("New Datarate: DR_%d, Max Payload %d", dr, max_size);
}

static struct lorawan_downlink_cb downlink_cb = {
	.port = LW_RECV_PORT_ANY,
	.cb = dl_callback
};

int lora_init(lora_callback_t callback) {
    g_callback = callback;

	const struct device *lora_dev;
	struct lorawan_join_config join_cfg;
	uint8_t dev_eui[] = LORAWAN_DEV_EUI;
	uint8_t join_eui[] = LORAWAN_JOIN_EUI;
	uint8_t app_key[] = LORAWAN_APP_KEY;
	uint32_t dev_nonce;
	int ret;

	LOG_MODULE_DECLARE(lora_fence);

	if (counter_storage_init() < 0) {
		LOG_ERR("Can't init non-volatile storage.");
		return -1;
	}

	lora_dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	if (!device_is_ready(lora_dev)) {
		LOG_ERR("%s: device not ready.", lora_dev->name);
		return -2;
	}

	ret = lorawan_set_region(LORAWAN_REGION_EU868);
	if (ret < 0) {
		LOG_ERR("lorawan_set_region failed: %d", ret);
		return -3;
	}

	ret = lorawan_set_conf_msg_tries(3);
	if (ret < 0) {
		LOG_ERR("lorawan_set_conf_msg_tries failed: %d", ret);
		return -5;
	} else {
		LOG_INF("lorawan_set_conf_msg_tries set to 3.");
	}

	ret = lorawan_start();
	if (ret < 0) {
		LOG_ERR("lorawan_start failed: %d", ret);
		return -7;
	}

	lorawan_register_downlink_callback(&downlink_cb);
	lorawan_register_dr_changed_callback(lorawan_datarate_changed);

	join_cfg.mode = LORAWAN_ACT_OTAA;
	join_cfg.dev_eui = dev_eui;
	join_cfg.otaa.join_eui = join_eui;
	join_cfg.otaa.app_key = app_key;
	join_cfg.otaa.nwk_key = app_key;
	
	while (1) {
		LOG_INF("Joining network over OTAA");
		dev_nonce = counter_storage_inc();
		join_cfg.otaa.dev_nonce = dev_nonce;
		ret = lorawan_join(&join_cfg);
		if (ret < 0) {
			LOG_ERR("lorawan_join_network failed: %d", ret);
			k_sleep(K_MSEC(3000));
			continue;
		}

		break;
	}

	LOG_INF("Joined!");

	k_sleep(K_MSEC(1000));

	ret = lorawan_set_class(LORAWAN_CLASS_C);
	if (ret < 0) {
		LOG_ERR("lorawan_set_class failed: %d", ret);
		return -4;
	} else {
		LOG_INF("Set to class C.");
	}

	ret = lorawan_set_datarate(LORAWAN_DR_3);
	if (ret < 0) {
		LOG_ERR("lorawan_set_datarate failed: %d", ret);
		return -6;
	} else {
		LOG_MSG_DBG("lorawan_set_datarate set to DR_3.");
	}

#ifdef CONFIG_LORAWAN_APP_CLOCK_SYNC
	LOG_INF("Syncing clock.");
	lorawan_clock_sync_run();
#endif

	return 0;
}

void lora_send(uint8_t *data, uint8_t size)
{
	int ret = lorawan_send(port, data, size, LORAWAN_MSG_UNCONFIRMED);

	if (ret < 0) {
		LOG_ERR("lorawan_send failed: %d", ret);
	}
	else {
		LOG_INF("Data sent!");
	}
}
