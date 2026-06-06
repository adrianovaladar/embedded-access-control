#include <avr/io.h>
#include <util/delay.h>

#define KEYPAD_PIN   PINA
#define KEYPAD_PORT  PORTA
#define KEYPAD_DDR   DDRA


/* Keypad initialization function */
void KEYPADInic(void);


/*
 * Function to read a pressed key from the keypad
 *
 * Layout:
 *   [1]  [2]  [3]  [12]
 *   [4]  [5]  [6]  [13]
 *   [7]  [8]  [9]  [14]
 *   [10] [0]  [11] [15]
 *
 * Returns the value associated with each key as shown above.
 * If no key is pressed, returns 0xFF (255).
 */
unsigned char getKeyPressed(void);


/* Functions */

void KEYPADInic(void) {
    KEYPAD_DDR = 0x0F;
    /* Upper half is configured as input and lower half as output */

    KEYPAD_PORT = 0xFF;
    /* Lower half is set high and upper half has pull-up resistors enabled */
}


/*
 * Each bit of PORTA is connected to a keypad column and each bit of PINA
 * is connected to a keypad row.
 *
 * This function makes a single low bit circulate through PORTA (columns),
 * with the rest set high, switching every 2 microseconds.
 *
 * This allows comparison between PINA and the active low PORTA bit,
 * identifying which row and column are active, and therefore which key
 * is pressed.
 *
 *
 * Column 1        Row (1 to 4)
 * KEYPAD_PORT     | keypad_input |
 * 0b----1110      | 0b1110----   | Col1 x Row1 = 1
 * 0b----1110      | 0b1101----   | Col1 x Row2 = 2
 * 0b----1110      | 0b1011----   | Col1 x Row3 = 3
 * 0b----1110      | 0b0111----   | Col1 x Row4 = 12
 *
 * Column 2        Row (1 to 4)
 * KEYPAD_PORT     | keypad_input |
 * 0b----1101      | 0b1110----   | Col2 x Row1 = 4
 * 0b----1101      | 0b1101----   | Col2 x Row2 = 5
 * 0b----1101      | 0b1011----   | Col2 x Row3 = 6
 * 0b----1101      | 0b0111----   | Col2 x Row4 = 13 (A)
 *
 * Column 3        Row (1 to 4)
 * KEYPAD_PORT     | keypad_input |
 * 0b----1011      | 0b1110----   | Col3 x Row1 = 7
 * 0b----1011      | 0b1101----   | Col3 x Row2 = 8
 * 0b----1011      | 0b1011----   | Col3 x Row3 = 9
 * 0b----1011      | 0b0111----   | Col3 x Row4 = 14 (B)
 *
 * Column 4        Row (1 to 4)
 * KEYPAD_PORT     | keypad_input |
 * 0b----0111      | 0b1110----   | Col4 x Row1 = 10 (*)
 * 0b----0111      | 0b1101----   | Col4 x Row2 = 0
 * 0b----0111      | 0b1011----   | Col4 x Row3 = 11 (#)
 * 0b----0111      | 0b0111----   | Col4 x Row4 = 15 (D)
 */

unsigned char getKeyPressed(void) {
    unsigned char keypad_input = 0xFF, keypad_output = 0xFF;

    KEYPAD_PORT = 0xFE; // 0b11111110
    _delay_us(2);

    keypad_input = KEYPAD_PIN & 0xF0;

    if (keypad_input == 0xE0)
        keypad_output = 0x01;
    else if (keypad_input == 0xD0)
        keypad_output = 0x02;
    else if (keypad_input == 0xB0)
        keypad_output = 0x03;
    else if (keypad_input == 0x70)
        keypad_output = 0x0C;


    KEYPAD_PORT = 0xFD;
    _delay_us(2);

    keypad_input = KEYPAD_PIN & 0xF0;

    if (keypad_input == 0xE0)
        keypad_output = 0x04;
    else if (keypad_input == 0xD0)
        keypad_output = 0x05;
    else if (keypad_input == 0xB0)
        keypad_output = 0x06;
    else if (keypad_input == 0x70)
        keypad_output = 0x0D;


    KEYPAD_PORT = 0xFB;
    _delay_us(2);

    keypad_input = KEYPAD_PIN & 0xF0;

    if (keypad_input == 0xE0)
        keypad_output = 0x07;
    else if (keypad_input == 0xD0)
        keypad_output = 0x08;
    else if (keypad_input == 0xB0)
        keypad_output = 0x09;
    else if (keypad_input == 0x70)
        keypad_output = 0x0E;


    KEYPAD_PORT = 0xF7;
    _delay_us(2);

    keypad_input = KEYPAD_PIN & 0xF0;

    if (keypad_input == 0xE0)
        keypad_output = 0x0A;
    else if (keypad_input == 0xD0)
        keypad_output = 0x00;
    else if (keypad_input == 0xB0)
        keypad_output = 0x0B;
    else if (keypad_input == 0x70)
        keypad_output = 0x0F;

    return keypad_output;
}
