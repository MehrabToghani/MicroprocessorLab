// Define frequency 1MHz (microcontroller frequency)
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "./lcd.h"
#include "./keypad.h"

/* function prototypes */
int BV(int bit);
void change_value();
void show_lcd();

int main()
{
    LCD_Init();    /* init lcd */
    keypad_init(); /* init keypads */

    unsigned char i = 0;
    unsigned char j = 0;
    while (1)
    {
        i = key_scan();
        if (i == 255 || i == 0)
        {
            continue;
        }

        if (i == j)
        {
            j = 0;
            _delay_ms(200);
        }
        else
        {
            j = i;
            LCD_Char(i);
        }
    }
}

// set specific bit to 1
int BV(int bit)
{
    return 1 << bit;
}