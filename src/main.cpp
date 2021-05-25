/*
https://www.locoduino.org/spip.php?article84


http://www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/



PORTD1  LED front (Red)

PORTD4  LED1 (Red)
PORTD5  LED2 (Green)
PORTD6  LED3 (Green)
PORTD7  LED4 (Red)

PORTB5  LED board (red)
*/

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define LEDPIN PORTB5
volatile bool trigger;

int main()
{
    // pins 2-7 and 13 => outputs
    DDRD |= B11111100;
    DDRB |= B00100000;

    // pins 2-7 and 13 => LOW
    PORTD &= ~(1 << 2) & ~(1 << 3) & ~(1 << 4) & ~(1 << 5) & ~(1 << 6) & ~(1 << 7);
    PORTB &= ~(1 << 5);

    // initialize Timer1
    cli();                  // disable global interrupts
    TCCR1A = 0b00000000;    // set entire TCCR1A register to 0
    TCCR1B = 0b00000000     //
             | (1 << WGM12) // turn on CTC mode:
             | (1 << CS10)  // Set CS10 and CS12 bits for 1024 prescaler:
             | (1 << CS11);
    OCR1A = 15624/2;           // set compare match register to desired timer count:
    TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt:
    sei();                   // enable global interrupts

    while (true)
    {
    }
}

ISR(TIMER1_COMPA_vect)
{
    static byte seconds;
    seconds++;
    if (seconds == 1)
    {
        bitWrite(PORTB, LEDPIN, !bitRead(PINB, LEDPIN));
        seconds = 0;
        // OCR1A -= 100;
        // if( OCR1A < 1000 )
        //     OCR1A = 15624;
    }
}
