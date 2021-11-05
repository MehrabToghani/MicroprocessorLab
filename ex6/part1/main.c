// Define frequency 1MHz (microcontroller frequency)
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <stdio.h>
#include "../lcd.h"

int BV(int bit);
void print_lcd(int value);

int ref = 1; /* reference voltage (= AVCC in circuit) */

enum Unit
{
    KELVIN,
    CELSIUS,
    FAHRENHEIT
};

char unit = CELSIUS;

int main()
{
    LCD_Init(); /* init lcd */

    GICR = BV(INT2); /* enable interrupt 2 */
    sei();           /* enable global interrupt */

    ADCSRA = BV(ADEN) | BV(ADPS2) | BV(ADPS1) | BV(ADPS0); /* enable ADC, prescale 128 => clock/128 */

    ADMUX = BV(REFS0); /* Voltage Reference of ADC = AVCC */

    while (1)
    {
        ADCSRA |= BV(ADSC); /* start analog to digital conversion */

        while ((ADCSRA & BV(ADIF)) == 0)
            ; /* wait to end conversion */

        int value = ADCW * ref / 10; /* Temperature in degrees Celsius */
        print_lcd(value);

        _delay_ms(10);
    }

    return 0;
}

// set specific bit to 1
int BV(int bit)
{
    return 1 << bit;
}

// interrupt 2 function
ISR(INT2_vect)
{
    /* change unit */
    if (unit == CELSIUS)
        unit = KELVIN;
    else if (unit == KELVIN)
        unit = FAHRENHEIT;
    else
        unit = CELSIUS;
}

void print_lcd(int value)
{
    char str[10];

    if (unit == CELSIUS)
        sprintf(str, "%5d C", value); /* temp in celsius */
    else if (unit == KELVIN)
        sprintf(str, "%5d K", value + 273); /* temp in kelvin */
    else
        sprintf(str, "%5d F", (int)(((float)value * 9 / 5) + 32)); /* temp in fahrenheit */

    LCD_Command(0x02); /* set curser at home */
    LCD_String(str);   /* show string in lcd */
}