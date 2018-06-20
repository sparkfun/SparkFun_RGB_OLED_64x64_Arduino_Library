#include "SparkFun_RGB_OLED_64x64.h"
#include "font8x16.h"

// The font8x16 header contains a monochrome bitmap font that is 8 pixels wide by 16 pixels tall. 
// The data is stored in program memory in the array called font8x16.
// This setup is similar to the way that the defualt 5x7 font is stored,
// so it is possible to use the font class 'MonochromeProgMemBMPFont'

uint8_t FontScratch8x16[8*16*2];  // This declares a space for the 8x16 font to work in.  
MonochromeProgMemBMPFont SSD1357Font8x16(font8x16,FontScratch8x16, NULL, 6);  // Construct an object usign the given font definition, the font scratch space, without provisions for an alpha channel, and with 6 bytes in the font header area.




// The most versatile way to create a custom font is to make a font class derived from the CustomFont65k class.
// Here we will create a font that encodes ASCII values into tiny little QR code-like pictures.

// I want the font to be 3 pixels wide and 4 pixels high. The left two columns will hold the data and the right column will be a space before the next character.
// The two columns will hold the 8-bit value of the character with the LSB in the upper-left spot, the MSB in the lower-right, and the bit order increasing left-to-right top-to-bottom like reading a book
// For example the character 'a' = 97 = 0x61 would look like:

// _____
// |XOO|
// |OOO|
// |OXO|
// |XOO|
// -----

// Where the X's represent filled-in pixels and the O's represent blank pixels

#define QRF_WIDTH 3
#define QRF_HEIGHT 4
#define QRF_NUM_BYTES_PER_CHAR 2

// This uses c++ classes, so if you need a refresher check out this website: http://www.cplusplus.com/doc/tutorial/classes/
class QRcodeFont : public CustomFont65k{      // This line derives the QRcodeFont class with the CustomFont65k class as it's parent
private:
protected:
  // uint16_t's: cursor_x and cursor_y - these are inherited from the CustomFont65k class to help keep track of where to write next
public:

  uint8_t data[QRF_WIDTH*QRF_HEIGHT*2];                           // Each character will be 3 pixels wide and 4 pixels tall, and 2 bytes are required for each pixel
  uint8_t frameData[4];
  
  QRcodeFont();                               // This is a constructor for the class - it is a special kind of function used to create an object of the class

  // This group of functions must be implemented because the parent class declared them as 'pure virtual' functions
  uint8_t * getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height);           // This function returns a pointer to the data that the driver should write to the display to show the character
  uint8_t * getAlpha(uint8_t val, uint16_t screen_width, uint16_t screen_height);         // This function is not currently used by the driver, but one day it would allow transparency in the font! Maybe you can implement this functionality in the library!
  uint8_t * getFrameData(uint8_t val, uint16_t screen_width, uint16_t screen_height);     // This function returns a pointer to a 4-byte long array that specifies the start and stop coordinates for the driver. This allows each character to be a different size!
  bool advanceState(uint8_t val, uint16_t screen_width, uint16_t screen_height);          // This function is called by the driver right before a character is displayed. It can be used to do things like move the cursor, but it MUST return a boolean value indicating whether or not the driver should actually display the data
  void resetCursor( void );                                                               // This last function adds a simple way to reset the cursor location

  // Now don't get too confused by these functions. They are required to allow the driver to use custom fonts.
  // The 'signature' of the member functions above has a special type corresponding to this class!
  // Because of that the driver cannot call them directly. The workaround is to use a static member function (because its signature is the same as any regular 'ol function)
  // But using static functions removes the ability to have multiple unique fonts because parameters like the cursor location would be shared. 
  // So the solution is to create these wrapper functions that take in a pointer to the object that we want to operate on. They can be called by the driver and 
  // will make sure to modify the right data
  static uint8_t * Wrapper_to_call_getBMP(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
  static uint8_t * Wrapper_to_call_getAlpha(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
  static uint8_t * Wrapper_to_call_getFrameData(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
  static bool Wrapper_to_call_advanceState(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
  static void Wrapper_to_call_resetCursor(void * pt2Object);
};






#define DRIVER_SPI SPI

#define CS_PIN 0
#define DC_PIN 1
#define RST_PIN 2

SSD1357 driver;

QRcodeFont myQRfont;

void setup() {
  Serial.begin(9600);
  DRIVER_SPI.begin();
  driver.begin(DC_PIN, RST_PIN, CS_PIN, DRIVER_SPI);
  while(!Serial){};
  Serial.println("Example 6 - Define Custom Fonts");
  Serial.println();

//  for(uint16_t indi = 0; indi < 8*16*2; indi++)
//  {
//    FontScratch8x16[indi] = 0;
//  }
//  print8x16scratch();
  

//  Serial.println("Printing to display with default font");
//  driver.print("Hello world!");

//  delay(5000);
  

//  Serial.println("Printing to display with 8x16 font");
// driver.setFont(
//  &SSD1357Font8x16,
//  SSD1357Font8x16.Wrapper_to_call_getBMP,
//  SSD1357Font8x16.Wrapper_to_call_getAlpha,
//  SSD1357Font8x16.Wrapper_to_call_getFrameData, 
//  SSD1357Font8x16.Wrapper_to_call_advanceState,
//  SSD1357Font8x16.Wrapper_to_call_resetCursor);

//  driver.write('a');
//driver.write(33);

//  Serial.println();
//  print8x16scratch();
////  driver.print("Hello world!");



Serial.println("Printing to display with QRcodeFont");
Serial.println();
driver.setFont(
  &myQRfont,
  myQRfont.Wrapper_to_call_getBMP,
  myQRfont.Wrapper_to_call_getAlpha,
  myQRfont.Wrapper_to_call_getFrameData, 
  myQRfont.Wrapper_to_call_advanceState,
  myQRfont.Wrapper_to_call_resetCursor);

//  driver.write('a');
//driver.write(33);

driver.print("Hello world!");

  

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




QRcodeFont::QRcodeFont()
{
  
}





/*
 * Here are the functions that we use to define the font!
 * 
 * 
 */
uint8_t * QRcodeFont::getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height)           // This function returns a pointer to the data that the driver should write to the display to show the character
{
  // I will fill out the scratch space called 'data' 2 bytes (1 pixel) at a time
  uint8_t pixel_y, pixel_x;
  uint8_t bit_index = 0;
  
  for(uint16_t indi = 0; indi < 2*QRF_WIDTH*QRF_HEIGHT; indi+=2)
  {
    // The driver assumes that data is written with the smallest index at the upper-left corner and left-to-right top-to-bottom
    pixel_y = (indi / (2 * QRF_WIDTH));
    pixel_x = (indi - (2 * pixel_y * QRF_WIDTH))/2;

//    Serial.print("indi: "); Serial.print(indi);
//    Serial.print(", x: "); Serial.print(pixel_x);
//    Serial.print(", y: "); Serial.print(pixel_y);
//    Serial.print(", bit index: "); Serial.println(bit_index);
    
    if(pixel_x < 2)
    {
      bit_index++;
    }

    if(pixel_x == 2)
    {
      Serial.println("O");
    }
    else
    {
      if(val & (0x01 << bit_index))
      {
        Serial.print("X");
        data[indi] = 0xFF;
        data[indi+1] = 0xFF;
      }
      else
      {
        Serial.print("O");
        data[indi] = 0x00;
        data[indi+1] = 0x00;
      }
    }
  }
  Serial.println();
  return data;
}
  uint8_t * QRcodeFont::getAlpha(uint8_t val, uint16_t screen_width, uint16_t screen_height)         // This function is not currently used by the driver, but one day it would allow transparency in the font! Maybe you can implement this functionality in the library!
  {
    return NULL;  // No alpha in this font
  } 
  uint8_t * QRcodeFont::getFrameData(uint8_t val, uint16_t screen_width, uint16_t screen_height)     // This function returns a pointer to a 4-byte long array that specifies the start and stop coordinates for the driver. This allows each character to be a different size!
{
  frameData[0] = cursor_y;
  frameData[1] = cursor_x;
  frameData[2] = QRF_HEIGHT;
  frameData[3] = QRF_WIDTH;

  return frameData;
}
  
  bool QRcodeFont::advanceState(uint8_t val, uint16_t screen_width, uint16_t screen_height)          // This function is called by the driver right before a character is displayed. It can be used to do things like move the cursor, but it MUST return a boolean value indicating whether or not the driver should actually display the data
  {
    // First things first move the cursor. 
  uint16_t newX = 0;
  uint16_t newY = 0;

  // A special case is the newline character '\n'
  if(val == '\n')
  {
    newX = 0;
    newY = cursor_y + QRF_HEIGHT;

    cursor_x = newX;
    cursor_y = newY;

    return false;           // The driver will not display anything for newline chars. It just starts a new line
  }

  // If the character that is about to be printed is not a newline character then it will probably take up space and so the cursor should be incremented. 
  // Its OK to increment cursor data because the frame data exists in another array that the driver will access 
  newX = cursor_x + QRF_WIDTH; // Move left-to-right first
  if((newX > (64 - QRF_WIDTH))) // But start a new line if you go over the edge
  {
    newX = 0;
    newY = cursor_y + QRF_HEIGHT;    //
    if((newY > (64 - QRF_HEIGHT)))
    {
      cursor_x = newX;
      cursor_y = newY;

      return false; // This indicates that the driver should not print anything
    }

    cursor_x = newX;
    cursor_y = newY;
  }

  return true;      // This tells the driver to go ahead and print the data
  // By the way if the cursor goes off the screen it is up to the user to reset the cursor to zero (after clearing the screen, most likely)

  }
  
  void QRcodeFont::resetCursor( void )                                                               // This last function adds a simple way to reset the cursor location
{
  setCursor(0, 0);  // Function and x/y values inherited from CustomFont65k
}












/*
 * 
 * Here are those wrapper functions again. They really aren't cmplicated, just UGLY.
 * Each one just casts the void object pointer to the right type so that it can use the member functions
 * and then calls the appropriate function with the given inputs. 
 * 
 */
uint8_t * QRcodeFont::Wrapper_to_call_getBMP(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
  QRcodeFont * self = (QRcodeFont *)pt2Object;
  self->getBMP(val, screen_width, screen_height);
}

uint8_t * QRcodeFont::Wrapper_to_call_getAlpha(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
  QRcodeFont * self = (QRcodeFont *)pt2Object;
  self->getAlpha(val, screen_width, screen_height);
}

uint8_t * QRcodeFont::Wrapper_to_call_getFrameData(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
  QRcodeFont * self = (QRcodeFont *)pt2Object;
  self->getFrameData(val, screen_width, screen_height);
}

bool QRcodeFont::Wrapper_to_call_advanceState(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
  QRcodeFont * self = (QRcodeFont *)pt2Object;
  self->advanceState(val, screen_width, screen_height);
}

void QRcodeFont::Wrapper_to_call_resetCursor(void * pt2Object)
{
  QRcodeFont * self = (QRcodeFont *)pt2Object;
  self->resetCursor();
}

