// Define frequency 1MHz (microcontroller frequency)
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <stdio.h>
#include "./lcd.h"
#include "./keypad.h"

/******************************** prototypes ********************************/
void display_lcd();
void display_led();
void take_action(unsigned char key);
void change_page(char new_page);
char is_admin();
void login();
void add_user();
void delete_user();
void edit_date();
void edit_clock();
void rotate_motor();
char str_equals(char *str1, char *str2);
void str_copy(char *destination, char *target, int first, int size);
void set_date(char *str);
void set_clock(char *str);
unsigned char BV(int bit);

enum Pages
{
    OFF,
    MESSAGE,
    LOGIN_USERNAME,
    LOGIN_PASSWORD,
    ADMIN_OPTIONS_PAGE1,
    ADMIN_OPTIONS_PAGE2,
    ADMIN_OPTIONS_PAGE3,
    ADMIN_DASHBOARD,
    ADMIN_SET_USER_USERNAME,
    ADMIN_SET_USER_PASSWORD,
    ADMIN_DELETE_USER,
    ADMIN_EDIT_DATE,
    ADMIN_EDIT_CLOCK,
    ROTATE_MOTOR,
    USER_OPTIONS_PAGE,
    USER_DASHBOARD
};

struct User
{
    char username[20], password[20];
};

/************************ global variables declaration ************************/
int overflow_count = 0;         /* counter of overflows in timer_0 */
int overflow_max_count = 4 - 1; /* number of timer_0 overflows to reach frequency 1Hz
                                        2^20 clock frequency, 1024 prescale, 2^8 count to overflow
                                            => ((2^20 / 1024) / 2^8) / 1 = 4 */

long long time = 0;
char page = OFF;
char previus_page = OFF;
struct User admin = {.username = "1", .password = "1234"};
struct User users[5];
char username[20] = "";
char password[20] = "";
char current_value[20] = "";
int current_index = 0;
char message[20] = "";
char success;
char current_username[20] = "";
char empty_str[20] = "";
const int minute_time = 60;
const int hour_time = 60 * 60;
const long day_time = (long)(60 * 60) * 24;
const long month_time = ((long)(60 * 60) * 24) * 30;
const long year_time = ((long)(60 * 60) * 24) * 30 * 12;
int temp = 0;

/************************************ main ************************************/
int main()
{
    TIMSK = BV(TOIE0);           /* unmask overflow interrupt for timer_0 */
    TCCR0 = BV(CS00) | BV(CS02); /* set timer_0 frequency to clock/1024 */

    UCSRB = BV(RXEN) | BV(TXEN); /* Enable RX and TX */
    UCSRB |= BV(RXCIE);          /* Enable RX interrupt */
    UBRRL = 25;                  /* 2400 bit rate */

    sei(); /* enable global interrupt */

    keypad_init(); /* init keypad */
    unsigned char key = 0;
    unsigned char previus_key = 0;
    long long time_snapshot = 0;

    while (1)
    {
        _delay_ms(10);
        display_lcd();
        display_led();

        key = key_scan();
        if (key == 255 || key == 0)
            continue;

        if (key == previus_key && (time == time_snapshot))
            continue;

        previus_key = key;
        time_snapshot = time;
        take_action(key);
    }

    return 0;
}

/************************ timer_0 overflow interrupt **************************/
ISR(TIMER0_OVF_vect)
{
    overflow_count++;
    if (overflow_count >= overflow_max_count)
    {
        time++;
        overflow_count = 0;
    }
}

/******************************* RX interrupt *********************************/
ISR(USART_RXC_vect)
{
    temp = UDR;
}

/********************************* functions **********************************/
void display_led()
{
    PORTB = PORTB & 0x3f;
    if (page == MESSAGE)
    {
        if (success == 0)
            PORTB = PORTB | 0x80;
        else
            PORTB = PORTB | 0x40;
    }
}

/******************************************************************************/
void display_lcd()
{
    if (page == OFF)
        return;
    else if (page == MESSAGE)
    {
        Go_Line(2);
        LCD_String(message);
    }
    else if (page == LOGIN_USERNAME)
    {
        Go_Line(1);
        LCD_String("-------Login-------");
        Go_Line(2);
        LCD_String("Username:          ");
        Go_Line(3);
        LCD_String(current_value);
    }
    else if (page == LOGIN_PASSWORD)
    {
        Go_Line(1);
        LCD_String("-------Login-------");
        Go_Line(2);
        LCD_String("Password:          ");
        Go_Line(3);
        LCD_String(current_value);
    }
    else if (page == ADMIN_OPTIONS_PAGE1)
    {
        Go_Line(1);
        LCD_String("-------Admin-------");
        Go_Line(2);
        LCD_String("1. Dashbord        ");
        Go_Line(3);
        LCD_String("2. Add (Edit) user ");
        Go_Line(4);
        LCD_String("3. Next page       ");
    }
    else if (page == ADMIN_OPTIONS_PAGE2)
    {
        Go_Line(1);
        LCD_String("-------Admin-------");
        Go_Line(2);
        LCD_String("1. Delete user     ");
        Go_Line(3);
        LCD_String("2. Edit date       ");
        Go_Line(4);
        LCD_String("3. Next page       ");
    }
    else if (page == ADMIN_OPTIONS_PAGE3)
    {
        Go_Line(1);
        LCD_String("-------Admin-------");
        Go_Line(2);
        LCD_String("1. Edit clock      ");
        Go_Line(3);
        LCD_String("2. Rotate the motor");
        Go_Line(4);
        LCD_String("3. Log out         ");
    }
    else if (page == ADMIN_DASHBOARD)
    {
        Go_Line(1);
        LCD_String("---Admin dashbord--");
        Go_Line(2);
        set_date(current_value);
        LCD_String(current_value);
        Go_Line(3);
        set_clock(current_value);
        LCD_String(current_value);
        Go_Line(4);
        sprintf(current_value, "Temp: %4d C        ", temp);
        LCD_String(current_value);
    }
    else if (page == ADMIN_SET_USER_USERNAME)
    {
        Go_Line(1);
        LCD_String("--Add (Edit) user--");
        Go_Line(2);
        LCD_String("Username:          ");
        Go_Line(3);
        LCD_String(current_value);
    }
    else if (page == ADMIN_SET_USER_PASSWORD)
    {
        Go_Line(1);
        LCD_String("--Set (Edit) user--");
        Go_Line(2);
        LCD_String("Password:          ");
        Go_Line(3);
        LCD_String(current_value);
    }
    else if (page == ADMIN_DELETE_USER)
    {
        Go_Line(1);
        LCD_String("----Delete user----");
        Go_Line(2);
        LCD_String("Username:          ");
        Go_Line(3);
        LCD_String(current_value);
    }
    else if (page == ADMIN_EDIT_DATE)
    {
        Go_Line(1);
        LCD_String("-----Edit date-----");
        Go_Line(2);
        LCD_String("Enter date:       ");
        Go_Line(3);
        LCD_String("Format: 2020-09-01 ");
        Go_Line(4);
        LCD_String(current_value);
    }
    else if (page == ADMIN_EDIT_CLOCK)
    {
        Go_Line(1);
        LCD_String("-----Edit clock----");
        Go_Line(2);
        LCD_String("Enter clock:       ");
        Go_Line(3);
        LCD_String("Format: 19:09:12   ");
        Go_Line(4);
        LCD_String(current_value);
    }
    else if (page == ROTATE_MOTOR)
    {
        Go_Line(1);
        LCD_String("----Rotate motor---");
        Go_Line(2);
        LCD_String("Steps (0-255):     ");
        Go_Line(3);
        LCD_String(current_value);
    }
    else if (page == USER_OPTIONS_PAGE)
    {
        Go_Line(1);
        LCD_String("--------User-------");
        Go_Line(2);
        LCD_String("1. Dashboard       ");
        Go_Line(3);
        LCD_String("2. Rotate motor    ");
        Go_Line(4);
        LCD_String("3. Log out         ");
    }
    else if (page == USER_DASHBOARD)
    {
        Go_Line(1);
        LCD_String("---User dashbord---");
        Go_Line(2);
        set_date(current_value);
        LCD_String(current_value);
        Go_Line(3);
        set_clock(current_value);
        LCD_String(current_value);
        Go_Line(4);
        sprintf(current_value, "Temp: %4d C        ", temp);
        LCD_String(current_value);
    }
}

/******************************************************************************/
void take_action(unsigned char key)
{
    if (key == '*' && current_index == 0 && page != MESSAGE)
    {
        if (page == LOGIN_PASSWORD)
            page = LOGIN_USERNAME;
        else if (page == ADMIN_OPTIONS_PAGE2)
            page = ADMIN_OPTIONS_PAGE1;
        else if (page == ADMIN_OPTIONS_PAGE3)
            page = ADMIN_OPTIONS_PAGE2;
        else if (page == ADMIN_DASHBOARD)
            page = ADMIN_OPTIONS_PAGE1;
        else if (page == ADMIN_SET_USER_USERNAME)
            page = ADMIN_OPTIONS_PAGE1;
        else if (page == ADMIN_SET_USER_PASSWORD)
            page = ADMIN_SET_USER_USERNAME;
        else if (page == ADMIN_DELETE_USER)
            page = ADMIN_OPTIONS_PAGE2;
        else if (page == ADMIN_EDIT_DATE)
            page = ADMIN_OPTIONS_PAGE2;
        else if (page == ADMIN_EDIT_CLOCK)
            page = ADMIN_OPTIONS_PAGE3;
        else if (page == ROTATE_MOTOR)
            if (is_admin())
                page = ADMIN_OPTIONS_PAGE3;
            else
                page = USER_OPTIONS_PAGE;
        else if (page == USER_DASHBOARD)
            page = USER_OPTIONS_PAGE;
    }

    else if (key == '*')
    {
        if (current_index > 0)
        {
            current_index--;
            current_value[current_index] = ' ';
        }
    }

    else if (key != '#')
    {
        if (current_index < 20)
        {
            current_value[current_index] = key;
            current_index++;
        }
    }

    if (page == OFF)
    {
        if (key == '#')
        {
            change_page(LOGIN_USERNAME);
            LCD_Init(); /* init lcd */
        }
    }

    else if (page == MESSAGE)
    {
        if (previus_page == LOGIN_PASSWORD)
        {
            if (success == 0)
                change_page(LOGIN_USERNAME);
            else if (is_admin())
                change_page(ADMIN_OPTIONS_PAGE1);
            else
                change_page(USER_OPTIONS_PAGE);
        }

        else if (previus_page == ADMIN_SET_USER_PASSWORD)
            change_page(ADMIN_OPTIONS_PAGE1);

        else if (previus_page == ADMIN_DELETE_USER)
            change_page(ADMIN_OPTIONS_PAGE2);

        else if (previus_page == ADMIN_EDIT_DATE)
        {
            if (success == 0)
                change_page(ADMIN_EDIT_DATE);
            else
                change_page(ADMIN_OPTIONS_PAGE2);
        }

        else if (previus_page == ADMIN_EDIT_CLOCK)
        {
            if (success == 0)
                change_page(ADMIN_EDIT_CLOCK);
            else
                change_page(ADMIN_OPTIONS_PAGE3);
        }

        else if (previus_page == ROTATE_MOTOR)
            change_page(ADMIN_OPTIONS_PAGE3);
    }

    else if (page == LOGIN_USERNAME)
    {
        if (key == '#')
        {
            str_copy(username, current_value, 0, 20);
            change_page(LOGIN_PASSWORD);
        }
    }

    else if (page == LOGIN_PASSWORD)
    {
        if (key == '#')
        {
            str_copy(password, current_value, 0, 20);
            login();
        }
    }

    else if (page == ADMIN_OPTIONS_PAGE1)
    {
        if (key == '1')
            change_page(ADMIN_DASHBOARD);
        if (key == '2')
            change_page(ADMIN_SET_USER_USERNAME);
        if (key == '3')
            change_page(ADMIN_OPTIONS_PAGE2);
    }

    else if (page == ADMIN_OPTIONS_PAGE2)
    {
        if (key == '1')
            change_page(ADMIN_DELETE_USER);
        if (key == '2')
            change_page(ADMIN_EDIT_DATE);
        if (key == '3')
            change_page(ADMIN_OPTIONS_PAGE3);
    }

    else if (page == ADMIN_OPTIONS_PAGE3)
    {
        if (key == '1')
            change_page(ADMIN_EDIT_CLOCK);
        if (key == '2')
            change_page(ROTATE_MOTOR);
        if (key == '3')
        {
            change_page(LOGIN_USERNAME);
            str_copy(current_username, empty_str, 0, 20);
        }
    }
    else if (page == ADMIN_SET_USER_USERNAME)
    {
        if (key == '#')
        {
            str_copy(username, current_value, 0, 20);
            change_page(ADMIN_SET_USER_PASSWORD);
        }
    }
    else if (page == ADMIN_SET_USER_PASSWORD)
    {
        if (key == '#')
        {
            str_copy(password, current_value, 0, 20);
            add_user();
        }
    }
    else if (page == ADMIN_DELETE_USER)
    {
        if (key == '#')
        {
            str_copy(username, current_value, 0, 20);
            delete_user();
        }
    }
    else if (page == ADMIN_EDIT_DATE)
    {
        if (key == '#')
            edit_date();
        else if (key == '*')
        {
            if (current_index == 4 || current_index == 7)
            {
                current_index--;
                current_value[current_index] = ' ';
            }
        }
        else
        {
            if (current_index == 4 || current_index == 7)
            {
                current_value[current_index] = '-';
                current_index++;
            }
        }
    }
    else if (page == ADMIN_EDIT_CLOCK)
    {
        if (key == '#')
            edit_clock();
        else if (key == '*')
        {
            if (current_index == 2 || current_index == 5)
            {
                current_index--;
                current_value[current_index] = ' ';
            }
        }
        else
        {
            if (current_index == 2 || current_index == 5)
            {
                current_value[current_index] = ':';
                current_index++;
            }
        }
    }
    else if (page == USER_OPTIONS_PAGE)
    {
        if (key == '1')
            change_page(USER_DASHBOARD);
        if (key == '2')
            change_page(ROTATE_MOTOR);
        if (key == '3')
        {
            change_page(LOGIN_USERNAME);
            str_copy(current_username, empty_str, 0, 20);
        }
    }
    else if (page == ROTATE_MOTOR)
    {
        if (key == '#')
            rotate_motor();
    }
}

/******************************************************************************/
void change_page(char new_page)
{
    previus_page = page;
    page = new_page;
    LCD_Clear();
    current_index = 0;
    str_copy(current_value, empty_str, 0, 20);
}

/******************************************************************************/
void login()
{
    change_page(MESSAGE);

    success = 0;

    if (!str_equals(username, empty_str))
    {
        if (str_equals(admin.username, username) && str_equals(admin.password, password))
        {
            str_copy(current_username, admin.username, 0, 20);
            str_copy(message, "Hello dear Admin   ", 0, 20);
            success = 1;
        }

        for (int i = 0; i < 10; i++)
        {
            if (str_equals(users[i].username, username) && str_equals(users[i].password, password))
            {
                str_copy(current_username, users[i].username, 0, 20);
                str_copy(message, "Hello dear User    ", 0, 20);
                success = 1;
                break;
            }
        }
    }

    if (success == 0)
    {
        str_copy(message, "Login failed!      ", 0, 20);
    }
    str_copy(username, empty_str, 0, 20);
    str_copy(password, empty_str, 0, 20);
}

/******************************************************************************/
void add_user()
{
    change_page(MESSAGE);
    for (int i = 0; i < 10; i++)
    {
        if (str_equals(empty_str, users[i].username))
        {
            str_copy(users[i].username, username, 0, 20);
            str_copy(users[i].password, password, 0, 20);

            str_copy(message, "Successfully added ", 0, 20);
            success = 1;
            return;
        }
        else if (str_equals(username, users[i].username))
        {
            str_copy(users[i].username, username, 0, 20);
            str_copy(users[i].password, password, 0, 20);

            str_copy(message, "Successfully edited ", 0, 20);
            success = 1;
            return;
        }
    }
    str_copy(username, empty_str, 0, 20);
    str_copy(password, empty_str, 0, 20);
    str_copy(message, "Memory is full!    ", 0, 20);
    success = 0;
}

/******************************************************************************/
void delete_user()
{
    change_page(MESSAGE);
    for (int i = 0; i < 10; i++)
    {
        if (str_equals(username, users[i].username))
        {
            str_copy(users[i].username, empty_str, 0, 20);
            str_copy(users[i].password, empty_str, 0, 20);

            str_copy(message, "Successfully delete", 0, 20);
            success = 1;
            return;
        }
    }
    str_copy(username, empty_str, 0, 20);
    str_copy(message, "User not found!    ", 0, 20);
    success = 0;
}

/******************************************************************************/
void edit_date()
{
    int year, month, day;
    sscanf(current_value, "%4d-%2d-%2d", &year, &month, &day);
    month--;
    day--;
    time = ((long long)year * year_time) + (month * month_time) + (day * day_time) + (time % day_time);
    str_copy(message, "Date changed!      ", 0, 20);
    success = 1;
    change_page(MESSAGE);
}

/******************************************************************************/
void edit_clock()
{
    int hour, minute, second;
    sscanf(current_value, "%2d:%2d:%2d", &hour, &minute, &second);
    time = ((long long)hour * hour_time) + (minute * minute_time) + second + ((time / day_time) * day_time);
    str_copy(message, "Clock changed!     ", 0, 20);
    success = 1;
    change_page(MESSAGE);
}

/******************************************************************************/
void rotate_motor()
{
    int steps;
    sscanf(current_value, "%3d", &steps);
    UDR = (unsigned char)steps;
    
    str_copy(message, "Rotate motor       ", 0, 20);
    success = 1;
    change_page(MESSAGE);
}

/******************************************************************************/
char str_equals(char *str1, char *str2)
{
    char equal = 1;
    for (int i = 0; i < 20; i++)
    {
        if (((str1[i] != 0) && (str1[i] != ' ')) || ((str2[i] != 0) && (str2[i] != ' ')))
            if (str1[i] != str2[i])
                equal = 0;
    }
    return equal;
}

/******************************************************************************/
void str_copy(char *destination, char *target, int first, int size)
{
    for (int i = 0; i < size; i++)
        destination[first + i] = target[i];
}

/******************************************************************************/
char is_admin()
{
    return str_equals(current_username, admin.username);
}

/******************************************************************************/
void set_date(char *str)
{
    int years = time / year_time;
    int months = (time % year_time) / month_time + 1;
    int days = (time % month_time) / day_time + 1;

    sprintf(str, "Date: %4d-%2d-%2d  ", years, months, days);
}

/******************************************************************************/
void set_clock(char *str)
{
    int hours = (time % day_time) / hour_time;
    int minutes = (time % hour_time) / minute_time;
    int seconds = time % minute_time;

    sprintf(str, "Clock: %4d:%2d:%2d ", hours, minutes, seconds);
}

/******************************************************************************/
unsigned char BV(int bit)
{
    return 1 << bit;
}