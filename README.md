# CAMERA SLOW MOTION CALIBRATION

<p align="center">
<img width=100% src="https://raw.githubusercontent.com/NicHub/camera-slow-motion-calibration/main/images/camera-slow-motion-calibration-001.jpg" />
</p>

## ABSTRACT

The goal of this project is to calibrate the slow motion frame rate of a smartphone.
14 LEDs are displaying a counter that counts from 0 to 16383 in binary.
The counter is updated at 1 ms interval with a timer.

## SETUP

This project works only with ATmega328P microcontrolers (ARDUINO UNO, Nano, Micro...).
Connect the LEDs to output 0 to 13

## ATmega328P datasheet

http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

## ARTICLE ABOUT TIMERS (in French)

https://www.locoduino.org/spip.php?article84


## Samsung S21

https://www.androidpolice.com/2021/01/20/samsungs-smaller-cheaper-s21-and-s21-beat-the-ultra-at-slow-mo-video/

https://www.samsung.com/global/galaxy/galaxy-s21-5g/specs/

Super Slow-mo only supports HD reans. On Galaxy S21 5G and S21+ 5G, users can record approximately 0.5 seconds of video captured at 960 fps with approximately 16 seconds of playback. On Galaxy S21 Ultra 5G, users can record approximately 1 second of video captured at 480 fps and digitally enhance the video to 960 fps with approximately 32 seconds of playback. Playback time can be edited in Super Slow-mo player.

## Samsung S20

https://www.samsung.com/ch_fr/smartphones/galaxy-s20/specs/

Slow Motion
960fps @HD, 240fps @FHD


## EXPLANATION OF HOW TIMER1 WORKS IN THIS PROGRAM

Timer1 is a 16 bit timer, that means it can count from 0 to (2^16 - 1).
So basically it will count during 2^16 / FCPU = 4.10E-03 s before overflowing and returning to 0 and generate an interrupt that the program can handle in the ISR procedure (Interrupt Service Request).
There are two mechanisms to tune the time at which the timer overflows :

1. THE PRESCALE FACTOR N

This permits to divide the timer speed compared to the clock.
Basicaly, the timer is incremented each N clock cycle.
N can take the following values: 1, 8, 64, 256, 1024
For each value N, we can calculate the longest possible time before the timer overflows:

    tmax = N / FCPU × 2^16

        N         tmax (s)
    =====================
        1        4.10E-03
        8        32.7E-03
       64         262E-03
      256        1.05E+00
     1024        4.19E+00

2. THE OUTPUT COMPARE REGISTER

Instead of counting up to the maximum value, which is MAX = N × 2^16, we can configure the timer to overflow at a smaller value called TOP.
This TOP value is set in the Output Compare Register OCRA1.
The time it takes for the timer to overflow is then:

    t = N / FCPU × ( 1 + OCR1A )


## THE CAMERA CALIBATION APPLICATION

The cameras I want to calibrate have a frame rate of 960 fps, so each frame lasts 1/960 = 1.042E-03 s.
This value is smaller than 4.10E-03 s in the table above, so we can use a prescaler N = 1.

Next we can calculate the TOP value to be stored in OCRA1:

    OCRA1 = ROUND( 1 / FRAME_RATE / N * FCPU - 1)
    OCRA1 = ROUND( 1 / 960 / 1 * 16E6 - 1)
    OCRA1 = 16666
