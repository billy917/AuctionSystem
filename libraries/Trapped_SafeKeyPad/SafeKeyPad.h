/*
  SafeKeyPad.h - Library for InWall safe keypad module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef SafeKeyPad_h
#define SafeKeyPad_h

#include "Arduino.h"  
#include <Keypad.h>  

class SafeKeyPad
{
  public:
    SafeKeyPad();

  private:
    Keypad _keypad;
};

#endif