/*

CAMERA SLOW MOTION CALIBRATION

# DESCRIPTION OF THE PROJECT

The goal of this project is to calibrate the slow motion frame rate of a smartphone.
14 LEDs are displaying a counter that counts from 0 to 16383 in binary.
The counter is updated at 1 ms interval with a timer.

# SETUP

This project works only with ATmega328P microcontrolers (ARDUINO UNO, Nano, Micro...).
Connect the LEDs to output 0 to 13

# ATmega328P datasheet

http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

# ARTICLE ABOUT TIMERS (in French)

https://www.locoduino.org/spip.php?article84


*/

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t COUNTER = 0;
volatile uint8_t PORTB_NEW = 0;
volatile uint8_t PORTD_NEW = 0;

/**
 *
 */
ISR(TIMER1_COMPA_vect)
{
    bitWrite(PORTB_NEW, 5, bitRead(COUNTER, 0));
    bitWrite(PORTB_NEW, 4, bitRead(COUNTER, 1));
    bitWrite(PORTB_NEW, 3, bitRead(COUNTER, 2));
    bitWrite(PORTB_NEW, 2, bitRead(COUNTER, 3));
    bitWrite(PORTB_NEW, 1, bitRead(COUNTER, 4));
    bitWrite(PORTB_NEW, 0, bitRead(COUNTER, 5));

    bitWrite(PORTD_NEW, 7, bitRead(COUNTER, 6));
    bitWrite(PORTD_NEW, 6, bitRead(COUNTER, 7));
    bitWrite(PORTD_NEW, 5, bitRead(COUNTER, 8));
    bitWrite(PORTD_NEW, 4, bitRead(COUNTER, 9));
    bitWrite(PORTD_NEW, 3, bitRead(COUNTER, 10));
    bitWrite(PORTD_NEW, 2, bitRead(COUNTER, 11));
    bitWrite(PORTD_NEW, 1, bitRead(COUNTER, 12));
    bitWrite(PORTD_NEW, 0, bitRead(COUNTER, 13));

    cli(); // Disable global interrupts.
    PORTB = PORTB_NEW;
    PORTD = PORTD_NEW;
    sei(); // Enable global interrupts.

    ++COUNTER;
}

/**
 *
 */
void setup()
{
    // All pins of PORTB and PORTD set to OUTPUT.
    DDRB = B11111111;
    DDRD = B11111111;

    // All ON.
    PORTB = B11111111;
    PORTD = B11111111;

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

    for t = 1 ms
    CS12 CS11 CS10 = 0 0 1 => prescale factor N = 1
    OCR1A = 15999
    t = 1 / 16E6 × ( 1 + 15999 ) = 1E-3

        t        N    OCRA1
    ========================
        4E-03    1    63999
        3E-03    1    47999
        2E-03    1    31999
        1E-03    1    15999
     0.50E-03    1    7999
     0.25E-03    1    3999

    */

    cli();                      // Disable global interrupts.
    TCCR1A = 0b00000000;        // Set normal mode of operation.
    TCCR1B = 0b00000000;        //
    bitWrite(TCCR1B, WGM12, 1); // Turn on CTC mode.
    bitWrite(TCCR1B, CS12, 0);  // Prescale factor N.
    bitWrite(TCCR1B, CS11, 0);  // See datasheet page 110
    bitWrite(TCCR1B, CS10, 1);  // for values of CS10, CS11 and CS12.
    OCR1A = 16666;              // Set compare match register to desired timer count.
    TIMSK1 |= (1 << OCIE1A);    // Enable timer compare interrupt.
    sei();                      // Enable global interrupts.
}

/**
 *
 */
void loop()
{
}
