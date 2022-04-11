#include "stdio.h"
#include "ADuCM3029.h"
#include "AD5940.h"


// Function Prototypes
void LCD_command(int code, int printLetter);
void LCD_init (void);
void LCD_cursor (int x, int y);
void LCD_print_string(char* str);
void LCD_clear(void);
void LCD_print_char(char op);
void LCD_print_number(int number);
void LCD_print_float(float number);
void LCD_mode (void);
void LCD_next_line();
