#include "CustomFont65k.h"





CustomFont65k::CustomFont65k()
{

}

void CustomFont65k::setCursor(uint16_t x, uint16_t y)
{
	cursor_x = x;
	cursor_y = y;
}

// uint8_t * getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height);
// uint8_t * getAlpha(uint8_t val, uint16_t screen_width, uint16_t screen_height);
// uint8_t * getFrameData(uint8_t val, uint16_t screen_width, uint16_t screen_height);
// void advanceState( void );