
/*

A library to control the 64x64 RGB 0.6in OLED display

Restricted only to the 4-wire SPI interface

Author:
Owen Lyke

*/


#ifndef SSD1357_OLED_H
#define SSD1357_OLED_H

#include <Arduino.h>
#include <SPI.h>
#include "CustomFont65k.h"
#include "fast_hsv2rgb.h"

#define HSV_USE_ASSEMBLY

#ifndef SSD1357_DONT_USE_DEF_FONT
	#include "util/font5x7.h"
#endif /* SSD1357_DONT_USE_DEF_FONT */












class MicroviewMonochromeProgMemBMPFont : public CustomFont65k{
private:
protected:
	uint8_t _fontWidth, _fontHeight, _startCharASCII, _totalCharsASCII, _fontHeaderSize;
	uint32_t _fontMapWidth;

public:
	const unsigned char * fontMapPtr;
	uint8_t * charDataPtr;
	uint8_t frameData[4];

	uint16_t _foregroundColor, _backgroundColor;

	MicroviewMonochromeProgMemBMPFont(const unsigned char * pMap, uint8_t * pPad, uint8_t headerSize);

	uint8_t * getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	uint8_t * getAlpha(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	uint8_t * getFrameData(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	bool advanceState(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	// setCursorValues is inherited from the CustomFont65k class, along with the variables that keep track of cursor, reset, and margins

	// Here are some shenanigans: static function wrappers to allow the driver to call the member functions of specific objects
	static uint8_t * Wrapper_to_call_getBMP(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
	static uint8_t * Wrapper_to_call_getAlpha(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
	static uint8_t * Wrapper_to_call_getFrameData(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
	static bool Wrapper_to_call_advanceState(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
	static void Wrapper_to_call_setCursorValues(void * pt2Object, uint16_t x, uint16_t y, uint16_t xReset, uint16_t yReset, uint16_t xMargin, uint16_t yMargin);
};





/*

	SSD1357 OLED Driver

*/

#define SSD1357_SPI_DATA_ORDER MSBFIRST
#define SSD1357_SPI_MODE SPI_MODE3
#define SSD1357_SPI_MAX_FREQ 10000000

#define SSD1357_MAX_WIDTH 128
#define SSD1357_MAX_HEIGHT 128

#define SSD1357_START_ROW 0
#define SSD1357_START_COL 0

#define SSD1357_STOP_ROW 127
#define SSD1357_STOP_COL 127

#define SSD1357_WORKING_BUFF_NUM_PIXELS	128
#define SSD1357_BYTES_PER_PIXEL 2

#define SSD1357_COLOR_MODE_256 0x00
#define SSD1357_COLOR_MODE_65k 0x01

typedef enum{
	SSD1357_CMD_SetColumnAddress = 0x15,
	// Discontinuous
	SSD1357_CMD_SetRowAddress = 0x75,
	// Discontinuous
	SSD1357_CMD_WriteRAM = 0x5C,
	SSD1357_CMD_ReadRAM,
	// Discontinuous
	SSD1357_CMD_SetRemapColorDepth = 0xA0,
	SSD1357_CMD_SetDisplayStartLine,
	SSD1357_CMD_SetDisplayOffset,
	// Discontinuous
	SSD1357_CMD_SDM_ALL_OFF = 0xA4,
	SSD1357_CMD_SDM_ALL_ON,
	SSD1357_CMD_SDM_RESET,
	SSD1357_CMD_SDM_INVERSE,
	// Discontinuous
	SSD1357_CMD_SetSleepMode_ON = 0xAE,
	SSD1357_CMD_SetSleepMode_OFF,
	// Discontinuous
	SSD1357_CMD_SetResetPrechargePeriod = 0xB1,
	// Discontinuous
	SSD1357_CMD_SetClkDiv = 0xB3,
	// Discontinuous
	SSD1357_CMD_SetSecondPrechargePeriod = 0xB6,
	// Discontinuous
	SSD1357_CMD_MLUTGrayscale = 0xB8,
	SSD1357_CMD_UseDefMLUT,
	// Discontinuous
	SSD1357_CMD_SetPrechargeVoltage = 0xBB,
	SSD1357_CMD_SetILUTColorA,
	SSD1357_CMD_SetILUTColorC,
	SSD1357_CMD_SetVCOMH,
	// Discontinuous
	SSD1357_CMD_SetContrastCurrentABC = 0xC1,
	// Discontinuous
	SSD1357_CMD_SetMasterContrastCurrent = 0xC7,
	// Discontinuous
	SSD1357_CMD_SetMuxRatio = 0xCA,
	// Discontinuous
	SSD1357_CMD_NOP = 0xE3,
	// Discontinuous
	SSD1357_CMD_SetCommandLock = 0xFD, 
	// Discontinuous
	SSD1357_CMD_Setup_Scrolling = 0x96,
	// Discontinuous
	SSD1357_SCROLL_STOP = 0x9E,
	SSD1357_SCROLL_START
}SSD1357_CMD_TypeDef;

class SSD1357 : public Print {
private:
protected:
	uint8_t _dc, _rst, _cs;		// Pin definitions
	SPIClass * _spi;			// Which SPI port to use (bit-banging is not supported yet... you could add it!)
	uint32_t _spiFreq;			// How fast to send data to the display
	uint8_t _width, _height;	// Physical dimensions of the display that the driver is connected to. Limited to 128x128
	uint8_t _colorMode;			// Knows if the display is in 65k or 256 color mode
	uint16_t _fillColor;		// The default color to use 

	uint8_t _cursorX, _cursorY, _xReset, _yReset, _xMargin, _yMargin; // Gives a memory of cursor settings

	// Font interface functions
	uint8_t * getFontBMP(uint8_t val);
	uint8_t * getFontAlpha(uint8_t val);
	uint8_t * getFontFrameData(uint8_t val);
	bool fontCallback( uint8_t val);

	// Function pointers that define the current font
	void 		* _object2operateOn;											// Pointer to the font object itself
	uint8_t 	* (*_userBMPFuncPtr)(void *, uint8_t, uint16_t, uint16_t);
	uint8_t 	* (*_userAlphaFuncPtr)(void *, uint8_t, uint16_t, uint16_t);
	uint8_t 	* (*_userFrameFuncPtr)(void *, uint8_t, uint16_t, uint16_t);
	bool 		(*_userFontCallbackPtr)(void *, uint8_t, uint16_t, uint16_t);
	void 		(*_userFontSetCursorValuesPtr)(void *, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);

	// Protected drawing functions
	void fill_working_buffer(uint16_t value, uint8_t num_pixels);

	void plotLineLow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value, uint8_t width);
	void plotLineHigh(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value, uint8_t width);

	void circle_Bresenham(uint8_t x, uint8_t y, uint8_t radius, uint16_t value, boolean fill);
	void circle_midpoint(uint8_t x, uint8_t y, uint8_t radius, uint16_t value, boolean fill);
	void circle_eight(uint8_t xc, uint8_t yc, int16_t dx, int16_t dy, uint16_t value, boolean fill);

	void fast_filled_rectangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1, int16_t value);

public:

	SSD1357( void );		// Constructor

	virtual void begin(uint8_t dcPin, uint8_t rstPin, uint8_t csPin, SPIClass &spiInterface = SPI, uint32_t spiFreq = SSD1357_SPI_MAX_FREQ);
	void startup( void );

	void setCSlow( void );
	void setCShigh(void);

	void write_bytes(uint8_t * pdata, bool DATAcmd, uint16_t size);																// Send data to SSD1357 with the proper D/C level
	void write_ram(uint8_t * pdata, uint8_t startrow, uint8_t startcol, uint8_t stoprow, uint8_t stopcol, uint16_t size);		// Raw data write to the GDDRAM 

	// Implementation of 'write' so that the print superclass will be supported
	size_t write(uint8_t);	

	// Change settings
	void setColumnAddress(uint8_t start, uint8_t stop);
	void setRowAddress(uint8_t start, uint8_t stop);
	void enableWriteRAM( void );
	void enableReadRAM( void );
	void setRemapColorDepth(bool inc_Vh, bool rev_ColAddr, bool swap_ColOrder, bool rev_SCAN, bool en_SplitOddEven, uint8_t color_depth_code);
	void setDisplayStartLine(uint8_t start_line);
	void setDisplayOffset(uint8_t offset);
	void setDisplayMode(uint8_t mode_code);
	void setSleepMode(bool sleep_on);
	void setResetPrechargePeriod(uint8_t reset_clocks, uint8_t precharge_clocks);
	void setClockDivider(uint8_t divider_code);
	void setSecondPrechargePeriod(uint8_t precharge_clocks);
	void setMLUTGrayscale(uint8_t * pdata63B);
	void useBuiltInLinearLUT( void );
	void setPrechargeVoltage( uint8_t voltage_scale_code);
	void setILUTColorA( uint8_t * pdata31B);
	void setILUTColorC( uint8_t * pdata31B);
	void setVCOMH( uint8_t voltage_scale_code);
	void setContrastCurrentABC(uint8_t ccA, uint8_t ccB, uint8_t ccC);
	void setMasterContrastCurrent(uint8_t ccCode);
	void setMUXRatio(uint8_t mux_ratio);
	void setCommandLock(bool locked);

	// Scrolling
	void setupHorizontalScroll(uint8_t scrollParameter, uint8_t startRow, uint8_t stopRow, uint8_t speed);
	void startScrolling( void );
	void stopScrolling( void );

	// Setup the display
	uint8_t 	getWidth( void );
	uint8_t 	getHeight( void );
	void 		setWidth(uint8_t val);
	void 		setHeight(uint8_t val);
	void 		setSPIFreq(uint32_t freq);
	uint32_t	getSPIFreq( void );

	// Interact with fonts
	void 	setFont(
					void * object,
					uint8_t * (*BMPFuncPtr)(void * pt2Object, uint8_t, uint16_t, uint16_t), 
					uint8_t * (*AlphaFuncPtr)(void * pt2Object, uint8_t, uint16_t, uint16_t),
					uint8_t * (*frameFuncPtr)(void * pt2Object, uint8_t, uint16_t, uint16_t), 
					bool 	(*fontCallbackPtr)(void * pt2Object, uint8_t, uint16_t, uint16_t),
					void 	(*setCursorValuesPtr)(void * pt2Object, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)
					);
	void 	linkDefaultFont( void );
	void 	setFontCursorValues(uint8_t x, uint8_t y, uint8_t xReset, uint8_t yReset, uint8_t xMargin, uint8_t yMargin);	// This guarantees that the user can always interact with the cursor reset function, even if the font is the default font that they can't access in the main file
	void 	setCursorRAM(uint8_t x, uint8_t y);
	void 	setDefaultFontColors(uint16_t foreground, uint16_t background);


	// Drawing functions - based on RAM coordinates
	void setFillColor(uint16_t color);

	void setPixelRAM(uint8_t x, uint8_t y);
	void setPixelRAM(uint8_t x, uint8_t y, uint16_t value);

	void lineRAM(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
	void lineRAM(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value);
	void lineWideRAM(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width);
	void lineWideRAM(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width, uint16_t value);
	void lineHRAM(uint8_t x, uint8_t y, uint8_t width);
	void lineHRAM(uint8_t x, uint8_t y, uint8_t width, uint16_t value);
	void lineVRAM(uint8_t x, uint8_t y, uint8_t height);
	void lineVRAM(uint8_t x, uint8_t y, uint8_t height, uint16_t value);

	void rectRAM(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
	void rectRAM(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value);
	void rectFillRAM(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
	void rectFillRAM(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value);

	void circleRAM(uint8_t x, uint8_t y, uint8_t radius);
	void circleRAM(uint8_t x, uint8_t y, uint8_t radius, uint16_t value);
	void circleFillRAM(uint8_t x, uint8_t y, uint8_t radius);
	void circleFillRAM(uint8_t x, uint8_t y, uint8_t radius, uint16_t value);
};

//Color utility functions
uint16_t get65kValueRGB(uint8_t R, uint8_t G, uint8_t B);
uint16_t get65kValueHSV(uint16_t hue, uint8_t sat, uint8_t val);









#endif /* SSD1357_OLED_H */




