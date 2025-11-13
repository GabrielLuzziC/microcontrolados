#ifndef LCD_H
#define LCD_H
#include <stdint.h>

void LCD_Init(void);
void LCD_Command(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_Write_String(char *str);
void LCD_Clean_Display(void);

#endif
