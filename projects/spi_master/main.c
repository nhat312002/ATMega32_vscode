/*
 * SPI_Master.c
 *
 * Created: 24/09/2026 20:37:32
 * Author : vqnha
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "LCD_4.h"
#include "spi.h"
#include "PORT_4.h"

#define MISO 6
#define MOSI 5
#define SS   4
#define SCK  7

int main(void)
{
    uint8_t count;
    char    buffer[8];

    LCD_Init();

    stSpiConfig spiConfig = {
        .mode      = eSPI_MASTER,
        .dataOrder = eMSB,
        .cpol      = 0,
        .cpha      = 0,
        .prescale  = 0,
        .enableInt = false,
    };

    SPI_Init(&spiConfig);
    SPI_Enable(true);

    LCD_String_xy(0, 0, "Master Device");
    LCD_String_xy(1, 0, "Sending Data:      ");

    count = 0;

    uint8_t arr[8] = {0, 1, 2, 3, 4};

    for (uint8_t i = 0; i < sizeof(arr); i++)
    {
        arr[i] = i;
    }
    while (1)
    {
        PORTB &= ~(1 << SS);
        SPI_Transmit(arr, NULL, sizeof(arr));
        PORTB |= (1 << SS);
        sprintf(buffer, "%d   ", count);
        LCD_String_xy(1, 13, buffer);
        count++;
        _delay_ms(500);

        static uint8_t j = 0;
        for (uint8_t i = 0; i < sizeof(arr); i++)
        {
            arr[i] = i + j;
        }
        j++;
    }
}
