/*
  LaserController.cpp - Library for Laser Controller module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/

#include "Arduino.h"
#include <LaserController.h>
#include <Constants.h>

LaserController::LaserController(int controllerId, bool isPrimary){
	_controllerId = controllerId;
	_isPrimary = isPrimary;
	_numRegisteredLasers=0;

	for(int i=0; i<3; i++){
		_localLaserIds[i] = 0;
		_laserPins[i] = 0;
		_laserStates[i] = false;
	}
}

bool NFCManager::canHandleI2CMessageType(uint8_t messageTypeId){}
	return MESSAGETYPEID_LASER_CONTROL == messageTypeId;
}

void NFCManager::setLaserPin(int laserId, int pin){
	_localLaserIds[_numRegisteredLasers] = laserId;
	_laserPins[_numRegisteredLasers] = pin;
	_numRegisteredLasers++;
}

bool NFCManager::_isLaserIndexLocal(int laserId){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(laserId == _localLaserIds){
			return true;
		}
	}
	return false;
}

void NFCManager::_turnOnLocalLaser(int laserId){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(laserId == _localLaserIds[i]){
			digitalWrite(_laserPins[i], HIGH);
			_laserStates[i] = true;
		}
	}
}

void NFCManager::_turnOffLocalLaser(int laserId){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(laserId == _localLaserIds[i]){
			digitalWrite(_laserPins[i], LOW);
			_laserStates[i] = false;
		}
	}
}

void NFCManager::handleI2CMessage(uint8_t dataLength, uint8_t data[]){	
	if(3 == dataLength){
		if(_isLaserIndexLocal(data[2])){
			if(MESSAGETYPEID_LASER_SENSOR_ON == data[1]){
				_turnOnLocalLaser(data[2]);
			} else if (MESSAGETYPEID_LASER_SENSOR_OFF == data[1]){
				_turnOffLocalLaser(data[2]);
			}
		} else if(_isPrimary) {
			_forwardMessageToOtherControllers(data[2], dataLength, data[]);
		}
	}
}

void NFCManager::_forwardMessageToOtherControllers(uint8_t laserId, uint8_t dataLength, uint8_t data[]){
	
}
