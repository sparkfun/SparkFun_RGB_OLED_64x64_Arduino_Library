#include "SparkFun_RGB_OLED_64x64.h"
#include "font8x16.h"

// The font8x16 header contains a monochrome bitmap font that is 8 pixels wide by 16 pixels tall. 
// The data is stored in program memory in the array called font8x16.
// This setup is similar to the way that the defualt 5x7 font is stored,
// so it is possible to use the font class 'MonochromeProgMemBMPFont'

uint8_t FontScratch8x16[8*16*2];  // This declares a space for the 8x16 font to work in.  
MonochromeProgMemBMPFont SSD1357Font8x16(font8x16,FontScratch8x16, NULL, 6);  // Construct an object usign the given font definition, the font scratch space, without provisions for an alpha channel, and with 6 bytes in the font header area.


#define DRIVER_SPI SPI

#define CS_PIN 0
#define DC_PIN 1
#define RST_PIN 2

SSD1357 driver;

void setup() {
  Serial.begin(9600);
  DRIVER_SPI.begin();
  driver.begin(DC_PIN, RST_PIN, CS_PIN, DRIVER_SPI);
  while(!Serial){};
  Serial.println("Example 6 - Define Custom Fonts");
  Serial.println();

  for(uint16_t indi = 0; indi < 8*16*2; indi++)
  {
    FontScratch8x16[indi] = 0;
  }
  print8x16scratch();
  

//  Serial.println("Printing to display with default font");
//  driver.print("Hello world!");

//  delay(5000);
  

  Serial.println("Printing to display with 8x16 font");
 driver.setFont(
  &SSD1357Font8x16,
  SSD1357Font8x16.Wrapper_to_call_getBMP,
  SSD1357Font8x16.Wrapper_to_call_getAlpha,
  SSD1357Font8x16.Wrapper_to_call_getFrameData, 
  SSD1357Font8x16.Wrapper_to_call_advanceState,
  SSD1357Font8x16.Wrapper_to_call_resetCursor);

//  driver.write('a');
driver.write(33);

  Serial.println();
  print8x16scratch();
//  driver.print("Hello world!");

  

}

void loop() {
Serial.println(" But I am, still ALiiive!");
delay(500);
}

void print8x16scratch( void ) 
{
  uint16_t count = 0;
  for(uint16_t indi = 0; indi < 8*16*2; indi++)
  {
    if(FontScratch8x16[indi] < 0x10)
    {
      Serial.print(0, DEC);
    }
    Serial.print(FontScratch8x16[indi], HEX);
    count++;
    if(count > (2*8-1))
    {
      count = 0;
      Serial.println();
    } 
  }
}

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

