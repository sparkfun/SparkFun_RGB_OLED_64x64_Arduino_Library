/*
SparkFun_SSD1357_OLED.cpp

This is the implemenation of the methods in 'SparkFun_SSD1357_OLED.h'
intended for use in the Aduino IDE. 

There are three main parts:
1) Implementation of the default font class 'MicroviewMonochromeProgMemBMPFont'


Owen Lyke
July 2018

*/
#include "SparkFun_SSD1357_OLED.h"



// Choosing an intermediate option between a full screen buffer (128*128*2)
// and having no working memory at all (which would require lots of
// separate SPI transactions to fill a screen)
uint8_t working_buff[SSD1357_WORKING_BUFF_NUM_PIXELS*SSD1357_BYTES_PER_PIXEL];

#ifndef SSD1357_DONT_USE_DEF_FONT
	uint8_t defFontScratch[5*8*2];		// A working area to convert the bit-encoded data to 16bit
	MicroviewMonochromeProgMemBMPFont SSD1357DefFont5x7(font5x7,defFontScratch, 6);	// This has to be defined here as opposed to in the h file because of multiple inclusion errors. This means that the user will not be able to operate on it (private to this cpp file) but thats OK because it is the default and not meant to be modified.	
#endif /* SSD1357_DONT_USE_DEF_FONT */



MicroviewMonochromeProgMemBMPFont::MicroviewMonochromeProgMemBMPFont(const unsigned char * pMap, uint8_t * pPad, uint8_t headerSize)
{
	fontMapPtr = pMap;
	charDataPtr = pPad;
	// alphaDataPtr = pAlphaPad;
	_fontHeaderSize = headerSize;

	_foregroundColor = 0xFFFF;
	_backgroundColor = 0x0000;

	_prevWriteCausedNewline = false;

	_fontWidth = pgm_read_byte(fontMapPtr + 0);
	_fontHeight = pgm_read_byte(fontMapPtr + 1);
	_startCharASCII = pgm_read_byte(fontMapPtr + 2);
	_totalCharsASCII = pgm_read_byte(fontMapPtr + 3);
	_fontMapWidth = (pgm_read_byte(fontMapPtr + 4) * 100) + pgm_read_byte(fontMapPtr + 5);	// Right at this moment I'm not sure how this works.. Why high byte * 100? Why not * 256?
}

uint8_t * MicroviewMonochromeProgMemBMPFont::getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	uint8_t ascii_char_diff = (val - _startCharASCII);
	uint8_t num_bytes_per_column = ((_fontHeight - 1)/8) + 1;

	uint8_t y_index = 0;
	uint8_t x_index = 0;
	uint32_t local_byte_index = 0;
	uint32_t byte_index = 0;
	uint32_t chars_per_row = _fontMapWidth / _fontWidth;
	uint32_t wrap_byte_offset = 0;
	uint8_t bit_index = 0;

	uint8_t data_byte = 0;

	for(uint16_t char_index = 0; char_index < 2*(_fontWidth * _fontHeight); char_index+=2)
	{
		// Loop through every pixel in the scratch space by incrementing the char_index (byte index) by 2 (because each pixel takes 2 bytes)
		// For each pixel determine if it should be filled in or not
		y_index = (char_index / (2*_fontWidth));				// Tells you where in the character frame you are - y axis
		x_index = ((char_index - (2*y_index*_fontWidth))/2);	// Tells you where in the character frame you are - x axis
		local_byte_index = (_fontMapWidth * ((y_index / 8)));	// Adds the map width for each "page" down you go (page is 8 rows high)
		wrap_byte_offset = (ascii_char_diff / chars_per_row)*num_bytes_per_column*_fontMapWidth; // Every time your character difference exceeds the number of characters wide that the map was then you also need to add more bytes to compensate
		byte_index = _fontWidth * (ascii_char_diff % chars_per_row)+ wrap_byte_offset + local_byte_index + x_index + _fontHeaderSize;
		bit_index = y_index - (8 * (y_index / 8));

		// Now conditionally fill the char data with either all ones or all zeros based on value of that bit
		data_byte = pgm_read_byte(fontMapPtr + byte_index);

		if(data_byte & (0x01 << bit_index))
		{
			*(charDataPtr + char_index) = ((_foregroundColor & 0xFF00) >> 8);
			*(charDataPtr + char_index + 1) = (_foregroundColor & 0x00FF); 
		}
		else
		{
			*(charDataPtr + char_index) = ((_backgroundColor & 0xFF00) >> 8);
			*(charDataPtr + char_index + 1) = (_backgroundColor & 0x00FF); 
		}
	}
	return charDataPtr;	// Return a pointer to the data so that the OLED driver knows where to find it
}

uint8_t * MicroviewMonochromeProgMemBMPFont::getAlpha(uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	// if(alphaDataPtr == NULL)
	// {
		return NULL; // This allows the user to decide not to include alpha channel to conserve space
	// }
}

uint8_t * MicroviewMonochromeProgMemBMPFont::getFrameData(uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	frameData[0] = cursor_y;
	frameData[1] = cursor_x;
	frameData[2] = _fontHeight;
	frameData[3] = _fontWidth;
	// It is critical that the number of uint16_t types returned by the BMP function is greater than or equal to height * width

	return frameData;
}

bool MicroviewMonochromeProgMemBMPFont::advanceState(uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	// Okay! Here is where we can change the location of text :)
	// this is the callback function that happens after the driver has already received the data pointers but hasn't actually output the data to GDDRAM yet, therefor it's not advisable to change the chardata, framedata, or alpha data
	// It IS advisable, however, to change the cursor location and provide a yea or nea to the driver for actually printing a character.

	// First things first move the cursor. 
	uint16_t newX = cursor_x;
	uint16_t newY = cursor_y;

	// A special case is the newline character '\n'
	if(val == '\n')
	{
		if(!_prevWriteCausedNewline)
		{
			newX = reset_x;
			// newY += _fontHeight + 1;
			newY += _fontHeight;

			cursor_x = newX;
			cursor_y = newY;

			_prevWriteCausedNewline = true;
		}

		return false;						// The driver will not display anything for newline chars. It just starts a new line
	}

	_prevWriteCausedNewline = false;

	if(val == '\r')
	{
		return false; 						// You can make a 'blacklist' of characters that should not cause any action on the display by returning false when they are requested
	}

	// If the character that is about to be printed is not a newline character then it will probably take up space and so the cursor should be incremented. 
	// Its OK to increment cursor data because the frame data exists in another array that the driver will access 
	// newX += _fontWidth + 1;	// Move left-to-right first
	newX += _fontWidth;
	if((newX > (margin_x - _fontWidth)))	// But start a new line if you go over the edge
	{
		newX = reset_x;
		// newY += _fontHeight + 1;
		newY += _fontHeight;

		_prevWriteCausedNewline = true;

		if((newY > (margin_y - _fontHeight)))
		{
			cursor_x = newX;
			cursor_y = newY;

			return false;	// This indicates that the driver should not print anything
		}	
	}

	cursor_x = newX;
	cursor_y = newY;

	return true;			// This tells the driver to go ahead and print the data
	// By the way if the cursor goes off the screen it is up to the user to reset the cursor to zero (after clearing the screen, most likely)
}

uint8_t * MicroviewMonochromeProgMemBMPFont::Wrapper_to_call_getBMP(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	MicroviewMonochromeProgMemBMPFont * self = (MicroviewMonochromeProgMemBMPFont *)pt2Object;
	return self->getBMP(val, screen_width, screen_height);
}

uint8_t * MicroviewMonochromeProgMemBMPFont::Wrapper_to_call_getAlpha(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	MicroviewMonochromeProgMemBMPFont * self = (MicroviewMonochromeProgMemBMPFont *)pt2Object;
	return self->getAlpha(val, screen_width, screen_height);
}

uint8_t * MicroviewMonochromeProgMemBMPFont::Wrapper_to_call_getFrameData(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	MicroviewMonochromeProgMemBMPFont * self = (MicroviewMonochromeProgMemBMPFont *)pt2Object;
	return self->getFrameData(val, screen_width, screen_height);
}

bool MicroviewMonochromeProgMemBMPFont::Wrapper_to_call_advanceState(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	MicroviewMonochromeProgMemBMPFont * self = (MicroviewMonochromeProgMemBMPFont *)pt2Object;
	return self->advanceState(val, screen_width, screen_height);
}

void MicroviewMonochromeProgMemBMPFont::Wrapper_to_call_setCursorValues(void * pt2Object, uint16_t x, uint16_t y, uint16_t xReset, uint16_t yReset, uint16_t xMargin, uint16_t yMargin)
{
	MicroviewMonochromeProgMemBMPFont * self = (MicroviewMonochromeProgMemBMPFont *)pt2Object;
	self->setCursorValues(x, y, xReset, yReset, xMargin, yMargin);
}

































SSD1357::SSD1357( void )
{
	
}

void SSD1357::begin(uint8_t dcPin, uint8_t rstPin, uint8_t csPin, SPIClass &spiInterface, uint32_t spiFreq)
{
	// Associate 
	_dc = dcPin;
	_rst = rstPin;
	_cs = csPin;
	_spi = &spiInterface;

	_spiFreq = spiFreq;

	_fillColor = 0xFFFF;

	linkDefaultFont();

	// Set pinmodes
	pinMode(_cs, OUTPUT);
	pinMode(_rst, OUTPUT);
	pinMode(_dc, OUTPUT);

	// Set pins to default positions
	digitalWrite(_cs, HIGH);
	digitalWrite(_rst, HIGH);
	digitalWrite(_dc, HIGH);


	// Power up the device

	
	/* 
	Okay, lesson time.
	SPI is pretty darn cool and hard to mess up, write?
	Mostly yes, but there is at least one gotcha that has
	bitten me a few times. When using a single SPI bus to
	talk to different devices that use different SPI
	modes (MODE0, MODE1, MODE2, and MODE3) the bus settings
	have to change. I've noticed that the first byte that 
	is sent after a mode change can be mis-interpreted. I
	have a hunch (but have not confirmed) that this is
	due to a mis-match of the clock polarity at idle. This 
	condition can also happen right after the SPI
	hardware is started for the first time with SPI.begin.

	I noticed that I had to call 'startup()' twice to get 
	the display working. After being stumped and trying 
	more delays and some code rearranging it struck me that 
	startup() was the first time I ever sent data on the 
	SPI peripheral.
	
	Without being bothered to 100% confirm the cause I
	tried a solution: send a random byte to no particular
	device (don't activate any chip selects) to 'set' the
	SPI peripheral into the right mode of operation. This
	technique, applied here, did the trick. This whole
	problem is probably worth an in-depth investigation
	in the future.

	*/

	// try starting SPI with a simple byte transmisssion to 'set' the SPI peripherals
	uint8_t temp_buff[1];
	_spi->beginTransaction(SPISettings(_spiFreq, SSD1357_SPI_DATA_ORDER, SSD1357_SPI_MODE));
	_spi->transfer(temp_buff, 1);
	_spi->endTransaction();

	startup();	// It really bothers me that I have to call startup twice... I've trid adding more of a delay - oh! Maybe there is a SPI problem. Bingo. See note above
}

void SSD1357::setCSlow( void )
{
	digitalWrite(_cs, LOW);
}

void SSD1357::setCShigh(void)
{
	digitalWrite(_cs, HIGH);
}

void SSD1357::startup( void )
{
	// Assume that VDD and VCC are stable when this function is called

	delay(20);

	digitalWrite(_rst, LOW);
	// delayMicroseconds(10);
	delay(5);
	digitalWrite(_rst, HIGH);

	delay(200);

	// Now you can do initialization
}

void SSD1357::write_ram(uint8_t * pdata, uint8_t startrow, uint8_t startcol, uint8_t stoprow, uint8_t stopcol, uint16_t size)
{
	// Use commands to set the starting/ending locations in GDDRAM 
	setRowAddress(startrow, stoprow);
	setColumnAddress(startcol, stopcol);

	// Use write_bytes() to send the data along with a data flag
	enableWriteRAM();				

	setCSlow();

	write_bytes(pdata, true, size);

	setCShigh();
}

void SSD1357::write_bytes(uint8_t * pdata, bool DATAcmd, uint16_t size)
{
	// digitalWrite(_cs, LOW);				// Set the chip select line
	digitalWrite(_dc, DATAcmd);		// Set whether transmitting data or command
	delayMicroseconds(5);

	// Now transmit the data
	_spi->beginTransaction(SPISettings(_spiFreq, SSD1357_SPI_DATA_ORDER, SSD1357_SPI_MODE));
	// _spi->transferOut(pdata, size);			// FYI this function would solve a lot of problems but it is not part of Arduino yet. All it does is send out a buffer without modifying the contents. If you need extra performance then try to implement something similar
	_spi->transfer(pdata, size);			
	_spi->endTransaction();

	// digitalWrite(_cs, HIGH);			// Stop talking to the driver
}


size_t SSD1357::write(uint8_t val)
{
	// Need code here to display a letter. Probably will need to advance a cursor index or something too
	
	#ifdef SSD1357_DONT_USE_DEF_FONT
	if((_userBMPFuncPtr == NULL) || (_userFrameFuncPtr == NULL))
	{
		return 0;		// This is provided to prevent undefined behavior. Printing will simply do nothing
	}
	#endif /* SSD1357_DONT_USE_DEF_FONT */

	// If the user either:
		// a) is OK with using the default font or
		// b) has supplied their own font
	// then we are clear to proceed with writing

	uint8_t * chardata = getFontBMP(val);
	// uint8_t * alphadata = getFontAlpha(val);
	uint8_t * framedata = getFontFrameData(val); // The length of data returned by userBMPFunc must correspond to the returned character width and height * 2
	// btw framedata[] = {starty, startx, yheight, xwidth} - user's responsibility that starty + yheight is less than the actual size of the display... same for x direction
	
	bool print_char = fontCallback(val);		// Use the fontCallback function in a custom font to determine what to do after a character is written
	
	// // Write the font data to the ram now

	if((chardata == NULL) || (framedata == NULL))
	{
		return 0;	// Protect memory
	}

	if(print_char)
	{
		uint8_t starty = *(framedata + 0);
		uint8_t startx = *(framedata + 1);
		uint8_t yheight = *(framedata + 2);
		uint8_t xwidth = *(framedata + 3);

		// // Alpha data currently not implemented. Doing so might look something like:
		// blendAlphaData(chardata, alphadata);

		// The only limitation on these fonts is that each character must fit in a rectangular frame.
		write_ram(chardata, starty, startx, (starty + yheight - 1), (startx + xwidth - 1), yheight * xwidth * 2);	// xwidth * yheight as returned by the frame data function MUST be the same as the number of pixels, which is the same as half the number of bytes in the chardata array.
	}
	// Otherwise don't print anything
	return 1;
}





uint8_t * SSD1357::getFontBMP(uint8_t val)
{
	// Calls a user-specified function to get the actual pixel data as a one-dimensional array of uint8_ts
	if(_userBMPFuncPtr == NULL)
	{
		// Use the default
		#ifndef SSD1357_DONT_USE_DEF_FONT
			return SSD1357DefFont5x7.getBMP(val, _width, _height);
		#else
			return NULL;
		#endif	/* SSD1357_DONT_USE_DEF_FONT */
	}
	// Don't use default
	return (*_userBMPFuncPtr)(_object2operateOn, val, _width, _height);
}

uint8_t * SSD1357::getFontAlpha(uint8_t val)
{
	// Though not really implemented yet this should allow the user to specify transparency on the characters.
	// The ideal way to implement this code would be to either 
	// a) have a scratch space representing the whole screen, that can be read from to deterime what goes under an alpha pixel or
	// b) be able to read back the GDDRAM from the display. This is not possible for the 64x64 RGB PMOLED that this library was originally intended for. (But it could be possible for other SSD1357 driven displays!)

	if(_userAlphaFuncPtr == NULL)
	{
		#ifndef SSD1357_DONT_USE_DEF_FONT
			return NULL;	// This just means that the default font does not support transparency
		#else
			return NULL;
		#endif	/* SSD1357_DONT_USE_DEF_FONT */
	}
	return (*_userAlphaFuncPtr)(_object2operateOn, val, _width, _height);
}

uint8_t * SSD1357::getFontFrameData(uint8_t val)
{
	if(_userFrameFuncPtr == NULL)
	{
		// Use default
		#ifndef SSD1357_DONT_USE_DEF_FONT
			return SSD1357DefFont5x7.getFrameData(val, _width, _height);
		#else
			return NULL;
		#endif	/* SSD1357_DONT_USE_DEF_FONT */
	}
	// Don't use the default
	return (*_userFrameFuncPtr)(_object2operateOn, val, _width, _height);
}

bool SSD1357::fontCallback( uint8_t val )
{
	// If the return is a 'true' then the driver will print to the screen
	// If the return is a 'false' then the driver will do nothing
	if(_userFontCallbackPtr == NULL)
	{
		#ifndef SSD1357_DONT_USE_DEF_FONT
			return SSD1357DefFont5x7.advanceState(val, _width, _height);
		#else
			return NULL;
		#endif	/* SSD1357_DONT_USE_DEF_FONT */
	}
	// Don't use the default
	return (*_userFontCallbackPtr)(_object2operateOn, val, _width, _height);
}




















void SSD1357::setColumnAddress(uint8_t start, uint8_t stop)
{
	uint8_t buff[3];
	buff[0] = SSD1357_CMD_SetColumnAddress;
	buff[1] = start;
	buff[2] = stop;

	setCSlow();

	write_bytes(&buff[0], false, 1);	
	write_bytes(&buff[1], true, 2);

	setCShigh();
}

void SSD1357::setRowAddress(uint8_t start, uint8_t stop)
{
	uint8_t buff[3];
	buff[0] = SSD1357_CMD_SetRowAddress;
	buff[1] = start;
	buff[2] = stop;

	setCSlow();

	write_bytes(&buff[0], false, 1);	
	write_bytes(&buff[1], true, 2);

	setCShigh();
}

void SSD1357::enableWriteRAM( void )
{
	uint8_t buff[1];
	buff[0] = SSD1357_CMD_WriteRAM;

	setCSlow();

	write_bytes(&buff[0], false, 1);

	setCShigh();
}

void SSD1357::enableReadRAM( void )
{
	uint8_t buff[1];
	buff[0] = SSD1357_CMD_ReadRAM;

	setCSlow();

	write_bytes(&buff[0], false, 1);

	setCShigh();
}

void SSD1357::setRemapColorDepth(bool inc_Vh, bool rev_ColAddr, bool swap_ColOrder, bool rev_SCAN, bool en_SplitOddEven, uint8_t color_depth_code)
{
	uint8_t buff[3];
	buff[0] = SSD1357_CMD_SetRemapColorDepth;
	buff[1] = 0x00;
	buff[2] = 0x00;
	if(inc_Vh)
	{
		buff[1] |= 0x01;
	}
	if(rev_ColAddr)
	{
		buff[1] |= 0x02;
	}
	if(swap_ColOrder)
	{
		buff[1] |= 0x04;
	}
	if(rev_SCAN)
	{
		buff[1] |= 0x10;
	}
	if(en_SplitOddEven)
	{
		buff[1] |= 0x20;
	}
	buff[1] |= ((0x03 & color_depth_code) << 6);

	if((0x03 & color_depth_code) == SSD1357_COLOR_MODE_65k)
	{
		_colorMode = SSD1357_COLOR_MODE_65k;
	}
	else if((0x03 & color_depth_code) == SSD1357_COLOR_MODE_256)
	{
		_colorMode = SSD1357_COLOR_MODE_256;
	}

	setCSlow();

	write_bytes(&buff[0], false, 1);	
	write_bytes(&buff[1], true, 2);

	setCShigh();
}

void SSD1357::setDisplayStartLine(uint8_t start_line)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetDisplayStartLine;
	buff[1] = start_line;

	setCSlow();

	write_bytes(&buff[0], false, 1);	
	write_bytes(&buff[1], true, 1);

	setCShigh();
}

void SSD1357::setDisplayOffset(uint8_t offset)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetDisplayOffset;
	buff[1] = offset;

	setCSlow();

	write_bytes(&buff[0], false, 1);	
	write_bytes(&buff[1], true, 1);

	setCShigh();
}

void SSD1357::setDisplayMode(uint8_t mode_code)
{
	uint8_t buff[1];
	buff[0] = mode_code;

	setCSlow();

	write_bytes(&buff[0], false, 1);

	setCShigh();
}

void SSD1357::setSleepMode(bool sleep_on)
{
	uint8_t buff[1];
	if(sleep_on == true)
	{
		buff[0] = SSD1357_CMD_SetSleepMode_ON;
	}
	else
	{
		buff[0] = SSD1357_CMD_SetSleepMode_OFF;
	}

	setCSlow();

	write_bytes(&buff[0], false, 1);

	setCShigh();
}

void SSD1357::setResetPrechargePeriod(uint8_t reset_clocks, uint8_t precharge_clocks)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetResetPrechargePeriod;
	buff[1] = (((precharge_clocks & 0x0F) << 4) | (reset_clocks & 0x0F));

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);

	setCShigh();
}

void SSD1357::setClockDivider(uint8_t divider_code)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetClkDiv;
	buff[1] = divider_code;

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);

	setCShigh();
}

void SSD1357::setSecondPrechargePeriod(uint8_t precharge_clocks)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetSecondPrechargePeriod;
	buff[1] = precharge_clocks;

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);

	setCShigh();
}


void SSD1357::setMLUTGrayscale(uint8_t * pdata63B)
{
	uint8_t buff[1];
	buff[0] = SSD1357_CMD_MLUTGrayscale;

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(pdata63B, true, 63);

	setCShigh();
}


void SSD1357::useBuiltInLinearLUT( void )
{
	uint8_t buff[1];
	buff[0] = SSD1357_CMD_UseDefMLUT;

	setCSlow();

	write_bytes(&buff[0], false, 1);

	setCShigh();
}

void SSD1357::setPrechargeVoltage( uint8_t voltage_scale_code)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetPrechargeVoltage;
	buff[1] = voltage_scale_code;

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);

	setCShigh();
}

/*
void SSD1357::setILUTColorA( uint8_t * pdata31B)
{
	
}
*/

/*
void SSD1357::setILUTColorC( uint8_t * pdata31B)
{

}
*/

void SSD1357::setVCOMH( uint8_t voltage_scale_code)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetVCOMH;
	buff[1] = voltage_scale_code;

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);

	setCShigh();
}

void SSD1357::setContrastCurrentABC(uint8_t ccA, uint8_t ccB, uint8_t ccC)
{
	uint8_t buff[4];
	buff[0] = SSD1357_CMD_SetContrastCurrentABC;
	buff[1] = ccA;
	buff[2] = ccB;
	buff[3] = ccC;

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 3);

	setCShigh();
}
void SSD1357::setMasterContrastCurrent(uint8_t ccCode)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetMasterContrastCurrent;
	buff[1] = ccCode;

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);

	setCShigh();
}
void SSD1357::setMUXRatio(uint8_t mux_ratio)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetMuxRatio;
	buff[1] = mux_ratio;

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);

	setCShigh();
}
void SSD1357::setCommandLock(bool locked)
{

	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetCommandLock;
	if(locked == true)
	{
		buff[1] = 0x16;
	}
	else
	{
		buff[1] = 0x12;
	}

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);

	setCShigh();
}











void SSD1357::setupHorizontalScroll(uint8_t scrollParameter, uint8_t startRow, uint8_t stopRow, uint8_t speed)
{
	uint8_t buff[6];
	buff[0] = SSD1357_CMD_Setup_Scrolling;
	buff[1] = scrollParameter;
	buff[2] = (startRow & 0x7F);
	buff[3] = (stopRow & 0x7F);
	buff[4] = (0x00); // Reserved
	buff[5] = (speed & 0x03);

	setCSlow();

	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 5);

	setCShigh();
}
	
void SSD1357::startScrolling( void )
{
	uint8_t buff[1];
	buff[0] = SSD1357_SCROLL_START;

	setCSlow();

	write_bytes(&buff[0], false, 1);

	setCShigh();
}

void SSD1357::stopScrolling( void )
{
	uint8_t buff[1];
	buff[0] = SSD1357_SCROLL_STOP;

	setCSlow();

	write_bytes(&buff[0], false, 1);

	setCShigh();
}
























uint8_t SSD1357::getWidth( void )
{
	return _width;
}

uint8_t 	SSD1357::getHeight( void )
{
	return _height;
}

uint32_t	SSD1357::getSPIFreq( void )
{
	return _spiFreq;
}

void 		SSD1357::setWidth(uint8_t val)
{
	_width = val;
}

void 		SSD1357::setHeight(uint8_t val)
{
	_height = val;
}

void 		SSD1357::setSPIFreq(uint32_t freq)
{
	_spiFreq = freq;
}

// Set a new font by supplying all required function pointers
void 	SSD1357::setFont(
					void * object,
					uint8_t * (*BMPFuncPtr)(void *, uint8_t, uint16_t, uint16_t), 
					uint8_t * (*AlphaFuncPtr)(void *, uint8_t, uint16_t, uint16_t),
					uint8_t * (*frameFuncPtr)(void *, uint8_t, uint16_t, uint16_t), 
					bool 	(*fontCallbackPtr)(void *, uint8_t, uint16_t, uint16_t),
					void 	(*setCursorValuesPtr)(void *, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)
)
{
	_object2operateOn = object;
	_userBMPFuncPtr = BMPFuncPtr;
	_userAlphaFuncPtr = AlphaFuncPtr;
	_userFrameFuncPtr = frameFuncPtr;
	_userFontCallbackPtr = fontCallbackPtr;
	_userFontSetCursorValuesPtr = setCursorValuesPtr;
}

// Set all font function pointers to the default functions
void SSD1357::linkDefaultFont( void )
{
	// Load the default font
	setFont(
					&SSD1357DefFont5x7,
					SSD1357DefFont5x7.Wrapper_to_call_getBMP, 
					SSD1357DefFont5x7.Wrapper_to_call_getAlpha,
					SSD1357DefFont5x7.Wrapper_to_call_getFrameData, 
					SSD1357DefFont5x7.Wrapper_to_call_advanceState,
					SSD1357DefFont5x7.Wrapper_to_call_setCursorValues);

	// Setup margins on the defualt font and zero out the cursor
	SSD1357DefFont5x7.setCursorValues(0, 0, 0, 0, 127, 127);	// These are the maximum values allowed in the SSD1357
}

// Set cursor location, reset location, and margins
void 	SSD1357::setFontCursorValues(uint8_t x, uint8_t y, uint8_t xReset, uint8_t yReset, uint8_t xMargin, uint8_t yMargin)
{
	_cursorX = x;			// Store the newest values for ease
	_cursorY = y;
	_xReset = xReset;
	_yReset = yReset;
	_xMargin = xMargin; 
	_yMargin = yMargin;
	(*_userFontSetCursorValuesPtr)(_object2operateOn, (uint16_t)x, (uint16_t)y, (uint16_t)xReset, (uint16_t)yReset, (uint16_t)xMargin, (uint16_t)yMargin);
}

// Moves the cursor while keeping the reset location and margins intact
void SSD1357::setCursorRAM(uint8_t x, uint8_t y)
{
	// Just moves the cursor, doesn't change margins or reset values
	setFontCursorValues(x, y, _xReset, _yReset, _xMargin, _yMargin);
}

// This function exists to allow the user access to the default font colors
void SSD1357::setDefaultFontColors(uint16_t foreground, uint16_t background)
{
	SSD1357DefFont5x7._foregroundColor = foreground;
	SSD1357DefFont5x7._backgroundColor = background;
}






// Drawing functions!

uint16_t get65kValueRGB(uint8_t R, uint8_t G, uint8_t B)
{
	uint16_t rScaled = (R*31)/255;
	uint16_t gScaled = (G*63)/255;
	uint16_t bScaled = (B*31)/255;
	return (((rScaled & 0x001F) << 11) | ((gScaled & 0x003F) << 5) | ((bScaled & 0x001F) << 0));
}
uint16_t get65kValueHSV(uint16_t hue, uint8_t sat, uint8_t val)
{
	//For an awesome analysis of HSV to RGB conversion for small CPUs check this out:  http://www.vagrearg.org/content/hsvrgb
	uint8_t r, g, b;
	fast_hsv2rgb_32bit(hue, sat, val, &r, &g, &b);
	return get65kValueRGB(r, g, b);
}

void SSD1357::setFillColor(uint16_t color)
{
	_fillColor = color;
}

void SSD1357::setPixelRAM(uint8_t x, uint8_t y)
{
	setPixelRAM(x, y, _fillColor);
}

void SSD1357::setPixelRAM(uint8_t x, uint8_t y, uint16_t value)
{
	if((x >= SSD1357_MAX_WIDTH) || (y >= SSD1357_MAX_HEIGHT))	// Make sure we are within the RAM limits
	{
		return;
	}

	working_buff[0] = ((value & 0xFF00) >> 8);
	working_buff[1] = ((value & 0x00FF) >> 0);
	write_ram(working_buff, y, x, SSD1357_STOP_ROW, SSD1357_STOP_COL, 2);
}




void SSD1357::lineRAM(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	lineRAM(x0, y0, x1, y1, _fillColor);
}
void SSD1357::lineRAM(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value)
{
	uint8_t absY, absX;

	if(y1 > y0)
	{
		absY = y1 - y0;
	}
	else
	{
		absY = y0 - y1;
	}

	if(x1 > x0)
	{
		absX = x1 - x0;
	}
	else
	{
		absX = x0 - x1;
	}



  	if( absY < absX )
  	{
	    if( x0 > x1 )
	    {
	      	plotLineLow(x1, y1, x0, y0, value, 0);
	    }
	    else
	    {
	      	plotLineLow(x0, y0, x1, y1, value, 0);
	    }
	}
  	else
	{

    	if( y0 > y1 )
      	{
      		plotLineHigh(x1, y1, x0, y0, value, 0);
      	}
    	else
    	{
      		plotLineHigh(x0, y0, x1, y1, value, 0);
  		}
	}
}

void SSD1357::lineWideRAM(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width)
{
	lineWideRAM(x0, y0, x1, y1, _fillColor, width);
}
void SSD1357::lineWideRAM(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width, uint16_t value)
{
	uint8_t absY, absX;

	if(y1 > y0)
	{
		absY = y1 - y0;
	}
	else
	{
		absY = y0 - y1;
	}

	if(x1 > x0)
	{
		absX = x1 - x0;
	}
	else
	{
		absX = x0 - x1;
	}



  	if( absY < absX )
  	{
	    if( x0 > x1 )
	    {
	      	plotLineLow(x1, y1, x0, y0, value, width);
	    }
	    else
	    {
	      	plotLineLow(x0, y0, x1, y1, value, width);
	    }
	}
  	else
	{

    	if( y0 > y1 )
      	{
      		plotLineHigh(x1, y1, x0, y0, value, width);
      	}
    	else
    	{
      		plotLineHigh(x0, y0, x1, y1, value, width);
  		}
	}
}

void SSD1357::lineHRAM(uint8_t x, uint8_t y, uint8_t width)
{
	lineHRAM(x, y, width, _fillColor);
}
void SSD1357::lineHRAM(uint8_t x, uint8_t y, uint8_t width, uint16_t value)
{
	fast_filled_rectangle(x, y, x+width, y, value);
}

void SSD1357::lineVRAM(uint8_t x, uint8_t y, uint8_t height)
{
	lineVRAM(x, y, height, _fillColor);
}
void SSD1357::lineVRAM(uint8_t x, uint8_t y, uint8_t height, uint16_t value)
{
	fast_filled_rectangle(x, y, x, y+height, value);
}


void SSD1357::rectRAM(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	rectRAM(x, y, width, height, _fillColor);
}
void SSD1357::rectRAM(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value)
{
	fast_filled_rectangle(x, y, x, y+height, value);
	fast_filled_rectangle(x+width, y, x+width, y+height, value);
	fast_filled_rectangle(x, y, x+width, y, value);
	fast_filled_rectangle(x, y+height, x+width, y+height, value);
}
void SSD1357::rectFillRAM(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	rectFillRAM(x, y, width, height, _fillColor);
}
void SSD1357::rectFillRAM(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value)
{
	fast_filled_rectangle(x, y, x+width, y+height, value);
}

void SSD1357::fast_filled_rectangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1, int16_t value)
{
	// This uses the boundaries on write_ram to quickly fill a given rectangle
	boolean x0offscreen = false;
	boolean x1offscreen = false;
	boolean y0offscreen = false;
	boolean y1offscreen = false;

	// Ensure bounds are good
	if(x0 >= SSD1357_MAX_WIDTH)
	{
		x0 = SSD1357_MAX_WIDTH-1;
		x0offscreen = true;
	}
	if(x1 >= SSD1357_MAX_WIDTH)
	{
		x1 = SSD1357_MAX_WIDTH-1;
		x1offscreen = true;
	}
	if(y0 >= SSD1357_MAX_HEIGHT)
	{
		y0 = SSD1357_MAX_HEIGHT-1;
		y0offscreen = true;
	}
	if(y1 >= SSD1357_MAX_HEIGHT)
	{
		y1 = SSD1357_MAX_HEIGHT-1;
		y1offscreen = true;
	}

	if(x0 < 0)
	{
		x0 = 0;
		x0offscreen = true;
	}
	if(x1 < 0)
	{
		x1 = 0;
		x1offscreen = true;
	}
	if(y0 < 0)
	{
		y0 = 0;
		y0offscreen = true;
	}
	if(y1 < 0)
	{
		y1 = 0;
		y1offscreen = true;
	}

	if((x1offscreen == true) && (x0offscreen == true))
	{
		return;
	}
	if((y1offscreen == true) && (y0offscreen == true))
	{
		return;
	}



	// Ensure the order is right
	if(x0 > x1)
	{
		uint8_t temp = x0;
		x0 = x1;
		x1 = temp;
	}

	if(y0 > y1)
	{
		uint8_t temp = y0;
		y0 = y1;
		y1 = temp;
	}

	uint8_t width = x1-x0+1;
	uint8_t height = y1-y0+1;

	uint8_t rows_per_block = SSD1357_WORKING_BUFF_NUM_PIXELS / width;
	uint8_t num_full_blocks = height/rows_per_block;
	uint8_t remaining_rows = height - (num_full_blocks * rows_per_block);

	uint8_t offsety = 0;

	for(uint8_t indi = 0; indi < num_full_blocks; indi++)
	{
		fill_working_buffer(value, rows_per_block*width);
		write_ram(working_buff, y0+offsety, x0, y1, x1, 2*rows_per_block*width);
		offsety += rows_per_block;
	}
	fill_working_buffer(value, remaining_rows*width);
	write_ram(working_buff, y0+offsety, x0, y1, x1, 2*remaining_rows*width);
}

void SSD1357::circleRAM(uint8_t x, uint8_t y, uint8_t radius)
{
	circleRAM(x, y, radius, _fillColor);
}

void SSD1357::circleRAM(uint8_t x, uint8_t y, uint8_t radius, uint16_t value)
{
	if(radius < 2)
	{
		circle_Bresenham(x, y, radius, value, false);
	}
	else
	{
		circle_midpoint(x, y, radius, value, false);
	}
	
}
void SSD1357::circleFillRAM(uint8_t x, uint8_t y, uint8_t radius)
{
	circleFillRAM(x, y, radius, _fillColor);
}

void SSD1357::circleFillRAM(uint8_t x, uint8_t y, uint8_t radius, uint16_t value)
{	
    if(radius < 2)
	{
		circle_Bresenham(x, y, radius, value, true);
	}
	else
	{
		circle_midpoint(x, y, radius, value, true);
	}
}	







// Proteted drawing functions
void SSD1357::fill_working_buffer(uint16_t value, uint8_t num_pixels)
{
	if(num_pixels > SSD1357_WORKING_BUFF_NUM_PIXELS)
	{
		num_pixels = SSD1357_WORKING_BUFF_NUM_PIXELS;
	}

	for(uint8_t indi = 0; indi < num_pixels; indi++)
	{
		working_buff[2*indi + 0] = ((value & 0xFF00) >> 8);
		working_buff[2*indi + 1] = ((value & 0x00FF) >> 0);
	}
}

void SSD1357::plotLineLow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value, uint8_t width)
{
  uint8_t dx = x1 - x0;	// Guaranteed positive
  int8_t dy = y1 - y0;
  int8_t yi = 1;
  if( dy < 0 )
  {
  	yi = -1;
    dy = -dy;
  }
  int16_t D = 2*dy - dx;
  uint8_t y = y0;

  for(uint8_t x = x0; x < x1; x++)
  {
  	if(width < 2)
  	{
  		setPixelRAM(x, y, value);
  	}
  	else
  	{
  		circleFillRAM(x, y, width/2, value);
  	}
    if( D > 0 )
    {
       y = y + yi;
       D = D - 2*dx;
    }
    D = D + 2*dy;
  }
}
void SSD1357::plotLineHigh(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value, uint8_t width)
{
  uint8_t dy = y1 - y0;	// Guaranteed positive
  int8_t dx = x1 - x0;
  int8_t xi = 1;
  if( dx < 0 )
  {
  	xi = -1;
    dx = -dx;
  }
  int16_t D = 2*dx - dy;
  uint8_t x = x0;

  for(uint8_t y = y0; y < y1; y++)
  {
  	if(width < 2)
  	{
  		setPixelRAM(x, y, value);
  	}
  	else
  	{
  		circleFillRAM(x, y, width/2, value);
  	}
    if( D > 0 )
    {
       x = x + xi;
       D = D - 2*dy;
    }
    D = D + 2*dx;
  }
}

void SSD1357::circle_eight(uint8_t xc, uint8_t yc, int16_t dx, int16_t dy, uint16_t value, boolean fill)
{
	setPixelRAM(xc+dx, yc+dy, value);
	setPixelRAM(xc-dx, yc+dy, value);
	setPixelRAM(xc+dx, yc-dy, value);
	setPixelRAM(xc-dx, yc-dy, value);
	setPixelRAM(xc+dy, yc+dx, value);
	setPixelRAM(xc-dy, yc+dx, value);
	setPixelRAM(xc+dy, yc-dx, value);
	setPixelRAM(xc-dy, yc-dx, value);

	if(fill)
	{
		fast_filled_rectangle(xc-dx, yc+dy, xc+dx, yc+dy, value);
    	fast_filled_rectangle(xc-dx, yc-dx, xc+dx, yc-dx, value);
    	fast_filled_rectangle(xc-dy, yc+dx, xc+dy, yc+dx, value);
    	fast_filled_rectangle(xc-dy, yc-dx, xc+dy, yc-dx, value);
	}
}

void SSD1357::circle_Bresenham(uint8_t x, uint8_t y, uint8_t radius, uint16_t value, boolean fill)
{
	// Thanks to the tutorial here: https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/
	uint8_t dx = 0;
	uint8_t dy = radius;
	uint8_t D = 3 - 2*radius;

	if(fill)
	{
		setPixelRAM(x, y, value);
		if(radius == 0)
		{
			return;
		}
	}

	if(radius == 0)
	{
		setPixelRAM(x, y, value);
		return;
	}

	while(dy >= dx)
	{
		circle_eight(x, y, dx, dy, value, fill);
		dx++;
		if(D > 0)
		{
			dy--; 
		    D = D + 4 * (dx - dy) + 10;
		}
		else
		{
			D = D + 4 * dx + 6;
		}
	}
}

void SSD1357::circle_midpoint(uint8_t xc, uint8_t yc, uint8_t radius, uint16_t value, boolean fill)
{		
	// Thanks to the tutorial here: https://www.geeksforgeeks.org/mid-point-circle-drawing-algorithm/
    uint8_t dx = radius;
    uint8_t dy = 0;

    // Set first or center pixel
    setPixelRAM(xc+dx, yc+dy, value);
    if (radius > 0)
    {
       	setPixelRAM(xc-dx, yc+dy, value);
        setPixelRAM(xc+dy, yc-dx, value);
        setPixelRAM(xc+dy, yc+dx, value);

        if(fill)
        {
        	fast_filled_rectangle(xc-dx, yc, xc+dx, yc, value);
        	fast_filled_rectangle(xc, yc-dx, xc, yc+dx, value);
        }
    }

    // Initializing the value of P
    int16_t P = 1 - radius;
    while (dx > dy)
    { 
        dy++;
        
        if (P <= 0)
        {
        	// Mid-point is inside or on the perimeter
            P = P + 2*dy + 1;
        }
        else
        {
        	// Mid-point is outside the perimeter
            dx--;
            P = P + 2*dy - 2*dx + 1;
        }
         
        // All the perimeter points have already been printed
        if (dx < dy)
        {
            break;
        }

        setPixelRAM(xc+dx, yc+dy, value);
        setPixelRAM(xc-dx, yc+dy, value);
        setPixelRAM(xc+dx, yc-dy, value);
        setPixelRAM(xc-dx, yc-dy, value);

        if(fill)
        {
        	fast_filled_rectangle(xc-dx, yc+dy, xc+dx, yc+dy, value);
        	fast_filled_rectangle(xc-dx, yc-dy, xc+dx, yc-dy, value);
        }
         
        // If the generated point is on the line x = y then 
        // the perimeter points have already been printed
        if (dx != dy)
        {
            setPixelRAM(xc+dy, yc+dx, value);
        	setPixelRAM(xc-dy, yc+dx, value);
        	setPixelRAM(xc+dy, yc-dx, value);
        	setPixelRAM(xc-dy, yc-dx, value);

        	if(fill)
	        {
	        	fast_filled_rectangle(xc-dy, yc+dx, xc+dy, yc+dx, value);
	        	fast_filled_rectangle(xc-dy, yc-dx, xc+dy, yc-dx, value);
	        }
        }
    } 
}

