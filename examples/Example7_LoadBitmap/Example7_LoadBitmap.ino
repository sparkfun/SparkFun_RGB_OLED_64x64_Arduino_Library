#include "SparkFun_RGB_OLED_64x64.h"

#define DRIVER_SPI SPI

#define CS_PIN 4
#define DC_PIN 3
#define RST_PIN 2

RGB_OLED_64x64  myOLED; // Declare OLED object

#define WORKING_BUFFER_LENGTH 2*64
uint8_t working_buffer[WORKING_BUFFER_LENGTH];

/*
 * Alright, so you want to put an image on the display huh? Now because this display is a whopping 64x64 pixels
 * and each one requires 2 bytes for the 65k color depth that coms out to a grand total of 8,192 bytes for a 
 * given image. This greatly exeeds the available RAM on an Arduino UNO (you could potentially use program 
 * memory to store an image, but that would still use up 25% of program storage space, and you can definitely 
 * find a better use for that memory...). In order to get around this we can use the memory that is built into 
 * the SSD1357 driver (which is then built in to the display). 
 */

void setup() {
  Serial.begin(115200);
  while(!Serial){};

  DRIVER_SPI.begin();
  myOLED.begin(DC_PIN, RST_PIN, CS_PIN, DRIVER_SPI, 2000000); // Choosing 2 MHz because Uno at 16 MHz has trouble (to say the least) executing the full 10 MHz maximum

  myOLED.clearDisplay();

  myOLED.setCursor(OLED_64x64_START_COL, OLED_64x64_START_ROW);
  myOLED.println("Waiting to load data");
  Serial.println("Waiting to load data");

  
}

void loop() {
  printMenu();
  while(Serial.available() == 0){};
//  if(Serial.available())
//  {
    char c = Serial.read();
    if(c == 'l')
    {
      // Load image. Arguments are startx, starty, width, height
      if(Serial.available() == 0)
      {
        Serial.print("Enter startX (Submit with '0x' prefix): ");
      }
      uint8_t startX = getUserByte();
      Serial.print("Got "); Serial.println(startX);
      
      if(Serial.available() == 0)
      {
        Serial.print("Enter startY (Submit with '0x' prefix): ");
      }
      uint8_t startY = getUserByte();
      Serial.print("Got "); Serial.println(startY);
      
      if(Serial.available() == 0)
      {
        Serial.print("Enter width (Submit with '0x' prefix): ");
      }
      uint8_t width = getUserByte();
      Serial.print("Got "); Serial.println(width);
      
      if(Serial.available() == 0)
      {
        Serial.print("Enter height (Submit with '0x' prefix): ");
      }
      uint8_t height = getUserByte();
      Serial.print("Got "); Serial.println(height);

      myOLED.setRowAddress(startY, startY+height-1);
      myOLED.setColumnAddress(startX, startX+width-1);
      myOLED.enableWriteRAM();
      
      receiveData(width, height );
    }
//  }
}

void receiveData(uint8_t W, uint8_t H )
{
  uint16_t count = 0;
  uint8_t  small_count = 0;
  while(count < W*H*2)
  {
    if(Serial.available())
    {
      char c = Serial.read();
      while(c != 'x')
      {
        c = Serial.read();
      }
      // Now the next two characters will be the hex value of the byte
      while(Serial.available() < 2){};
      uint8_t val = getHexByte();
//      Serial.println(val,HEX);
      count++;
      small_count++;

      working_buffer[small_count] = val;
    }

    if(small_count >= (WORKING_BUFFER_LENGTH))
    {
      // Send the data to the screen 
      myOLED.setCSlow();
      myOLED.write_bytes(working_buffer, true, WORKING_BUFFER_LENGTH);
      myOLED.setCShigh();
      small_count = 0;
    }
  }
  myOLED.setCSlow();
  myOLED.write_bytes(working_buffer, true, small_count);
  myOLED.setCShigh();
}

uint8_t getUserByte()
{
  while(Serial.available() == 0){};
  char c = Serial.read();
  while(c != 'x')
  {
    c = Serial.read();
  }
  while(Serial.available() < 2){};
  return getHexByte(); 
}

uint8_t getHexByte()
{
  uint8_t high, low;
  high = hex2nibble(Serial.read());
  low = hex2nibble(Serial.read());
  return ((high << 4) | low);
}

uint8_t hex2nibble(uint8_t val)
{
    switch(val)
    {
      case '0' : return 0; break;
      case '1' : return 1; break;
      case '2' : return 2; break;
      case '3' : return 3; break;
      case '4' : return 4; break;
      case '5' : return 5; break;
      case '6' : return 6; break;
      case '7' : return 7; break;
      case '8' : return 8; break;
      case '9' : return 9; break;

      case 'A' : return 10; break;
      case 'B' : return 11; break;
      case 'C' : return 12; break;
      case 'D' : return 13; break;
      case 'E' : return 14; break;
      case 'F' : return 15; break;

      case 'a' : return 10; break;
      case 'b' : return 11; break;
      case 'c' : return 12; break;
      case 'd' : return 13; break;
      case 'e' : return 14; break;
      case 'f' : return 15; break;

      default : return 0; break;
    }
}

void printMenu( void )
{
  Serial.println("Image uploader: send 'l' to begin loading");
}

