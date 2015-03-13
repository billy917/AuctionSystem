/*
  NFCDetector.cpp - Library for NFC detector module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_PN532.h"
#include "NFCDetector.h"

#define PN532_SCK  (8)
#define PN532_MOSI (11)
#define PN532_SS   (10)
#define PN532_MISO (9)

#define PN532_IRQ   (2)
#define PN532_RESET (3) 

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

NFCDetector::NFCDetector(int detectorId, int i2cAddressId){
	_detectorId = detectorId;
	//TODO: do i need these?
	//_uid = { 0, 0, 0, 0, 0, 0, 0 };
	//_lastUid = { 0, 0, 0, 0, 0, 0, 0 };

	//init I2C channel
	Wire.begin(i2cAddressId);

	_isRegistered = false;
	_lastDetectedChip = false;

	_expectedUid[0] = 0x04;
	_expectedUid[1] = 0xEC;
	_expectedUid[2] = 0x82;
	_expectedUid[3] = 0xEA;
	_expectedUid[4] = 0xC2;
	_expectedUid[5] = 0x23;
	_expectedUid[6] = 0x80;

	_expectedToolUid[0] = 0x04;
	_expectedToolUid[1] = 0x8B;
	_expectedToolUid[2] = 0x82;
	_expectedToolUid[3] = 0xEA;
	_expectedToolUid[4] = 0xC2;
	_expectedToolUid[5] = 0x23;
	_expectedToolUid[6] = 0x80;
	
}

void NFCDetector::initNFCCard(){
	nfc.begin();
	_nfcFirmwareVersion = nfc.getFirmwareVersion();
	nfc.SAMConfig();
}

bool NFCDetector::isRegistered(){
	return _isRegistered;
}

bool NFCDetector::isNFCCardDetected(){
	return !_nfcFirmwareVersion?false:true;
}

void NFCDetector::printFirmwareInfo(){
	Serial.print(F("Found chip PN5")); Serial.println((_nfcFirmwareVersion>>24) & 0xFF, HEX); 
 	Serial.print(F("Firmware ver. ")); Serial.print((_nfcFirmwareVersion>>16) & 0xFF, DEC); 
  	Serial.print('.'); Serial.println((_nfcFirmwareVersion>>8) & 0xFF, DEC);
}

bool NFCDetector::_detectedExpectedChip(){
	for(int i=0; i< UID_LENGTH; i++){
	    if(_uid[i] != _expectedUid[i]){
	      return false;
	    }  
	}
  	return true;
}

bool NFCDetector::_detectedToolChip(){
	for(int i=0; i< UID_LENGTH; i++){
	    if(_uid[i] != _expectedToolUid[i]){
	      return false;
	    }  
	}
  	return true;
}

void NFCDetector::registerWithNFCManager(){
	//TODO: Use I2C request/response to register this Detector with Manager
	_isRegistered = true;
}

void NFCDetector::detectNFCChanges(){
	//Check if any NFC tags are detected
	uint8_t detectedUIDLength = 0;
	if(nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, _uid, &detectedUIDLength)){
		if(!_lastDetectedChip){ // previously did not detect chip
			if(_detectedExpectedChip()){
				_notifyFoundNFCChip();
				_lastDetectedChip = true;
			} else if (_detectedToolChip()){
				_notifyFoundToolChip();
				_lastDetectedChip = true;
			} else {
				// detected a chip that is not expected & not tool - do nothing
			}
		} else { // previously detected a chip
			if(_isSameAsLastDetectedChip()){
				Serial.println(F("Is same as last chip"));
			} else if(_detectedExpectedChip()){
				_notifyFoundNFCChip();
				_lastDetectedChip = true;
			} else if (_detectedToolChip()){
				_notifyFoundToolChip();
				_lastDetectedChip = true;
			}
		}
		_updateLastUID();
	} else {
		if(_lastDetectedChip){
			_lastDetectedChip = false; // set flag before notifying
			_clearLastUID();
			_notifyCannotFindNFCChip();
		} else {
			_lastDetectedChip = false;
		}
	}
}

void NFCDetector::_updateLastUID(){
	memcpy( _lastUid, _uid, UID_LENGTH);
}

void NFCDetector::_clearLastUID(){
	for(int i=0; i<UID_LENGTH; i++){
 		_lastUid[i] = 0;
 	}
}

bool NFCDetector::_isSameAsLastDetectedChip(){
  	for(int i=0; i< UID_LENGTH; i++){
	    if(_uid[i] != _lastUid[i]){
	      return false;
	    }  
	  }
  	return true;
}

void NFCDetector::_notifyFoundToolChip(){
	Serial.println(F("Tool Found"));
	Wire.beginTransmission(NFC_MANAGER_I2C_ADDR); // transmit to device #100 (NFCDetectorManager
	Wire.write(MESSAGETYPEID_NFC);
	Wire.write(_detectorId); // this NFC detector Id
	Wire.write(MESSAGETYPEID_NFC_FOUNDTOOL);    
		// detected new expected NFC (1) 
		// cannot detected NFC (2)
		// detected tool NFC (3) 

	Wire.endTransmission();
	Serial.println(F("Done writing Found"));
}

void NFCDetector::_notifyFoundNFCChip(){
	Serial.println(F("NFC Chip Found"));
	Wire.beginTransmission(NFC_MANAGER_I2C_ADDR); // transmit to device #100 (NFCDetectorManager
	Wire.write(MESSAGETYPEID_NFC);
	Wire.write(_detectorId); // this NFC detector Id
	Wire.write(MESSAGETYPEID_NFC_FOUNDEXPECTED);    
		// detected new expected NFC (1) 
		// cannot detected NFC (2)
		// detected tool NFC (3) 

	Wire.endTransmission();
	Serial.println(F("Done writing Found"));
}

void NFCDetector::_notifyCannotFindNFCChip(){
	Serial.println(F("NFC Not Found"));
	Wire.beginTransmission(NFC_MANAGER_I2C_ADDR); // transmit to device #100 (NFCDetectorManager
	Wire.write(MESSAGETYPEID_NFC);
	Wire.write(_detectorId); // this NFC detector Id
	Wire.write(MESSAGETYPEID_NFC_NOTFOUND);    
		// detected new expected NFC (1) 
		// cannot detected NFC (2)
		// detected tool NFC (3) 

	Wire.endTransmission();
	Serial.println(F("Done writing Found"));
}

bool NFCDetector::detectedNFC(){
	return _detectedExpectedChip() || _detectedToolChip();
}