



#ifndef CUSTOM_FONT_H
#define CUSTOM_FONT_H

#include <Arduino.h>



class CustomFont65k{
private:
protected:
public:
	uint16_t cursor_x, cursor_y;

	CustomFont65k();

	void setCursor(uint16_t x, uint16_t y);

	virtual uint8_t * getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	virtual uint8_t * getAlpha(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	virtual uint8_t * getFrameData(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	virtual bool advanceState(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	virtual void resetCursor( void );
};


#endif /* CUSTOM_FONT_H */