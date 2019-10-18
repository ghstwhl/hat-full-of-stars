#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <TCL.h>


#define PIN 3  // Using Digital Pin 3 doe the

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(135, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


// BEGIN Globals necessary for the orbital ring
  int LastOrbitPosition;  // A placeholder for resetting the last lit pixel
  int CurrentOrbitPosition;

  byte orbit_red_current;
  byte orbit_green_current;
  byte orbit_blue_current;

  byte orbit_red_target;
  byte orbit_green_target;
  byte orbit_blue_target;
  int orbit_blackout = 0;
// END Globals necessary for the orbital ring

// BEGIN Globals necessary for the star field
  const int LEDS = 4;
  int LEDposition = 0;
  const int update_interval = 5; // Milliseconds between color updates

  /* Current values for the pixels are stored in the following three arrays */
  byte red_values[LEDS];
  byte green_values[LEDS];
  byte blue_values[LEDS];

  byte red_target[LEDS];
  byte green_target[LEDS];
  byte blue_target[LEDS];
  int starfield_blackout = 0;
// END Globals necessary for the star field


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

  randomSeed(analogRead(0));  // Seed the random number generator
  RandomizeOrbitColor();

  // Initialize the orbitalal path to black  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // Cool Neon Arduino Remote
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  
  TCL.begin(); // Begin protocol for communicating with the TCL strand
  Blackout_Stars();

//  Serial.begin(9600); // Start serial communication at 9600 baud
}

void loop() {

  
  // If Button B is on, commence orbit.  Else, go blackout.
  if ( digitalRead(5) == 0 ) {
    OrbitTick();
    orbit_blackout = 0;
  }
  else if (orbit_blackout != 1)  {
    strip.setPixelColor(LastOrbitPosition, 0, 0, 0);
    strip.show();
    orbit_blackout = 1;
  }


  // If Button B is on, commence orbit.  Else, go blackout.
  if ( digitalRead(4) == 0 ) {
    StarfieldTick();
    starfield_blackout = 0;
  }
  else if (starfield_blackout != 1)  {
    Blackout_Stars();
    starfield_blackout = 1;
  }



  
}


void StarfieldTick() {
  int i; // A variable for looping
  for(i=0;i<LEDS;i++) {
    if ( red_values[i] < red_target[i]) { red_values[i]++; }
    else if ( red_values[i] > red_target[i]) { red_values[i]--; }

    if ( green_values[i] < green_target[i]) { green_values[i]++; }
    else if ( green_values[i] > green_target[i]) { green_values[i]--; }

    if ( blue_values[i] < blue_target[i]) { blue_values[i]++; }
    else if ( blue_values[i] > blue_target[i]) { blue_values[i]--; }


    if (digitalRead(6) == 1) {
      // If running in full color mode, only reset the target if all colors have achieved target
      if ( red_values[i] == red_target[i] && green_values[i] == green_target[i]  && blue_values[i] == blue_target[i] ) { RandomizeStarColor(i); }
    }
    else {
      // If running in white star mode, reset the target if red has achieved target
      if ( red_values[i] == red_target[i] ) { RandomizeStarColor(i); }
    }


  }

  UpdateStarfield(); // Send the black pixels to the strand to turn off all LEDs.

}



void OrbitTick() {
  strip.setPixelColor(LastOrbitPosition, 0, 0, 0);
  strip.setPixelColor(CurrentOrbitPosition, gamma[orbit_red_current], gamma[orbit_green_current], gamma[orbit_blue_current]);

  LastOrbitPosition = CurrentOrbitPosition;

  if ( digitalRead(7) == 1 ) {
    if (CurrentOrbitPosition < (strip.numPixels() - 1) ) { CurrentOrbitPosition++; }
    else { CurrentOrbitPosition = 0; }
  }
  else {
    if (CurrentOrbitPosition > 0 ) { CurrentOrbitPosition--; }
    else { CurrentOrbitPosition = (strip.numPixels() - 1); }
  }

  OrbitColorStep();
  
}

void RandomizeOrbitColor() {
//  Serial.println("Randomizing target colors");
  orbit_red_target = random(0,256);
  orbit_green_target = random(0,256);
  orbit_blue_target = random(0,256);
}

void OrbitColorStep() {
//  Serial.println("Stepping through target colors");
  if ( orbit_red_current < orbit_red_target) { orbit_red_current++; }
  else if ( orbit_red_current > orbit_red_target) { orbit_red_current--; }

  if ( orbit_green_current < orbit_green_target) { orbit_green_current++; }
  else if ( orbit_green_current > orbit_green_target) { orbit_green_current--; }

  if ( orbit_blue_current < orbit_blue_target) { orbit_blue_current++; }
  else if ( orbit_blue_current > orbit_blue_target) { orbit_blue_current--; }

  if ( orbit_red_current == orbit_red_target && orbit_green_current == orbit_green_target  && orbit_blue_current == orbit_blue_target ) {
    RandomizeOrbitColor();
  } 

  strip.show();

}


void Blackout_Stars() {
  int i;

  TCL.sendEmptyFrame();
  for(i=0;i<LEDS;i++) {
    TCL.sendColor(0,0,0);
  }
  TCL.sendEmptyFrame();
}

void RandomizeStarColor(int x){
  red_target[x] = random(0,256);
  blue_target[x] = random(0,256);
  green_target[x] = random(0,256);
}


void UpdateStarfield() {
  int i;

  TCL.sendEmptyFrame();
  if (digitalRead(6) == 1) {
    for(i=0;i<LEDS;i++) { TCL.sendColor(gamma[red_values[i]],gamma[green_values[i]],gamma[blue_values[i]]); }
  }
  else {
    for(i=0;i<LEDS;i++) { TCL.sendColor(gamma[red_values[i]],gamma[red_values[i]],gamma[red_values[i]]); }
  }
  TCL.sendEmptyFrame();

  // Drop the delay if the starfield is active.
  if ( digitalRead(5) == 1 ) { delay(update_interval); }






}

