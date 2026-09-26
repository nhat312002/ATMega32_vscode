#ifndef LED_H
#define LED_H

#include <stdint.h>

void led_init(void);
void led_toggle(void);
void led_write(uint8_t on);
void led_write_raw(uint8_t value);

#endif
