#include "SparkFun_SSD1357_OLED.h"
/*
INDEX:



*/

#ifndef SSD1357_DONT_USE_DEF_FONT
uint8_t defFontScratch[5*8*2];		// A working area to convert the bit-encoded data to 16bit
MonochromeProgMemBMPFont SSD1357DefFont5x7(font5x7,defFontScratch, NULL, 6);	// This has to be defined here as opposed to in the h file because of multiple inclusion errors. This means that the user will not be able to operate on it (private to this cpp file) but thats OK because it is the default and not meant to be modified.	
#endif /* SSD1357_DONT_USE_DEF_FONT */




MonochromeProgMemBMPFont::MonochromeProgMemBMPFont(unsigned char * pMap, uint8_t * pPad, uint8_t * pAlphaPad, uint8_t headerSize)
{
	fontMapPtr = pMap;
	charDataPtr = pPad;
	alphaDataPtr = pAlphaPad;
	_fontHeaderSize = headerSize;

	_fontWidth = pgm_read_byte(fontMapPtr + 0);
	_fontHeight = pgm_read_byte(fontMapPtr + 1);
	_startCharASCII = pgm_read_byte(fontMapPtr + 2);
	_totalCharsASCII = pgm_read_byte(fontMapPtr + 3);
	_fontMapWidth = (pgm_read_byte(fontMapPtr + 4) * 100) + pgm_read_byte(fontMapPtr + 5);	// Right at this moment I'm not sure how this works.. Why high byte * 100? Why not * 256?
}

void MonochromeProgMemBMPFont::resetCursor( void )
{
	setCursor(0, 0);	// Function and x/y values inherited from CustomFont65k
}

void MonochromeProgMemBMPFont::setMargins(uint8_t left, uint8_t right, uint8_t top, uint8_t bottom)
{
	leftMargin = left;
	rightMargin = right;
	topMargin = top;
	bottomMargin = bottom;
}

uint8_t * MonochromeProgMemBMPFont::getBMP(uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	uint8_t ascii_char_diff = (val - _startCharASCII);
	uint8_t num_bytes_per_column = ((_fontHeight - 1)/8) + 1;

	uint8_t y_index = 0;
	uint8_t x_index = 0;
	uint16_t local_byte_index = 0;
	uint16_t byte_index = 0;
	uint8_t bit_index = 0;

	uint8_t data_byte = 0;

	uint8_t tempcount = 0;
	Serial.print("Font width: "); Serial.println(_fontWidth, DEC);
	Serial.print("Font height: "); Serial.println(_fontHeight, DEC);
	Serial.print("Val: 0x"); Serial.println(val, HEX);
	Serial.print("ASCII start char: "); Serial.println(_startCharASCII, DEC);
	Serial.print("Total chars: "); Serial.println(_totalCharsASCII, DEC);
	Serial.print("ASCII Char Difference: "); Serial.println(ascii_char_diff, DEC);

	for(uint16_t char_index = 0; char_index < 2*(_fontWidth * _fontHeight); char_index+=2)
	{
		// Loop through every pixel in the scratch space by incrementing the char_index (byte index) by 2 (because each pixel takes 2 bytes)
		// For each pixel determine if it should be filled in or not
		y_index = (char_index / (2*_fontWidth));
		x_index = ((char_index - (2*y_index*_fontWidth))/2);
		local_byte_index = (_fontWidth * (y_index / 8));
		byte_index = _fontWidth * ascii_char_diff + local_byte_index + _fontHeaderSize + x_index;
		bit_index = y_index - (8 * (y_index / 8));

		// // Serial.println(byte_index, DEC);
		// // Serial.println(bit_index, DEC);
		// // byte_index = 0;

		// // if(byte_index > (1542-1))
		// // {
		// // 	Serial.println("Byte index size warning");
		// // 	byte_index = 1541;
		// // }

		

		// Now conditionally fill the char data with either all ones or all zeros based on value of that bit
		data_byte = pgm_read_byte(fontMapPtr + byte_index);
		if(data_byte & (0x01 << bit_index))
		{
			*(charDataPtr + char_index) = 0xFF;
			*(charDataPtr + char_index + 1) = 0xFF; 
			Serial.print("X");
		}
		else
		{
			*(charDataPtr + char_index) = 0x00;
			*(charDataPtr + char_index + 1) = 0x00; 
			Serial.print(" ");
		}

		tempcount++;
		if(tempcount > (_fontWidth-1))
		{
			Serial.println();
			tempcount = 0;
		}

		// Serial.println(char_index, DEC);
	}
	Serial.println();
	return charDataPtr;	// Return a pointer to the data so that the OLED driver knows where to find it
}

uint8_t * MonochromeProgMemBMPFont::getAlpha(uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	if(alphaDataPtr == NULL)
	{
		return NULL; // This allows the user to decide not to include alpha channel to conserve space
	}
}

uint8_t * MonochromeProgMemBMPFont::getFrameData(uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	frameData[0] = cursor_y;
	frameData[1] = cursor_x;
	frameData[2] = _fontHeight;
	frameData[3] = _fontWidth;
	// It is critical that the number of uint16_t types returned by the BMP function is greater than or equal to height * width

	return frameData;
}

bool MonochromeProgMemBMPFont::advanceState(uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	// Okay! Here is where we can change the location of text :)
	// this is the callback function that happens after the driver has already received the data pointers but hasn't actually output the data to GDDRAM yet, therefor it's not advisable to change the chardata, framedata, or alpha data
	// It IS advisable, however, to change the cursor location and provide a yea or nea to the driver for actually printing a character.

	// First things first move the cursor. 
	uint16_t newX = 0;
	uint16_t newY = 0;

	// A special case is the newline character '\n'
	if(val == '\n')
	{
		newX = leftMargin;
		newY = cursor_y + _fontHeight;

		cursor_x = newX;
		cursor_y = newY;

		return false;						// The driver will not display anything for newline chars. It just starts a new line
	}

	// If the character that is about to be printed is not a newline character then it will probably take up space and so the cursor should be incremented. 
	// Its OK to increment cursor data because the frame data exists in another array that the driver will access 
	newX = cursor_x + _fontWidth;	// Move left-to-right first
	if((newX > (rightMargin - _fontWidth)))	// But start a new line if you go over the edge
	{
		newX = leftMargin;
		newY = cursor_y + _fontHeight;		//
		if((newY > (bottomMargin - _fontHeight)))
		{
			cursor_x = newX;
			cursor_y = newY;

			return false;	// This indicates that the driver should not print anything
		}

		cursor_x = newX;
		cursor_y = newY;
	}

	return true;			// This tells the driver to go ahead and print the data
	// By the way if the cursor goes off the screen it is up to the user to reset the cursor to zero (after clearing the screen, most likely)
}

uint8_t * MonochromeProgMemBMPFont::Wrapper_to_call_getBMP(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	MonochromeProgMemBMPFont * self = (MonochromeProgMemBMPFont *)pt2Object;
	self->getBMP(val, screen_width, screen_height);
}

uint8_t * MonochromeProgMemBMPFont::Wrapper_to_call_getAlpha(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	MonochromeProgMemBMPFont * self = (MonochromeProgMemBMPFont *)pt2Object;
	self->getAlpha(val, screen_width, screen_height);
}

uint8_t * MonochromeProgMemBMPFont::Wrapper_to_call_getFrameData(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	MonochromeProgMemBMPFont * self = (MonochromeProgMemBMPFont *)pt2Object;
	self->getFrameData(val, screen_width, screen_height);
}

bool MonochromeProgMemBMPFont::Wrapper_to_call_advanceState(void * pt2Object, uint8_t val, uint16_t screen_width, uint16_t screen_height)
{
	MonochromeProgMemBMPFont * self = (MonochromeProgMemBMPFont *)pt2Object;
	self->advanceState(val, screen_width, screen_height);
}

void MonochromeProgMemBMPFont::Wrapper_to_call_resetCursor(void * pt2Object)
{
	MonochromeProgMemBMPFont * self = (MonochromeProgMemBMPFont *)pt2Object;
	self->resetCursor();
}

































SSD1357::SSD1357( void )
{
	
}

void SSD1357::begin(uint8_t dcPin, uint8_t rstPin, uint8_t csPin, SPIClass &spiInterface = SPI)
{
	// Associate 
	_dc = dcPin;
	_rst = rstPin;
	_cs = csPin;
	_spi = &spiInterface;

	// Load the default font
	setFont(
					&SSD1357DefFont5x7,
					SSD1357DefFont5x7.Wrapper_to_call_getBMP, 
					SSD1357DefFont5x7.Wrapper_to_call_getAlpha,
					SSD1357DefFont5x7.Wrapper_to_call_getFrameData, 
					SSD1357DefFont5x7.Wrapper_to_call_advanceState,
					SSD1357DefFont5x7.Wrapper_to_call_resetCursor);

	// Setup margins on the defualt font and zero out the cursor
	SSD1357DefFont5x7.setMargins(0, 64, 0, 64);
	SSD1357DefFont5x7.resetCursor();

	// Set pinmodes
	pinMode(_cs, OUTPUT);
	pinMode(_rst, OUTPUT);
	pinMode(_dc, OUTPUT);

	// Set pins to default positions
	digitalWrite(_cs, HIGH);
	digitalWrite(_rst, HIGH);
	digitalWrite(_dc, HIGH);


	// Power up the device
	startup();
}

void SSD1357::startup( void )
{
	digitalWrite(_rst, LOW);
	delay(1);

	digitalWrite(_rst, HIGH);
	delayMicroseconds(5);

	setDisplayMode(SSD1357_CMD_SDM_ALL_OFF);

	setDisplayMode(SSD1357_CMD_SDM_RESET);
	setDisplayMode(SSD1357_CMD_SDM_ALL_ON);
}

void SSD1357::write_ram(uint8_t * pdata, uint8_t startrow, uint8_t startcol, uint8_t stoprow, uint8_t stopcol, uint16_t size)
{
	// Use commands to set the starting/ending locations in GDDRAM 
	setColumnAddress(startrow, stoprow);
	setColumnAddress(startcol, stopcol);

	// Use write_bytes() to send the data along with a data flag
	enableWriteRAM();
	write_bytes(pdata, true, size);
}

void SSD1357::write_bytes(uint8_t * pdata, bool DATAcmd, uint16_t size)
{
	digitalWrite(_cs, LOW);				// Set the chip select line
	digitalWrite(_dc, DATAcmd);		// Set whether transmitting data or command

	// Now transmit the data
	_spi->beginTransaction(SPISettings(SSD1357_SPI_MAX_FREQ, SSD1357_SPI_DATA_ORDER, SSD1357_SPI_MODE));
	_spi->transfer(pdata, size);
	_spi->endTransaction();

	digitalWrite(_cs, HIGH);			// Stop talking to the driver
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
	uint8_t * alphadata = getFontAlpha(val);
	uint8_t * framedata = getFontFrameData(val); // The length of data returned by userBMPFunc must correspond to the returned character width and height * 2
	// btw framedata[] = {starty, startx, yheight, xwidth} - user's responsibility that starty + yheight is less than the actual size of the display... same for x direction
	
	bool print_char = fontCallback(val);		// Use the fontCallback function in a custom font to determine what to do after a character is written
	
	// // Write the font data to the ram now

	if(print_char)
	{
		uint8_t starty = *(framedata + 0);
		uint8_t startx = *(framedata + 1);
		uint8_t yheight = *(framedata + 2);
		uint8_t xwidth = *(framedata + 3);

		// // Alpha data currently not implemented. Doing so might look something like:
		// blendAlphaData(chardata, alphadata);

		// The only limitation on these fonts is that each character must fit in a rectangular frame.
		write_ram(chardata, starty, startx, (starty + yheight), (startx + xwidth), yheight * xwidth);	// xwidth * yheight as returned by the frame data function MUST be the same as the number of pixels, which is the same as half the number of bytes in the chardata array.
	}
	// Otherwise don't print anything
	return 1;
}

uint8_t * SSD1357::getFontBMP(uint8_t val)
{
	// Calls a user-specified function to get the actual pixel data as a one-dimensional array of uint8_ts
	if(_userBMPFuncPtr == NULL)
	{
		SSD1357DefFont5x7.getBMP(val, _width, _height);
		// Use the default
		Serial.println("Null BMP Pointer");// Just hre for debugging, remove serial print later
		uint8_t chardata[] = {0x00};
		return chardata;
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
		return NULL;	// This just means that the default font does not support transparency
	}
	return (*_userAlphaFuncPtr)(_object2operateOn, val, _width, _height);
}

uint8_t * SSD1357::getFontFrameData(uint8_t val)
{
	if(_userFrameFuncPtr == NULL)
	{
		SSD1357DefFont5x7.getFrameData(val, _width, _height);
		// Use default
		Serial.println("Null frame pointer");
		uint8_t framedata[] = {0x00};
		return framedata;
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
		return SSD1357DefFont5x7.advanceState(val, _width, _height);
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

	write_bytes(&buff[0], false, 1);	
	write_bytes(&buff[1], true, 2);
}

void SSD1357::setRowAddress(uint8_t start, uint8_t stop)
{
	uint8_t buff[3];
	buff[0] = SSD1357_CMD_SetRowAddress;
	buff[1] = start;
	buff[2] = stop;

	write_bytes(&buff[0], false, 1);	
	write_bytes(&buff[1], true, 2);
}

void SSD1357::enableWriteRAM( void )
{
	uint8_t buff[1];
	buff[0] = SSD1357_CMD_WriteRAM;
	write_bytes(&buff[0], false, 1);
}

void SSD1357::enableReadRAM( void )
{
	uint8_t buff[1];
	buff[0] = SSD1357_CMD_ReadRAM;
	write_bytes(&buff[0], false, 1);
}

void SSD1357::setRemapColorDepth(bool inc_Vh, bool rev_ColAddr, bool swap_ColOrder, bool rev_SCAN, bool en_SplitOddEven, uint8_t color_depth_code)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetRemapColorDepth;
	buff[1] = 0x00;
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

	write_bytes(&buff[0], false, 1);	
	write_bytes(&buff[1], true, 1);
}

void SSD1357::setDisplayStartLine(uint8_t start_line)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetDisplayStartLine;
	buff[1] = start_line;

	write_bytes(&buff[0], false, 1);	
	write_bytes(&buff[1], true, 1);
}

void SSD1357::setDisplayOffset(uint8_t offset)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetDisplayOffset;
	buff[1] = offset;

	write_bytes(&buff[0], false, 1);	
	write_bytes(&buff[1], true, 1);
}

void SSD1357::setDisplayMode(uint8_t mode_code)
{
	uint8_t buff[1];
	buff[0] = mode_code;
	write_bytes(&buff[0], false, 1);
}

void SSD1357::setSleepMode(bool sleep_on)
{
	uint8_t buff[1];
	if(sleep_on)
	{
		buff[0] = SSD1357_CMD_SetSleepMode_ON;
	}
	else
	{
		buff[0] = SSD1357_CMD_SetSleepMode_OFF;
	}
	write_bytes(&buff[0], false, 1);
}

void SSD1357::setResetPrechargePeriod(uint8_t reset_clocks, uint8_t precharge_clocks)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetResetPrechargePeriod;
	buff[1] = (((precharge_clocks & 0x0F) << 4) | (reset_clocks & 0x0F));
	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);
}

void SSD1357::setClockDivider(uint8_t divider_code)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetClkDiv;
	buff[1] = divider_code;
	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);
}

void SSD1357::setSecondPrechargePeriod(uint8_t precharge_clocks)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetSecondPrechargePeriod;
	buff[1] = precharge_clocks;
	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);
}

/*
void SSD1357::setMLUTGrayscale(uint8_t * pdata63B)
{

}
*/

void SSD1357::useBuiltInLinearLUT( void )
{
	uint8_t buff[1];
	buff[0] = SSD1357_CMD_UseDefMLUT;
	write_bytes(&buff[0], false, 1);
}

void SSD1357::setPrechargeVoltage( uint8_t voltage_scale_code)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetPrechargeVoltage;
	buff[1] = voltage_scale_code;
	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);
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
	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);
}

void SSD1357::setContrastCurrentABC(uint8_t ccA, uint8_t ccB, uint8_t ccC)
{
	uint8_t buff[4];
	buff[0] = SSD1357_CMD_SetContrastCurrentABC;
	buff[1] = ccA;
	buff[2] = ccB;
	buff[3] = ccC;
	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 3);
}
void SSD1357::setMasterContrastCurrent(uint8_t ccCode)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetMasterContrastCurrent;
	buff[1] = ccCode;
	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);
}
void SSD1357::setMUXRatio(uint8_t mux_ratio)
{
	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetMuxRatio;
	buff[1] = mux_ratio;
	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);
}
void SSD1357::setCommandLock(bool locked)
{

	uint8_t buff[2];
	buff[0] = SSD1357_CMD_SetCommandLock;
	if(locked)
	{
		buff[1] = 0x16;
	}
	else
	{
		buff[1] = 0x12;
	}
	write_bytes(&buff[0], false, 1);
	write_bytes(&buff[1], true, 1);
}

uint8_t SSD1357::getWidth( void )
{
	return _width;
}

uint8_t SSD1357::getHeight( void )
{
	return _height;
}

void 	SSD1357::setWidth(uint8_t val)
{
	_width = val;
}

void 	SSD1357::setHeight(uint8_t val)
{
	_height = val;
}

void 	SSD1357::setFont(
					void * object,
					uint8_t * (*BMPFuncPtr)(void * pt2Object, uint8_t, uint8_t, uint8_t), 
					uint8_t * (*AlphaFuncPtr)(void * pt2Object, uint8_t, uint8_t, uint8_t),
					uint8_t * (*frameFuncPtr)(void * pt2Object, uint8_t, uint8_t, uint8_t), 
					bool 	(*fontCallbackPtr)(void * pt2Object, uint8_t, uint8_t, uint8_t),
					void 	(*resetCursorPtr)(void * pt2Object) 
)
{
	_object2operateOn = object;
	_userBMPFuncPtr = BMPFuncPtr;
	_userAlphaFuncPtr = AlphaFuncPtr;
	_userFrameFuncPtr = frameFuncPtr;
	_userFontCallbackPtr = fontCallbackPtr;
	_userFontResetCursorPtr = resetCursorPtr;
}

void SSD1357::resetFontDefault( void )
{
	_object2operateOn = NULL;
	_userBMPFuncPtr = NULL;
	_userAlphaFuncPtr = NULL;
	_userFrameFuncPtr = NULL;
	_userFontCallbackPtr = NULL;
	_userFontResetCursorPtr = NULL;
}

void 	SSD1357::resetFontCursor( void )
{
	(*_userFontResetCursorPtr)(_object2operateOn);
}