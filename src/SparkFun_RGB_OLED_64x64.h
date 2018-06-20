/* 

A libary to use the SSD1357 driver in conjuction with a particular OLED display that is 64x64

*/

#ifndef SF_RGB_OLED_64X64_H
#define	SF_RGB_OLED_64X64_H

// #include "screen65k.h"				// This is a method of storing arbitrary RGB images in 16-bit depth where two colors are 5 bits and the last is 6 bits
#include "util/SparkFun_SSD1357_OLED.h"	// This is a driver that takes screens and displays them on a physical device

#define OLED_64x64_WIDTH 	64
#define OLED_64x64_HEIGHT 	64


class RGB_OLED_64x64{
private:
protected:
public:

	// SSD1357 * driverPtr;
	SSD1357 oled_driver;

	RGB_OLED_64x64();

	void begin(uint8_t dcPin, uint8_t rstPin, uint8_t csPin, SPIClass &spiInterface = SPI);
};








#endif /* SF_RGB_OLED_64X64_H */