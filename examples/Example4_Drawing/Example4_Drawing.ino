/*
  Get an introduction to drawing functions on the 64x64 RGB OLED
  By: Owen Lyke
  SparkFun Electronics
  Date: July 17th 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
  Please, if anything is unclear or needs to be fixed let me know about it by submitting an issue on github! 
  https://github.com/sparkfun/Sparkfun_RGB_OLED_64x64_Arduino_Library

  Example4_Drawing

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
  Serial.println("Example4_Drawing");

  SPI.begin();
  myOLED.begin(DC_PIN, RST_PIN, CS_PIN, SPI, 8000000);              // Use 8 MHz SPI on Arduino Uno instead of the default/maximum (10 MHz)
  myOLED.clearDisplay();            // Fills the screen with black

  /*
   * There are four primitive drawing types in this library
   * 1) set a pixel
   * 2) draw a line
   * 3) draw a rectangle
   * 4) draw a circle
   * 
   * Up next will be examples of those methods as well as any special variations they 
   * may have, but first a discussion of common themes.
   * 
   * There are two versions of any given function. The most basic asks the user for the
   * color to use each time. The other version instead uses the '_fillColor' member of 
   * of the myOLED object. 
   * 
   * The (x, y) coordinates on the display begin at top left. Horizontal is x, vertical is y
   *  []--------------> x
   *  |
   *  |
   *  |
   *  |
   *  |
   *  v
   *  y
   */

   /* 
    * Set a pixel:
    * This is very simple. Just specify where on the screen you want to set the pixel, and the color if you choose
    */
    myOLED.setPixel(0,0);   // Uses the '_fillColor' member, which is initialized to 0xFFFF after 'begin'
    myOLED.setPixel(1,1,get65kValueHSV(0.666*HSV_HUE_MAX, 255, 255)); // Uses a color of your choice

    /*
     * Draw a line:
     * There are four unique line functions
     * 1) line        - draws a one-pixel wide line from start coords to end coords
     * 2) lineWide    - draws a line of a variable width from start to end coords
     * 3) lineH       - draws a thin line from start to start + width in the x direction
     * 4) lineV       - draws a thin line from start to start + height in the y direction
     */
     myOLED.setFillColor(get65kValueRGB(255,0,0));     // Set the default fill color 
     myOLED.line(5, 0, 25, 3);                        // Use the fill color that was just set, draw a line
     myOLED.lineWide(5,10,25,13,4, get65kValueHSV(1080, 255, 255));
     myOLED.lineH(25,0, 20, get65kValueHSV(720, 255, 255));
     myOLED.lineV(0, 25, 20, get65kValueHSV(2160, 255, 255));

     /* 
      *  Draw a rectangle
      *  There are two rectangle functions
      *  1) rect      - Draws a one-pixel wide box with a given width and height from a starting coordinate
      *  2) rectFill  - Fills in a box with a given width and height from a starting coordinate
      */
      myOLED.rect(20,20,43,43, get65kValueRGB(255,255,0));
      myOLED.rectFill(25,25,33,33, get65kValueRGB(255,200,0));

      /*
       * Draw a circle
       * Just like the rectangle there are two circle functions
       * 1) circle      - Draws a cicle outline with a given radius from a center coordinate
       * 2) circleFill  - Draws a filled in circle with a given radius from a center coordinate
       * 
       */
       myOLED.circle(41,41, 15, get65kValueHSV(2500, 255, 255));
       myOLED.circleFill(41,41, 10, get65kValueHSV(2700, 255, 255));
}
void loop() {
  // No loop
}
