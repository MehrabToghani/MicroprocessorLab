#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#define LCD_Data_Dir DDRD      /* Define LCD data port direction */
#define LCD_Command_Dir DDRC   /* Define LCD command port direction register */
#define LCD_Data_Port PORTD    /* Define LCD data port */
#define LCD_Command_Port PORTC /* Define LCD data port */
#define RS PC0                 /* Define Register Select (data/command reg.)pin */
#define RW PC1                 /* Define Read/Write signal pin */
#define EN PC2                 /* Define Enable signal pin */

void LCD_Command(unsigned char cmnd)
{
    LCD_Data_Port = cmnd;
    LCD_Command_Port &= ~(1 << RS); /* RS=0 command reg. */
    LCD_Command_Port &= ~(1 << RW); /* RW=0 Write operation */
    LCD_Command_Port |= (1 << EN);  /* Enable pulse */
    _delay_us(1);
    LCD_Command_Port &= ~(1 << EN);
    _delay_ms(3);
}

void LCD_Init(void) /* LCD Initialize function */
{
    LCD_Command_Dir = 0xFF; /* Make LCD command port direction as o/p */
    LCD_Data_Dir = 0xFF;    /* Make LCD data port direction as o/p */

    _delay_ms(20);     /* LCD Power ON delay always >15ms */
    LCD_Command(0x38); /* Initialization of 16X4 LCD in 8bit mode */
    LCD_Command(0x0C); /* Display ON Cursor OFF */
    LCD_Command(0x06); /* Auto Increment cursor */
    LCD_Command(0x01); /* clear display */
    LCD_Command(0x80); /* cursor at home position */
}

void LCD_Char(unsigned char char_data) /* LCD data write function */
{
    LCD_Data_Port = char_data;
    LCD_Command_Port |= (1 << RS);  /* RS=1 Data reg. */
    LCD_Command_Port &= ~(1 << RW); /* RW=0 write operation */
    LCD_Command_Port |= (1 << EN);  /* Enable Pulse */
    _delay_us(1);
    LCD_Command_Port &= ~(1 << EN);
    _delay_ms(1);
}

void LCD_String(char *str)
{
    for (int i = 0; str[i] != 0; i++) /* send each char of string till the NULL */
    {
        LCD_Char(str[i]); /* call LCD data write */
    }
}

void LCD_Clear()
{
    LCD_Command(0x01); /* clear display */
    LCD_Command(0x80); /* cursor at home position */
}

int main()
{
    DDRA = 0;            // set PORT A for input
    DDRB = DDRB & ~0x01; // set first bit of PORT B for input

    char data[200];

    for (int i = 0; i < 200; i++)
    {
        while ((PINB & 0x01) == 0)
            _delay_ms(1);
        data[i] = PINA; // read data from pin A and store it
        while ((PINB & 0x01) != 0)
            _delay_ms(1);
    }

    LCD_Init(); /* Initialize LCD */

    char Force_2st_line = 0xC0; /* Force cursor to beginning to 2st line */
    char Force_3st_line = 0x90; /* Force cursor to beginning to 3st line */
    char Force_4st_line = 0xD0; /* Force cursor to beginning to 4st line */

    int line = 1;

    for (int i = 0; i < 200;)
    {
        switch (line)
        {
        case 1:
            LCD_Clear();
            break;
        case 2:
            LCD_Command(Force_2st_line);
            break;
        case 3:
            LCD_Command(Force_3st_line);
            break;
        case 4:
            LCD_Command(Force_4st_line);
            break;
        default:
            break;
        }

        const int message_size = 16;
        char message[message_size]; // this message show on one line in LCD
        for (int j = 0; j < message_size; j++)
        {
            if (i + j < 200)
                message[j] = data[i + j];
            else
                message[j] = ' ';
        }

        LCD_String(message);

        line = line % 4 + 1;
        i += message_size;

        _delay_ms(1000);
    }

    while (1)
        ;

    return 0;
}