
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

#ifndef SSD1357_DONT_USE_DEF_FONT
	#include "util/font5x7.h"
#endif /* SSD1357_DONT_USE_DEF_FONT */

#define SSD1357_SPI_DATA_ORDER MSBFIRST
#define SSD1357_SPI_MODE SPI_MODE3
#define SSD1357_SPI_MAX_FREQ 10000000

#define SSD1357_MAX_WIDTH 128
#define SSD1357_MAX_HEIGHT 128





class MonochromeProgMemBMPFont : public CustomFont65k{
private:
protected:
	// uint8_t getData(unsigned char * pdata);	// Needed to a
	uint8_t _fontWidth, _fontHeight, _startCharASCII, _totalCharsASCII, _fontMapWidth, _fontHeaderSize;

public:
	unsigned char * fontMapPtr;
	uint8_t * charDataPtr;
	uint8_t * alphaDataPtr;

	uint8_t leftMargin;
	uint8_t rightMargin;
	uint8_t topMargin;
	uint8_t bottomMargin;

	uint8_t frameData[4];

	MonochromeProgMemBMPFont(unsigned char * pMap, uint8_t * pPad, uint8_t * pAlphaPad, uint8_t headerSize);

	uint8_t * getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	uint8_t * getAlpha(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	uint8_t * getFrameData(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	bool advanceState(uint8_t val, uint16_t screen_width, uint16_t screen_height);
	void resetCursor( void );

	void setMargins(uint8_t left, uint8_t right, uint8_t top, uint8_t bottom);

	// Here are some shenanigans: static function wrappers to allow the driver to call the member functions...
	static uint8_t * Wrapper_to_call_getBMP(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
	static uint8_t * Wrapper_to_call_getAlpha(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
	static uint8_t * Wrapper_to_call_getFrameData(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
	static bool Wrapper_to_call_advanceState(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height);
	static void Wrapper_to_call_resetCursor(void * pt2Object);
};





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
	SSD1357_CMD_SetCommandLock = 0xFD
}SSD1357_CMD_TypeDef;



class SSD1357 : public Print {
private:
	
protected:

	uint8_t _dc, _rst, _cs;
	SPIClass * _spi;

	uint32_t _spiFreq;

	uint8_t _width, _height;	// Physical dimensions of the display that the driver is connected to. Limited to 128x128


	void write_bytes(uint8_t * pdata, bool DATAcmd, uint16_t size);
	void write_ram(uint8_t * pdata, uint8_t startrow, uint8_t startcol, uint8_t stoprow, uint8_t stopcol, uint16_t size);		// Raw data write to the GDDRAM 

	void linkDefaultFont( void );

	uint8_t * getFontBMP(uint8_t val);
	uint8_t * getFontAlpha(uint8_t val);
	uint8_t * getFontFrameData(uint8_t val);
	bool fontCallback( uint8_t val);

	void 		* _object2operateOn;
	uint8_t 	* (*_userBMPFuncPtr)(void *, uint8_t, uint8_t, uint8_t);
	uint8_t 	* (*_userAlphaFuncPtr)(void *, uint8_t, uint8_t, uint8_t);
	uint8_t 	* (*_userFrameFuncPtr)(void *, uint8_t, uint8_t, uint8_t);
	bool 		(*_userFontCallbackPtr)(void *, uint8_t, uint8_t, uint8_t);
	void 		(*_userFontResetCursorPtr)(void *);

public:

	SSD1357( void );

	virtual void begin(uint8_t dcPin, uint8_t rstPin, uint8_t csPin, SPIClass &spiInterface = SPI, uint32_t spiFreq = SSD1357_SPI_MAX_FREQ);
	void startup( void );

	void setCSlow( void );
	void setCShigh(void);

	// From print library
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
					uint8_t * (*BMPFuncPtr)(void * pt2Object, uint8_t, uint8_t, uint8_t), 
					uint8_t * (*AlphaFuncPtr)(void * pt2Object, uint8_t, uint8_t, uint8_t),
					uint8_t * (*frameFuncPtr)(void * pt2Object, uint8_t, uint8_t, uint8_t), 
					bool 	(*fontCallbackPtr)(void * pt2Object, uint8_t, uint8_t, uint8_t),
					void 	(*resetCursorPtr)(void * pt2Object) 
					);
	void 	resetFontDefault( void );
	void 	resetFontCursor(void);	// This guarantees that the user can always interact with the cursor reset function, even if the font is the default font that they can't access in the main file

	void 	write_ram_wrapper(uint8_t * pdata, uint8_t startrow, uint8_t startcol, uint8_t stoprow, uint8_t stopcol, uint16_t size);

};









#endif /* SSD1357_OLED_H */