


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

NFCManager::NFCManager(int managerId, bool isPrimary){
	_managerId = managerId;
	_isPrimary = isPrimary;
	_shelfIsLocked = true;
	_numRegisteredDetector = 3;
	_registeredDetectorIds[0] = 3;
	_registeredDetectorIds[1] = 4;
	_registeredDetectorIds[2] = 5;
	_registeredDetectorStates[0] = false;
	_registeredDetectorStates[1] = false;
	_registeredDetectorStates[2] = false;
	_managerStates[0] = false;
	_managerStates[1] = false;
	_managerStates[2] = false;
	_managerStates[3] = false;
	_managerStates[4] = false;
	_xBeePayload[0]=0;
	_xBeePayload[1]=0;
	_xBeePayload[2]=0;

	_inDebugMode = true;

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

void NFCManager::handleI2CMessage(uint8_t dataLength, uint8_t data[]){	
	if(MESSAGETYPEID_NFC == data[0]){ // Detector <-> Manager
		if(data[2] == MESSAGETYPEID_NFC_REGISTER){
			// got a message from detector requesting to register itself
			if(_numRegisteredDetector < 3){
				_registerDetector(data[1]);					
			}
		} else  if (data[2] == MESSAGETYPEID_NFC_MANAGE_FOUND || data[2] == MESSAGETYPEID_NFC_MANAGE_NOTFOUND){
			// got a message from detector, relay it to the lock manager via Laser1	
			bool detected = true;
			if(data[2] == MESSAGETYPEID_NFC_MANAGE_NOTFOUND){
				detected = false;
			}		
			_updateDetectorStates(data[1], detected);
			if(_inDebugMode){
				// notify PrimaryManager right away
				if(!_isPrimary){
					_notifyPrimaryManager(data[1], detected);
				} else {
					// any local detector changes should check for potential lock updates
					_managerStates[data[1]] = detected;
					_checkAndUpdateLock();
				}
			} else {
				// update local states and only update PrimaryManager when all registered detectors are detecting expected NFCs
				if (_areAllDetectorDetected()){
					_notifyPrimaryManager(0, true);
				}
			}	
		}
	} else if (MESSAGETYPEID_NFC_MANAGE == data[0]){ // Manager <-> Primary Manager
		if(MESSAGETYPEID_NFC_MANAGE_FOUND == data[2]){ // Manager telling me (Primary) that their detector detected NFC
			_managerStates[data[3]] = true;
			_checkAndUpdateLock();
		} else if (MESSAGETYPEID_NFC_MANAGE_NOTFOUND == data[2]){ // Manager tell me (Primary) that thier detector no longer detect NFC
			_managerStates[data[3]] = false;
			_checkAndUpdateLock();
		} 
	} else if (MESSAGETYPEID_NFC_TOOL == data[0]){ // Primary Manager <-> Tool
		if(MESSAGETYPEID_NFC_TOOL_REQUEST == data[1]){ // tool requesting for overall status
			// send message to tool with overal status
			_notifyToolOverallStatus();
		}
	}
}

void NFCManager::_checkAndUpdateLock(){
	bool shouldBeLocked = false;
	for(int i=0; i<5; i++){
		if(!_managerStates[i]){
			shouldBeLocked = true;
			break;
		}
	}

	if(shouldBeLocked && !_shelfIsLocked){
		// lock shelf
		//_sendLockMessage(MESSAGETYPEID_NFC_LOCK_LOCK);
		_sendLockMessage(MESSAGETYPEID_LOCK_LOCK);
        _shelfIsLocked = true;
	} else if (!shouldBeLocked && _shelfIsLocked){
		// unlock shelf
		//_sendLockMessage(MESSAGETYPEID_NFC_LOCK_UNLOCK);
        _sendLockMessage(MESSAGETYPEID_LOCK_UNLOCK);
		_shelfIsLocked = false;
	}
}

void NFCManager::_sendLockMessage(uint8_t state){
	// send I2C message to lock
	Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
    Wire.write (MESSAGETYPEID_LOCK);
    Wire.write (MESSAGETYPEID_LOCK_LOCKID_SHELF);
	Wire.write(state); 
	Wire.endTransmission();
}

void NFCManager::_notifyToolOverallStatus(){
	//Sends Tool a XBee message with a status of each detector
	_xBeePayload[0] = MESSAGETYPEID_NFC_TOOL;
	_xBeePayload[1] = MESSAGETYPEID_NFC_TOOL_STATUS;
	for(int i=0; i<5; i++){
		_xBeePayload[i+2] = _managerStates[i];
	}
	_xbee_pointer->send(_toolZBTxRequest);
}

void NFCManager::_notifyPrimaryManager(uint8_t detectorId, bool detected){
//	_xBeePayload[0] = MESSAGETYPEID_NFC_MANAGE;
    _xBeePayload[0] = LOCK_MANAGER_I2C_ADDR;
//	_xBeePayload[1] = _managerId;
    _xBeePayload[1] = MESSAGETYPEID_LOCK;
//	_xBeePayload[2] = detected ? MESSAGETYPEID_NFC_MANAGE_FOUND : MESSAGETYPEID_NFC_MANAGE_NOTFOUND;
    _xBeePayload[2] = MESSAGETYPEID_LOCK_LOCKID_SHELF;
//	_xBeePayload[3] = detectorId;
    _xBeePayload[3] = MESSAGETYPEID_LOCK_UNLOCK;
//
	_xbee_pointer->send(_laser1ZBTxRequest);
	_xbee_pointer->send(_toolZBTxRequest);
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
	for(i=0; i<_numRegisteredDetector; i++){
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
