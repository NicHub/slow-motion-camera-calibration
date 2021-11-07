/*

SLOW MOTION CAMERA CALIBRATION

https://github.com/NicHub/slow-motion-camera-calibration

© september 2021, ouilogique.com

*/

#include <Arduino.h>
// #include <avr/io.h>
// #include <avr/interrupt.h>

volatile uint16_t COUNTER = 0;
volatile uint8_t PORTB_NEW = 0;
volatile uint8_t PORTD_NEW = 0;

#define DEBUG true

#if DEBUG == true
// DELAY_CYCLES(n) is for time measurement on the oscilloscope.
//
// https://gcc.gnu.org/onlinedocs/gcc/AVR-Built-in-Functions.html
// Delay execution for ticks cycles.
// Note that this built-in function does not take into account the effect
// of interrupts that might increase delay time.
// ticks must be a compile-time integer constant;
// delays with a variable number of cycles are not supported.
// n = 16000 cycles => delay = 1ms
#define DELAY_CYCLES(n) __builtin_avr_delay_cycles(n)
#endif

/**
 *
 */
ISR(TIMER1_COMPA_vect)
{
#if DEBUG == true
    bitToggle(PORTC, PORTC0);
#endif

    // Disable global interrupts.
    // Update PORTB and PORTD, i.e. display COUNTER binary value with the LEDs.
    // Enable global interrupts again.
    cli();
    PORTB = PORTB_NEW;
    PORTD = PORTD_NEW;
    sei();

    // Increment COUNTER.
    // Split COUNTER in 2 parts, one for PORTB and another for PORTD.
    // These values will be displayed on the next timer interrupt.
    ++COUNTER;
    PORTB_NEW = COUNTER >> (8);
    PORTD_NEW = COUNTER;
}

/**
 *
 */
void configureGpio()
{
    // All pins of PORTB, PORTC and PORTD set to OUTPUT.
    DDRB = B11111111;
    DDRC = B11111111;
    DDRD = B11111111;

    // All outputs ON.
    PORTB = B11111111;
    PORTC = B11111111;
    PORTD = B11111111;
}

/**
 * Explanation of the configuration of timer1 can be found here:
 * https://github.com/NicHub/slow-motion-camera-calibration#explanation-of-how-timer1-works-in-this-program
 */
void configureTimer1()
{
    cli(); // Disable global interrupts.

    TCCR1A = 0b00000000; // Set normal mode of operation.

    TCCR1B = 0b00000000;        //
    bitWrite(TCCR1B, WGM12, 1); // Turn on CTC mode.
    bitWrite(TCCR1B, CS12, 0);  // Prescale factor N.
    bitWrite(TCCR1B, CS11, 0);  //      See datasheet page 110
    bitWrite(TCCR1B, CS10, 1);  //      for values of CS10, CS11 and CS12.

    OCR1A = 16666; // Set compare match register to desired timer count.

    TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt.
    sei();                   // Enable global interrupts.
}

/**
 *
 */
void displayInfoOnSerial()
{
    // Write the settings to serial output.
    // Beware that PORTD0 and PORTD1 are both used for serial
    // communication and for LED GPIOs. That means that they cannot
    // work together and we need to close the serial port as
    // soon as we have written the information below.
    Serial.begin(BAUD_RATE);
    while (!Serial)
        yield();
    Serial.flush();
    Serial.println(F("\n\n\n# SLOW MOTION\n"));
    Serial.print(F("FILE NAME:        "));
    Serial.println(F(__FILE__));
    Serial.print(F("COMPILATION DATE: "));
    Serial.println(F(COMPILATION_DATE));
    Serial.print(F("COMPILATION TIME: "));
    Serial.println(F(COMPILATION_TIME));
    Serial.print(F("C++ version:      "));
    if (__cplusplus == 201103L)
        Serial.println(F("C++ 11"));
    else
        Serial.println(__cplusplus);
    Serial.println("");

    Serial.print(F("TCCR1A: B"));
    Serial.println(TCCR1A, BIN);
    Serial.print(F("TCCR1B: B"));
    Serial.println(TCCR1B, BIN);
    Serial.print(F("TIMSK1: B"));
    Serial.println(TIMSK1, BIN);
    Serial.print(F("OCIE1A: B"));
    Serial.println(OCIE1A, BIN);
    Serial.println("");

#define prescaler                   \
    bitRead(TCCR1B, CS10) * 1 +     \
        bitRead(TCCR1B, CS11) * 2 + \
        bitRead(TCCR1B, CS12) * 4

#define period_ms \
    prescaler / F_CPU *(OCR1A + 1)

    Serial.print(F("OCR1A:             "));
    Serial.println(OCR1A);
    Serial.print(F("PRESCALER N:       "));
    Serial.println(prescaler);
    Serial.print(F("CPU PERIOD (ns):   "));
    Serial.println(1.0 / float(F_CPU) * 1E9, 2);
    Serial.print(F("CPU FREQ (MHz):    "));
    Serial.println(F_CPU / 1E6);
    Serial.print(F("TIMER PERIOD (ms): "));
    Serial.println(period_ms * 1000.0, 6);
    Serial.print(F("TIMER FREQ (Hz):   "));
    Serial.println(1 / period_ms, 1);
    Serial.println(F(""));
    Serial.println(F("Type CTRL+C to close the connection."));
    Serial.println(F(""));
    Serial.end();
}

/**
 *
 */
void switchOffAllGpios()
{
    // All outputs OFF.
    PORTB = B00000000;
    PORTC = B00000000;
    PORTD = B00000000;
}

/**
 *
 */
void setup()
{
    configureGpio();
    configureTimer1();
    displayInfoOnSerial();
    switchOffAllGpios();
}

/**
 *
 */
void loop()
{
    // Everything is done in the timer ISR routine.
    // So there is nothing here.
}
