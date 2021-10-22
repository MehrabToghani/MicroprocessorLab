// Define frequency 1MHz (microcontroller frequency)
#define F_CPU 1000000UL

#define LCD_Data_Dir DDRD      /* Define LCD data port direction */
#define LCD_Command_Dir DDRC   /* Define LCD command port direction register */
#define LCD_Data_Port PORTD    /* Define LCD data port */
#define LCD_Command_Port PORTC /* Define LCD command port */
#define RS PC0                 /* Define Register Select (data/command reg.)pin */
#define RW PC1                 /* Define Read/Write signal pin */
#define EN PC2                 /* Define Enable signal pin */

#include <avr/io.h>
#include <util/atomic.h>
#include "../lcd.h"

int main()
{
    LCD_Init();

    LCD_String("Hello World!");

    while (1)
    {
    }
    return 0;
}