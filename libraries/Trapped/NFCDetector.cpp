/*
  NFCDetector.cpp - Library for NFC detector module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_PN532.h>
#include "NFCDetector.h"

#define SCK  (8)
#define MOSI (11)
#define SS   (10)
#define MISO (9)

Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);

NFCDetector::NFCDetector(int detectorId, int i2cAddressId){
	_detectorId = detectorId;
	_uidLength = 7;
	_uid[] = { 0, 0, 0, 0, 0, 0, 0 };
	_lastUid[] = { 0, 0, 0, 0, 0, 0, 0 };

	Serial.begin(9600);

	//init NFC board
	nfc.begin();
	_nfcFirmwareVersion = nfc.getFirmwareVersion();
	nfc.SAMConfig();

	//init I2C channel
	Wire.begin(i2cAddressId);

	_isRegistered = false;

}

bool NFCDetector::isRegistered(){
	return _isRegistered;
}

bool NFCDetector::isNFCCardDetected(){
	return !_nfcFirmwareVersion?false:true;
}

void NFCDetector::printFirmwareInfo(){
	Serial.print("Found chip PN5"); Serial.println((_nfcFirmwareVersion>>24) & 0xFF, HEX); 
 	Serial.print("Firmware ver. "); Serial.print((_nfcFirmwareVersion>>16) & 0xFF, DEC); 
  	Serial.print('.'); Serial.println((_nfcFirmwareVersion>>8) & 0xFF, DEC);
}

void NFCDetector::registerWithNFCManager(){
	//Use I2C request/response to register this Detector with Manager
}

void NFCDetector::detectNFCChanges(){
	//Check if any NFC tags are detected
	if(nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, _uid, &_uidLength)){

	}
}