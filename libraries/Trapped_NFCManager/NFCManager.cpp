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
		_laserIds[i] = 0;
		_laserPins[i] = 0;
		_laserStates[i] = false;
	}
}

bool NFCManager::canHandleI2CMessageType(uint8_t messageTypeId){}
	return MESSAGETYPEID_LASER_CONTROL == messageTypeId;
}

void NFCManager::setLaserPin(int laserId, int pin){

}

void NFCManager::handleI2CMessage(uint8_t dataLength, uint8_t data[]){	
	if(3 == dataLength){
		if(_isLaserIndexLocal(data[2])){
			if(MESSAGETYPEID_LASER_SENSOR_ON == data[1]){
				_turnOnLaser
			} else if (MESSAGETYPEID_LASER_SENSOR_OFF == data[1]){

			}
		} else if(_isPrimary) {
			_forwardMessageToOtherControllers(data[2], dataLength, data[]);
		}
	}
}
