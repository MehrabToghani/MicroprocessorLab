// Define frequency 1MHz (microcontroller frequency)
//   this is used by the __delay_ms(xx), __delay_us(xx) and timers functions
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/atomic.h>

unsigned int count = 0;                /* number in timer, from 0 to 9999 */
unsigned int overflow_count_1 = 0;     /* counter of overflows in timer_1 */
unsigned int overflow_max_count_1 = 5; /* number of timer_1 overflows to reach desired 
                                            frequency of 7segments timer */

unsigned int overflow_count_0 = 0;       /* counter of overflows in timer_0 */
unsigned int overflow_max_count_0 = 180; /* number of timer_0 overflows to reach desired 
                                            frequency of turn off/on 7segments */

// set specific bit to 1
int BV(int bit)
{
    return 1 << bit;
}

/* convert numbers to 7seg LEDs mode */
unsigned char seven_segment(unsigned char number)
{
    /* note: the 7seg LEDs pin is active low */
    switch (number)
    {
    case 0:
        return 0b0111111; /* number 0 */
    case 1:
        return 0b0000110; /* number 1 */
    case 2:
        return 0b1011011; /* number 2 */
    case 3:
        return 0b1001111; /* number 3 */
    case 4:
        return 0b1100110; /* number 4 */
    case 5:
        return 0b1101101; /* number 5 */
    case 6:
        return 0b1111101; /* number 6 */
    case 7:
        return 0b0000111; /* number 7 */
    case 8:
        return 0b1111111; /* number 8 */
    case 9:
        return 0b1101111; /* number 9 */
    default:
        return 0;
    }
}

ISR(TIMER1_OVF_vect)
{
    overflow_count_1++; /* count overflows */
    if (overflow_count_1 >= overflow_max_count_1)
    {
        count = (count + 1) % 10000;
        overflow_count_1 = 0;
    }
}

ISR(TIMER0_OVF_vect)
{
    overflow_count_0++; /* count overflows */
    if (overflow_count_0 >= overflow_max_count_0)
    {
        PORTD = seven_segment(0xFF); /* turn off all LEDs */
        switch (PORTC)
        {
        case (unsigned char)~0x01:
            PORTC = ~0x02;                            /* enale 7seg two (avtive low) */
            PORTD = seven_segment((count / 10) % 10); /* show count in 7seg two */
            break;
        case (unsigned char)~0x02:
            PORTC = ~0x04;                             /* enale 7seg three (avtive low) */
            PORTD = seven_segment((count / 100) % 10); /* show count in 7seg three */
            break;
        case (unsigned char)~0x04:
            PORTC = ~0x08;                              /* enale 7seg three (avtive low) */
            PORTD = seven_segment((count / 1000) % 10); /* show count in 7seg four */
            break;
        default:
            PORTC = ~0x01;                     /* enale 7seg one (avtive low) */
            PORTD = seven_segment(count % 10); /* show count in 7seg */
            break;
        }

        overflow_count_0 = 0;
    }
}

int main()
{
    TIMSK = BV(TOIE0) | BV(TOIE1); /* unmask overflow interrupt for timer_0 and timer_1 */
    TCCR0 = BV(CS00);              /* set timer_0 frequency to clock */
    TCCR1B = BV(CS10);             /* set timer_1 frequency to clock */
    sei();                         /* enable global interrupt */

    DDRC |= 0x0f; /* set 4 bits of PORT C for output */
    DDRD |= 0x7f; /* set 7 bits of PORT D for output */

    PORTC = ~0x01; /* enale 7seg one (enable is actice low)*/

    PORTD = seven_segment(0); /* init 7seg */

    while (1)
    {
    }
    return 0;
}