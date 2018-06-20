#include "SparkFun_RGB_OLED_64x64.h"


RGB_OLED_64x64::RGB_OLED_64x64()
{

}

void RGB_OLED_64x64::begin(uint8_t dcPin, uint8_t rstPin, uint8_t csPin, SPIClass &spiInterface = SPI)
{
	oled_driver.begin(dcPin, rstPin, csPin, spiInterface);
}