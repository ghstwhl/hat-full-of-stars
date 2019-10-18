/*****************************************************************************
 * TopHat.ino
 * Version 1.1
 *
 * This sketch does a gentle color cycle for a four pixel strand of TCL LEDS.
 * It was developed for Chris Knight's Top Hat
 *
 * Copyright 2012 Chris O'Halloran <arduinocode@ghostwheel.com>
 * Released under Creative Commons Attribution License (CC BY 3.0)
 # https://creativecommons.org/licenses/by/3.0/legalcode
 *
 * Requires the TCL library developed by Christopher De Vries
 * Documented and available at http://bit.ly/PuqmIo
 *
 ****************************************************************************/
#include <SPI.h>
#include <TCL.h>

const int LEDS = 4;
const int update_interval = 5; // Milliseconds between color updates

/* Current values for the pixels are stored in the following three arrays */
byte red_values[LEDS];
byte green_values[LEDS];
byte blue_values[LEDS];

byte red_target[LEDS];
byte green_target[LEDS];
byte blue_target[LEDS];

const uint8_t gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

void setup() {
  int i;

  TCL.begin(); // Begin protocol for communicating with the TCL strand

  /* Start by initializing all pixels to black */
  for(i=0;i<LEDS;i++) {
    red_values[i]=0;
    green_values[i]=0;
    blue_values[i]=0;
  }
  update_strand(); // Send the black pixels to the strand to turn off all LEDs.
  randomSeed(analogRead(0));
  for(i=0;i<LEDS;i++) {
    RandomizeTargetColor(i);
  }

}

void loop() {
  int i; // A variable for looping

  for(i=0;i<LEDS;i++) {

    if ( red_values[i] < red_target[i]) {
      red_values[i]++;
    }
    if ( red_values[i] > red_target[i]) {
      red_values[i]--;
    }

    if ( green_values[i] < green_target[i]) {
      green_values[i]++;
    }
    if ( green_values[i] > green_target[i]) {
      green_values[i]--;
    }

    if ( blue_values[i] < blue_target[i]) {
      blue_values[i]++;
    }
    if ( blue_values[i] > blue_target[i]) {
      blue_values[i]--;
    }

    if ( red_values[i] == red_target[i] && green_values[i] == green_target[i]  && blue_values[i] == blue_target[i] ) {
    RandomizeTargetColor(i);
    }
  }

  update_strand(); // Send the black pixels to the strand to turn off all LEDs.
  delay(update_interval);
}





void RandomizeTargetColor(int x){
  red_target[x] = random(0,256);
  blue_target[x] = random(0,256);
  green_target[x] = random(0,256);
}

void update_strand() {
  int i;

  TCL.sendEmptyFrame();
  for(i=0;i<LEDS;i++) {
    TCL.sendColor(gamma[red_values[i]],gamma[green_values[i]],gamma[blue_values[i]]);
  }
  TCL.sendEmptyFrame();
}



