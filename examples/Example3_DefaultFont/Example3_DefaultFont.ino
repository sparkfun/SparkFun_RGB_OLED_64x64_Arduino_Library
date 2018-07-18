/*
  Learn about fonts on the display using the default font
  By: Owen Lyke
  SparkFun Electronics
  Date: July 17th 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
  Please, if anything is unclear or needs to be fixed let me know about it by submitting an issue on github! 
  https://github.com/sparkfun/Sparkfun_RGB_OLED_64x64_Arduino_Library

  Example3_DefaultFont

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
  Serial.println("Example3_DefaultFont");

  SPI.begin();
  myOLED.begin(DC_PIN, RST_PIN, CS_PIN, SPI, 8000000);              // Use 8 MHz SPI on Arduino Uno instead of the default/maximum (10 MHz)
  myOLED.clearDisplay();            // Fills the screen with black

  // After the 'begin' function the default font comes ready-to-use at the upper left corner of the display
  myOLED.print("Ready!");

  // Printing without a new line will be bounded by the margins, which default to the size of the display.
  myOLED.print(" wrap...");

  // Using a new line will - you guessed it - start a new line
  myOLED.println();
  myOLED.print("A new hope");

  // The setCursor function can jump the font to any location on the screen
  myOLED.setCursor(3,26);           // This sets the location of the upper-left pixel of the next character drawn
  myOLED.print("Like here!");

  // Using a newline from an odd location resets to the left reset location (defaults to 0) and moves the cursor down by the height of the font
  myOLED.println();
  myOLED.println("Or here");

  // Now let's change the margins... 
  // This function is much more general and applies to whatever font is being used at the time. The values that you provide
  // are also in terms of the base SSD1357 RAM memory. You can use the define start addresses OLED_64x64_START_COL (for x) 
  // and OLED_64x64_START_ROW (for y) to make it easier to know where you are.
  // The first two numbers set the cursor location directly (x, y)
  // The next two set the 'reset' location (xReset, yReset)
  // and the last two set the margin boundaries (xMargin, yMargin)

  // Here we will set the cursor and move the x margin to the center of the screen. Then you can observe how the font wraps when it gets to the middle
  myOLED.setFontCursorValues(OLED_64x64_START_COL + 0, OLED_64x64_START_ROW + 45,  OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_START_COL + 31, OLED_64x64_STOP_ROW);
  myOLED.print("Wrapping");
  
  // The default font also supports color changing. You can set both the background and forground colors as such:
  uint16_t FGcolor = get65kValueHSV(0.75*HSV_HUE_MAX,255,255);
  uint16_t BGcolor = get65kValueHSV(0.25*HSV_HUE_MAX,255,255);

  myOLED.setDefaultFontColors(FGcolor, BGcolor);
  myOLED.print(3.1415, 4);                          // This also shows how all standard Arduino 'print' functions are supported


  // Now you know a lot about the default font. The library has fairly good support for custom fonts so if you're looking
  // for something with a little more pizzaz check out Example6_CustomFonts
}

void loop() {
  // No loop
}
