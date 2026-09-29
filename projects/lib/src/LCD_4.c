#include <avr/io.h>
#include <util/delay.h>
#include "LCD_4.h"
#include "PORT_4.h"

void LCD_Command(unsigned char cmnd)
{
    LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0);
    LCD_Port &= ~(1 << RS);
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);

    _delay_us(200);

    LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);
    if (cmnd == 0x01 || cmnd == 0x02)
    {
        _delay_ms(2);
    }
    else
    {
        _delay_us(50);
    }
}

void LCD_Char(unsigned char data)
{
    LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0);
    LCD_Port |= (1 << RS);
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);

    _delay_us(200);

    LCD_Port = (LCD_Port & 0x0F) | (data << 4);
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);
    _delay_us(50);
}

void LCD_String(char* str)
{
    int i;
    for (i = 0; str[i] != 0; i++)
    {
        LCD_Char(str[i]);
    }
}

void LCD_Init()
{
    LCD_Dir = 0xFF;
    _delay_ms(50);

    LCD_Port &= ~(1 << RS);
    LCD_Port &= ~(1 << EN);

    // Gui nibble 0x03 lan 1
    LCD_Port = (LCD_Port & 0x0F) | 0x30;
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);
    _delay_ms(5);

    // Gui nibble 0x03 lan 2
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);
    _delay_us(150);

    // Gui nibble 0x03 lan 3
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);
    _delay_us(150);

    // Chuyen sang che do 4-bit
    LCD_Port = (LCD_Port & 0x0F) | 0x20;
    LCD_Port |= (1 << EN);
    _delay_us(1);
    LCD_Port &= ~(1 << EN);
    _delay_us(150);

    LCD_Command(0x28);  // 4-bit, 2 dong, font 5x8
    LCD_Command(0x0C);  // Bat hien thi, tat con tro
    LCD_Command(0x06);  // Tu dong tang con tro
    LCD_Command(0x01);  // Xoa man hinh
}

void LCD_String_xy(char row, char pos, char* str)
{
    if (row == 1 && pos < 16)
        LCD_Command((pos & 0x0F) | 0x80);
    else if (row == 2 && pos < 16)
        LCD_Command((pos & 0x0F) | 0xC0);
    LCD_String(str);
}
