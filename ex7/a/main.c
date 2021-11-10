// Define frequency 1MHz (microcontroller frequency)
#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "./lcd.h"
#include "./keypad.h"

/* prototypes */
int BV(int bit);
void show_present_status();
void action();
void change_status();
void login();
void add_user();
void delete_user();
char str_equals(char *str1, char *str2);
void set_empty_str(char *str);
void str_copy(char *destination, char *target);
char is_admin();

enum Status
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

/* global variables declaration */
char status = OFF;
char previus_status = OFF;
struct User admin = {.username = "1", .password = "1234"};
struct User users[10];
char username[20] = "";
char password[20] = "";
char currentValue[20] = "";
char empty_str[20] = "";
int currentIndex = 0;
unsigned char key = 0;
char success;
char current_username[20] = "";

int main()
{
    keypad_init(); /* init keypad */

    unsigned char previousKey = 0;

    while (1)
    {
        _delay_ms(10);
        show_present_status();

        key = key_scan();
        if (key == 255 || key == 0)
            continue;

        if (key == previousKey)
        {
            previousKey = 0;
            _delay_ms(200);
            continue;
        }

        previousKey = key;

        action();
    }
}

// set specific bit to 1
int BV(int bit)
{
    return 1 << bit;
}

void show_present_status()
{
    if (status == OFF)
        return;
    else if (status == MESSAGE)
    {
        Go_Line(2);
        LCD_String(currentValue);
        PORTB = PORTB & 0x3f;
        if (success == 0)
            PORTB = PORTB | 0x80;
        else
            PORTB = PORTB | 0x40;
    }
    else if (status == LOGIN_USERNAME)
    {
        Go_Line(1);
        LCD_String("-------Login-------");
        Go_Line(2);
        LCD_String("Username:          ");
        Go_Line(3);
        LCD_String(currentValue);
    }
    else if (status == LOGIN_PASSWORD)
    {
        Go_Line(1);
        LCD_String("-------Login-------");
        Go_Line(2);
        LCD_String("Password:          ");
        Go_Line(3);
        LCD_String(currentValue);
    }
    else if (status == ADMIN_OPTIONS_PAGE1)
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
    else if (status == ADMIN_OPTIONS_PAGE2)
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
    else if (status == ADMIN_OPTIONS_PAGE3)
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
    else if (status == ADMIN_DASHBOARD)
    {
        Go_Line(1);
        LCD_String("---Admin dashbord--");
    }
    else if (status == ADMIN_SET_USER_USERNAME)
    {
        Go_Line(1);
        LCD_String("--Add (Edit) user--");
        Go_Line(2);
        LCD_String("Username:          ");
        Go_Line(3);
        LCD_String(currentValue);
    }
    else if (status == ADMIN_SET_USER_PASSWORD)
    {
        Go_Line(1);
        LCD_String("--Set (Edit) user--");
        Go_Line(2);
        LCD_String("Password:          ");
        Go_Line(3);
        LCD_String(currentValue);
    }
    else if (status == ADMIN_DELETE_USER)
    {
        Go_Line(1);
        LCD_String("----Delete user----");
        Go_Line(2);
        LCD_String("Username:          ");
        Go_Line(3);
        LCD_String(currentValue);
    }
    else if (status == ADMIN_EDIT_DATE)
    {
        Go_Line(1);
        LCD_String("-----Edit date-----");
        Go_Line(2);
        LCD_String("Enter date:       ");
        Go_Line(3);
        LCD_String("Format: 2020-09-01 ");
    }
    else if (status == ADMIN_EDIT_CLOCK)
    {
        Go_Line(1);
        LCD_String("-----Edit clock----");
        Go_Line(2);
        LCD_String("Enter clock:       ");
        Go_Line(3);
        LCD_String("Format: 19:09:12   ");
    }
    else if (status == ROTATE_MOTOR)
    {
        Go_Line(1);
        LCD_String("----Rotate motor---");
        Go_Line(2);
        LCD_String("Steps:             ");
    }
    else if (status == USER_OPTIONS_PAGE)
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
    else if (status == USER_DASHBOARD)
    {
        Go_Line(1);
        LCD_String("---User dashbord---");
    }
}

void action()
{
    if (key == 'b' && currentIndex == 0 && status != MESSAGE)
    {
        if (status == LOGIN_PASSWORD)
            status = LOGIN_USERNAME;
        else if (status == ADMIN_OPTIONS_PAGE2)
            status = ADMIN_OPTIONS_PAGE1;
        else if (status == ADMIN_OPTIONS_PAGE3)
            status = ADMIN_OPTIONS_PAGE2;
        else if (status == ADMIN_DASHBOARD)
            status = ADMIN_OPTIONS_PAGE1;
        else if (status == ADMIN_SET_USER_USERNAME)
            status = ADMIN_OPTIONS_PAGE1;
        else if (status == ADMIN_SET_USER_PASSWORD)
            status = ADMIN_SET_USER_USERNAME;
        else if (status == ADMIN_DELETE_USER)
            status = ADMIN_OPTIONS_PAGE2;
        else if (status == ADMIN_EDIT_DATE)
            status = ADMIN_OPTIONS_PAGE2;
        else if (status == ADMIN_EDIT_CLOCK)
            status = ADMIN_OPTIONS_PAGE3;
        else if (status == ROTATE_MOTOR)
            if (is_admin())
                status = ADMIN_OPTIONS_PAGE3;
            else
                status = USER_OPTIONS_PAGE;
        else if (status == USER_DASHBOARD)
            status = USER_OPTIONS_PAGE;
    }
    else if (key == 'b')
    {
        if (currentIndex > 0)
        {
            currentIndex--;
            currentValue[currentIndex] = ' ';
        }
    }
    else if (key != 'e')
    {
        if (currentIndex < 20)
        {
            currentValue[currentIndex] = key;
            currentIndex++;
        }
    }
    if (status == OFF)
    {
        if (key == 'e')
        {
            previus_status = status;
            status = LOGIN_USERNAME;
            change_status();
            LCD_Init(); /* init lcd */
        }
    }
    else if (status == MESSAGE)
    {
        if (previus_status == LOGIN_PASSWORD)
        {
            if (success == 0)
                status = LOGIN_USERNAME;
            else if (is_admin())
                status = ADMIN_OPTIONS_PAGE1;
            else
                status = USER_OPTIONS_PAGE;
        }
        else if (previus_status == ADMIN_SET_USER_PASSWORD)
        {
            status = ADMIN_OPTIONS_PAGE1;
        }
        else if (previus_status == ADMIN_DELETE_USER)
        {
            status = ADMIN_OPTIONS_PAGE2;
        }
        else if (previus_status == ADMIN_EDIT_DATE)
        {
            if (success == 0)
                status = ADMIN_EDIT_DATE;
            else
                status = ADMIN_OPTIONS_PAGE2;
        }
        else if (previus_status == ADMIN_EDIT_CLOCK)
        {
            if (success == 0)
                status = ADMIN_EDIT_CLOCK;
            else
                status = ADMIN_OPTIONS_PAGE3;
        }
        else if (previus_status == ROTATE_MOTOR)
        {
            status = ADMIN_OPTIONS_PAGE3;
        }

        change_status();
        PORTB = PORTB & 0x3f;
    }
    else if (status == LOGIN_USERNAME)
    {
        if (key == 'e')
        {
            str_copy(username, currentValue);
            previus_status = status;
            status = LOGIN_PASSWORD;
            change_status();
        }
    }
    else if (status == LOGIN_PASSWORD)
    {
        if (key == 'e')
        {
            str_copy(password, currentValue);
            login();
        }
    }
    else if (status == ADMIN_OPTIONS_PAGE1)
    {
        if (key == '1')
        {
            previus_status = status;
            status = ADMIN_DASHBOARD;
            change_status();
        }
        if (key == '2')
        {
            previus_status = status;
            status = ADMIN_SET_USER_USERNAME;
            change_status();
        }
        if (key == '3')
        {
            previus_status = status;
            status = ADMIN_OPTIONS_PAGE2;
            change_status();
        }
    }
    else if (status == ADMIN_OPTIONS_PAGE2)
    {
        if (key == '1')
        {
            previus_status = status;
            status = ADMIN_DELETE_USER;
            change_status();
        }
        if (key == '2')
        {
            previus_status = status;
            status = ADMIN_EDIT_DATE;
            change_status();
        }
        if (key == '3')
        {
            previus_status = status;
            status = ADMIN_OPTIONS_PAGE3;
            change_status();
        }
    }
    else if (status == ADMIN_OPTIONS_PAGE3)
    {
        if (key == '1')
        {
            previus_status = status;
            status = ADMIN_EDIT_CLOCK;
            change_status();
        }
        if (key == '2')
        {
            previus_status = status;
            status = ROTATE_MOTOR;
            change_status();
        }
        if (key == '3')
        {
            previus_status = status;
            status = LOGIN_USERNAME;
            str_copy(current_username, empty_str);
            change_status();
        }
    }
    else if (status == ADMIN_SET_USER_USERNAME)
    {
        if (key == 'e')
        {
            str_copy(username, currentValue);
            previus_status = status;
            status = ADMIN_SET_USER_PASSWORD;
            change_status();
        }
    }
    else if (status == ADMIN_SET_USER_PASSWORD)
    {
        if (key == 'e')
        {
            str_copy(password, currentValue);
            add_user();
        }
    }
    else if (status == ADMIN_DELETE_USER)
    {
        if (key == 'e')
        {
            str_copy(username, currentValue);
            delete_user();
        }
    }
    else if (status == USER_OPTIONS_PAGE)
    {
        if (key == '1')
        {
            previus_status = status;
            status = USER_DASHBOARD;
            change_status();
        }
        if (key == '2')
        {
            previus_status = status;
            status = ROTATE_MOTOR;
            change_status();
        }
        if (key == '3')
        {
            previus_status = status;
            status = LOGIN_USERNAME;
            str_copy(current_username, empty_str);
            change_status();
        }
    }
}

void change_status()
{
    LCD_Clear();
    currentIndex = 0;
    set_empty_str(currentValue);
}

void login()
{
    previus_status = status;
    status = MESSAGE;
    change_status();

    success = 0;
    if (str_equals(admin.username, username) && str_equals(admin.password, password))
    {
        str_copy(current_username, admin.username);
        str_copy(currentValue, "Hello dear Admin   ");
        success = 1;
    }

    for (int i = 0; i < 10; i++)
    {
        if (str_equals(users[i].username, username) && str_equals(users[i].password, password))
        {
            str_copy(current_username, users[i].username);
            str_copy(currentValue, "Hello dear User    ");
            success = 1;
            break;
        }
    }

    if (success == 0)
    {
        str_copy(currentValue, "Login failed!      ");
    }
    set_empty_str(username);
    set_empty_str(password);
}

void add_user()
{
    previus_status = status;
    status = MESSAGE;
    change_status();
    for (int i = 0; i < 10; i++)
    {
        if (str_equals(empty_str, users[i].username))
        {
            str_copy(users[i].username, username);
            str_copy(users[i].password, password);

            str_copy(currentValue, "Successfully added ");
            success = 1;
            return;
        }
        else if (str_equals(username, users[i].username))
        {
            str_copy(users[i].username, username);
            str_copy(users[i].password, password);

            str_copy(currentValue, "Successfully edited ");
            success = 1;
            return;
        }
    }
    set_empty_str(username);
    set_empty_str(password);
    str_copy(currentValue, "Memory is full!    ");
    success = 0;
}

void delete_user()
{
    previus_status = status;
    status = MESSAGE;
    change_status();
    for (int i = 0; i < 10; i++)
    {
        if (str_equals(username, users[i].username))
        {
            str_copy(users[i].username, empty_str);
            str_copy(users[i].password, empty_str);

            str_copy(currentValue, "Successfully delete");
            success = 1;
            return;
        }
    }
    set_empty_str(username);
    str_copy(currentValue, "User not found!    ");
    success = 0;
}

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

void set_empty_str(char *str)
{
    for (int i = 0; i < 20; i++)
        str[i] = 0;
}

void str_copy(char *destination, char *target)
{
    for (int i = 0; i < 20; i++)
        destination[i] = target[i];
}

char is_admin()
{
    return str_equals(current_username, admin.username);
}