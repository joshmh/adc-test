typedef void (*lora_callback_t)(uint8_t port, const uint8_t*, size_t);

int lora_init(lora_callback_t callback);
void lora_send(uint8_t *data, uint8_t size);

