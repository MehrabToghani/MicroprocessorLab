// Define frequency 1MHz (microcontroller frequency)
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "../lcd.h"

enum Mode
{
    TEMP,      /* show temperature */
    SET_MIN_H, /* set hysteresis upper range */
    SET_MAX_H  /* set hysteresis bottom range */
};

int BV(int bit);
void change_value();
void show_lcd();

char mode = TEMP; /* mode of content of LCD */

int ref = 1; /* reference voltage (= AVCC in circuit) */

/* hysteresis range (initial value)*/
int max_h = 25;
int min_h = 20;

int value; /* temperature in celsius */

int main()
{
    DDRB = 0x01; /* set one bit of PIN B to output(PORT) and other PINs to input(PIN) */

    LCD_Init(); /* init lcd */

    GICR = BV(INT2);      /* enable interrupt 2 */
    MCUCSR = (1 << ISC2); /* set interrupt 2 as rising edge*/
    sei();                /* enable global interrupt */

    ADCSRA = BV(ADEN) | BV(ADPS2) | BV(ADPS1) | BV(ADPS0); /* enable ADC, prescale 128 => clock/128 */

    ADMUX = BV(REFS0); /* Voltage Reference of ADC = AVCC */

    while (1)
    {
        ADCSRA |= BV(ADSC); /* start analog to digital conversion */

        while ((ADCSRA & BV(ADIF)) == 0)
            ; /* wait to end conversion */

        value = ADCW * ref / 10; /* Temperature in degrees Celsius */
        change_value();

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
    if ((PINB & BV(7)) != 0) /* change mode */
    {
        if (mode == TEMP)
            mode = SET_MIN_H;
        else if (mode == SET_MIN_H)
            mode = SET_MAX_H;
        else
            mode = TEMP;

        LCD_Clear(); /* clear lcd */
    }
    if ((PINB & BV(5)) != 0) /* UP */
    {
        if (mode == SET_MIN_H)
        {
            min_h++;
            if (min_h > max_h) /* min_h never bigger than max_h */
                max_h = min_h;
        }
        if (mode == SET_MAX_H)
            max_h++;
    }
    if ((PINB & BV(6)) != 0) /* DOWN */
    {
        if (mode == SET_MIN_H)
            min_h--;
        if (mode == SET_MAX_H)
        {
            if (max_h > min_h) /* min_h never bigger than max_h */
                max_h--;
        }
    }

    show_lcd();
}

void change_value()
{
    if (value >= max_h)
        PORTB = 0x01; /* turn on motor */

    if (value <= min_h)
        PORTB = 0x00; /* turn off motor */

    if (mode == TEMP)
        show_lcd();
}

void show_lcd()
{
    char str[15];
    if (mode == TEMP)
    {
        sprintf(str, "Temp: %5d C", value); /* temp in celsius */

        if ((PORTB & 0x01) != 0)
            str[13] = '!'; /* ! sign for show dangerus */
        else
            str[13] = ' ';
    }
    else if (mode == SET_MIN_H)
    {
        sprintf(str, "Min H: %5d C ", min_h); /* min_h in celsius */
    }
    else if (mode == SET_MAX_H)
    {
        sprintf(str, "Max H: %5d C ", max_h); /* max_h in celsius */
    }

    LCD_Command(0x02); /* set curser at home */
    LCD_String(str);   /* show string in lcd  */
}