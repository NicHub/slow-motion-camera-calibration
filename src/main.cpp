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

#define DECIMAL2GRAY(dec_num) (dec_num ^ dec_num >> 1)

volatile uint16_t COUNTER_DEC = 0;
volatile uint16_t COUNTER_GRAY = 0;
volatile uint8_t PORTB_NEW = 0;
volatile uint8_t PORTD_NEW = 0;

/**
 *
 */
ISR(TIMER1_COMPA_vect)
{
    bitWrite(PORTB_NEW, PORTB5, bitRead(COUNTER_GRAY, 0));
    bitWrite(PORTB_NEW, PORTB4, bitRead(COUNTER_GRAY, 1));
    bitWrite(PORTB_NEW, PORTB3, bitRead(COUNTER_GRAY, 2));
    bitWrite(PORTB_NEW, PORTB2, bitRead(COUNTER_GRAY, 3));
    bitWrite(PORTB_NEW, PORTB1, bitRead(COUNTER_GRAY, 4));
    bitWrite(PORTB_NEW, PORTB0, bitRead(COUNTER_GRAY, 5));

    bitWrite(PORTD_NEW, PORTD7, bitRead(COUNTER_GRAY, 6));
    bitWrite(PORTD_NEW, PORTD6, bitRead(COUNTER_GRAY, 7));
    bitWrite(PORTD_NEW, PORTD5, bitRead(COUNTER_GRAY, 8));

    cli(); // Disable global interrupts.
    PORTB = PORTB_NEW;
    PORTD = PORTD_NEW;
    sei(); // Enable global interrupts.

    ++COUNTER_DEC;
    COUNTER_GRAY = DECIMAL2GRAY(COUNTER_DEC);
}

/**
 *
 */
void setup()
{
    // All pins of PORTB and PORT set to OUTPUT.
    DDRD = B11111111;
    DDRB = B11111111;

    // All ON.
    PORTD = B11111111;
    PORTB = B11111111;

    /*

   Initialize Timer1.

   t = N / clkIO × ( 1 + OCR1A )

   t: interval between each interrupt (s)
   N: prescale factor (-)
   clkIO: clock frequency (Hz), 16 MHz on regular UNO boards
   OCR1A: output compare 1A used as TOP value

    for t = 1 s
    CS12 CS11 CS10 = 1 0 1 => prescale factor N = 1024
    OCR1A = 15624
    t = 1024 / 16E6 × ( 1 + 15624 ) = 1

    for t = 4 ms
    CS12 CS11 CS10 = 0 0 1 => prescale factor N = 1
    OCR1A = 63999
    t = 1 / 16E6 × ( 1 + 63999 ) = 4E-3

    t        N    OCRA1
    ===================
    4E-03    1    63999
    3E-03    1    47999
    2E-03    1    31999
    1E-03    1    15624

    */

    cli();               // Disable global interrupts.
    TCCR1A = 0b00000000; // Set normal mode of operation.

    TCCR1B = 0b00000000;        //
    bitWrite(TCCR1B, WGM12, 1); // Turn on CTC mode.
    bitWrite(TCCR1B, CS12, 0);  // Prescale factor N.
    bitWrite(TCCR1B, CS11, 0);  // See datasheet page 110
    bitWrite(TCCR1B, CS10, 1);  // for values of CS10, CS11 and CS12.

    OCR1A = 15624;           // Set compare match register to desired timer count.
    TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt.
    sei();                   // Enable global interrupts.
}

/**
 *
 */
void loop()
{
}
