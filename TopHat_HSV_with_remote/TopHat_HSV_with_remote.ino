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

// Array of pixel values
byte PIXELS[LEDS][3];
float pixel_interval;
float hval;

const float hinterval = 10.0;
const float v = 0.99;
const float sat = 1.0;


byte gammaTable[256];
const float gamma = 2.2;
float local_h;

void setup() {
  int i;
  for(i=0;i<256;i++) {
    gammaTable[i] = (byte)(pow(i/255.0,gamma)*255.0+0.5);
  }

  // This is Button A of the Cool Neon Arduino Remote
  pinMode(4, INPUT_PULLUP);
  // This is Button B of the Cool Neon Arduino Remote
  pinMode(5, INPUT_PULLUP);

  pixel_interval = 360.0/LEDS;
  hval = 0.0;

  TCL.begin(); // Begin protocol for communicating with the TCL strand
  blackout_strand();
  
}

void loop() {
  

/*    if ( digitalRead(4) == 1 && digitalRead(5) == 1) {
      blackout_strand();
      while ( digitalRead(4) == 1 && digitalRead(5) == 1) {
        delay(250);
      }
    }
*/
    for(int i=0;i<LEDS;i++) {
      local_h = hval+i*pixel_interval;
      while(local_h>=360.0) {
        local_h-=360.0;
      }
      HSVtoRGB(local_h,sat,v,&PIXELS[i][0],&PIXELS[i][1],&PIXELS[i][2]);
    }
  
    update_strand();
    delay(25);
    hval+=hinterval;
    while(hval>=360.0) {
      hval-=360.0;
    }

}










void update_strand() {
  int i;

  TCL.sendEmptyFrame();
  for(i=0;i<LEDS;i++) {
    TCL.sendColor(gammaTable[PIXELS[i][0]],gammaTable[PIXELS[i][1]],gammaTable[PIXELS[i][2]]);
  }
  TCL.sendEmptyFrame();
}


void blackout_strand() {
  int i;
  int j;

  for(i=0;i<LEDS;i++) {
    for(j=0; j<3; j++) {
      PIXELS[i][j]=0;
    }
  }
  update_strand(); // Send the black pixels to the strand to turn off all LEDs.
}



/* Convert hsv values (0<=h<360, 0<=s<=1, 0<=v<=1) to rgb values (0<=r<=255, etc) */
void HSVtoRGB(float h, float s, float v, byte *r, byte *g, byte *b) {
  int i;
  float f, p, q, t;
  float r_f, g_f, b_f;

  if( s < 1.0e-6 ) {
    /* grey */
    r_f = g_f = b_f = v;
  }
  
  else {
    h /= 60.0;              /* Divide into 6 regions (0-5) */
    i = (int)floor( h );
    f = h - (float)i;      /* fractional part of h */
    p = v * ( 1.0 - s );
    q = v * ( 1.0 - s * f );
    t = v * ( 1.0 - s * ( 1.0 - f ) );

    switch( i ) {
      case 0:
        r_f = v;
        g_f = t;
        b_f = p;
        break;
      case 1:
        r_f = q;
        g_f = v;
        b_f = p;
        break;
      case 2:
        r_f = p;
        g_f = v;
        b_f = t;
        break;
      case 3:
        r_f = p;
        g_f = q;
        b_f = v;
        break;
      case 4:
        r_f = t;
        g_f = p;
        b_f = v;
        break;
      default:    // case 5:
        r_f = v;
        g_f = p;
        b_f = q;
        break;
    }
  }
  
  *r = (byte)floor(r_f*255.99);
  *g = (byte)floor(g_f*255.99);
  *b = (byte)floor(b_f*255.99);
}
