#include <avr/io.h>
#include <util/delay.h>

#ifndef LCD_Data_Dir
#define LCD_Data_Dir DDRA /* Define LCD data port direction */
#endif

#ifndef LCD_Command_Dir
#define LCD_Command_Dir DDRB /* Define LCD command port direction register */
#endif

#ifndef LCD_Data_Port
#define LCD_Data_Port PORTA /* Define LCD data port */
#endif

#ifndef LCD_Command_Port
#define LCD_Command_Port PORTB /* Define LCD command port */
#endif

#ifndef RS
#define RS PB2 /* Define Register Select (data/command reg.)pin */
#endif

#ifndef RW
#define RW PB1 /* Define Read/Write signal pin */
#endif

#ifndef EN
#define EN PB0 /* Define Enable signal pin */
#endif

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

void Go_Line(char line)
{
    switch (line)
    {
    case 4:
        LCD_Command(0xD4); /* go line 4 */
        break;
    case 3:
        LCD_Command(0x94); /*go line 3 */
        break;
    case 2:
        LCD_Command(0xC0); /* go line 2 */
        break;
    default:
        LCD_Command(0x80); /*go line 1 */
        break;
    }
}