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

// Update - I've now added some color so that the background is a rainbow and the 'filled in' pixels are actually black
// I also tripled the size of each character so that it would be easier to see



#ifndef QRCODEFONT_H
#define QRCODEFONT_H

#include <Arduino.h>
#include "CustomFont65k.h"
#include "fast_hsv2rgb.h"


#define QRF_SCALE_FACTOR 3
#define QRF_FONT_WIDTH 3*QRF_SCALE_FACTOR
#define QRF_FONT_HEIGHT 4*QRF_SCALE_FACTOR
#define QRF_NUM_BYTES_PER_CHAR 2

// This uses c++ classes, so if you need a refresher check out this website: http://www.cplusplus.com/doc/tutorial/classes/
class QRcodeFont : public CustomFont65k{      // This line derives the QRcodeFont class with the CustomFont65k class as it's parent
private:
protected:
  // uint16_t's: cursor_x and cursor_y - these are inherited from the CustomFont65k class to help keep track of where to write next
public:

  uint8_t data[QRF_FONT_WIDTH*QRF_FONT_HEIGHT*2];                           // Each character will be 3 pixels wide and 4 pixels tall, and 2 bytes are required for each pixel
  uint8_t frameData[4];

  uint16_t hue;
  
  QRcodeFont();                               // This is a constructor for the class - it is a special kind of function used to create an object of the class

  // This group of functions must be implemented because the parent class declared them as 'pure virtual' functions
  uint8_t * getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height);           // This function returns a pointer to the data that the driver should write to the display to show the character
  uint8_t * getAlpha(uint8_t val, uint16_t screen_width, uint16_t screen_height);         // This function is not currently used by the driver, but one day it would allow transparency in the font! Maybe you can implement this functionality in the library!
  uint8_t * getFrameData(uint8_t val, uint16_t screen_width, uint16_t screen_height);     // This function returns a pointer to a 4-byte long array that specifies the start and stop coordinates for the driver. This allows each character to be a different size!
  bool advanceState(uint8_t val, uint16_t screen_width, uint16_t screen_height);          // This function is called by the driver right before a character is displayed. It can be used to do things like move the cursor, but it MUST return a boolean value indicating whether or not the driver should actually display the data
//  void resetCursor( void );                                                               // This last function adds a simple way to reset the cursor location

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
  static void Wrapper_to_call_setCursorValues(void * pt2Object, uint16_t x, uint16_t y, uint16_t xReset, uint16_t yReset, uint16_t xMargin, uint16_t yMargin);

  // Here are two functions added beyond what is required, so that the font can be colorful!
  uint16_t get65kValueRGB(uint8_t R, uint8_t G, uint8_t B);
  uint16_t get65kValueHSV(uint16_t hue, uint8_t sat, uint8_t val);

};



#endif /* QRCODEFONT_H */