#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/fs/nvs.h>

static struct nvs_fs fs;

#define NVS_PARTITION		storage_partition
#define NVS_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(NVS_PARTITION)

#define COUNTER_ID 1


int counter_storage_init(void)
{
    int rc;

	struct flash_pages_info info;

	/* define the nvs file system by settings with:
	 *	sector_size equal to the pagesize,
	 *	3 sectors
	 *	starting at NVS_PARTITION_OFFSET
	 */
	fs.flash_device = NVS_PARTITION_DEVICE;
	if (!device_is_ready(fs.flash_device)) {
		printk("Flash device %s is not ready\n", fs.flash_device->name);
		return -1;
	}
	fs.offset = NVS_PARTITION_OFFSET;
	rc = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
	if (rc) {
		printk("Unable to get page info\n");
		return -2;
	}
	fs.sector_size = info.size;
	fs.sector_count = 3U;

	rc = nvs_mount(&fs);
	if (rc) {
		printk("Flash Init failed\n");
		return -3;
	}

    printk("Flash initialized\n");

    return 0;
}

uint32_t counter_storage_inc(void) {
    int rc;
	uint32_t counter = 10U;

	/* COUNTER_ID is used to store the reboot counter, lets see
	 * if we can read it from flash
	 */
	rc = nvs_read(&fs, COUNTER_ID, &counter, sizeof(counter));
	if (rc > 0) { /* item was found, show it */
		printk("Id: %d, counter: %d\n", COUNTER_ID, counter);
        counter++;
		(void)nvs_write(&fs, COUNTER_ID, &counter, sizeof(counter));
	} else   {/* item was not found, add it */
		printk("No Reboot counter found, adding it at id %d\n", COUNTER_ID);
	}
    (void)nvs_write(&fs, COUNTER_ID, &counter, sizeof(counter));

    return counter;
}

