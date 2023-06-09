#include <stdint.h>

#include "fence.h"
#include "gpio.h"

static int energized = 0;

const int reset = 0;
const int set = 1;

int is_energized() {
    return energized;
}

void set_energized(uint16_t mv_value) {
    if (mv_value > 6000) {
        energized = 1;
    } else {
        energized = 0;
    }
}

void set_relay_pin(int do_set, int value) {
    if (do_set) {
        gpio_pin_set_dt(&relay_set, value);
    } else {
        gpio_pin_set_dt(&relay_reset, value);
    }
}

void set_relay(int do_set) {
    set_relay_pin(do_set, 1);
    k_sleep(K_MSEC(100));
    set_relay_pin(do_set, 0);
}

void energize() {
    if (is_energized()) {
        return;
    }

    set_relay(set);   
}

void de_energize() {
    if (!is_energized()) {
        return;
    }

    set_relay(reset);
}