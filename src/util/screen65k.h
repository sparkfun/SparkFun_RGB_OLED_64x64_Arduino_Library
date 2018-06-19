/*

A library to control a 65k color-depth screen of arbitrary dimensions

*/

#ifndef SCREEEN65K_H
#define	SCREEEN65K_H

#include <Arduino.h>

class screen65k : public Print
{
private:
	uint16_t * _pmem;				//

protected:
	uint16_t _width, _height;

public:
	screen65k( void );			// Constructor

};

#endif /* SCREEEN65K_H */