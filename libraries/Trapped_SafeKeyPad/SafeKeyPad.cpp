/*
  SafeKeyPad.cpp - Library for InWallSafe KeyPad module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/

#include "Arduino.h"
#include <SafeKeyPad.h>
#include <Constants.h>
#include <Keypad.h>


SafeKeyPad::SafeKeyPad(){
	 
	const byte ROWS = 4; //four rows
 	const byte COLS = 3; //three columns

	char keys[ROWS][COLS] = {
	    {'1','2','3'},
	    {'4','5','6'},
	    {'7','8','9'},
	    {'*','0','#'}
	};
	byte rowPins[ROWS] = {5, 6, 7, 8}; //connect to the row pinouts of the keypad
	byte colPins[COLS] = {2, 3, 4};
	_keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
	
}
