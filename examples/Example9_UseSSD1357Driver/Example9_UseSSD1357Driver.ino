/*
  Use the included SSD1357 driver to run a different display, as long as it uses the SSD1357 IC
  By: Owen Lyke
  SparkFun Electronics
  Date: July 17th 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Example32_UseSSD1357Driver

  Hardware:
  Even though this is an example of how to use the SSD1357 directly (on any screen that uses it) the hardware
  used was still the 64x64 RGB OLED breakout.
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
#include "SparkFun_SSD1357_OLED.h"  // Click here to get the library: http://librarymanager/All#SparkFun_RGB_OLED_64x64

#define CS_PIN 4
#define DC_PIN 3
#define RST_PIN 2

#define DRIVER_SPI SPI  

SSD1357 driver;                     // An object of the SSD1357 class 

#define WB_LEN 2*64                 // Define a length for a working buffer (used in the screen clear function)


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){};
  Serial.println("Hello command line!");

  SPI.begin();
  driver.begin(DC_PIN, RST_PIN, CS_PIN, DRIVER_SPI, 8000000);   // Use 8 MHz SPI on Arduino Uno instead of the default/maximum (10 MHz)
  
  configure();                                              // These two functions are specific to the display that the driver is used in
  clearDisplay();                                           // So they are defined below using the driver functions
  
  driver.println("Using the driver directly...");

  driver.rectRAM(0x20+5, 0x00 + 31, 54, 20);                // Note, the hexadecimal numbers are the offset for the particular display being used
  driver.rectFillRAM(0x20+10, 0x00 + 36, 44, 10, 0xF800);   // The RAM suffix for these functions indicates that the x,y coordinates are relative 
                                                            // to the SSD1357 RAM and so you may need an offset for your particular display
}

void loop() {
  // No loop
}

void clearDisplay( void )
{
  // Fill up the working buffer with all zeros
  uint8_t buff[WB_LEN];
  for(uint8_t indi = 0; indi < WB_LEN; indi++)
  {
    buff[indi] = 0;
  }

  // Then transmit that buffer to the SSD1357 enough times to cover the display...
  for(uint8_t indi = 0; indi < 64; indi++)
  {
    // Note: if SPI.transfer() did not replace the contents of the buffer then it would not be necessary to re-initialize the buffer every time. Could be decent speed improvements
      driver.write_ram(buff, 0x00+indi, 0x20, 0x3F, 0x5F, WB_LEN);
  }
}

void configure( void )
{
// This is the suggested initialization routine for the 64x64 RGB OLED - Change settings as needed for your particular display
    driver.setCommandLock(false);
    driver.setSleepMode(true);

    // Initial settings configuration
    driver.setClockDivider(0xB0);
    driver.setMUXRatio(0x3F);
    driver.setDisplayOffset(0x40);
    driver.setDisplayStartLine(0x00);
    driver.setRemapColorDepth(false, true, true, true, true, SSD1357_COLOR_MODE_65k);
    driver.setContrastCurrentABC(0x88, 0x32, 0x88);
    driver.setMasterContrastCurrent(0x0F);
    driver.setResetPrechargePeriod(0x02, 0x03);
  
    uint8_t MLUT[63] = {0x02, 0x03, 0x04, 0x05,
                        0x06, 0x07, 0x08, 0x09,
                        0x0A, 0x0B, 0x0C, 0x0D,
                        0x0E, 0x0F, 0x10, 0x11,
                        0x12, 0x13, 0x15, 0x17,
                        0x19, 0x1B, 0x1D, 0x1F,
                        0x21, 0x23, 0x25, 0x27,
                        0x2A, 0x2D, 0x30, 0x33,
                        0x36, 0x39, 0x3C, 0x3F,
                        0x42, 0x45, 0x48, 0x4C,
                        0x50, 0x54, 0x58, 0x5C,
                        0x60, 0x64, 0x68, 0x6C,
                        0x70, 0x74, 0x78, 0x7D,
                        0x82, 0x87, 0x8C, 0x91,
                        0x96, 0x9B, 0xA0, 0xA5,
                        0xAA, 0xAF, 0xB4};
    driver.setMLUTGrayscale(MLUT);

    driver.setPrechargeVoltage(0x17);
    driver.setVCOMH(0x05);
    driver.setColumnAddress(0x20, 0x5F);
    driver.setRowAddress(0x00, 0x3F);
    driver.setDisplayMode(SSD1357_CMD_SDM_RESET);

    driver.setSleepMode(false);

    delay(200);

    driver.setFontCursorValues(0x20, 0x00, 0x20, 0x00, 0x5F, 0x3F);
}
