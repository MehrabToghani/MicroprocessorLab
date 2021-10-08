// Define frequency 1MHz (microcontroller frequency)
//   this is used by the __delay_ms(xx) and __delay_us(xx) functions
#define F_CPU 1000000UL

// Define frequency 1Hz for LED (it toggle every 500ms)
#define F_LED 1UL

#include <avr/io.h>
#include <util/delay.h>

int main()
{
    DDRB = 0xFF;         /* set PORT B for output */
    DDRA = DDRA & ~0x01; /* set first bit of PORT A for input */

    PORTB = 0; /* init PORT B */

    unsigned long cycles = F_CPU * 2 * F_LED; // calculate delay

    while ((PINA & 0x01) == 0)
        ; /* wait to pushed button */

    while (1)
    {
        PORTB = ~PORTB;
        _delay_ms(1000000000 / cycles); // delay
    }
    return 0;
}