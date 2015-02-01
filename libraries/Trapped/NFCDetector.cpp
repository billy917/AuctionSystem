/*
  NFCDetector.cpp - Library for NFC detector module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#include "Arduino.h"
#include <Wire.h>
#include "NFCDetector.h"

NFCDetector::NFCDetector(int detectorId, int i2cAddressId){
	_detectorId = detectorId;
	Wire.begin(i2cAddressId);

	//TODO: register with NFCManager

	_isRegistered = false;
}

boolean NFCDetector::isRegistered(){
	return _isRegistered;
}

void NFCDetector::registerWithNFCManager(){
	//Use I2C request/response to register this Detector with Manager
}
