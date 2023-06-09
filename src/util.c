#include <stdint.h>

void convert_uint16_to_uint8_array(uint16_t input, uint8_t output[2]) {
    output[0] = (uint8_t)(input & 0xFF);         // Lower byte
    output[1] = (uint8_t)((input >> 8) & 0xFF);  // Higher byte
}
