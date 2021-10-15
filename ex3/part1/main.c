// Define frequency 1MHz (microcontroller frequency)
//   this is used by the __delay_ms(xx), __delay_us(xx) and timers functions
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/atomic.h>

unsigned char count = 0;              /* number in timer, from 0 to 9 */
unsigned int overflow_count = 0;      /* counter of overflows in timer_1 */
unsigned int overflow_max_count = 10; /* number of timer_1 overflows to reach desired 
                                            frequency of 7seg timer */

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
    overflow_count++; /* count overflows */
    if (overflow_count >= overflow_max_count)
    {
        count = (count + 1) % 10;
        PORTD = seven_segment(count); /* show count in 7seg */

        overflow_count = 0;
    }
}

int main()
{
    TIMSK = BV(TOIE1); /* unmask overflow interrupt for timer_1 */
    TCCR1B = BV(CS10); /* set timer_1 frequency to clock */
    sei();             /* enable global interrupt */

    DDRC |= 0x01; /* set first bits of PORT C for output */
    DDRD |= 0x7f; /* set 7 bits of PORT D for output */

    PORTC = ~0x01; /* enale 7seg (active low) */

    PORTD = seven_segment(0); /* init 7seg */

    while (1)
    {
    }
    return 0;
}