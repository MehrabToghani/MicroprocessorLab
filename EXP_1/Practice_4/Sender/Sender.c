#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>
#include <util/delay.h>

#define START_BYTE 0x31

int main()
{
    char message[] = "Test Message From Atmega16 Microcontroller, Test Message From Atmega16 Microcontroller, Test Message From Atmega16 Microcontroller, Test Message From Atmega16 Microcontroller, Test Message, Good Bye!";

    DDRC = 0xFF; // set PORT C for output

    eeprom_busy_wait();                              // wait to eeprom be free
    eeprom_write_block(message, 0, strlen(message)); // write message to eeprom

    // start messaging
    PORTC = START_BYTE;
    _delay_ms(10);

    for (int i = 0; i < strlen(message); i++)
    {
        PORTC = eeprom_read_byte((const uint8_t*) i);
        _delay_ms(10);
    }

    return 0;
}