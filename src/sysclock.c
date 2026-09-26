#include "sysclock.h"

#include <avr/io.h>

#if !defined(F_CPU)
#error "F_CPU must be defined (see CMakeLists.txt)"
#endif

void sysclock_init(void)
{
#if defined(CLKPR)
    /* Devices with a clock prescaler: switch to the internal RC, no division. */
    CLKPR = (1u << CLKPCE) | (0u << CLKSEL);
#else
    /* ATmega32 (avr5) has no prescaler: the internal 8 MHz RC is divided by 8
       unless the CKDIV8 fuse is unprogrammed, see linker/atmega32-low-fuse.hex */
    (void)F_CPU;
#endif
}
