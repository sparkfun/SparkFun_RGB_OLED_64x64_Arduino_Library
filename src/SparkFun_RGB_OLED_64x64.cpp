#include "SparkFun_RGB_OLED_64x64.h"


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
	
}

void RGB_OLED_64x64::clear( void )
{
	// Used to blank the screen!
	// oled_driver.
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
  	setRemapColorDepth(false, true, true, true, true, 0b01);   // 65k color mode
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
  	setColumnAddress(0x20, 0x5F);
  	setRowAddress(0x00, 0x3F);
  	setDisplayMode(SSD1357_CMD_SDM_RESET);

  	setSleepMode(false);

  	delay(200);

  	setCSlow();
  	enableWriteRAM();
  	setCShigh();
}