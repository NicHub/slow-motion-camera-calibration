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


# Samsung S21

https://www.androidpolice.com/2021/01/20/samsungs-smaller-cheaper-s21-and-s21-beat-the-ultra-at-slow-mo-video/

https://www.samsung.com/global/galaxy/galaxy-s21-5g/specs/

Super Slow-mo only supports HD reans. On Galaxy S21 5G and S21+ 5G, users can record approximately 0.5 seconds of video captured at 960 fps with approximately 16 seconds of playback. On Galaxy S21 Ultra 5G, users can record approximately 1 second of video captured at 480 fps and digitally enhance the video to 960 fps with approximately 32 seconds of playback. Playback time can be edited in Super Slow-mo player.

# Samsung S20

https://www.samsung.com/ch_fr/smartphones/galaxy-s20/specs/

Slow Motion
960fps @HD, 240fps @FHD

*/

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t COUNTER = 0;
volatile uint8_t PORTB_NEW = 0;
volatile uint8_t PORTD_NEW = 0;

#define DEBUG false

#if DEBUG == true
// For time measurement on the oscilloscope.
#define DELAY_CYCLES(n) __builtin_avr_delay_cycles(n)
#endif

/**
 * Reverse the order of a 16 bit number.
 * It takes ~440 µs to execute.
 */
uint16_t reverse(uint16_t number)
{
    uint16_t ans = 0;
    bitWrite(ans, 0, bitRead(number, 15));
    bitWrite(ans, 1, bitRead(number, 14));
    bitWrite(ans, 2, bitRead(number, 13));
    bitWrite(ans, 3, bitRead(number, 12));
    bitWrite(ans, 4, bitRead(number, 11));
    bitWrite(ans, 5, bitRead(number, 10));
    bitWrite(ans, 6, bitRead(number, 9));
    bitWrite(ans, 7, bitRead(number, 8));
    bitWrite(ans, 8, bitRead(number, 7));
    bitWrite(ans, 9, bitRead(number, 6));
    bitWrite(ans, 10, bitRead(number, 5));
    bitWrite(ans, 11, bitRead(number, 4));
    bitWrite(ans, 12, bitRead(number, 3));
    bitWrite(ans, 13, bitRead(number, 2));
    bitWrite(ans, 14, bitRead(number, 1));
    bitWrite(ans, 15, bitRead(number, 0));
    return ans;
}

/**
 *
 */
ISR(TIMER1_COMPA_vect)
{

#if DEBUG == true
    bitWrite(PORTC, PORTC0, HIGH);
    DELAY_CYCLES(100);
    bitWrite(PORTC, PORTC0, LOW);
#endif

    // Disable global interrupts,
    // Update output values and
    // enable again global interrupts.
    // Duration measured on oscilloscope:
    // ~0.6 µs => 10 clock cycles
    cli();
    PORTB = PORTB_NEW;
    PORTD = PORTD_NEW;
    sei();

#if DEBUG == true
    bitWrite(PORTC, PORTC0, HIGH);
#endif

    // Update counter for next iteration.
    ++COUNTER;

    uint16_t reverse_counter = reverse(COUNTER);
    PORTB_NEW = reverse_counter >> 8;
    PORTD_NEW = reverse_counter; // Only the 8 first bits will be copied.

#if DEBUG == true
    bitWrite(PORTC, PORTC0, LOW);
#endif
}

/**
 *
 */
void setup()
{
    // All pins of PORTB and PORTD set to OUTPUT.
    DDRB = B11111111;
    DDRC = B11111111;
    DDRD = B11111111;

    // All outputs ON.
    PORTB = B11111111;
    PORTC = B11111111;
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
      2.08E-3    1    33332
        2E-03    1    31999
     1.04E-03    1    16666
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

    // All outputs OFF.
    PORTB = B00000000;
    PORTC = B00000000;
    PORTD = B00000000;
}

/**
 *
 */
void loop()
{
}
