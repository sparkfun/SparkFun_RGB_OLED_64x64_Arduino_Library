/*
  Demo code for SparkX tours
  By: Owen Lyke
  SparkFun Electronics
  Date: August 7 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

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

#include "SparkFun_RGB_OLED_64x64.h"        // Click here to get the library: http://librarymanager/All#SparkFun_RGB_OLED_64x64
#include "bitmaps.h"
#include "QRcodeFont.h"
#include "font8x16.h"


#define DRIVER_SPI SPI

#define CS_PIN 4
#define DC_PIN 3
#define RST_PIN 2

RGB_OLED_64x64  myOLED; // Declare OLED object

#define WORKING_BUFFER_LENGTH 2*64
uint8_t working_buffer[WORKING_BUFFER_LENGTH];
uint16_t hue = 0;

uint8_t FontScratch8x16[8*16*2];   
MicroviewMonochromeProgMemBMPFont SSD1357Font8x16(font8x16, FontScratch8x16, 6);

QRcodeFont myQRfont; 



void showBitmap(uint8_t * pdata);
void transferData(uint8_t W, uint8_t H, uint8_t * pdata);
void showMonaLisa( void );
void showMagma( void );
void showFlame( void );
void showGalaxy( void );

void setup() {
  Serial.begin(115200);   // Choose the fastest serial connection you can!
//  while(!Serial){};

  DRIVER_SPI.begin();
  myOLED.begin(DC_PIN, RST_PIN, CS_PIN, DRIVER_SPI, 8000000); // Use 8 MHz SPI on Arduino Uno instead of the default/maximum (10 MHz)

  myOLED.clearDisplay();

}

void loop() {
  // put your main code here, to run repeatedly:

  delay(1000);
  showMonaLisa();
  delay(1000);
  showGalaxy();
  delay(1000);
  showMagma();
  delay(1000);
  showFlame();
  delay(1000);
  
  doScroll();

  doFonts();

}


void showBitmap(uint8_t * pdata)
{
  uint8_t startX = *(pdata + 1);
  uint8_t startY = *(pdata + 2);
  uint8_t width = *(pdata + 3);
  uint8_t height = *(pdata + 4);

    myOLED.setRowAddress(OLED_64x64_START_ROW+startY, OLED_64x64_START_ROW+startY+height-1);
    myOLED.setColumnAddress(OLED_64x64_START_COL+startX, OLED_64x64_START_COL+startX+width-1);
    myOLED.enableWriteRAM();

    transferData(width, height, pdata+ 5 );
}

void transferData(uint8_t W, uint8_t H, uint8_t * pdata)
{
  uint16_t count = 0;
  uint16_t idle_count = 0;
  uint8_t  small_count = 0;

  // Repeat this process until the required number of bytes have been handled
  while(count < W*H*2)
  {
    // Add a byte to the working buffer
    // if(Serial.available())
    // {
      // char c = Serial.read();
      // while(c != 'x')
      // {
      //   c = Serial.read();
      // }
      // Now the next two characters will be the hex value of the byte
      // while(Serial.available() < 2){};
      uint8_t val = *(pdata + count);
      count++;
      small_count++;

      working_buffer[small_count] = val;
    
//
//    // Swap the order??
//    for(uint16_t indi = 0; indi < WORKING_BUFFER_LENGTH; indi+=2)
//    {
//      uint8_t temp = *(working_buffer+indi+0);
//      *(working_buffer+indi+0) = *(working_buffer+indi+1);
//      *(working_buffer+indi+1) = temp;
//    }



    // When the working buffer fills up send it to the display and reset for more
    if(small_count >= (WORKING_BUFFER_LENGTH))
    {
      // Send the data to the screen 
      myOLED.setCSlow();
      myOLED.write_bytes(working_buffer, true, WORKING_BUFFER_LENGTH);
      myOLED.setCShigh();
      small_count = 0;
    }
  }

  // This method allows you to write more data to the screen without having to specify where to start as in the write_ram function
  myOLED.setCSlow();
  myOLED.write_bytes(working_buffer, true, small_count);
  myOLED.setCShigh();

  Serial.println("Finished receiving image");
}

void showMonaLisa( void )
{
  showBitmap(monalisa);
}

void showMagma( void )
{
  showBitmap(magma);
}

void showGalaxy( void )
{
  showBitmap(galaxy);
}

void showFlame( void )
{
  showBitmap(sfeflame);
}


void doScroll( void )
{
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

void doFonts( void )
{
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

