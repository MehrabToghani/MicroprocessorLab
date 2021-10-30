// Define frequency 1MHz (microcontroller frequency)
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/atomic.h>
#include <stdbool.h>

enum Status
{
    NOT_STARTED,
    STARTED_CLOCKWISE,
    STARTED_ANTI_CLOCKWISE
};

char rotations[] = {0b0101, 0b0110, 0b1010, 0b1001}; /* 4 commands for 4 rotations: T1, T2, T3, T4 */
int turn = 0;                                        /* rotation turn */

int overflow_count = 0;          /* counter of overflows in timer_0 */
int overflow_max_count = 20 - 1; /* number of timer_0 overflows to reach frequency 200Hz (5ms) 
                                        2^20 clock frequency, 1 prescale, 2^8 count to overflow
                                            => ((2^20 / 1) / 2^8) / 200 = 20 */

char status = NOT_STARTED; /* status of system */

// set specific bit to 1
int BV(int bit)
{
    return 1 << bit;
}

// timer_0 overflow interrupt function
ISR(TIMER0_OVF_vect)
{
    overflow_count++;
    if (overflow_count >= overflow_max_count)
    {
        if (status == STARTED_CLOCKWISE)
        {
            PORTA = rotations[turn]; /* send command to motor */
            turn = (turn + 1) % 4;   /* next turn, clockwise */
        }
        else if (status == STARTED_ANTI_CLOCKWISE)
        {
            PORTA = rotations[turn]; /* send command to motor */
            turn = (turn + 3) % 4;   /* next turn, anti clockwise*/
        }

        overflow_count = 0;
    }
}

// interrupt 0 function
ISR(INT0_vect)
{
    if ((PINC & BV(1)) == 0)
    {
        if ((status == NOT_STARTED) || (status == STARTED_ANTI_CLOCKWISE))
            status = STARTED_CLOCKWISE;
        else
            status = STARTED_ANTI_CLOCKWISE;
    }
}

int main()
{
    TIMSK = BV(TOIE0); /* unmask overflow interrupt for timer_0 */
    TCCR0 = BV(CS00);  /* set timer_0 frequency to clock */

    GICR = BV(INT0);               /* enable interrupt 0 */
    MCUCR = BV(ISC00) | BV(ISC01); /* config interrupt 0 as rising edge */

    sei(); /* enable global interrupt */

    DDRC = DDRC & ~BV(2); /* set third bit of pin C to input(PIN) */
    DDRA = DDRA | 0x0F;   /* set four bit of pin A to ouput(PORT) */

    while (1)
    {
    }
    return 0;
}