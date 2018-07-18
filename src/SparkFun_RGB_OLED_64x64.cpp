#include "SparkFun_RGB_OLED_64x64.h"


		


RGB_OLED_64x64::RGB_OLED_64x64()
{

}

void RGB_OLED_64x64::begin(uint8_t dcPin, uint8_t rstPin, uint8_t csPin, SPIClass &spiInterface, uint32_t spiFreq)
{
	// Associate 
	_dc = dcPin;
	_rst = rstPin;
	_cs = csPin;
	_spi = &spiInterface;

	_spiFreq = spiFreq;

	linkDefaultFont();

	// Set pinmodes
	pinMode(_cs, OUTPUT);
	pinMode(_rst, OUTPUT);
	pinMode(_dc, OUTPUT);

	// Set pins to default positions
	digitalWrite(_cs, HIGH);
	digitalWrite(_rst, HIGH);
	digitalWrite(_dc, HIGH);

	// Transmit just one byte without a target to 'set' the spi hardware
	uint8_t temp_buff[1];
	_spi->beginTransaction(SPISettings(_spiFreq, SSD1357_SPI_DATA_ORDER, SSD1357_SPI_MODE));
	_spi->transfer(temp_buff, 1);
	_spi->endTransaction();

	// Perform the startup procedure
	startup();
	defaultConfigure();

	// Specific to the 64x64 display:
	_width = OLED_64x64_WIDTH;
	_height = OLED_64x64_HEIGHT;

	_fillColor = 0xFFFF;
	
}



void RGB_OLED_64x64::defaultConfigure( void )
{
	// This is the suggested initialization routine from WiseChip (pg. 9 of the datasheet)
	setCommandLock(false);
  	setSleepMode(true);

  	// Initial settings configuration
  	setClockDivider(0xB0);
  	setMUXRatio(0x3F);
  	setDisplayOffset(0x40);
  	setDisplayStartLine(0x00);
  	setRemapColorDepth(false, true, true, true, true, SSD1357_COLOR_MODE_65k);
  	_colorMode = SSD1357_COLOR_MODE_65k;
  	setContrastCurrentABC(0x88, 0x32, 0x88);
  	setMasterContrastCurrent(0x0F);
  	setResetPrechargePeriod(0x02, 0x03);
  
  	uint8_t MLUT[63] = {0x02, 0x03, 0x04, 0x05,
                      	0x06, 0x07, 0x08, 0x09,
                      	0x0A, 0x0B, 0x0C, 0x0D,
                      	0x0E, 0x0F, 0x10, 0x11,
                      	0x12, 0x13, 0x15, 0x17,
                      	0x19, 0x1B, 0x1D, 0x1F,
                      	0x21, 0x23, 0x25, 0x27,
                      	0x2A, 0x2D, 0x30, 0x33,
                      	0x36, 0x39, 0x3C, 0x3F,
                      	0x42, 0x45, 0x48, 0x4C,
                      	0x50, 0x54, 0x58, 0x5C,
                      	0x60, 0x64, 0x68, 0x6C,
                      	0x70, 0x74, 0x78, 0x7D,
                      	0x82, 0x87, 0x8C, 0x91,
                      	0x96, 0x9B, 0xA0, 0xA5,
                      	0xAA, 0xAF, 0xB4};
  	setMLUTGrayscale(MLUT);

  	setPrechargeVoltage(0x17);
  	setVCOMH(0x05);
  	setColumnAddress(OLED_64x64_START_COL, OLED_64x64_STOP_COL);
  	setRowAddress(OLED_64x64_START_ROW, OLED_64x64_STOP_ROW);
  	setDisplayMode(SSD1357_CMD_SDM_RESET);

  	setWidth(64);
  	setHeight(64);

  	setSleepMode(false);

  	delay(200);

  	setFontCursorValues(OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_STOP_COL, OLED_64x64_STOP_ROW);
}

void RGB_OLED_64x64::clearDisplay(uint8_t mode)
{
	fillDisplay(0x0000);
}

void RGB_OLED_64x64::fillDisplay(uint16_t value)
{
	fast_filled_rectangle(OLED_64x64_START_COL, OLED_64x64_START_ROW, OLED_64x64_STOP_COL, OLED_64x64_STOP_ROW, value);
}

void RGB_OLED_64x64::display(void)						// Because there is no local data (always writing to display ram) there is no need for this function, but it is kept for compatibility
{
	return;
}

// void setCursor(uint8_t x, uint8_t y)
// {

// }



void RGB_OLED_64x64::invert(boolean inv)
{
	if(inv)
	{
		setDisplayMode(SSD1357_CMD_SDM_INVERSE);
	}
	else
	{
		setDisplayMode(SSD1357_CMD_SDM_RESET);
	}
}

// void RGB_OLED_64x64::setContrast(uint8_t contrast)
// {

// }

// void RGB_OLED_64x64::flipVertical(boolean flip)		// No support yet


void RGB_OLED_64x64::flipHorizontal(boolean flip)
{
	_isFlippedH = flip;
	setRemapColorDepth(_incV, flip, _coSwapped, _scanReversed, true, _colorMode);
}




void RGB_OLED_64x64::setCursor(uint8_t x, uint8_t y)
{
	setCursorRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y);
}


uint16_t RGB_OLED_64x64::getDisplayWidth(void)
{
	return OLED_64x64_WIDTH;
}

uint16_t RGB_OLED_64x64::getDisplayHeight(void)
{
	return OLED_64x64_HEIGHT;
}

void RGB_OLED_64x64::setDisplayWidth(uint16_t width)
{

}

void RGB_OLED_64x64::setDisplayHeight(uint16_t height)
{
	setMUXRatio(height);
}

void RGB_OLED_64x64::setColor(uint16_t value)
{
	_fillColor = value;
}


void RGB_OLED_64x64::scrollRight(uint8_t start, uint8_t stop, uint8_t speed)
{
	uint8_t scrollParameter = 0xFF;
	setupHorizontalScroll(scrollParameter, start, stop, speed);
	startScrolling();
}

void RGB_OLED_64x64::scrollLeft(uint8_t start, uint8_t stop, uint8_t speed)
{
	uint8_t scrollParameter = 0x01;
	setupHorizontalScroll(scrollParameter, start, stop, speed);
	startScrolling();
}

//TODO Add 0x29/0x2A vertical scrolling commands
// void RGB_OLED_64x64::scrollUp(uint8_t start, uint8_t stop);
//void scrollVertLeft(uint8_t start, uint8_t stop);

void RGB_OLED_64x64::scrollStop(void)
{
	stopScrolling();
}



// Drawing functions that have the x, y coordinates in terms of the 64x64 RGB OLED
void RGB_OLED_64x64::setPixel(uint8_t x, uint8_t y)
{
	setPixelRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y);
}
void RGB_OLED_64x64::setPixel(uint8_t x, uint8_t y, uint16_t value)
{
	setPixelRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, value);
}

void RGB_OLED_64x64::line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	lineRAM(OLED_64x64_START_COL + x0, OLED_64x64_START_ROW + y0, OLED_64x64_START_COL + x1, OLED_64x64_START_ROW + y1);
}
void RGB_OLED_64x64::line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value)
{
	lineRAM(OLED_64x64_START_COL + x0, OLED_64x64_START_ROW + y0, OLED_64x64_START_COL + x1, OLED_64x64_START_ROW + y1, value);
}
void RGB_OLED_64x64::lineWide(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width)
{
	lineWideRAM(OLED_64x64_START_COL + x0, OLED_64x64_START_ROW + y0, OLED_64x64_START_COL + x1, OLED_64x64_START_ROW + y1, width);
}
void RGB_OLED_64x64::lineWide(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value, uint8_t width)
{
	lineWideRAM(OLED_64x64_START_COL + x0, OLED_64x64_START_ROW + y0, OLED_64x64_START_COL + x1, OLED_64x64_START_ROW + y1, value, width);
}
	void RGB_OLED_64x64::lineH(uint8_t x, uint8_t y, uint8_t width)
{
	lineHRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, width);
}
void RGB_OLED_64x64::lineH(uint8_t x, uint8_t y, uint8_t width, uint16_t value)
{
	lineHRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, width, value);
}	
void RGB_OLED_64x64::lineV(uint8_t x, uint8_t y, uint8_t height)
{
	lineVRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, height);
}
void RGB_OLED_64x64::lineV(uint8_t x, uint8_t y, uint8_t height, uint16_t value)
{	
	lineVRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, height, value);
}

void RGB_OLED_64x64::rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	rectRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, width, height);
}
void RGB_OLED_64x64::rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value)
{
	rectRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, width, height, value);
}
void RGB_OLED_64x64::rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{	
	rectFillRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, width, height);
}
void RGB_OLED_64x64::rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value)
{
	rectFillRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, width, height, value);
}

void RGB_OLED_64x64::circle(uint8_t x, uint8_t y, uint8_t radius)
{
	circleRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, radius);
}
void RGB_OLED_64x64::circle(uint8_t x, uint8_t y, uint8_t radius, uint16_t value)
{
	circleRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, radius, value);
}
void RGB_OLED_64x64::circleFill(uint8_t x, uint8_t y, uint8_t radius)
{
	circleFillRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, radius);
}
void RGB_OLED_64x64::circleFill(uint8_t x, uint8_t y, uint8_t radius, uint16_t value)
{
	circleFillRAM(OLED_64x64_START_COL + x, OLED_64x64_START_ROW + y, radius, value);
}