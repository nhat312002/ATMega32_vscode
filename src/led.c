#include "led.h"

#include "board.h"

void led_init(void)
{
    LED_DDR |= LED_MASK;
    LED_PORT = LED_ALL_OFF;
}

void led_toggle(void)
{
    LED_PORT ^= LED_MASK;
}

void led_write(uint8_t on)
{
    if (on) {
        led_write_raw(LED_ALL_ON);
    } else {
        led_write_raw(LED_ALL_OFF);
    }
}

void led_write_raw(uint8_t value)
{
    LED_PORT = (uint8_t)((LED_PORT & (uint8_t)~LED_MASK) | (uint8_t)(value & LED_MASK));
}
