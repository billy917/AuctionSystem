/*
  NFCManager.cpp - Library for NFC Manager module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/

#include "Arduino.h"
#include <Wire.h>
#include <NFCManager.h>
#include <XBee.h>

NFCManager::NFCManager(int managerId, bool localLockManager){
	_managerId = managerId;
	_localLockManager = localLockManager;
}

void NFCManager::registerWithShelfLockManager(){

}

void NFCManager::handleI2CMessage(uint8_t dataLength, uint8_t data[]){

}

void NFCManager::setXBeeReference(XBee* xbee_pointer){
	_xbee_pointer = xbee_pointer;
}
  	