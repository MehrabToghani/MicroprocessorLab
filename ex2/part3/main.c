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

    while (1)
    {
        while ((PINA & 0x01) == 0)
            ; /* wait for pushed button */

        // forward
        for (int i = 0; i < 8; i++)
        {
            PORTB = 1 << i;
            _delay_ms(1000000000 / cycles); // delay
        }

        // backward
        for (int i = 1; i < 8; i++)
        {
            PORTB = 1 << (7 - i);
            _delay_ms(1000000000 / cycles); // delay
        }

        // clear PORT B
        PORTB = 0;
    }
    return 0;
}