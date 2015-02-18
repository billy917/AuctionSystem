/*
  LockManager.cpp - Library for Lock Manager module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/

#include "Arduino.h"
#include <Wire.h>
#include <LockManager.h>
#include <Constants.h>

LockManager::LockManager(){
}

bool LockManager::canHandleI2CMessage(uint8_t messageTypeId){
	return MESSAGETYPEID_LOCK == messageTypeId;
}

void LockManager::handleI2CMessage(uint8_t dataLength, uint8_t data[]){	
	if(MESSAGETYPEID_LOCK == data[0]){ // Detector <-> Manager		
		if(MESSAGETYPEID_LOCK_LOCK == data[2]){
			lock(data[1]);
		} else if (MESSAGETYPEID_LOCK_UNLOCK == data[2]){
			unlock(data[1]);
		} else if (MESSAGETYPEID_LOCK_UNLOCK_5SEC == data[2]){
			_timedUnlocked(data[1], 5);
		}		
	}
}

int LockManager::_getSignalPin(uint8_t lockId){
	if(MESSAGETYPEID_LOCK_LOCKID_SHELF == lockId){
		return LOCK_MANAGER_SHELF_PIN;
	} else if (MESSAGETYPEID_LOCK_LOCKID_INWALL == lockId){
		return LOCK_MANAGER_INWALL_PIN;
	} else if (MESSAGETYPEID_LOCK_LOCKID_WHITEDOOR == lockId){
		return LOCK_MANAGER_WDOOR_PIN;
	} else if (MESSAGETYPEID_LOCK_LOCKID_MAINDOOR == lockId){
		return LOCK_MANAGER_MDOOR_PIN;
	}
	return -1;
}

bool LockManager::_isFailSafeLock(uint8_t lockId){
	// Fail-Safe means lock is unlocked when there's no power
	// conversely, Fail-Secure means lock is locked when there's no power
	if(MESSAGETYPEID_LOCK_LOCKID_SHELF == lockId || MESSAGETYPEID_LOCK_LOCKID_WHITEDOOR == lockId){
		return false;
	}
	return true;
}

void LockManager::lock(int lockId){
	Serial.println("Locking");
	int signalPin = _getSignalPin(lockId);
	if(signalPin != -1){
		if(_isFailSafeLock(lockId)){
			digitalWrite(signalPin, HIGH);
		} else {
			digitalWrite(signalPin, LOW);
		}
	}
	Serial.print("Lock:"); Serial.println(lockId);
}		

void LockManager::unlock(int lockId){
	int signalPin = _getSignalPin(lockId);
	if(signalPin != -1){
		if(_isFailSafeLock(lockId)){
			digitalWrite(signalPin, LOW);
		} else {
			digitalWrite(signalPin, HIGH);
		}
		Serial.print("Unlock:"); Serial.println(lockId);
	}
}

void LockManager::_timedUnlocked(int lockId, int durationInSec){
	int signalPin = _getSignalPin(lockId);
	if(signalPin != -1){
		
	}
}
