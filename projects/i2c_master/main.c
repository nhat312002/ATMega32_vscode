/*
 * Title : main.c
 * Author : thanhtrung210502
 * Description : Master main program using i2c.c driver
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "LCD_4.h"
#include "PORT_4.h"
#include "i2c.h"
#include "trace.h"

#define SLAVE_ADDRESS 0x50
#define COUNT         5  // Số lượng phần tử gửi/nhận

void UART_Init(unsigned long baud)
{
    // Bật chế độ nhân đôi tốc độ (U2X = 1) để giảm sai số baudrate ở tần số thấp 1MHz
    UCSRA = (1 << U2X);

    // Tính toán giá trị UBRR với hệ số chia 8 (áp dụng cho U2X = 1)
    unsigned int ubrr = (F_CPU / (8UL * baud)) - 1;

    // Ghi vào thanh ghi UBRRH và UBRRL
    UBRRH = (unsigned char)(ubrr >> 8);
    UBRRL = (unsigned char)ubrr;

    // Bật bộ thu (RXEN) và bộ truyền (TXEN)
    UCSRB = (1 << RXEN) | (1 << TXEN);

    // Cấu hình khung truyền 8N1 (8 bit data, 1 bit stop, không parity)
    // ATmega32 yêu cầu set bit URSEL (bit 7) để ghi vào UCSRC
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

// 2. Hàm truyền 1 ký tự (dùng cho printf)
int UART_Transmit(char c, FILE* stream)
{
    (void)stream;
    if (c == '\n')
    {
        while (!(UCSRA & (1 << UDRE)));
        UDR = '\r';  // Thêm ký tự xuống dòng CR cho Terminal
    }

    while (!(UCSRA & (1 << UDRE)));  // Chờ bộ đệm truyền trống
    UDR = c;
    return 0;
}

// Khai báo stream chuẩn cho printf
static FILE mystdout = FDEV_SETUP_STREAM(UART_Transmit, NULL, _FDEV_SETUP_WRITE);

int main(void)
{
    stI2cConfig i2cConfig = {
        .mode  = eI2C_MASTER,
        .clock = 50000UL,
    };

    LCD_Init();

    I2C_Init(&i2cConfig);

    I2C_Enable(true);

    LCD_String_xy(0, 0, "Master Device");

    UART_Init(9600);
    stdout = &mystdout;

    while (1)
    {
        uint8_t dataToSend[] = {0x00, 0x00, 0x55};
        if (I2C_Write(SLAVE_ADDRESS, dataToSend, sizeof(dataToSend), 100) == eFAIL)
        {
            trace_error();
        }
        else
        {
            trace("Write data success");
        }
        _delay_ms(100);

        if (I2C_Write(SLAVE_ADDRESS, dataToSend, 2, 1000) == eFAIL)
        {
            trace_error();
        }
        else
        {
            trace("Write address success");
        }

        uint8_t rxData[2] = {0};
        if (I2C_Read(SLAVE_ADDRESS, rxData, 1, 1000) == eFAIL)
        {
            trace_error();
        }
        else
        {
            trace_hex("rxData", rxData, 1);
        }

        _delay_ms(2000);
    }
}
