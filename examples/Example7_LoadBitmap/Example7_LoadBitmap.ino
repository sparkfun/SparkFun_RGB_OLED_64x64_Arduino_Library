/*
  Stream an image from a file on your computer to the RGB OLED 64x64 display over a serial connection
  By: Owen Lyke
  SparkFun Electronics
  Date: July 17th 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  Example7_LoadBitmap

  Of course drawing your own pictures using the library functions is cool, but it would be very tedious to, for 
  instance, replicate the mona lisa. This example will put the powers of this display to the test. In other examples
  the image data may have been stored on the microcontroller but because of the resolution and color depth of this 
  display that would be impossible for an Uno. In this case the image data will be sent over the serial connection to 
  the controller. As the data arrives it will be held in a small temporary array. When that array fills up the data 
  will be displayed on the screen. This will continue until the whole image has been transferred.

  If you want to make your own images to upload with this program use these steps:
  1 -  resize the image you want so that it will fit on the screen (64x64 is good, but smaller sizes will work too). Here's a link: http://resizeimage.net/
  2 -  convert the image to an array of data to send. Here's a good tool https://littlevgl.com/image-to-c-array
       Download with no transparency as a C array. You will see 3 blocks of text - each one is a different color depth. For this 
       display you need the middle block. Delete all other text leaving only what looks like the insides of an array.
       If you want to streamline the process add 'l' followed by the startx and starty locations and width and height in hex
       to the beginninng of the file
  3 - Then send that file over the serial connection

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
 * the SSD1357 driver (which is then built in to the display). You still need a small amount of memory onboard 
 * the microcontroller to reduce the 
 */

void setup() {
  Serial.begin(115200);   // Choose the fastest serial connection you can!
  while(!Serial){};

  DRIVER_SPI.begin();
  myOLED.begin(DC_PIN, RST_PIN, CS_PIN, DRIVER_SPI, 8000000); // Use 8 MHz SPI on Arduino Uno instead of the default/maximum (10 MHz)

  myOLED.clearDisplay();

  myOLED.setCursor(0, 0);
  myOLED.println("Ready for command");
  Serial.println("Ready for command");

  
}

void loop() {
  while(Serial.available() == 0){};   // Wait until data appears
  char c = Serial.read();
  if(c == 'l')                        // Check to see if it's a load command
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

    Serial.print("Receiving data...");

    myOLED.setRowAddress(OLED_64x64_START_ROW+startY, OLED_64x64_START_ROW+startY+height-1);
    myOLED.setColumnAddress(OLED_64x64_START_COL+startX, OLED_64x64_START_COL+startX+width-1);
    myOLED.enableWriteRAM();
    
    transferData(width, height );
  }
  else if(c == 'c')
  {
    myOLED.clearDisplay();
    myOLED.setCursor(0, 0);
    myOLED.println("Ready for command");
  }
  else
  {
    printMenu();
  }
}

// This function will transfer data to the display until the whole image (W*H*2 bytes) has been handled
void transferData(uint8_t W, uint8_t H )
{
  uint16_t count = 0;
  uint16_t idle_count = 0;
  uint8_t  small_count = 0;

  // Repeat this process until the required number of bytes have been handled
  while(count < W*H*2)
  {
    // Add a byte to the working buffer
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
      count++;
      small_count++;

      working_buffer[small_count] = val;
    }

    // When the working buffer fills up send it to the display and reset for more
    if(small_count >= (WORKING_BUFFER_LENGTH))
    {
      // Send the data to the screen 
      myOLED.setCSlow();
      myOLED.write_bytes(working_buffer, true, WORKING_BUFFER_LENGTH);
      myOLED.setCShigh();
      small_count = 0;
    }

    if(idle_count++ > 500)
    {
      idle_count = 0;
      Serial.print("Received ");
      Serial.print(count); 
      Serial.print("/");
      Serial.print(W*H*2);
      Serial.println(" bytes");
    }
  }

  // This method allows you to write more data to the screen without having to specify where to start as in the write_ram function
  myOLED.setCSlow();
  myOLED.write_bytes(working_buffer, true, small_count);
  myOLED.setCShigh();

  Serial.println("Finished receiving image");
}

// This function looks for a data byte using an 'x' character
// as a marker
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

// Takes the next two bytes fromt the serial stream and
// makes them into a single byte (assuming they were a
// hexadecimal representation of a number)
uint8_t getHexByte()
{
  uint8_t high, low;
  high = hex2nibble(Serial.read());
  low = hex2nibble(Serial.read());
  return ((high << 4) | low);
}

// This takes in an ascii character 0-9 and A-F (or a-f)
// and returns the decimal value associated with it.
// Returns 0 for out-of-bounds characters
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
  Serial.println("Image uploader: ");
  Serial.println("  'l' to begin loading");
  Serial.println("  'c' to clear the screen");
  Serial.println("  any other character to show this menu");
}

