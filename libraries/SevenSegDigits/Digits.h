/*
  Digits.h - Library for showing digits on a 7 segmented display using shift resistors.
  Created by Julian Dolce
*/

#ifndef DIGITS_H
#define DIGITS_H

#include "Arduino.h"

class Digits
{

private:
	uint8_t _dataPin;
	uint8_t _clockPin;
	uint8_t _latchPin;
	byte _dataArray[10];
	void shiftDigit(byte myDataOut);

public:
	Digits(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin);
	void show( int digits[], int len );
};
#endif