// Define frequency 1MHz (microcontroller frequency)
#define F_CPU 1000000UL

#define LCD_Data_Dir DDRA      /* Define LCD data port direction */
#define LCD_Command_Dir DDRB   /* Define LCD command port direction register */
#define LCD_Data_Port PORTA    /* Define LCD data port */
#define LCD_Command_Port PORTB /* Define LCD command port */
#define RS PB2                 /* Define Register Select (data/command reg.)pin */
#define RW PB1                 /* Define Read/Write signal pin */
#define EN PB0                 /* Define Enable signal pin */

#include <avr/io.h>
#include <util/atomic.h>
#include <stdio.h>
#include "../lcd.h"

enum WatchMode
{
    clock = 0,     /* displays the current time */
    set_clock = 1, /* set up current time */
};

enum SetClockMode
{
    set_hour = 0,   /* for change hour */
    set_minute = 1, /* for change minute */
};

int overflow_count = 0;         /* counter of overflows in timer_2 */
int overflow_max_count = 4 - 1; /* number of timer_2 overflows to reach frequency 1Hz 
                                        2^20 clock frequency, 1024 prescale, 256 count to overflow
                                            => (2^20 / 2^10) / 2^8 = 2^2 */

long timeInDay = 0; /* time based on the number of seconds spent */

int mode = clock; /* watch mode */

int set_clock_mode = set_hour; /* set clock init mode */
int set_hour_value = 0;        /* init value in set_clock mode for hour*/
int set_minute_value = 0;      /* init value in set_clock mode for minute*/

// set specific bit to 1
int BV(int bit)
{
    return 1 << bit;
}

int get_hour()
{
    return (timeInDay / 60) / 60; /* hour */
}

int get_minute()
{
    return (timeInDay / 60) - (get_hour() * 60); /* minute */
}

int get_second()
{
    return timeInDay - (get_hour() * 60 * 60) - (get_minute() * 60); /* second */
}

/* display current status on lcd */
void display()
{
    if (mode == clock)
    {
        /* convert integer to string */
        char str[8];
        sprintf(str, "%.2d:%.2d:%.2d", get_hour(), get_minute(), get_second());

        LCD_Command(0x02); /* set curser at home */
        LCD_String(str);
    }
    else if (mode == set_clock)
    {
        char str[5];
        sprintf(str, "%.2d:%.2d", set_hour_value, set_minute_value);

        LCD_Command(0x02); /* set curser at home */
        LCD_String(str);
    }
}

// timer_2 overflow interrupt function
ISR(TIMER2_OVF_vect)
{
    overflow_count++; /* count overflows */
    if (overflow_count >= overflow_max_count)
    {
        display();

        timeInDay++;
        if (timeInDay >= ((long)(60 * 60) * 24))
        {
            timeInDay = 0;
        }

        overflow_count = 0;
    }
}

// interrupt 0 function
ISR(INT0_vect)
{
    if ((PINC & 0x01) != 0)
    { /* mode should be change */
        if (mode == clock)
            mode = set_clock;
        else if (mode == set_clock)
            mode = clock;
        LCD_Clear();
        set_hour_value = 0;
        set_minute_value = 0;
    }
    if ((PINC & 0x02) != 0)
    { /* up mode */
        if (set_clock_mode == set_hour)
        {
            set_hour_value += 1; /* hour should be up */
            if (set_hour_value == 24)
                set_hour_value = 0;
        }
        else
        {
            set_minute_value += 1; /* minute should be up */
            if (set_minute_value == 60)
                set_minute_value = 0;
        }
    }
    else if ((PINC & 0x04) != 0)
    { /* down mode */
        if (set_clock_mode == set_hour)
        {
            set_hour_value -= 1; /* hour should be down */
            if (set_hour_value == -1)
                set_hour_value = 23;
        }
        else
        {
            set_minute_value -= 1; /* minute should be down */
            if (set_minute_value == -1)
                set_minute_value = 59;
        }
    }
    else if ((PINC & 0x08) != 0)
    { /* submit mode */
        if (set_clock_mode == set_hour)
        {
            set_clock_mode = set_minute;
        }
        else
        {
            timeInDay = (long) set_hour_value * 60 * 60 + set_minute_value * 60;
            set_clock_mode = set_hour;
            mode = clock;
        }
    }
}

int main()
{
    TIMSK = BV(TOIE2);                      /* unmask overflow interrupt for timer_2 */
    TCCR2 = BV(CS20) | BV(CS21) | BV(CS22); /* set timer_2 frequency to clock/1024 */

    GICR = BV(INT0);               /* enable interrupt 0 */
    MCUCR = BV(ISC00) | BV(ISC01); /* config interrupt 0 as rising edge */

    sei(); /* enable global interrupt */

    LCD_Init(); /* init lcd */

    DDRC = DDRC | 0x0F; /* set 4 bit of pin C to output(PORT) */

    while (1)
    {
    }
    return 0;
}