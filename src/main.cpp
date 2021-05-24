#include <Arduino.h>

/*
PORTD1  LED front (Red)

PORTD4  LED1 (Red)
PORTD5  LED2 (Green)
PORTD6  LED3 (Green)
PORTD7  LED4 (Red)

PORTB5  LED board (red)
*/

void setup()
{
    // pins 2-7 and 13 => outputs
    DDRD |= B11111100;
    DDRB |= B00000100;

    // pins 2-7 and 13 => LOW
    PORTD &= ~(1 << 2) & ~(1 << 3) & ~(1 << 4) & ~(1 << 5) & ~(1 << 6) & ~(1 << 7);
    PORTB &= ~(1 << 5);
}

void loop()
{
    return;
    PORTD |= (1 << 1);
    _delay_ms(500);
    PORTD &= ~(1 << 1);
    _delay_ms(500);
}
