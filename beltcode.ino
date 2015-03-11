// This is a simple program for an interactive LED belt that uses a gyroscope and microphone to change the color and brightness of LEDs. It uses the Adafruit library and includes some example functions towards the end from that library.
// Associated Instructable: http://www.instructables.com/id/Making-an-Interactive-Fuzzy-LED-Belt/
// Created by Ashley Newton (i@ashleynewton.net), 2013.
// You will need to install the L3G gyroscope library (https://github.com/pololu/l3g-arduino) and Adafruit Neopixel library (https://github.com/adafruit/Adafruit_NeoPixel).
//

#include <Wire.h>
#include <L3G.h>

#include "SPI.h"

#include <Adafruit_NeoPixel.h>
#define NUM_LEDS 116
#define LED_PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

int audioPin = A0;
int audio = 0;

int hue = 0;

L3G gyro; // initialize gyroscope

// gyroscope position variables
int xpos = 0;
int ypos = 0;
int zpos = 0;


void setup() {
  Serial.begin(57600);
  Wire.begin();
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
  }

  Serial.println("I am a belt!");

  gyro.enableDefault();
  // delay(1000);
  //gyro.calibrate();
  strip.begin();

  // Update LED contents, to start they are all 'off'
  strip.setBrightness(20);
  strip.show();
}

// Main program function
void loop() {

  // Loop through first row of LEDs  
  for (int cnt = 0; cnt <NUM_LEDS/2; cnt ++) updateLED(cnt); // get updated gyroscope and audio info and update LED at current position


  // Loop through second row of LEDs
  for (int cnt = NUM_LEDS; cnt >NUM_LEDS/2; cnt --) updateLED(cnt); // get updated gyroscope and audio info and update LED at current position
}

void updateLED(int cnt) {
  gyro.read();  // read position date from gyroscope
  ypos = ypos + (gyro.g.y*.0004); // set ypos to previous ypos value plus add scaled down new value from gyroscope
  ypos = ypos % 255; // make value between 0 and 255 in order to use it to set LED color (which is also a value from 0-255)

  //Serial.print(xpos); // for debugging
  audio = analogRead(audioPin); // read in audio input from microphone
  //   Serial.print(" audio read: "); // for debugging
  //   Serial.print(audio); // for debugging
  audio = constrain(map(audio,400,1023,1,200),1,200); // scale audio value and constrain it to that we don't accidently end up with too large of a value; constraining to 1-200 to reduce brightness of belt
  // Serial.print("  mapped audio: "); // for debugging
  //   Serial.println(audio); // for debugging
  if (ypos==0) ypos=audio; // if belt isn't moving, set xpos (LED color) to audio value - this keeps the belt from defaulting to a single color when not moving
  drawPixelHue(cnt,ypos, audio); // set pixel at cnt position to be color value of "ypos" and brightness of "audio"
  strip.show(); // update / show LEDs

}

// example/default code that generates a series of rainbow color fades - from adafruit
void rainbow(uint8_t wait) {
  int i, j;

  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255,1));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// example code that one makes the rainbow wheel equally distributed 
// along the chain - from adafruit
void rainbowCycle(uint8_t wait) {
  int i, j;

  for (j=0; j < 256 * 2; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256,1 ) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}


/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b, float bright)
{
  uint32_t c;
  r=byte(constrain(r*(bright/4)-140,0,20));
  g=byte(constrain(g*(bright/4)-140,0,20));
  b=byte(constrain(b*(bright/4)-140,0,20));
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, float bright) {
  if(WheelPos < 85) {
    return strip.Color((WheelPos * 3)/(bright),((255 - WheelPos * 3)/(bright)), 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color((255 - WheelPos * 3)/(bright), 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, (WheelPos * 3)/(bright), (255 - WheelPos * 3)/(bright));
  }
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t WheelOld(byte WheelPos, float bright)
{
  if (WheelPos < 85) {
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0, bright);
  } 
  else if (WheelPos < 170) {
    WheelPos -= 85;
    return Color(255 - WheelPos * 3, 0, WheelPos * 3, bright);
  } 
  else {
    WheelPos -= 170; 
    return Color(0, WheelPos * 3, 255 - WheelPos * 3, bright);
  }
}

void drawPixel(int pix, int r, int g, int b) {

  strip.setPixelColor(pix, Color(r,g,b,1));

}


void drawPixelHue(int pix,int hue, float bright) {

  strip.setPixelColor(pix, Wheel(hue,bright));

}








