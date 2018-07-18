/*
  Get to know how to use colors on the display
  By: Owen Lyke
  SparkFun Electronics
  Date: July 17th 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
  Please, if anything is unclear or needs to be fixed let me know about it by submitting an issue on github! 
  https://github.com/sparkfun/Sparkfun_RGB_OLED_64x64_Arduino_Library
  
  Example2_Colors

  Hardware:
  This example is for the RGB OLED 64x64 Breakout, but the underlying driver (SSD1357) can be applied to other
  displays in some cases. If you are using the breakout then all the various voltage regulation and level 
  shifting is already taken care of. Just connect to a controller such as an Arduino Uno as follows:

  Breakout Pin  -->      Uno Pin
  ------------------------------
  GND           -->         GND
  VIN           -->          5V (3.3V works too)
  RST           -->           2
  MOSI          -->          11
  SCLK          -->          13
  D/C           -->           3
  CS            -->           4 

  If you want to support development of software like this consider purchasing the breakout from SparkFun!
  https://www.sparkfun.com/products/14680
  
*/
#include "SparkFun_RGB_OLED_64x64.h"      // Click here to get the library: http://librarymanager/All#SparkFun_RGB_OLED_64x64

#define CS_PIN 4
#define DC_PIN 3
#define RST_PIN 2

RGB_OLED_64x64  myOLED;                   // Declare OLED object of the RGB_OLED_64x64 class

void setup() {
  Serial.begin(9600);
  Serial.println("Example2_Colors");

  SPI.begin();
  myOLED.begin(DC_PIN, RST_PIN, CS_PIN, SPI, 8000000);                // Use 8 MHz SPI on Arduino Uno instead of the default/maximum (10 MHz)
  myOLED.clearDisplay();            // Fills the screen with black

  /* 
   *  The display uses a 16 bit color depth (~65k colors) for each pixel. Because 16 can't be exactly divided 
   *  by three there are actually 5 bits of data for Red and Blue, and 6 bits for Green. Any time that this 
   *  libraryasks for a color (or 'value') it needs a 16 bit number. To simplify things there are two 
   *  functions that can return the right 16 bit number using both the RGB and HSV color space models 
   *  (https://en.wikipedia.org/wiki/Color_space)
   *  
   *  The RGB function 'get65kValueRGB' takes in a byte for each component R, G, and B. For each byte 0 means
   *  off and 255 means full brightness - the scaling to either 5 or 6 bits happens internally to the function
   *  
   *  The HSV function 'get65kValueHSV' uses an awesome library by vagrearg for fast computation of RGB from HSV.
   *  You can read about the code here: http://www.vagrearg.org/content/hsvrgb
   *  The Hue is a 16 bit number that can be anything from HSV_HUE_MIN to HSV_HUE_MAX. Scanning through HUE values
   *  will get you a ainbow of colors. The saturation and value variables also have min/max definitions, but they
   *  are just common 8-bit numbers from 0 - 255. 
   */

  // Try changing the colors here and see how they come out on the display!
  uint16_t myRGBcolor = get65kValueRGB(151,0,230);    // R, G, B  -  Each input can span from 0-255
  uint16_t myHSVcolor = get65kValueHSV(0.25*HSV_HUE_MAX,255,255);    // hue, sat, val  -  Hue changes the color

  myOLED.setCursor(0,0);
  myOLED.print("RGB    HSV");
  myOLED.rectFill(0,8,31,48,myRGBcolor);
  myOLED.rectFill(32,8,31,48,myHSVcolor);
  myOLED.rectFill(0,49,64,16,0xFFFF);           // You can of course also use constants for color... 
                                                // Any number you use here in place of 0xFFFF (from 0 to 65535) 
                                                // represents a unique color!
  
}

void loop() {
  // No loop
}
