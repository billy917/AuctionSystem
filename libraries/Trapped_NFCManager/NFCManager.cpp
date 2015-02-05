/*
  NFCManager.cpp - Library for NFC Manager module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/

#include "Arduino.h"
#include <Wire.h>
#include <NFCManager.h>
#include <Constants.h>
#include <XBee.h>

NFCManager::NFCManager(int managerId, bool localLockManager){
	_managerId = managerId;
	_localLockManager = localLockManager;
	_numRegisteredDetector = 3;
	_registeredDetectorIds[0] = 3;
	_registeredDetectorIds[1] = 4;
	_registeredDetectorIds[2] = 5;
	_registeredDetectorStates[0] = false;
	_registeredDetectorStates[1] = false;
	_registeredDetectorStates[2] = false;
	_xBeePayload[0]=0;
	_xBeePayload[1]=0;
	_xBeePayload[2]=0;

	_laser1Addr = XBeeAddress64(0x0013a200, 0x40c0edf); //Laser1 xBee 
	_laser1ZBTxRequest = ZBTxRequest(_laser1Addr, _xBeePayload, sizeof(_xBeePayload));

	_toolAddr = XBeeAddress64(0x0013a200, 0x40b9df66); //Tool xBee 
	_toolZBTxRequest = ZBTxRequest(_toolAddr, _xBeePayload, sizeof(_xBeePayload));
}

void NFCManager::registerWithShelfLockManager(){

}

void NFCManager::setLockManagerI2CAdder(int i2cAddr){
	_localLockManagerAddr = i2cAddr;
}

void NFCManager::handleI2CMessage(uint8_t dataLength, volatile uint8_t data[]){
	if(3 == dataLength){
		if(data[0] == MESSAGETYPEID_NFC){
			if(data[2] == MESSAGETYPEID_NFC_REGISTER){
				// got a message from detector requesting to register itself
				if(_numRegisteredDetector < 3){
					_registerDetector(data[1]);					
				}
			} else {
				bool previousState = _areAllDetectorDetected();
				// got a message from detector, relay it to the lock manager via Laser1			
				_updateDetectorStates(data[1], (bool) data[2]);
				if(_areAllDetectorDetected()){
					_notifyLockManager(true);
				} else if (previousState){
					_notifyLockManager(false);
				}
			}
		}
	}
}

void NFCManager::_notifyLockManager(bool allDetectedExpectedNFC){
	if(_localLockManager){
		Wire.beginTransmission(_localLockManagerAddr); // transmit to device #100 (NFCDetectorManager
		Wire.write(MESSAGETYPEID_NFC_MANAGE);
		Wire.write(_managerId); // this NFC detector Id
		Wire.write(allDetectedExpectedNFC);    
			// all detector detected expected NFC chips (1) 
			// one detector cannot detect expected NFC (2)			
		Wire.endTransmission();
	} else {
		_xBeePayload[0] = MESSAGETYPEID_NFC_MANAGE;
		_xBeePayload[1] = _managerId;
		_xBeePayload[2] = allDetectedExpectedNFC ? MESSAGETYPEID_NFC_MANAGE : MESSAGETYPEID_NFC_MANAGE_NOTFOUND;
		_xbee_pointer->send(_laser1ZBTxRequest);
		_xbee_pointer->send(_toolZBTxRequest);
	}

}

void NFCManager::_registerDetector(uint8_t detectorId){
	_registeredDetectorIds[_numRegisteredDetector] = detectorId;
	_numRegisteredDetector++;
	//TODO: reply to detector on expected UID
}

void NFCManager::setXBeeReference(XBee* xbee_pointer){
	_xbee_pointer = xbee_pointer;
}

void NFCManager::_updateDetectorStates(uint8_t detectorId, bool detectedExpectedNFC){
	int i=0;
	for(int i=0; i<_numRegisteredDetector; i++){
		if(detectorId == _registeredDetectorIds[i]){
			break;
		}
	}
	_registeredDetectorStates[i] = detectedExpectedNFC;
}

bool NFCManager::_areAllDetectorDetected(){
	for(int i=0; i<_numRegisteredDetector; i++){
		if(!_registeredDetectorStates[i]){
			return false;
		}
	}
	return true;
}