# CAMERA SLOW MOTION CALIBRATION

<p align="center">
<img width=100% src="https://raw.githubusercontent.com/NicHub/camera-slow-motion-calibration/main/images/camera-slow-motion-calibration-001.jpg" />
</p>

## ABSTRACT

The goal of this project is to calibrate the slow motion frame rate of a smartphone.
14 LEDs are displaying a counter that counts from 0 to 16383 in binary.
The counter is updated at 960 Hz by a timer.

## SETUP

This project works only with ATmega328P microcontrolers (ARDUINO UNO, Nano, Micro...).
Connect the LEDs to output 0 to 13.

## ATmega328P datasheet

http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf

## ARTICLE ABOUT TIMERS (in French)

https://www.locoduino.org/spip.php?article84

## Samsung S21 ULTRA 5G

https://www.androidpolice.com/2021/01/20/samsungs-smaller-cheaper-s21-and-s21-beat-the-ultra-at-slow-mo-video/

https://www.samsung.com/global/galaxy/galaxy-s21-5g/specs/

Super Slow-mo only supports HD resolution. On Galaxy S21 5G and S21+ 5G, users can record approximately 0.5 seconds of video captured at 960 fps with approximately 16 seconds of playback. On Galaxy S21 Ultra 5G, users can record approximately 1 second of video captured at 480 fps and digitally enhance the video to 960 fps with approximately 32 seconds of playback. Playback time can be edited in Super Slow-mo player.

## Samsung S20

https://www.samsung.com/ch_fr/smartphones/galaxy-s20/specs/

Slow Motion
960fps @HD, 240fps @FHD


## EXPLANATION OF HOW TIMER1 WORKS IN THIS PROGRAM

Timer1 is a 16-bit timer, which means that it can count from 0 to (2^16 - 1).
So it will count for 4.10E-03 s (2^16 / FCPU) before it overflows and returns to 0 and generates an interrupt that the program can handle in the ISR (Interrupt Service Request) procedure.
There are two mechanisms for setting the time when the timer overflows:

1. THE PRESCALE FACTOR N

The pre-scale factor N is used to divide the timer speed from the clock speed.
This is achieved by incrementing the timer every N clock cycles.
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

Instead of counting up to the maximum value, which is `MAX = N × 2^16`, we can configure the timer to overflow at a smaller value called TOP.
This TOP value is set in the Output Compare Register OCRA1.
The time it takes for the timer to overflow is then:

    t = N / FCPU × ( 1 + OCR1A )


## THE CAMERA CALIBATION APPLICATION

The cameras I want to calibrate have a frame rate of 960 fps, so each frame lasts `1/960 = 1.042E-03 s`.
This value is smaller than `4.10E-03 s` in the table above, so we can use a prescaler `N = 1`.

Next we can calculate the TOP value to be stored in OCRA1:

    OCRA1 = ROUND( 1 / CAMERA_FRAME_RATE / N * FCPU - 1 )
    OCRA1 = ROUND( 1 / 960 / 1 * 16E6 - 1 )
    OCRA1 = 16666

Finaly, we can verify that the error on the final value is acceptable.

    ARDUINO_FRAME_RATE = 1 / ( ( OCRA1 + 1 ) * N / FCPU )
    ARDUINO_FRAME_RATE = 1 / ( ( 16666 + 1 ) * 1 / 16E6 )
    ARDUINO_FRAME_RATE = 959.980

    ERROR_ABS = ARDUINO_FRAME_RATE - CAMERA_FRAME_RATE
    ERROR_ABS = 959.980 - 960
    ERROR_ABS = 0.02

    ERROR_REL = ARDUINO_FRAME_RATE / CAMERA_FRAME_RATE - 1
    ERROR_REL = 959.980 / 960 - 1
    ERROR_REL = -20 ppm

We can see that after 1 s, which is the acquisition time of the cameras, the difference (jitter) between them and the Arduino will be by -0.02 frame (-20 ppm).

## ANALYSIS

The analysis of the movie is done with a Python 3 script `analysis/analysis.py`.

Run it once to extract the images of the movie. You can kill it after it has generated the first image.

Then use the html file `analysis/get-px-coordinates.html` to get the coordinates of the LED on one picture.

Modify the Python script with the correct coordinates and run it again.

The results are stored in `analysis/camera-calib.csv`.

Open the spreadsheet `analysis/camera-slow-motion-calibration.ods`.

Duplicate an existing analysis sheet and copy-paste your results in this new sheet.

<p align="center">
<img width=100% src="https://raw.githubusercontent.com/NicHub/camera-slow-motion-calibration/main/images/20210920_171411-flip-1-000011.jpg" />
</p>

## RESULTS

### SAMSUNG GALAXY S21 ULTRA 5G

The frame rate of this smartphone is not constant over time.
It goes faster (i.e. not slowmo) at the beginning and at the end of the movie.
So there are 3 distinct parts to calibrate.
The middle part is the slow motion part.
There are 1018 frames in total.

| PART | FIRST FRAME | LAST FRAME | FRAME COUNT | ΔT (s) | FRAME RATE (fps) | FRAME DURATION (ms) |
| ---: | ----------: | ---------: | ----------: | -----: | ---------------: | ------------------: |
|    1 |           1 |         27 |          27 |  1.066 |             25.3 |              39.468 |
|    2 |          28 |        990 |         963 |  0.999 |            963.7 |               1.038 |
|    3 |         991 |       1018 |          28 |  0.934 |             30.0 |              33.340 |

<p align="center">
<img width=100% src="https://raw.githubusercontent.com/NicHub/camera-slow-motion-calibration/main/images/samsung-s21-ultra-5g-plot.png" />
</p>
