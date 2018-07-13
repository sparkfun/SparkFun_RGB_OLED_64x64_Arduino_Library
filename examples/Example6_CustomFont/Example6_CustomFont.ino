#include "SparkFun_RGB_OLED_64x64.h"
#include "font8x16.h"
#include "QRcodeFont.h"

// The font8x16 header contains a monochrome bitmap font that is 8 pixels wide by 16 pixels tall. 
// The data is stored in program memory in the array called font8x16.
// This setup is similar to the way that the defualt 5x7 font is stored,
// so it is possible to use the font class 'MonochromeProgMemBMPFont'

uint8_t FontScratch8x16[8*16*2];  // This declares a space for the 8x16 font to work in.  
MonochromeProgMemBMPFont SSD1357Font8x16(font8x16,FontScratch8x16, NULL, 6);  // Construct an object usign the given font definition, the font scratch space, without provisions for an alpha channel, and with 6 bytes in the font header area.

// The most versatile way to create a custom font is to make a font class derived from the CustomFont65k class.
// Here we will create a font that encodes ASCII values into tiny little QR code-like pictures.
// The definition of the font is now moved to the "QRcodeFont.h" and "QRcodeFont.cpp" files to keep this file easier to read.
// You can create a new font in the main file, or write your own include files

#define DRIVER_SPI SPI

#define CS_PIN 4
#define DC_PIN 3
#define RST_PIN 2

RGB_OLED_64x64  myOLED; // Declare OLED object

QRcodeFont myQRfont;

void setup() {
  Serial.begin(9600);
  while(!Serial){};

  DRIVER_SPI.begin();
  myOLED.begin(DC_PIN, RST_PIN, CS_PIN, DRIVER_SPI, 2000000); // Choosing 2 MHz because Uno at 16 MHz has trouble (to say the least) executing the full 10 MHz maximum

  myOLED.clearDisplay();

  Serial.println("Printing to display with default font");
  myOLED.setFontCursorValues(OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_STOP_COL, OLED_64x64_STOP_ROW);
  myOLED.print("Hello world!");

  delay(5000);
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
  myOLED.print("Hello world!");


  delay(5000);
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
  myOLED.print("Hello world!");

}

void loop() {
//Serial.println(" But I am, still ALiiive!");
//delay(500);
if(Serial.available())
  {
    char cmd = Serial.read();
    if(cmd == 'c')
    {
      myOLED.clearDisplay();
    }
    else if(cmd == 'f')
    {
      myOLED.fillDisplay(0xFFFF);
    }
  }
}

//void print8x16scratch( void ) 
//{
//  uint16_t count = 0;
//  for(uint16_t indi = 0; indi < 8*16*2; indi++)
//  {
//    if(FontScratch8x16[indi] < 0x10)
//    {
//      Serial.print(0, DEC);
//    }
//    Serial.print(FontScratch8x16[indi], HEX);
//    count++;
//    if(count > (2*8-1))
//    {
//      count = 0;
//      Serial.println();
//    } 
//  }
//}

//void print5x7scratch( void ) 
//{
//  uint16_t count = 0;
//  for(uint16_t indi = 0; indi < 5*7*2; indi++)
//  {
//    Serial.print(FontScratch5x7[indi], HEX);
//    count++;
//    if(count > (2*8-1))
//    {
//      count = 0;
//      Serial.println();
//    } 
//  }
//}













