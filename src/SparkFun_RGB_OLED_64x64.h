/* 

A libary to use the SSD1357 driver in conjuction with a particular OLED display that is 64x64

*/

#ifndef SF_RGB_OLED_64X64_H
#define	SF_RGB_OLED_64X64_H

// #include "screen65k.h"				// This is a method of storing arbitrary RGB images in 16-bit depth where two colors are 5 bits and the last is 6 bits
#include "SparkFun_SSD1357_OLED.h"	// This is a driver that takes screens and displays them on a physical device

#define OLED_64x64_WIDTH 	64
#define OLED_64x64_HEIGHT 	64
#define OLED_64x64_START_ROW	0x00
#define OLED_64x64_START_COL	0x20
#define OLED_64x64_STOP_ROW 	0x3F
#define OLED_64x64_STOP_COL		0x5F
#define OLED_64x64_MAX_BYTES_PER_PIXEL	2
#define OLED_WORING_BUFF_NUM_PIXELS OLED_64x64_WIDTH

#define OLED_SCROLL_NORMAL 0x01
#define OLED_SCROLL_SLOW 0x02
#define OLED_SCROLL_SLOWEST 0x03

class RGB_OLED_64x64 : public SSD1357 {
// class RGB_OLED_64x64 : public SSD1357 {
private:
protected:

	boolean _isInverted, _isFlippedH, _isFlippedV, _incV, _coSwapped, _scanReversed;

	uint8_t _colorMode;


public:

	RGB_OLED_64x64();

	void begin(uint8_t dcPin, uint8_t rstPin, uint8_t csPin, SPIClass &spiInterface = SPI, uint32_t spiFreq = SSD1357_SPI_MAX_FREQ);
	void defaultConfigure( void );

	// LCD Draw functions
    void clearDisplay(uint8_t mode = 0x00);
    void fillDisplay(uint16_t value);
    void display(void);
    // void setCursor(uint8_t x, uint8_t y);

    void invert(boolean inv);
    // void setContrast(uint8_t contrast);
    // void flipVertical(boolean flip);
    void flipHorizontal(boolean flip);

    

    // void drawChar(uint8_t x, uint8_t y, uint8_t c);
    // void drawChar(uint8_t x, uint8_t y, uint8_t c, uint8_t color, uint8_t mode);

    // void drawBitmap(uint8_t *bitArray);

    uint16_t getDisplayWidth(void);
    uint16_t getDisplayHeight(void);
    void setDisplayWidth(uint16_t width);
    void setDisplayHeight(uint16_t height);
    void setColor(uint16_t value);
    // void setDrawMode(uint8_t mode);
    // uint8_t *getScreenBuffer(void);

    //Font functions
    // uint8_t getFontWidth(void);
    // uint8_t getFontHeight(void);
    // uint8_t getTotalFonts(void);
    // uint8_t getFontType(void);
    // boolean setFontType(uint8_t type);
    // uint8_t getFontStartChar(void);
    // uint8_t getFontTotalChar(void);
    void setCursor(uint8_t x, uint8_t y);

    // LCD Rotate Scroll functions
    void scrollRight(uint8_t start, uint8_t stop, uint8_t speed);
    void scrollLeft(uint8_t start, uint8_t stop, uint8_t speed);

    //TODO Add 0x29/0x2A vertical scrolling commands
    void scrollUp(uint8_t start, uint8_t stop);
    //void scrollVertLeft(uint8_t start, uint8_t stop);

    void scrollStop(void);


    // Drawing functions - based on 64x64 RGB OLED coordinates for ease
    void setPixel(uint8_t x, uint8_t y);
    void setPixel(uint8_t x, uint8_t y, uint16_t value);

    void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
    void line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value);
    void lineWide(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width);
    void lineWide(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value, uint8_t width);
    void lineH(uint8_t x, uint8_t y, uint8_t width);
    void lineH(uint8_t x, uint8_t y, uint8_t width, uint16_t value);
    void lineV(uint8_t x, uint8_t y, uint8_t height);
    void lineV(uint8_t x, uint8_t y, uint8_t height, uint16_t value);

    void rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
    void rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value);
    void rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
    void rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value);

    void circle(uint8_t x, uint8_t y, uint8_t radius);
    void circle(uint8_t x, uint8_t y, uint8_t radius, uint16_t value);
    void circleFill(uint8_t x, uint8_t y, uint8_t radius);
    void circleFill(uint8_t x, uint8_t y, uint8_t radius, uint16_t value);


};








#endif /* SF_RGB_OLED_64X64_H */