/*
******************************************************************************
 Datei:       I2C_LCD_LIB.cc
 Version:     0.1
 Autor:       Moritz Habegger & Schena Micha
 Datum:       04.04.2014
 Funktion:    Benutzung des I2C-LCD für Textausgabe und Einstellungen
******************************************************************************
*/

// variables
byte LCD_Address, prefix, command, parameter, specialCommand;
/*
*   specialCommand is, that the Method LCD_SET_CURSOR works with position 1,1
*   which leads the parameter to be 0x00, which says the I2C_SEND,
*   that no parameter is set -> specialCommand to verify it.
*/

// initialises I2C LCD
void LCD_INIT(void)
{
    LCD_Address = 0x50;
    prefix = 0xFE;
    command = 0x00;
    parameter = 0x00;
    specialCommand = 0x00;

    I2C_Init(I2C_100kHz);

    LCD_DISPLAY_ON();
    LCD_SET_BACKLIGHT_BRIGHTNESS(2);
    LCD_CLEAN_SCREEN();
}

// turn on LCD screen
void LCD_DISPLAY_ON(void)
{
    command = 0x41;
    I2C_SEND();
}

// turn off LCD screen
void LCD_DISPLAY_OFF(void)
{
    command = 0x42;
    I2C_SEND();
}

/* put cursor at location pos
*  line 1 => 0x00 - 0x0F [columns 1 - 16]
*  line 2 => 0x40 - 0x4F [columns 1 - 16]
*/
void LCD_SET_CURSOR(int row, int position)
{
    command = 0x45;
    parameter = (row-1) * 0x40 + (position - 1);
    specialCommand = 0x01;
    I2C_SEND();
}

// position cursor at line 1 column 1
void LCD_CURSOR_HOME(void)
{
    command = 0x46;
    I2C_SEND();
}

// turn on underline cursor
void LCD_CURSOR_UNDERLINE_ON(void)
{
    command = 0x47;
    I2C_SEND();
}

// turn off underline cursor
void LCD_CURSOR_UNDERLINE_OFF(void)
{
    command = 0x48;
    I2C_SEND();
}

// move cursor left 1 space
void LCD_CURSOR_MOVE_LEFT(void)
{
    command = 0x49;
    I2C_SEND();
}

// move cursor right 1 space
void LCD_CURSOR_MOVE_RIGHT(void)
{
    command = 0x4A;
    I2C_SEND();
}

// turn on the blinking cursor
void LCD_CURSOR_BLINK_ON(void)
{
    command = 0x4B;
    I2C_SEND();
}

// turn off the blinking cursor
void LCD_CURSOR_BLINK_OFF(void)
{
    command = 0x4C;
    I2C_SEND();
}

// move cursor back one space, delete last character
void LCD_CURSOR_BACKSPACE(void)
{
    command = 0x4E;
    I2C_SEND();
}

// clear LCD and move cursor to line 1 column 1
void LCD_CLEAN_SCREEN(void)
{
    command = 0x51;
    I2C_SEND();
}

// set the display contrast, value between 1 and 50
void LCD_SET_DISPLAY_CONTRAST(int val)
{
    command = 0x52;
    parameter = val;
    I2C_SEND();
}

// set the backlight brightness level, value between 1 and 8
void LCD_SET_BACKLIGHT_BRIGHTNESS(int val)
{
    command = 0x53;
    parameter = val;
    I2C_SEND();
}

// custom characters...
void LCD_CREATE_CHAR(void)
{
    // pending..
}

// shift the LCD screen to the left 1 space
void LCD_MOVE_DISPLAY_LEFT(void)
{
    command = 0x55;
    I2C_SEND();
}

// shift the LCD screen to the right 1 space
void LCD_MOVE_DISPLAY_RIGHT(void)
{
    command = 0x56;
    I2C_SEND();
}

// change RS-232 BAUD rate - USE CAREFULLY!
void LCD_SET_RS232_BAUD_RATE(int val)
{
    command = 0x61;
    parameter = val;
    I2C_SEND();
}

// change I2C address - USE CAREFULLY!
void LCD_SET_I2C_ADDRESS(int val)
{
    command = 0x62;
    parameter = val;
    I2C_SEND();
}

// display the firmware version number
void LCD_DISPLAY_FIRMWARE_VERSION(void)
{
    command = 0x70;
    I2C_SEND();
}

// display RS-232 BAUD rate
void LCD_DISPLAY_RS232_BAUD_RATE(void)
{
    command = 0x71;
    I2C_SEND();
}

// display I2C address
void LCD_DISPLAY_I2C_ADDRESS(void)
{
    command = 0x72;
    I2C_SEND();
}

// writes text from char to LCD
void LCD_WRITE_CHAR(char text)
{
    parameter = text;
    I2C_SEND();
}

// writes text from char-array to LCD
void LCD_WRITE_TEXT(char text[])
{
    int i;
    i = 0;
    while(text[i] != 0)
    {
        parameter = text[i];
        I2C_SEND();
        i++;
    }
}

// writes float to LCD
void LCD_WRITE_FLOAT(float val, int len)
{
    char text[16];
    Str_WriteFloat(val, len, text, 0);
    int i;
    i = 0;
    while(text[i] != 0)
    {
        parameter = text[i];
        I2C_SEND();
        i++;
    }
}

//!! rhy  writes word to LCD
void LCD_WRITE_WORD(word val, int len)
{
    char text[16];
    Str_WriteWord(val, 10, text, 0, len);
    int i;
    i = 0;
    while(text[i] != 0)
    {
        parameter = text[i];
        I2C_SEND();
        i++;
    }
}

//!! rhy  writes integer to LCD
void LCD_WRITE_INT(int val)
{
    char text[16];
    Str_WriteInt(val, text, 0);
    int i;
    i = 0;
    while(text[i] != 0)
    {
        parameter = text[i];
        I2C_SEND();
        i++;
    }
}

// sends commands and parameters over I2C
void I2C_SEND(void)
{
    // command mode with prefix & parameter
    if(command > 0x00 && (parameter > 0x00 || specialCommand > 0x00))
    {
        I2C_Start();
        I2C_Write(LCD_Address);
        I2C_Write(prefix);
        I2C_Write(command);
        I2C_Write(parameter);
        I2C_Stop();
    }
    // command mode with prefix without parameter
    else if(command > 0x00)
    {
        I2C_Start();
        I2C_Write(LCD_Address);
        I2C_Write(prefix);
        I2C_Write(command);
        I2C_Stop();
    }
    // text mode only
    else
    {
        I2C_Start();
        I2C_Write(LCD_Address);
        I2C_Write(parameter);
        I2C_Stop();
    }
    // reset variables
    command = 0x00;
    parameter = 0x00;
    specialCommand = 0x00;
    AbsDelay(5);
}

