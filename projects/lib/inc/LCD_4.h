#ifndef LCD_4_H
#define LCD_4_H

void LCD_Init(void);
void LCD_Command(unsigned char);
void LCD_Char(unsigned char);
void LCD_String(char *str);
void LCD_String_xy(char row, char pos, char *str);

#endif