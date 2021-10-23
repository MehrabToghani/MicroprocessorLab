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
    timer = 0,     /* displays the current status of timer */
    set_timer = 1, /* set up current time */
};

enum SetTimerMode
{
    set_minute = 0, /* for change minute */
    set_second = 1, /* for change second */
};

int overflow_count = 0;          /* counter of overflows in timer_1 */
int overflow_max_count = 16 - 1; /* number of timer_1 overflows to reach frequency 1Hz 
                                        2^20 clock frequency, 1 prescale, 2^16 count to overflow
                                            => (2^20 / 1) / 2^16 = 2^4 */

long times = 0; /* time based on the number of seconds remain */

int mode = set_timer; /* timer mode */

int set_timer_mode = set_minute; /* set clock init mode */
int set_minute_value = 0;        /* init value in set_clock mode for hour*/
int set_second_value = 0;        /* init value in set_clock mode for minute*/

// set specific bit to 1
int BV(int bit)
{
    return 1 << bit;
}

int get_minute()
{
    return (times) / 60; /* minute */
}

int get_second()
{
    return times - (get_minute() * 60); /* second */
}

/* display current status on lcd */
void display()
{
    if (mode == timer)
    {
        if (times != 0)
        {
            PORTB &= ~0x08; /* LED off */

            /* convert integer to string */
            char str[5];
            sprintf(str, "%.2d:%.2d", get_minute(), get_second());

            LCD_Command(0x02); /* set curser at home */
            LCD_String(str);
        }
        else
        {
            char str[] = "Alarm!"; /* show message in LCD */
            PORTB |= 0x08;         /* LED on */
            LCD_Command(0x02);     /* set curser at home */
            LCD_String(str);
        }
    }
    else if (mode == set_timer)
    {
        PORTB &= ~0x08; /* LED off */

        char str[5];
        sprintf(str, "%.2d:%.2d", set_minute_value, set_second_value);

        LCD_Command(0x02); /* set curser at home */
        LCD_String(str);
    }
}

// timer_1 overflow interrupt function
ISR(TIMER1_OVF_vect)
{
    overflow_count++; /* count overflows */
    if (overflow_count >= overflow_max_count)
    {
        display();

        if (times != 0)
        {
            times--;
        }

        overflow_count = 0;
    }
}

// interrupt 0 function
ISR(INT0_vect)
{
    if ((PINC & 0x01) != 0)
    { /* mode should be change */
        if (mode == timer)
            mode = set_timer;
        else if (mode == set_timer)
            mode = timer;
        LCD_Clear();
        set_minute_value = 0;
        set_second_value = 0;
    }
    if ((PINC & 0x02) != 0)
    { /* up mode */
        if (set_timer_mode == set_minute)
        {
            set_minute_value += 1; /* minute should be up */
        }
        else
        {
            set_second_value += 1; /* second should be up */
            if (set_second_value == 60)
                set_second_value = 0;
        }
    }
    else if ((PINC & 0x04) != 0)
    { /* down mode */
        if (set_timer_mode == set_minute)
        {
            if (set_minute_value > 0)
                set_minute_value -= 1; /* minute should be down */
        }
        else
        {
            set_second_value -= 1; /* second should be down */
            if (set_second_value == -1)
                set_second_value = 59;
        }
    }
    else if ((PINC & 0x08) != 0)
    { /* submit mode */
        if (set_timer_mode == set_minute)
        {
            set_timer_mode = set_second;
        }
        else
        {
            times = (long)set_minute_value * 60 + set_second_value;
            set_timer_mode = set_minute;
            mode = timer;
        }
    }
}

int main()
{
    TIMSK = BV(TOIE1); /* unmask overflow interrupt for timer_1 */
    TCCR1B = BV(CS10); /* set timer_1 frequency to clock */

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