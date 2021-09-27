#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>
#include <util/delay.h>

#define BUFFER_SIZE 10
#define START_BYTE 0x31

int main()
{
    DDRA = 0;    // set PORT A for input
    DDRC = 0xFF; // set PORT C for output

    eeprom_busy_wait(); // wait to eeprom be free

    char buffer[BUFFER_SIZE];

    while (PINA != START_BYTE) // wait to start messaging
        ;
    _delay_ms(15);
    
    for (int i = 0, j = 0; i < 200; i++, j++)
    {
        buffer[j] = PINA; // read data from pin A and store it into buffer
        PORTC = PINA;

        if (j >= BUFFER_SIZE)
        {
            eeprom_write_block(buffer, (void *) i - BUFFER_SIZE, BUFFER_SIZE); // write buffer to eeprom
            j = -1;
        }
        _delay_ms(10);
    }

    // PORTC = eeprom_read_byte((const uint8_t*) 0);

    while (1)
        ;

    return 0;
}