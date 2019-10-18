/*****************************************************************************
 * TopHat.ino
 * Version 1.0
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
    TCL.sendColor(red_values[i],green_values[i],blue_values[i]);
  }
  TCL.sendEmptyFrame();
}



