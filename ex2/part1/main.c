// Define frequency 1MHz (microcontroller frequency)
//   this is used by the __delay_ms(xx) and __delay_us(xx) functions
#define F_CPU 1000000UL

// Define frequency 1Hz for LED (it toggle every 500ms)
#define F_LED 1UL

#include <avr/io.h>
#include <util/delay.h>

int main()
{
    DDRC = DDRC ^ 0x01; /* set first bit of PORT C for output */

	PORTC = PORTC | 0x01; /* init PORT C */
	
	unsigned long cycles = F_CPU * 2 * F_LED; // calculate delay
	
    while (1)
    {
        PORTC = PORTC ^ 0x01;
        _delay_ms(1000000000 / cycles);           // delay
    }
    return 0;
}