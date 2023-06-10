enum fence_cmd {
    FENCE_NONE = 0,
    FENCE_ENERGIZE = 1,
    FENCE_DE_ENERGIZE = 2,
};

uint8_t fence_is_energized(uint16_t mv_value);
void fence_handle_cmd(uint16_t mv_value, enum fence_cmd cmd);
