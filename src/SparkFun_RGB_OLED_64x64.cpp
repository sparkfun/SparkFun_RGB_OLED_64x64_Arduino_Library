#include "SparkFun_RGB_OLED_64x64.h"

// Choosing an intermediate option between a full screen buffer (64*64*2)
// and having no working memory at all (which would require lots of
// separate SPI transactions to fill a screen)
uint8_t working_buff[OLED_WORING_BUFF_NUM_PIXELS*OLED_64x64_MAX_BYTES_PER_PIXEL];		


RGB_OLED_64x64::RGB_OLED_64x64()
{

}

void RGB_OLED_64x64::begin(uint8_t dcPin, uint8_t rstPin, uint8_t csPin, SPIClass &spiInterface = SPI, uint32_t spiFreq = SSD1357_SPI_MAX_FREQ)
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
	uint8_t temp_buff;
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

  	setSleepMode(false);

  	delay(200);

  	enableWriteRAM();
}

void RGB_OLED_64x64::fill_working_buffer(uint16_t value, uint8_t num_pixels)
{
	if(num_pixels > OLED_64x64_WIDTH)
	{
		num_pixels = OLED_64x64_WIDTH;
	}

	for(uint8_t indi = 0; indi < num_pixels; indi++)
	{
		working_buff[2*indi + 0] = ((value & 0xFF00) >> 8);
		working_buff[2*indi + 1] = ((value & 0x00FF) >> 0);
	}
}

void RGB_OLED_64x64::clearDisplay(uint8_t mode = 0x00)
{
	fillDisplay(0x0000);
}

void RGB_OLED_64x64::fillDisplay(uint16_t value)
{
	// Serial.println(OLED_64x64_START_ROW, HEX);
	// Use the working buffer, which is as wide as the screen, to scan down the height and set all to zero
	for(uint8_t indi = 0; indi < OLED_64x64_HEIGHT; indi++)
	{
		// Note: if SPI.transfer() did not replace the contents of the buffer then it would not be necessary to re-initialize the buffer every time. Could be decent speed improvements
		fill_working_buffer(value, OLED_64x64_WIDTH);
	    write_ram(working_buff, OLED_64x64_START_ROW+indi, OLED_64x64_START_COL, OLED_64x64_STOP_ROW, OLED_64x64_STOP_COL, OLED_64x64_WIDTH*OLED_64x64_MAX_BYTES_PER_PIXEL);
	}
}

void RGB_OLED_64x64::display(void)						// Because there is no local data (always writing to display ram) there is no need for this function, but it is kept for compatibility
{
	return;
}

// void setCursor(uint8_t x, uint8_t y); 	// Setting the cursor will be handled by the custom font driver



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


void RGB_OLED_64x64::setPixel(uint8_t x, uint8_t y)
{
	setPixel(x, y, _fillColor);
}

void RGB_OLED_64x64::setPixel(uint8_t x, uint8_t y, uint16_t value)
{
	if((x >= OLED_64x64_WIDTH) || (x >= OLED_64x64_HEIGHT))	// Make sure we are within the display
	{
		return;
	}

	working_buff[0] = ((value & 0xFF00) >> 8);
	working_buff[1] = ((value & 0x00FF) >> 0);
	write_ram(working_buff, OLED_64x64_START_ROW+y, OLED_64x64_START_COL+x, OLED_64x64_STOP_ROW, OLED_64x64_STOP_COL, 2);
}


void RGB_OLED_64x64::plotLineLow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value, uint8_t width)
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
  		setPixel(x, y, value);
  	}
  	else
  	{
  		circleFill(x, y, width/2, value);
  	}
    if( D > 0 )
    {
       y = y + yi;
       D = D - 2*dx;
    }
    D = D + 2*dy;
  }
}
void RGB_OLED_64x64::plotLineHigh(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value, uint8_t width)
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
  		setPixel(x, y, value);
  	}
  	else
  	{
  		circleFill(x, y, width/2, value);
  	}
    if( D > 0 )
    {
       x = x + xi;
       D = D - 2*dy;
    }
    D = D + 2*dx;
  }
}

void RGB_OLED_64x64::line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	line(x0, y0, x1, y1, _fillColor);
}
void RGB_OLED_64x64::line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value)
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

void RGB_OLED_64x64::lineWide(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t width)
{
	lineWide(x0, y0, x1, y1, _fillColor, width);
}
void RGB_OLED_64x64::lineWide(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t value, uint8_t width)
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

void RGB_OLED_64x64::lineH(uint8_t x, uint8_t y, uint8_t width)
{
	lineH(x, y, width, _fillColor);
}
void RGB_OLED_64x64::lineH(uint8_t x, uint8_t y, uint8_t width, uint16_t value)
{
	fast_filled_rectangle(x, y, x+width, y, value);
}

void RGB_OLED_64x64::lineV(uint8_t x, uint8_t y, uint8_t height)
{
	lineV(x, y, height, _fillColor);
}
void RGB_OLED_64x64::lineV(uint8_t x, uint8_t y, uint8_t height, uint16_t value)
{
	fast_filled_rectangle(x, y, x, y+height, value);
}


void RGB_OLED_64x64::rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	rect(x, y, width, height, _fillColor);
}
void RGB_OLED_64x64::rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value)
{
	fast_filled_rectangle(x, y, x, y+height, value);
	fast_filled_rectangle(x+width, y, x+width, y+height, value);
	fast_filled_rectangle(x, y, x+width, y, value);
	fast_filled_rectangle(x, y+height, x+width, y+height, value);
}
void RGB_OLED_64x64::rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
	rectFill(x, y, width, height, _fillColor);
}
void RGB_OLED_64x64::rectFill(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value)
{
	fast_filled_rectangle(x, y, x+width, y+height, value);
}

void RGB_OLED_64x64::fast_filled_rectangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1, int16_t value)
{
	// This uses the boundaries on write_ram to quickly fill a given rectangle
	boolean x0offscreen = false;
	boolean x1offscreen = false;
	boolean y0offscreen = false;
	boolean y1offscreen = false;

	// Ensure bounds are good
	if(x0 >= OLED_64x64_WIDTH)
	{
		x0 = OLED_64x64_WIDTH-1;
		x0offscreen = true;
	}
	if(x1 >= OLED_64x64_WIDTH)
	{
		x1 = OLED_64x64_WIDTH-1;
		x1offscreen = true;
	}
	if(y0 >= OLED_64x64_HEIGHT)
	{
		y0 = OLED_64x64_HEIGHT-1;
		y0offscreen = true;
	}
	if(y1 >= OLED_64x64_HEIGHT)
	{
		y1 = OLED_64x64_HEIGHT-1;
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

	uint8_t rows_per_block = OLED_WORING_BUFF_NUM_PIXELS / width;
	uint8_t num_full_blocks = height/rows_per_block;
	uint8_t remaining_rows = height - (num_full_blocks * rows_per_block);

	uint8_t offsety = 0;

	for(uint8_t indi = 0; indi < num_full_blocks; indi++)
	{
		fill_working_buffer(value, rows_per_block*width);
		write_ram(working_buff, OLED_64x64_START_ROW+y0+offsety, OLED_64x64_START_COL+x0, OLED_64x64_START_ROW+y1, OLED_64x64_START_COL+x1, 2*rows_per_block*width);
		offsety += rows_per_block;
	}
	fill_working_buffer(value, remaining_rows*width);
	write_ram(working_buff, OLED_64x64_START_ROW+y0+offsety, OLED_64x64_START_COL+x0, OLED_64x64_START_ROW+y1, OLED_64x64_START_COL+x1, 2*remaining_rows*width);
}

    // void RGB_OLED_64x64::circle(uint8_t x, uint8_t y, uint8_t radius);
void RGB_OLED_64x64::circle(uint8_t x, uint8_t y, uint8_t radius, uint16_t value)
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
void RGB_OLED_64x64::circleFill(uint8_t x, uint8_t y, uint8_t radius)
{
	circleFill(x, y, radius, _fillColor);
}

void RGB_OLED_64x64::circleFill(uint8_t x, uint8_t y, uint8_t radius, uint16_t value)
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
void RGB_OLED_64x64::circle_eight(uint8_t xc, uint8_t yc, int16_t dx, int16_t dy, uint16_t value, boolean fill)
{
	setPixel(xc+dx, yc+dy, value);
	setPixel(xc-dx, yc+dy, value);
	setPixel(xc+dx, yc-dy, value);
	setPixel(xc-dx, yc-dy, value);
	setPixel(xc+dy, yc+dx, value);
	setPixel(xc-dy, yc+dx, value);
	setPixel(xc+dy, yc-dx, value);
	setPixel(xc-dy, yc-dx, value);

	if(fill)
	{
		fast_filled_rectangle(xc-dx, yc+dy, xc+dx, yc+dy, value);
    	fast_filled_rectangle(xc-dx, yc-dx, xc+dx, yc-dx, value);
    	fast_filled_rectangle(xc-dy, yc+dx, xc+dy, yc+dx, value);
    	fast_filled_rectangle(xc-dy, yc-dx, xc+dy, yc-dx, value);
	}
}

void RGB_OLED_64x64::circle_Bresenham(uint8_t x, uint8_t y, uint8_t radius, uint16_t value, boolean fill)
{
	// Thanks to the tutorial here: https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/
	uint8_t dx = 0;
	uint8_t dy = radius;
	uint8_t D = 3 - 2*radius;

	if(fill)
	{
		setPixel(x, y, value);
		if(radius == 0)
		{
			return;
		}
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

void RGB_OLED_64x64::circle_midpoint(uint8_t xc, uint8_t yc, uint8_t radius, uint16_t value, boolean fill)
{		
	// Thanks to the tutorial here: https://www.geeksforgeeks.org/mid-point-circle-drawing-algorithm/
    uint8_t dx = radius;
    uint8_t dy = 0;

    // Set first or center pixel
    setPixel(xc+dx, yc+dy, value);
    if (radius > 0)
    {
       	setPixel(xc-dx, yc+dy, value);
        setPixel(xc+dy, yc-dx, value);
        setPixel(xc+dy, yc+dx, value);

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

        setPixel(xc+dx, yc+dy, value);
        setPixel(xc-dx, yc+dy, value);
        setPixel(xc+dx, yc-dy, value);
        setPixel(xc-dx, yc-dy, value);

        if(fill)
        {
        	fast_filled_rectangle(xc-dx, yc+dy, xc+dx, yc+dy, value);
        	fast_filled_rectangle(xc-dx, yc-dy, xc+dx, yc-dy, value);
        }
         
        // If the generated point is on the line x = y then 
        // the perimeter points have already been printed
        if (dx != dy)
        {
            setPixel(xc+dy, yc+dx, value);
        	setPixel(xc-dy, yc+dx, value);
        	setPixel(xc+dy, yc-dx, value);
        	setPixel(xc-dy, yc-dx, value);

        	if(fill)
	        {
	        	fast_filled_rectangle(xc-dy, yc+dx, xc+dy, yc+dx, value);
	        	fast_filled_rectangle(xc-dy, yc-dx, xc+dy, yc-dx, value);
	        }
        }
    } 
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