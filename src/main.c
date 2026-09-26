#include <avr/io.h>
#include <util/delay.h>

#include "board.h"
#include "led.h"
#include "sysclock.h"

#ifndef BLINK_DELAY_MS
#define BLINK_DELAY_MS 500u
#endif

int main(void)
{
    sysclock_init();
    led_init();

    for (;;) {
        led_toggle();
        _delay_ms(BLINK_DELAY_MS);
    }
}
