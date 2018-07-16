#include "SparkFun_RGB_OLED_64x64.h"
#include "font8x16.h"
#include "QRcodeFont.h"

// The font8x16 header contains a monochrome bitmap font that is 8 pixels wide by 16 pixels tall. 
// The data is stored in program memory in the array called font8x16.
// This setup is similar to the way that the defualt 5x7 font is stored,
// so it is possible to use the font class 'MonochromeProgMemBMPFont'

uint8_t FontScratch8x16[8*16*2];  // This declares a space for the 8x16 font to work in.  
MicroviewMonochromeProgMemBMPFont SSD1357Font8x16(font8x16,FontScratch8x16, 6);  // Construct an object usign the given font definition, the font scratch space, without provisions for an alpha channel, and with 6 bytes in the font header area.

// The most versatile way to create a custom font is to make a font class derived from the CustomFont65k class.
// Here we will create a font that encodes ASCII values into tiny little QR code-like pictures.
// The definition of the font is now moved to the "QRcodeFont.h" and "QRcodeFont.cpp" files to keep this file easier to read.
// You can create a new font in the main file, or write your own include files

#define DRIVER_SPI SPI

#define CS_PIN 4
#define DC_PIN 3
#define RST_PIN 2

RGB_OLED_64x64  myOLED; // Declare OLED object

QRcodeFont myQRfont;    // Declare an object of the QRcodeFont class (Imagine - you can create a "family" of fonts (the class) and make members with different qualities - for example color or a parametric size...)

void setup() {
  Serial.begin(9600);
  while(!Serial){};

  DRIVER_SPI.begin();
  myOLED.begin(DC_PIN, RST_PIN, CS_PIN, DRIVER_SPI, 2000000); // Choosing 2 MHz because Uno at 16 MHz has trouble (to say the least) executing the full 10 MHz maximum

  myOLED.clearDisplay();
}

void loop() {
  
  Serial.println("Printing to display with default font");
  myOLED.linkDefaultFont();
  myOLED.setFontCursorValues(OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_STOP_COL, OLED_64x64_STOP_ROW);
  myOLED.println("Hello world!");
  delay(1000);
  myOLED.clearDisplay();



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
  myOLED.println("Hello world!");
  delay(1000);
  myOLED.clearDisplay();



 Serial.println("Printing to display with 8x16 font");
  myOLED.setFont(
  &SSD1357Font8x16,
  SSD1357Font8x16.Wrapper_to_call_getBMP,
  SSD1357Font8x16.Wrapper_to_call_getAlpha,
  SSD1357Font8x16.Wrapper_to_call_getFrameData, 
  SSD1357Font8x16.Wrapper_to_call_advanceState,
  SSD1357Font8x16.Wrapper_to_call_setCursorValues);

  myOLED.setFontCursorValues(OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_STOP_COL, OLED_64x64_STOP_ROW);
  myOLED.println("Hello world!");
  delay(1000);
  myOLED.clearDisplay();
}
