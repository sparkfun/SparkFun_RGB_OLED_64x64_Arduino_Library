#include "CustomFont65k.h"





// CustomFont65k::CustomFont65k()
// {
// 	cursor_y = 0;
// 	cursor_x = 0;

// 	margin_x = 0xFFFF;
// 	margin_y = 0xFFFF;	

// 	reset_x = 0;
// 	reset_y = 0;
// }

void CustomFont65k::setCursorValues(uint16_t x, uint16_t y, uint16_t xReset, uint16_t yReset, uint16_t xMargin, uint16_t yMargin)
{
	cursor_x = x;
	cursor_y = y;

	margin_x = xMargin;
	margin_y = yMargin;

	reset_x = xReset;
	reset_y = yReset;
}