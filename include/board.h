#ifndef BOARD_H
#define BOARD_H

#include <avr/io.h>
#include <stdint.h>

#define LED_PORT        PORTC
#define LED_DDR         DDRC
#define LED_PIN         PINC
#define LED_MASK        0xFFu
#define LED_ALL_OFF     0x00u
#define LED_ALL_ON      0xFFu

#define LED0_PORT       PORTC
#define LED0_DDR        DDRC
#define LED0_PIN        PINC
#define LED0_MASK       (1u << PC0)

#endif
