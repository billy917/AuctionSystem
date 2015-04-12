/*
  ClockConroller.cpp - Library for ClockConroller module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#include "Arduino.h"
#include <Digits.h>
#include <Clock.h>
#include <Constants.h>

Clock::Clock(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin){
  _digitArray[0] = _digitArray[1] = _digitArray[2] = _digitArray[3] = 0;
  _digits = new Digits(dataPin, clockPin, latchPin);
  resetClock();

  /* Play Warning sound variables */
  playWarningFlag = false;
  _playWarningCounter = 30;
}

void Clock::handleI2CMessage(uint8_t dataLength, uint8_t data[]){
  if(dataLength > 1 && MESSAGETYPEID_CLOCK == data[0]){
    if(MESSAGETYPEID_CLOCK_RESET == data[1]){
      resetClock();
    } else if (MESSAGETYPEID_CLOCK_START == data[1]){
      clockMode = CLOCK_MODE_COUNTDOWN;
    } else if (MESSAGETYPEID_CLOCK_PAUSE == data[1]){
      clockMode = CLOCK_MODE_PAUSE;
    } else if(MESSAGETYPEID_CLOCK_MODIFY_ADD == data[1]){
      minutes += data[2];
      if(seconds > 60){
        minutes = minutes + 1;
        seconds = seconds - 60;  
      }
    } else if (MESSAGETYPEID_CLOCK_MODIFY_SUBTRACT == data[1]){
      if(minutes > 0){
        minutes -= data[2];
      } else {
        seconds = 0;
      }      
    }
  }
}

void Clock::oneSecondLater(){
  if(CLOCK_MODE_COUNTDOWN == clockMode){
    seconds -= 1;
    if(-1 == seconds){
      minutes -= 1;
      if(-1 == minutes){
        minutes = 0;
        seconds = 0;
      } else {
        seconds = 59;
      }
    }

    if(CLOCK_MODE_COUNTDOWN == clockMode){
      _updateClockDisplay();
    }
    if(minutes <= 0  && seconds <= 0){
      clockMode = CLOCK_MODE_PAUSE;

      // set flag to play for 30sec
      playWarningFlag = true;
    }
    
    if ((clockMode == CLOCK_MODE_PAUSE) && (playWarningFlag)){
        if (_playWarningCounter >= 30){
            Wire.beginTransmission (SOUNDFX_I2C_ADDR);
            Wire.write (MESSAGETYPEID_CLOCK);
            Wire.write (MESSAGETYPEID_CLOCK_PLAY_LAST_TRACK);
            Wire.endTransmission();
        }
        
        if (_playWarningCounter == 0){
            Wire.beginTransmission (SOUNDFX_I2C_ADDR);
            Wire.write (MESSAGETYPEID_CLOCK);
            Wire.write (MESSAGETYPEID_CLOCK_STOP_LAST_TRACK);
            Wire.endTransmission();

            playWarningFlag = false;
        }

        _playWarningCounter -= 0;
    }
  }
}

void Clock::resetClock(){
  minutes = 45;
  seconds = 0;
  clockMode = CLOCK_MODE_PAUSE;
  _updateClockDisplay();
}

void Clock::startClock(){
  clockMode = CLOCK_MODE_COUNTDOWN;
}

void Clock::_updateClockDisplay(){
  int digit4 = seconds % 10;
  int digit3 = ( (seconds%100) - digit4 ) / 10;

  int digit2 = minutes % 10;
  int digit1 = ( (minutes%100) - digit2 ) / 10;
  
  //Digits are added to the array in the same order as in the circuit
  _digitArray[0] = digit1;
  _digitArray[1] = digit2;
  _digitArray[2] = digit3;
  _digitArray[3] = digit4;

  _digits->show( _digitArray, 4 );
}
