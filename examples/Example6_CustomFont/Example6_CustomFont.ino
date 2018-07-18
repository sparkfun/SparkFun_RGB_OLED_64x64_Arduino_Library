/*
  Create and use completely custom fonts for the SSD1357 OLED driver and 64x64 RGB OLED screen
  By: Owen Lyke
  SparkFun Electronics
  Date: July 17th 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Example6_CustomFont

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

#define SSD1357_DONT_USE_DEF_FONT         // Defining this variable removes the default font from the code

#include "SparkFun_RGB_OLED_64x64.h"      // Click here to get the library: http://librarymanager/All#SparkFun_RGB_OLED_64x64
#include "font8x16.h"                     // This file simply holds a bitmap definition of the 8x16 font in program memory
#include "QRcodeFont.h"                   // This file contains a class that was written to make a completely custom font

/* 
 *  The first custom font to implement is another bitmap font, but a little larger. Because the 8x16 and 5x7 (default) 
 *  font map definitions are borrowed from the MicroView library they work very similarly to one another. This means 
 *  that the font class built-in to the SSD1357 driver can be used for the 8x16 font as well. 
 */
uint8_t FontScratch8x16[8*16*2];                                                  // This declares a space for the 8x16 font to work in.  
MicroviewMonochromeProgMemBMPFont SSD1357Font8x16(font8x16, FontScratch8x16, 6);   // Construct an object using the given font definition, the font scratch space, and with 6 bytes in the font header area.


/*
 * The second custom font that will be implemented shows that you don't need a bitmap font - instead you can generate a
 * font procedurally. This can help save on memory, and also create very cool things! To make a completely custom font 
 * you derive a class from the 'CustomFont65k' class and create the necessary functions:
 *      getBMP        - You return a pointer to a W*H*2 long array of bytes that represents the charactr to draw
 *      getFrameData  - You specify the corner coordiantes as well as Width and Height of the given character (yes, different characters can be different sizes!)
 *      advanceState  - You choose what characters to actually display, as wel as how to move the cursor
 *      getAlpha      - Not implemented yet in the SSD1357 driver, but could allow for transparency effects (Just return NULL)
 *      
 * Creating a class for custom fonts is also cool because it allows you to instantiate objects ('fonts') with unique properties
 * parametrically. For example you could make a class called 'coloredFonts' and make an object called 'redFont' that always
 * prints in red. The possibilities are endless!
 *      
 * An example is given in the "QRcodeFont.h" header file.
 */

#define DRIVER_SPI SPI 

#define CS_PIN 4
#define DC_PIN 3
#define RST_PIN 2

RGB_OLED_64x64  myOLED; // Declare OLED object
QRcodeFont myQRfont;    // Declare an object of the QRcodeFont class - no special parameters here, each instantiation is identical

void setup() {
  Serial.begin(9600);
  while(!Serial){};

  DRIVER_SPI.begin();
  myOLED.begin(DC_PIN, RST_PIN, CS_PIN, DRIVER_SPI, 8000000); // Choosing 8 MHz because Uno at 16 MHz can only do HW SPI at F_CPU/2

  myOLED.clearDisplay();
}

void loop() {
  
  printWithDefault("Hello world!");
  delay(1000);
  myOLED.clearDisplay();

  printWith8x16("Hello world!");
  delay(1000);
  myOLED.clearDisplay();

  printWithQRcode("Hello world!");
  delay(1000);
  myOLED.clearDisplay();
}


// This function prints a string on the display using the SSD1357 defualt font (5x7 bitmap)
void printWithDefault( const char * buff )
{
  Serial.println("Printing to display with default font");
  myOLED.linkDefaultFont();
  myOLED.setFontCursorValues(OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_STOP_COL, OLED_64x64_STOP_ROW);
  myOLED.println(buff);
}

// This function prints a string on the display using the 8x16 bitmap font
void printWith8x16( const char * buff )
{
  Serial.println("Printing to display with 8x16 font");
  myOLED.setFont(
    &SSD1357Font8x16,
    SSD1357Font8x16.Wrapper_to_call_getBMP,
    SSD1357Font8x16.Wrapper_to_call_getAlpha,
    SSD1357Font8x16.Wrapper_to_call_getFrameData, 
    SSD1357Font8x16.Wrapper_to_call_advanceState,
    SSD1357Font8x16.Wrapper_to_call_setCursorValues);
  myOLED.setFontCursorValues(OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_STOP_COL, OLED_64x64_STOP_ROW);
  myOLED.println(buff);
}

// This function prints a string on the display using the procedural 'QRcode' font
void printWithQRcode( const char * buff )
{
  Serial.println("Printing to display with QRcodeFont");
  Serial.println();
  myOLED.setFont(
    &myQRfont,
    myQRfont.Wrapper_to_call_getBMP,
    myQRfont.Wrapper_to_call_getAlpha,
    myQRfont.Wrapper_to_call_getFrameData, 
    myQRfont.Wrapper_to_call_advanceState,
    myQRfont.Wrapper_to_call_setCursorValues);
  myOLED.setFontCursorValues(OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_STOP_COL, OLED_64x64_STOP_ROW);
  myOLED.println(buff);
}



