/*
  SafeKeyPad.h - Library for Clock module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef Clock_h
#define Clock_h

#include "Arduino.h"  
#include <Digits.h>

class Clock
{
  public:
    Clock(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin);
    void handleI2CMessage(uint8_t dataLength, uint8_t data[]);
    void oneSecondLater();
    void resetClock();
    void startClock();
  private:
  	int i;
    int clockMode, minutes, seconds;
  	Digits* _digits;
  	
  	int _digitArray[4];
  	void _updateClockDisplay();
};

#endif