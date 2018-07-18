/*
  Use the scrolling feature of the SSD1357
  By: Owen Lyke
  SparkFun Electronics
  Date: July 17th 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
  Please, if anything is unclear or needs to be fixed let me know about it by submitting an issue on github! 
  https://github.com/sparkfun/Sparkfun_RGB_OLED_64x64_Arduino_Library

  Example5_Scrolling

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

uint16_t hue = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Example5_Scrolling");

  SPI.begin();
  myOLED.begin(DC_PIN, RST_PIN, CS_PIN, SPI, 8000000);
  myOLED.clearDisplay();            // Fills the screen with black

  /*
   * Scrolling with the SSD1357 means shifting the contents of the display ram either left or right at one 
   * of three speeds (NORMAL, SLOW, SLOWEST) for a given range of rows. 
   */

  // First draw a graphic...
  myOLED.rectFill(0,8,63,63-8);
  myOLED.circleFill(31,63,10, get65kValueRGB(255,0,0));
  for(uint8_t indi = 0; indi < 64; indi += 4)
  {
    myOLED.line(0,63-indi,31,63, get65kValueRGB(255,0,0));
    myOLED.line(indi,0,31,63, get65kValueRGB(255,0,0));
    myOLED.line(63,63-indi,31,63, get65kValueRGB(255,0,0));
  }
  myOLED.rectFillRAM(0,0,127,8, 0x0000); // Note - I am using a different function to write data beyond the limits of the screen, but within the limits of the display ram

  // Now make a banner to scroll across!
  myOLED.setFontCursorValues(0,0,0,0,127,127);
  myOLED.print("Phoenix. Photons + glass");     // Err, this is supposed to be like the description besides a piece of art...

   // This is the line that starts scrolling. What it means is that rows 0-8 will scroll from the right to the left at the normal speed.
   myOLED.scrollRight(0, 8, OLED_SCROLL_NORMAL);

   // Note: subsequent scroll commands will override the last one, so unfortunately you cant have different 
   // sections moving at different speeds, or different directions. 

    delay(10000);

   // If you want to stop scrolling just use this:
    myOLED.scrollStop();
}
void loop() {

  // A neat fact about the way the display scrolls is that it actually moves the data within the RAM instead of
  // changing what part of the ram is showing. This means that the drawing functions will still draw in the same
  // place relative to the physical screen. You can actually make some cool effects because of this...

  
  for(uint8_t indi = 0; indi < 31; indi++)
  {
    myOLED.rect(31-indi, 31-indi, 2*indi, 2*indi, get65kValueHSV(hue+=37, 255, 255));
    myOLED.rectFillRAM(0,0,10,63, 0x0000);  // Using an advanced function to fill in black off-screen so that there will be a clear canvas
    if(hue > HSV_HUE_MAX)
    {
      hue -= HSV_HUE_MAX;
    }
    myOLED.scrollLeft(0, 63, OLED_SCROLL_NORMAL);
    delay(100);
    myOLED.scrollStop();
  }

  for(uint8_t indi = 0; indi < 31; indi++)
  {
    myOLED.rect(0+indi, 0+indi, 62-2*indi, 62-2*indi, get65kValueHSV(hue+=37, 255, 255));
    myOLED.rectFillRAM(0,0,31,63, 0x0000);  // Using an advanced function to fill in black off-screen so that there will be a clear canvas

    if(hue > HSV_HUE_MAX)
    {
      hue -= HSV_HUE_MAX;
    }
    myOLED.scrollLeft(0, 63, OLED_SCROLL_NORMAL);
    delay(100);
    myOLED.scrollStop();
  }

  for(uint8_t indi = 0; indi < 31; indi++)
  {
    myOLED.circle(31, 31, indi, get65kValueHSV(hue+=37, 255, 255));
    myOLED.rectFillRAM(0,0,10,63, 0x0000);  // Using an advanced function to fill in black off-screen so that there will be a clear canvas
    if(hue > HSV_HUE_MAX)
    {
      hue -= HSV_HUE_MAX;
    }
    myOLED.scrollLeft(0, 63, OLED_SCROLL_NORMAL);
    delay(100);
    myOLED.scrollStop();
  }

  for(uint8_t indi = 0; indi < 31; indi++)
  {
    myOLED.circle(31, 31, 31-indi, get65kValueHSV(hue+=37, 255, 255));
    myOLED.rectFillRAM(0,0,31,63, 0x0000);  // Using an advanced function to fill in black off-screen so that there will be a clear canvas

    if(hue > HSV_HUE_MAX)
    {
      hue -= HSV_HUE_MAX;
    }
    myOLED.scrollLeft(0, 63, OLED_SCROLL_NORMAL);
    delay(100);
    myOLED.scrollStop();
  }
}
