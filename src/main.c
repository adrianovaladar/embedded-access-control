#include <avr/io.h>
#define F_CPU 1000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "lcd.h"
#include "keypad.h"
#include "TIMER0.h"

#define TRUE    1
#define FALSE   0

unsigned int newYear = 0, newMonth = 0, newDay = 0, newHour = 0, newMinute = 0, newSecond = 0;
unsigned int failedAttempts;
unsigned int year = 2016;
unsigned int month = 1;
unsigned int day = 1;
unsigned int hours = 0;
unsigned int minutes = 0;
unsigned int seconds = 0;
unsigned int decisecond = 0;
unsigned int printPos = 1;
unsigned int savePos = 0;
unsigned int ds5 = 0;
unsigned int ID = 0;
unsigned char adminOn = 0;
const char months[13][10] =
{
    " \0", "JAN\0", "FEB\0", "MAR\0", "APR\0", "MAY\0", "JUN\0", "JUL\0", "AUG\0", "SEP\0", "OCT\0", "NOV\0", "DEC\0"
};
int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

struct Log {
    int ID;
    char seconds;
    char minutes;
    char hours;
    char day;
    char month;
    int year;
} logs;

struct Log Logs[99];

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////Function prototypes//////////////////////////////////////////////////////////////
void init(); //Initialization

int determineLeapYear(); //Function to determine a leap year
void Clock(); //Clock function
void printDate(); //Function to print time

unsigned int validatePassword(unsigned int passwordEntered, unsigned int inputID); //Password validator
unsigned int calcPass4(unsigned int key, unsigned int position, unsigned int currentVal); //Number grouper
unsigned int calcPass2(unsigned int key, unsigned int position, unsigned int currentVal); //Number grouper

uint8_t readEEPROM(uint16_t uiAddress);

void writeEEPROM(uint16_t uiAddress, uint8_t ucData);

void WritePassToEEPROM(uint16_t pass, uint16_t ID);

uint16_t readPassFromEEPROM(uint16_t ID);

unsigned int par(unsigned int number);

unsigned int getPassword(); //Function of password introduction
unsigned int getUserId(); //Function of collection of address (ID user), where the password is stored in EEPROM

void openDoor(); //Lock opening function
void closeDoor(); //Lock closing function
void insertDate();

void insertYear();

void insertMonth();

void insertDay();

void insertHour();

void insertMinute();

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////INITIALIZATION////////////////////////////////////////////////////////
void init() {
    DDRD = 0xff;
    PORTD = 0;
    DDRC = 0xff;
    PORTC = 0b01000000;
    timer0inic(); //timer initialization
    KEYPADInic(); //keypad initialization
    LCDInit(0); //LCD initialization
    LCDClear(); //Clear LCD
    WritePassToEEPROM(1234, 0000);
    /*while(1)
    {
        LCDWriteInt(GetKeyPressed(),3);
        _delay_ms(20);
        LCDClear();
    }*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////TIMER0 INTERRUPTION////////////////////////////////////////////////////
ISR(TIMER0_COMPA_vect) //Handler of Timer0 interruption
{
    ds5++;
    if (ds5 == 5) {
        if (PORTD == 0x00)
            PORTD = 0xff;
        else
            PORTD = 0x00;
        ds5 = 0;
    }
    decisecond++; //each iteration lasts ~1/10 seconds
    Clock();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////TIME///////////////////////////////////////////////////////////////
int determineLeapYear(int y) {
    if ((y % 4 == FALSE && y % 100 != FALSE) || y % 400 == FALSE) {
        daysInMonth[2] = 29;
        return TRUE;
    }
    daysInMonth[2] = 28;
    return FALSE;
}

void Clock() {
    if (decisecond > 10) {
        decisecond = 0;
        seconds++; //because a second has passed
    }
    if (seconds > 59) //
    {
        seconds = 0;
        minutes++; //because a minute has passed
    }
    if (minutes > 59) {
        minutes = 0;
        hours++; //...
    }
    if (hours > 23) {
        hours = 0;
        day++; //...
    }
    if (day > daysInMonth[month]) {
        day = 1;
        determineLeapYear(year);
        month++; //...
    }
    if (month == 12) //...
    {
        month = 1;
        year++; //...
    }
}

void printDate() {
    LCDWriteIntXY(0, 0, year, 4); //writes in position x,y the year with 4 digits
    LCDWriteString("/");
    LCDWriteString(months[month]); //writes the current month
    LCDWriteString("/");
    LCDWriteInt(day, 2); //writes the day
    LCDWriteString(" ");
    //LCDWriteString(":");
    LCDWriteIntXY(4, 1, hours, 2); //writes hours
    LCDWriteString(":");
    LCDWriteInt(minutes, 2); //writes minutes
    LCDWriteString(":");
    LCDWriteInt(seconds, 2); //writes seconds
}

void saveLog() {
    Logs[savePos].minutes = minutes;
    Logs[savePos].hours = hours;
    Logs[savePos].day = day;
    Logs[savePos].month = month;
    Logs[savePos].year = year;
    Logs[savePos].seconds = seconds;
    Logs[savePos].ID = ID;
}

void printLog() {
    LCDClear();
    LCDWriteInt(Logs[printPos].day, 2);
    LCDWriteString("/");
    LCDWriteString(months[Logs[printPos].month]);
    LCDWriteString("/");
    LCDWriteInt(Logs[printPos].year, 4);
    LCDWriteIntXY(0, 1, Logs[printPos].hours, 2);
    LCDWriteString(":");
    LCDWriteInt(Logs[printPos].minutes, 2);
    LCDWriteString(":");
    LCDWriteInt(Logs[printPos].seconds, 2);
    LCDWriteString(" ID:");
    LCDWriteInt(Logs[printPos].ID, 4);
}

void insertYear() {
    int key, position = 0;
    newYear = 0;
newyearinput:
    LCDClear();
    LCDWriteString("Year:");
    LCDWriteInt(newYear, 4);
    while (getKeyPressed() == 225 || getKeyPressed() >= 10) {
        _delay_ms(5);
    }
    key = getKeyPressed();
    while (getKeyPressed() == key) {
        _delay_ms(5);
    }

    newYear = calcPass4(key, position, newYear);
    position++;
    LCDClear();
    LCDWriteInt(newYear, 4);
    if (position < 4) {
        goto newyearinput;
    }
    year = newYear;
}

void insertMonth() {
    int key, position;
repeatmonthinput:
    position = 0;
    newMonth = 0;
newmonthinput:
    LCDClear();
    LCDWriteString("Month:");
    LCDWriteInt(newMonth, 2);
    while (getKeyPressed() == 225 || getKeyPressed() >= 10) {
        _delay_ms(5);
    }
    key = getKeyPressed();
    while (getKeyPressed() == key) {
        _delay_ms(5);
    }

    newMonth = calcPass2(key, position, newMonth);
    position++;
    LCDClear();
    LCDWriteInt(newMonth, 2);
    if (position < 2) {
        goto newmonthinput;
    }
    if (newMonth == 0 || newMonth > 12)
        goto repeatmonthinput;
    month = newMonth;
}

void insertDay() {
    int key, position;
repeatdayinput:
    position = 0;
    newDay = 0;
newdayinput:
    LCDClear();
    LCDWriteString("Day:");
    LCDWriteInt(newDay, 2);
    while (getKeyPressed() == 225 || getKeyPressed() >= 10) {
        _delay_ms(5);
    }
    key = getKeyPressed();
    while (getKeyPressed() == key) {
        _delay_ms(5);
    }

    newDay = calcPass2(key, position, newDay);
    position++;
    LCDClear();
    LCDWriteInt(newDay, 2);
    if (position < 2) {
        goto newdayinput;
    }
    determineLeapYear(year);
    if (newDay == 0 || newDay > daysInMonth[newMonth])
        goto repeatdayinput;
    day = newDay;
}

void insertHour() {
    int key, position;
repeathourinput:
    position = 0;
    newHour = 0;
newhourinput:
    LCDClear();
    LCDWriteString("Hour:");
    LCDWriteInt(newHour, 2);
    while (getKeyPressed() == 225 || getKeyPressed() >= 10) {
        _delay_ms(5);
    }
    key = getKeyPressed();
    while (getKeyPressed() == key) {
        _delay_ms(5);
    }

    newHour = calcPass2(key, position, newHour);
    position++;
    LCDClear();
    LCDWriteInt(newHour, 2);
    if (position < 2) {
        goto newhourinput;
    }
    if (newHour > 23)
        goto repeathourinput;
    hours = newHour;
}

void insertMinute() {
    int key, position;
repeatminuteinput:
    position = 0;
    newMinute = 0;
newminuteinput:
    LCDClear();
    LCDWriteString("Minutes:");
    LCDWriteInt(newMinute, 2);
    while (getKeyPressed() == 225 || getKeyPressed() >= 10) {
        _delay_ms(5);
    }
    key = getKeyPressed();
    while (getKeyPressed() == key) {
        _delay_ms(5);
    }

    newMinute = calcPass2(key, position, newMinute);
    position++;
    LCDClear();
    LCDWriteInt(newMinute, 2);
    if (position < 2) {
        goto newminuteinput;
    }
    if (newMinute > 59)
        goto repeatminuteinput;
    minutes = newMinute;
}

void insertDate() {
    insertYear();
    insertMonth();
    insertDay();
    insertHour();
    insertMinute();
    seconds = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////EEPROM///////////////////////////////////////////////////////////////
uint8_t readEEPROM(uint16_t uiAddress) {
    /* Wait for completion of previous write */
    while (EECR & (1 << EEPE));
    /* Set up address register */
    EEAR = uiAddress;
    /* Start eeprom read by writing EERE */
    EECR |= (1 << EERE);
    /* Return data from data register */
    return EEDR;
}

void writeEEPROM(uint16_t uiAddress, uint8_t ucData) {
    /* Wait for completion of previous write */
    while (EECR & (1 << EEPE));
    /* Set up address and data registers */
    EEAR = uiAddress;
    EEDR = ucData;
    /* Write logical one to EEMWE */
    EECR |= (1 << EEMPE);
    /* Start eeprom write by setting EEWE */
    EECR |= (1 << EEPE);
}

void WritePassToEEPROM(uint16_t pass, uint16_t ID) {
    writeEEPROM(ID, (pass % 100));
    writeEEPROM(ID + 1, (pass / 100));
}

uint16_t readPassFromEEPROM(uint16_t ID) {
    return (readEEPROM(ID + 1) * 100 + readEEPROM(ID));
}

unsigned int par(unsigned int number) {
    if (number % 2 == 0)
        return 1;
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////PASSWORD CONTROL///////////////////////////////////////////////////////

unsigned int calcPass4(unsigned int key, unsigned int position, unsigned int currentVal) {
    unsigned multi = 0;
    if (position == 0)
        multi = 1000;
    else if (position == 1)
        multi = 100;
    else if (position == 2)
        multi = 10;
    else if (position == 3)
        multi = 1;

    return currentVal + key * multi;
}

unsigned int calcPass2(unsigned int key, unsigned int position, unsigned int currentVal) {
    unsigned multi = 0;
    if (position == 0)
        multi = 10;
    else if (position == 1)
        multi = 1;
    return currentVal + key * multi;
}

unsigned int getPassword() {
    unsigned int key;
    unsigned int password;
    unsigned int position;
start:
    password = 0;
    position = 0;

    while (position < 4) {
        while (getKeyPressed() != 255) {
            key = getKeyPressed();
            while (key == getKeyPressed()) { _delay_ms(10); }
            if (key < 10) {
                password = calcPass4(key, position, password);
                LCDWriteStringXY(position, 1, "*");
                position++;
                _delay_ms(50);
            } else if (key == 10) {
                goto start;
            }
        }
    }
    return password;
}

unsigned int validatePassword(unsigned int passwordEntered, unsigned int inputID) {
    if (passwordEntered == readPassFromEEPROM(inputID)) {
        _delay_ms(500);
        ID = inputID;
        if (inputID == 0) {
            LCDClear();
            LCDWriteString("WELCOME");
            LCDWriteStringXY(4, 1, "ADMIN");
            _delay_ms(1500);
            adminOn = 1;
        } else {
            LCDClear();
            LCDWriteString("WELCOME");
            LCDWriteStringXY(4, 1, "GUEST");
            _delay_ms(1500);
            adminOn = 0;
        }
        return 1;
    }
    if (passwordEntered != readPassFromEEPROM(inputID)) {
        LCDClear();
        adminOn = 0;
        return 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////DOOR CONTROL////////////////////////////////////////////////////////
void openDoor() {
    LCDClear();
    //Opens lock (GREEN LED)
    PORTC = 0b10000000;
    LCDWriteStringXY(0, 0, "Door");
    LCDWriteStringXY(4, 1, "OPEN");
    _delay_ms(6000); //Open lock for 6 seconds
    LCDClear();
}

void closeDoor() {
    PORTC = 0b01000000;
    LCDWriteStringXY(0, 0, "Door");
    LCDWriteStringXY(4, 1, "CLOSED");
    _delay_ms(1000);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////MAIN////////////////////////////////////////////////////////////////////
int main(void) {
    unsigned int key = getKeyPressed();

    init();

    LCDClear();
    while (1) {
        int password;
        int inputID;
        adminOn = 0;
    back:
        LCDClear();
        _delay_ms(50);
        key = getKeyPressed();
        _delay_ms(50);
        while (key == getKeyPressed()) {
            _delay_ms(50);
            printDate();
        }
        if (key < 10) {
            goto back;
        }
        LCDClear();
        LCDWriteString("ID:");
        inputID = getPassword();
        if (par(inputID) == 0) {
            goto back;
        }
        LCDClear();
        LCDWriteString("Code:");
        password = getPassword();
        if (validatePassword(password, inputID) == 0) {
            LCDClear();
            LCDWriteString("Wrong password");
            _delay_ms(500);
            goto back;
        }
        _delay_ms(50);
        _delay_ms(50);
        LCDClear();
        if (adminOn == 1) {
            LCDWriteString("Menu-");
            LCDWriteStringXY(4, 1, "A/B/C/D");
        } else {
            LCDWriteString("Menu-");
            LCDWriteStringXY(4, 1, "A");
        }
        while (getKeyPressed() == 255 || getKeyPressed() < 10) {
            _delay_ms(50);
        }
        key = getKeyPressed();
        switch (key) {
            case 12: //PRESSED A - THE KEY TO OPEN DOOR
                openDoor();
                closeDoor();
                savePos++;
                saveLog();
                break;
            case 13: //PRESSED B - LIST ACCESSES
                if (adminOn == 0)
                    goto back;
                while (getKeyPressed() == 13) {
                    _delay_ms(2);
                }
                while (1) {
                    switch (getKeyPressed()) {
                        ////////////////////////////////////////////////////
                        case 4: //PRESSED 4 - GO DOWN IN THE LIST OF REGISTERS
                            if (printPos == 1 || printPos == 0) {
                                printPos++;
                            }
                            printPos--;
                            LCDClear();
                            LCDWriteInt(printPos, 2);
                            while (getKeyPressed() == 4) {
                                _delay_us(500);
                            }
                            LCDClear();
                            break;
                        ///////////////////////////////////////////////////
                        case 1: //PRESSED 1 - GO UP IN THE LIST OF REGISTERS
                            if (printPos == savePos) {
                                printPos--;
                            }
                            printPos++;
                            LCDClear();
                            LCDWriteInt(printPos, 2);
                            while (getKeyPressed() == 1) {
                                _delay_us(500);
                            }
                            LCDClear();
                            break;
                        ////////////////////////////////////////////////////
                        case 0: //PRESSED 0 - DELETE REGISTERS
                            if (adminOn == 0)
                                goto back;
                            savePos = 0;
                            printPos = 0;
                            break;
                        //////////////////////////////////////////////////
                        case 13:
                            while (getKeyPressed() == 13) //PRESSED B 2 TIMES - LEAVE
                            {
                                _delay_us(1);
                            }
                            goto back;
                            break;

                        default:
                            LCDClear();
                            printLog();
                            _delay_ms(500);
                            break;
                    }
                }
                break;
            case 14: //PRESSED C - CHANGE PASSWORD OR ADD USER IF THE USER PRESSING THE KEY IS AN ADMIN
                if (adminOn == 0)
                    goto back;
                if (adminOn == 1) {
                redo:
                    LCDClear();
                    LCDWriteString("ID to change:");
                    inputID = getPassword();
                    if (par(inputID) == 0) {
                        goto redo;
                    }
                    LCDClear();
                    LCDWriteString("New code:");
                    password = getPassword();
                    WritePassToEEPROM(password, inputID);
                    LCDClear();
                    LCDWriteStringXY(0, 0, "ID:");
                    LCDWriteInt(inputID, 4);
                    LCDWriteStringXY(4, 1, "Password:")
                    LCDWriteInt(password, 4);
                    break;
                }
            case 15: //PRESSED D - CHANGE DATE/HOURS
                if (adminOn == 0)
                    goto back;
                insertDate();
                break;
            default:
                break;
        }
    }
}
