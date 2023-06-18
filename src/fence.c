#include <stdint.h>

#include "fence.h"
#include "gpio.h"

enum relay_state_t { Energize, De_energize };
const int relay_time_ms = 100;

uint8_t fence_is_energized(uint16_t mv_value) {
    if (mv_value > 5000) {
        return 1;
    } 

    return 0;
}

void set_relay_pin(enum relay_state_t relay_state, int value) {
    if (relay_state == Energize) {
        gpio_pin_set_dt(&gpio_relay_reset, value);
    } else if (relay_state == De_energize) {
        gpio_pin_set_dt(&gpio_relay_set, value);
    }
}

void set_relay(enum relay_state_t relay_state) {
    set_relay_pin(relay_state, 1);
    k_sleep(K_MSEC(relay_time_ms));
    set_relay_pin(relay_state, 0);
}

void fence_handle_cmd(uint16_t mv_value, enum fence_cmd cmd) {
    int energized = fence_is_energized(mv_value);

    if (cmd == FENCE_ENERGIZE && !energized) {
        set_relay(Energize);
    } else if (cmd == FENCE_DE_ENERGIZE && energized) {
        set_relay(De_energize);
    }
}
