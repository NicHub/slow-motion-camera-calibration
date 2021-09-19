/*

CAMERA SLOW MOTION CALIBRATION

https://github.com/NicHub/camera-slow-motion-calibration

© september 2021, ouilogique.com

*/

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t COUNTER_NUMBER = 0;
volatile uint16_t COUNTER_FRAME = 0;
volatile uint8_t PORTB_NEW = 0;
volatile uint8_t PORTD_NEW = 0;

#define DEBUG true

#if DEBUG == true
// For time measurement on the oscilloscope.
#define DELAY_CYCLES(n) __builtin_avr_delay_cycles(n)
#endif

/**
 * Reverse the bit order of a 16-bit number.
 * It takes ~2.25 µs to execute (~36 clock cycles).
 * This is measured by toggling the PORTC1 pin
 * and measuring the time the signal is LOW.
 * With my old oscilloscope I need to set OCR1A=200
 * to be able to see the signal.
 */
uint16_t reverse_bit_order(uint16_t number)
{
#if DEBUG == true
    bitWrite(PORTC, PORTC1, LOW);
#endif
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
#if DEBUG == true
    bitWrite(PORTC, PORTC1, HIGH);
#endif
    return ans;
}

/**
 *
 */
volatile uint16_t number = 0;
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
    ++COUNTER_FRAME;

    uint16_t reverse_counter = reverse_bit_order(COUNTER_NUMBER);
    PORTB_NEW = reverse_counter >> (8 + 2); // PORTB has only 6 GPIOs. So we need to
                                            // shift the result by (8-6) = 2 bits.
    PORTD_NEW = reverse_counter >> (0 + 2); // Only the 8 first bits will be copied.

    if (COUNTER_FRAME % 5 != 0)
    {
        PORTB_NEW = B00000000;
        PORTD_NEW = B00000000;
    }
    else
    {
        ++COUNTER_NUMBER;
    }

#if DEBUG == true
    bitWrite(PORTC, PORTC0, LOW);
#endif
}

/**
 *
 */
void configure_gpio()
{
    // All pins of PORTB and PORTD set to OUTPUT.
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
 * https://github.com/NicHub/camera-slow-motion-calibration#explanation-of-how-timer1-works-in-this-program
 */
void configure_timer1()
{
    cli(); // Disable global interrupts.

    TCCR1A = 0b00000000; // Set normal mode of operation.
    TCCR1B = 0b00000000; //

    bitWrite(TCCR1B, WGM12, 1); // Turn on CTC mode.

    bitWrite(TCCR1B, CS12, 0); // Prescale factor N.
    bitWrite(TCCR1B, CS11, 0); //      See datasheet page 110
    bitWrite(TCCR1B, CS10, 1); //      for values of CS10, CS11 and CS12.

    OCR1A = 16666; // Set compare match register to desired timer count.

    TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt.
    sei();                   // Enable global interrupts.
}

/**
 *
 */
void display_info_on_serial()
{
    // Write the settings to serial output.
    // Beware that PORTD 0 and 1 are used both by the serial
    // communication and the LEDs. That means that they cannot
    // work together and we need to close the serial port as
    // soon as we have written the information below.
    Serial.begin(BAUD_RATE);
    Serial.println(F("\n\n\n# CAMERA SLOW MOTION CALIBRATION\n"));
    Serial.print(F("FILE NAME:        "));
    Serial.println(__FILE__);
    Serial.print(F("COMPILATION DATE: "));
    Serial.println(COMPILATION_DATE);
    Serial.print(F("COMPILATION TIME: "));
    Serial.println(COMPILATION_TIME);
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

    float prescaler =
        bitRead(TCCR1B, CS10) * 1 +
        bitRead(TCCR1B, CS11) * 2 +
        bitRead(TCCR1B, CS12) * 4;
    float period_ms = prescaler / F_CPU * (OCR1A + 1);

    Serial.print(F("OCR1A:           "));
    Serial.println(OCR1A);
    Serial.print(F("PRESCALER N:     "));
    Serial.println(prescaler);
    Serial.print(F("PERIOD CPU (ns): "));
    Serial.println(1.0 / float(F_CPU) * 1E9, 2);
    Serial.print(F("F CPU (MHz):     "));
    Serial.println(F_CPU / 1E6);
    Serial.print(F("PERIOD LED (ms): "));
    Serial.println(period_ms * 1000.0, 6);
    Serial.print(F("F LED (Hz):      "));
    Serial.println(1 / period_ms, 1);
    Serial.print(F(""));

    Serial.end();
}

/**
 *
 */
void switch_off_all_gpios()
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
    configure_gpio();
    configure_timer1();
    display_info_on_serial();
    switch_off_all_gpios();
}

/**
 *
 */
void loop()
{
}
