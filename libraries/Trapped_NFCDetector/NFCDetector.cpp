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

bool NFCDetector::_detectedExpectedChip(uint8_t firstByte, uint8_t lastByte){
	if(lastByte == 0 && firstByte != 0){
		return true;
	}
	return false;
}

bool NFCDetector::_detectedToolChip(uint8_t firstByte, uint8_t lastByte){
	if(lastByte == 255){
		return true;
	}
  	return false;
}

void NFCDetector::registerWithNFCManager(){
	//TODO: Use I2C request/response to register this Detector with Manager
	_isRegistered = true;
}

void NFCDetector::detectNFCChanges(){
	//Check if any NFC tags are detected
	uint8_t detectedUIDLength = 0;
	if(nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, _uid, &detectedUIDLength)){
			    
	    if (nfc.mifareultralight_ReadPage (4, _data)){
			if(!_lastDetectedChip){ // previously did not detect chip
				if(_detectedExpectedChip(_data[0],_data[31])){
					_notifyFoundNFCChip(_data[0]);
					_lastDetectedChip = true;
				} else if (_detectedToolChip(_data[0],_data[31])){
					_notifyFoundToolChip();
					_lastDetectedChip = true;
				} else {
					// detected a chip that is not expected & not tool - do nothing
				}
			} else { // previously detected a chip
				if(_isSameAsLastDetectedChip()){
					Serial.println(F("Is same as last chip"));
				} else if(_detectedExpectedChip(_data[0],_data[31])){
					_notifyFoundNFCChip(_data[0]);
					_lastDetectedChip = true;
				} else if (_detectedToolChip(_data[0],_data[31])){
					_notifyFoundToolChip();
					_lastDetectedChip = true;
				}
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

void NFCDetector::_notifyFoundNFCChip(uint8_t firstByte){
	Serial.println(F("NFC Chip Found"));
	Wire.beginTransmission(NFC_MANAGER_I2C_ADDR); // transmit to device #100 (NFCDetectorManager
	Wire.write(MESSAGETYPEID_NFC);
	Wire.write(_detectorId); // this NFC detector Id
	Wire.write(MESSAGETYPEID_NFC_FOUNDEXPECTED);   
	Wire.write(firstByte); 
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
	return _lastDetectedChip;
}
