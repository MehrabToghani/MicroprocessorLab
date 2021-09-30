#include <avr/io.h>
#include <avr/eeprom.h>
#include <string.h>
#include <util/delay.h>

int main()
{
    char message[] = "Test Message From Atmega16 Microcontroller, Test Message From Atmega16 Microcontroller, Test Message From Atmega16 Microcontroller, Test Message From Atmega16 Microcontroller, Test Message, Good Bye!!";

    DDRC = DDRC | 0x01; // set first bit of PORT C for output
    DDRD = DDRD | 0x01; // set first bit of PORT D for output

    PORTC = 0; // init PORT C
    PORTD = 0; // init PORT D

    eeprom_busy_wait();                              // wait to eeprom be free
    eeprom_write_block(message, 0, strlen(message)); // write message to eeprom

    for (int i = 0; i < strlen(message); i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if ((message[i] & (1 << j)) != 0)
            {
                PORTC = 0x01;
            }
            else
            {
                PORTC = 0;
            }
            PORTD = 0x01;
            _delay_ms(2);
            PORTD = 0;
            _delay_ms(2);
        }
    }

    PORTC = 0;

    return 0;
}