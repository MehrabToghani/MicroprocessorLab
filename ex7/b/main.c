// Define frequency 1MHz (microcontroller frequency)
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <util/atomic.h>

/******************************** prototypes ********************************/
int BV(int bit);

/************************ global variables declaration ************************/
int ref = 1; /* reference voltage (= AVCC in circuit) */
int temp;    /* temperature in celsius */

char rotations[] = {0b0101, 0b0110, 0b1010, 0b1001}; /* 4 commands for 4 rotations: T1, T2, T3, T4 */
int turn = 0;                                        /* rotation turn */
int ticks = 0;

/************************************ main ************************************/
int main()
{
    DDRC = DDRC | 0x0F; /* set four bit of pin A to ouput(PORT) */

    TIMSK = BV(TOIE0); /* unmask overflow interrupt for timer_0 */
    TCCR0 = BV(CS00) | BV(CS02); /* set timer_0 frequency to clock/1024 */

    ADCSRA = BV(ADEN) | BV(ADPS2) | BV(ADPS1) | BV(ADPS0); /* enable ADC, prescale 128 => clock/128 */
    ADMUX = BV(REFS0);                                     /* Voltage Reference of ADC = AVCC */

    UCSRB = BV(RXEN) | BV(TXEN); /* Enable RX and TX */
    UCSRB |= BV(RXCIE);          /* Enable interrupt on RX */
    UBRRL = 25;                  /* 2400 bit rate */

    sei(); /* enable global interrupt */

    while (1)
    {
        ADCSRA |= BV(ADSC); /* start analog to digital conversion */

        while ((ADCSRA & BV(ADIF)) == 0)
            ; /* wait to end conversion */

        temp = ADCW * ref / 10; /* Temperature in degrees Celsius */

        _delay_ms(10);
    }

    return 0;
}

/********************************* functions **********************************/
int BV(int bit)
{
    return 1 << bit;
}

/************************ timer_0 overflow interrupt **************************/
ISR(TIMER0_OVF_vect)
{
    if (ticks > 0)
    {
        PORTC = rotations[turn];
        turn = (turn + 1) % 4;
        ticks--;
    }
    UDR = temp;
}

/******************************* RX interrupt *********************************/
ISR(USART_RXC_vect)
{
    ticks = UDR;
}