/*
 * SPI_Slave.c
 *
 * Created: 24/09/2026 21:00:49
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
    // uint8_t count;
    char    buffer[32] = {0};
    uint8_t arr[8];

    LCD_Init();

    stSpiConfig spiConfig = {
        .mode      = eSPI_SLAVE,
        .dataOrder = eMSB,
        .cpol      = 0,
        .cpha      = 0,
        .prescale  = 0,
        .enableInt = false,
    };

    SPI_Init(&spiConfig);
    SPI_Enable(true);

    LCD_String_xy(0, 0, "Slave Device");
    LCD_String_xy(1, 0, "Receive Data:      ");

    // count = 0;

    while (1)
    {
        SPI_Receive(arr, sizeof(arr));
        buffer[0] = 0;
        for (uint8_t i = 0; i < 8; i++)
        {
            char temp[4] = {0};
            snprintf(temp, sizeof(temp), "%02X", arr[i]);
            strcat(buffer, temp);
        }
        LCD_String_xy(2, 0, buffer);
    }
}
