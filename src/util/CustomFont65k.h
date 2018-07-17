



#ifndef CUSTOM_FONT_H
#define CUSTOM_FONT_H

#include <Arduino.h>

class CustomFont65k{
private:
protected:
public:

	uint16_t cursor_x, cursor_y;
	uint16_t margin_x, margin_y;
	uint16_t reset_x, reset_y;

	boolean _prevWriteCausedNewline;

	// CustomFont65k();							// Make CustomFont65k an 'abstract' or 'existential' class to allow pure-virtual functions. This means you can't make an bject of this class - rather you have to derive another class from it and implement the speicified functions

	virtual uint8_t * getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	virtual uint8_t * getAlpha(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	virtual uint8_t * getFrameData(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	virtual bool advanceState(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	virtual void setCursorValues(uint16_t x, uint16_t y, uint16_t xReset, uint16_t yReset, uint16_t xMargin, uint16_t yMargin);
};


#endif /* CUSTOM_FONT_H */