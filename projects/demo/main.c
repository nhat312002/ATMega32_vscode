#include <avr/io.h>
#include <util/delay.h>

#define BLINK_DELAY_MS 150

int main(void)
{
    DDRC = 0xFF;

    for (;;)
    {
        PORTC = ~PORTC;
        _delay_ms(BLINK_DELAY_MS);
    }
}
