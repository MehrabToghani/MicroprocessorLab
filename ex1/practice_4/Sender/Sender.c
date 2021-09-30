#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>
#include <util/delay.h>

int main()
{
    char message[] = "Test Message From Atmega16 Microcontroller, Test Message From Atmega16 Microcontroller, Test Message From Atmega16 Microcontroller, Test Message From Atmega16 Microcontroller, Test Message, Good Bye!!";

    DDRC = 0xFF; // set PORT C for output
    DDRD = DDRD | 0x01; // set first bit of PORT D for output

    PORTC = 0; // init PORT C
    PORTD = 0; // init PORT D

    eeprom_busy_wait();                              // wait to eeprom be free
    eeprom_write_block(message, 0, strlen(message)); // write message to eeprom

    for (int i = 0; i < strlen(message); i++)
    {
        PORTC = message[i];
        PORTD = 0x01;
        _delay_ms(10);
        PORTD = 0;
        _delay_ms(10);
    }

    PORTC = 0;

    return 0;
}