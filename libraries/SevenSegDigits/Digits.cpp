/*
  Digits.cpp - Library for showing digits on a 7 segmented display using shift resistors.
  Created by Julian Dolce
*/


#include "Digits.h"

Digits::Digits(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin)
{
    _dataArray[0] = 0x3F; //00111111 - 0
    _dataArray[1] = 0x06; //00000110 - 1
    _dataArray[2] = 0x5B; //01011011 - 2
    _dataArray[3] = 0x4F; //01001111 - 3
    _dataArray[4] = 0x66; //01100110 - 4
    _dataArray[5] = 0x6D; //01101101 - 5
    _dataArray[6] = 0x7D; //01111101 - 6
    _dataArray[7] = 0x07; //00000111 - 7
    _dataArray[8] = 0x7F; //01111111 - 8
    _dataArray[9] = 0x67; //01100111 - 9
	_dataPin = dataPin;
	_clockPin = clockPin;
	_latchPin = latchPin;
	pinMode(_latchPin, OUTPUT);
}

void Digits::show( int digits[], int len )
{
	digitalWrite(_latchPin, 0);
 
	for( int i=len-1; i>=0; i--)
	{
		shiftDigit(_dataArray[digits[i]]);
	}
	
	digitalWrite(_latchPin, 1);
}

void Digits::shiftDigit(byte myDataOut)
{
  int i=0;
  int pinState;
  pinMode(_clockPin, OUTPUT);
  pinMode(_dataPin, OUTPUT);

  digitalWrite(_dataPin, 0);
  digitalWrite(_clockPin, 0);

  for (i=7; i>=0; i--)  
  {
    digitalWrite(_clockPin, 0);
    if( myDataOut & (1<<i) ) 
    {
	pinState= 1;
    }
    else 
    {
	pinState= 0;
    }

    digitalWrite(_dataPin, pinState);
    digitalWrite(_clockPin, 1);
    digitalWrite(_dataPin, 0);
  }
  digitalWrite(_clockPin, 0);
}