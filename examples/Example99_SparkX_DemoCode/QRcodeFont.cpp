
#include "QRcodeFont.h"

QRcodeFont::QRcodeFont()
{
  hue = HSV_HUE_MIN;
}





/*
 * Here are the functions that we use to define the font!
 * 
 * 
 */
uint8_t * QRcodeFont::getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height)           // This function returns a pointer to the data that the driver should write to the display to show the character
{
  // I will fill out the scratch space called 'data' 2 bytes (1 pixel) at a time
  uint8_t pixel_y, pixel_x, subpixel_x, subpixel_y;
  uint8_t bit_index = 0;
  uint8_t count = 0;
  
  for(uint16_t indi = 0; indi < 2*QRF_FONT_WIDTH*QRF_FONT_HEIGHT; indi+=2)
  {
    // The driver assumes that data is written with the smallest index at the upper-left corner and left-to-right top-to-bottom
    // pixel_y = (indi / (2 * QRF_FONT_WIDTH));
    // pixel_x = (indi - (2 * pixel_y * QRF_FONT_WIDTH))/2;

    subpixel_y = (indi / (2 * QRF_FONT_WIDTH));
    subpixel_x = (indi - (2 * subpixel_y * QRF_FONT_WIDTH))/2;

    // Serial.print("sub(x,y): ("); Serial.print(subpixel_x); Serial.print(", "); Serial.print(subpixel_y); Serial.println(")");


    pixel_y = subpixel_y / QRF_SCALE_FACTOR;
    pixel_x = subpixel_x / QRF_SCALE_FACTOR;

    // Serial.print("pix(x,y): ("); Serial.print(pixel_x); Serial.print(", "); Serial.print(pixel_y); Serial.println(")");
    
    bit_index = 2*pixel_y + pixel_x;  // This only needs to work for pixel_x = 0 or 1

    if(pixel_x != 2)
    {
      if(val & (0x01 << bit_index))
      {
        // data[indi] = 0xFF;
        // data[indi+1] = 0xFF;
        data[indi] = 0x00;
        data[indi+1] = 0x00;
      }
      else
      {
        uint16_t backgroundColor = get65kValueHSV(hue, 255, 255);
        data[indi] = ((backgroundColor & 0xFF00) >> 8);
        data[indi+1] = backgroundColor & 0x00FF;
      }
    }
    else
    {
        uint16_t backgroundColor = get65kValueHSV(hue, 255, 255);
        data[indi] = ((backgroundColor & 0xFF00) >> 8);
        data[indi+1] = backgroundColor & 0x00FF;
    }
  }
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
  frameData[2] = QRF_FONT_HEIGHT;
  frameData[3] = QRF_FONT_WIDTH;

  return frameData;
}
  
  bool QRcodeFont::advanceState(uint8_t val, uint16_t screen_width, uint16_t screen_height)          // This function is called by the driver right before a character is displayed. It can be used to do things like move the cursor, but it MUST return a boolean value indicating whether or not the driver should actually display the data
  {
// First things first move the cursor. 
  uint16_t newX = cursor_x;
  uint16_t newY = cursor_y;

  // Update background hue!
  hue += 128;
  if(hue > HSV_HUE_MAX)
  {
    hue = HSV_HUE_MIN;
  }

  // A special case is the newline character '\n'
  if(val == '\n')
  {
    if(!_prevWriteCausedNewline)
    {
      newX = reset_x;
      newY += QRF_FONT_HEIGHT + 1;

      cursor_x = newX;
      cursor_y = newY;

      _prevWriteCausedNewline = true;
    }

    return false;           // The driver will not display anything for newline chars. It just starts a new line
  }

  if(val == '\r')
  {
    return false;             // You can make a 'blacklist' of characters that should not cause any action on the display by returning false when they are requested
  }

  // If the character that is about to be printed is not a newline character then it will probably take up space and so the cursor should be incremented. 
  // Its OK to increment cursor data because the frame data exists in another array that the driver will access 
  newX += QRF_FONT_WIDTH + 1; // Move left-to-right first
  if((newX > (margin_x - QRF_FONT_WIDTH)))  // But start a new line if you go over the edge
  {
    newX = reset_x;
    newY += QRF_FONT_HEIGHT + 1;    //

    _prevWriteCausedNewline = true;

    if((newY > (margin_y - QRF_FONT_HEIGHT)))
    {
      cursor_x = newX;
      cursor_y = newY;

      return false; // This indicates that the driver should not print anything
    } 
  }

  cursor_x = newX;
  cursor_y = newY;

  return true;      // This tells the driver to go ahead and print the data
  // By the way if the cursor goes off the screen it is up to the user to reset the cursor to zero (after clearing the screen, most likely)
}
  
//  void QRcodeFont::resetCursor( void )                                                               // This last function adds a simple way to reset the cursor location
//{
//  setCursorValues(0, 0);  // Function and x/y values inherited from CustomFont65k
//}



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
  return self->getBMP(val, screen_width, screen_height);
}

uint8_t * QRcodeFont::Wrapper_to_call_getAlpha(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
  QRcodeFont * self = (QRcodeFont *)pt2Object;
  return self->getAlpha(val, screen_width, screen_height);
}

uint8_t * QRcodeFont::Wrapper_to_call_getFrameData(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
  QRcodeFont * self = (QRcodeFont *)pt2Object;
  return self->getFrameData(val, screen_width, screen_height);
}

bool QRcodeFont::Wrapper_to_call_advanceState(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
  QRcodeFont * self = (QRcodeFont *)pt2Object;
  return self->advanceState(val, screen_width, screen_height);
}

void QRcodeFont::Wrapper_to_call_setCursorValues(void * pt2Object, uint16_t x, uint16_t y, uint16_t xReset, uint16_t yReset, uint16_t xMargin, uint16_t yMargin)
{
  QRcodeFont * self = (QRcodeFont *)pt2Object;
  return self->setCursorValues(x, y, xReset, yReset, xMargin, yMargin);
}

uint16_t QRcodeFont::get65kValueRGB(uint8_t R, uint8_t G, uint8_t B)
{
  uint16_t rScaled = (R*31)/255;
  uint16_t gScaled = (G*63)/255;
  uint16_t bScaled = (B*31)/255;
  return (((rScaled & 0x001F) << 11) | ((gScaled & 0x003F) << 5) | ((bScaled & 0x001F) << 0));
}
uint16_t QRcodeFont::get65kValueHSV(uint16_t hue, uint8_t sat, uint8_t val)
{
  //For an awesome analysis of HSV to RGB conversion for small CPUs check this out:  http://www.vagrearg.org/content/hsvrgb
  uint8_t r, g, b;
  fast_hsv2rgb_32bit(hue, sat, val, &r, &g, &b);
  return get65kValueRGB(r, g, b);
}