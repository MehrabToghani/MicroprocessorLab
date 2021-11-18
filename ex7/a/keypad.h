#include <avr/io.h>
#include <util/delay.h>

#ifndef KEY_PORT
#define KEY_PORT PORTC
#endif

#ifndef KEY_DDR
#define KEY_DDR DDRC
#endif

#ifndef KEY_PIN
#define KEY_PIN PINC
#endif

#ifndef R1
#define R1 PINC0
#endif

#ifndef R2
#define R2 PINC1
#endif

#ifndef R3
#define R3 PINC2
#endif

#ifndef A
#define A PORTC4
#endif

#ifndef B
#define B PORTC5
#endif

#ifndef C
#define C PORTC6
#endif

#ifndef D
#define D PORTC7
#endif

/* function prototypes */
void keypad_init(void);
unsigned char key_released(void);
unsigned char key_pressed(void);
unsigned char key_scan(void);

/* global variables declaration */
unsigned char table[13] = {
    '1', '2', '3',
    '4', '5', '6',
    '7', '8', '9',
    '*', '0', '#'};

unsigned char ports[] = {A, B, C, D};

/* when you want to read a PIN right after writing to PORT you should wait */
const int Delay = 20;

/***********************************************************************/

unsigned char bv(int bit)
{
    return 1 << bit;
}

/***********************************************************************/

void keypad_init(void)
{
    KEY_DDR = bv(A) | bv(B) | bv(C) | bv(D);
    KEY_PORT = bv(A) | bv(B) | bv(C) | bv(D);
    KEY_PIN = bv(R1) | bv(R2) | bv(R3);
}

/***********************************************************************/

unsigned char key_pressed(void)
{

    KEY_PORT = 0;
    KEY_PIN = bv(R1) | bv(R2) | bv(R3);
    _delay_ms(Delay);
    if ((KEY_PIN & (bv(R1) | bv(R2) | bv(R3))) != 0)
    {
        _delay_ms(Delay);
        if ((KEY_PIN & (bv(R1) | bv(R2) | bv(R3))) != 0)
            return 1;
    }
    return 0;
}

/***********************************************************************/

unsigned char key_scan(void)
{
    unsigned char i, key;
    if (key_pressed())
    {
        key = 0;
        for (i = 0; i < 4; i++)
        {
            KEY_PORT = ~bv(ports[i]);
            KEY_PIN = bv(R1) | bv(R2) | bv(R3);
            _delay_ms(Delay);
            if ((KEY_PIN & bv(R1)) == 0)
                key = table[i * 3];
            else if ((KEY_PIN & bv(R2)) == 0)
                key = table[i * 3 + 1];
            else if ((KEY_PIN & bv(R3)) == 0)
                key = table[i * 3 + 2];
        }

        return key;
    }
    else
        return 255;
}