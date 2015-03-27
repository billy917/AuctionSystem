/*
  LaserController.cpp - Library for Laser Controller module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/

#include "Arduino.h"
#include <LaserController.h>
#include <Constants.h>
#include <Wire.h>
#include <XBee.h>


LaserController::LaserController(int controllerId, bool isPrimary){
	_controllerId = controllerId;
	_isPrimary = isPrimary;
	_numRegisteredLasers=0;

	for(int i=0; i<3; i++){
		_localLaserIds[i] = 0;
		_laserPins[i] = 0;
		_xBeePayload[i] = 0;
		_laserStates[i] = false;
	}

	_laser1Addr = XBeeAddress64(0x0013a200, 0x40c04edf); //Laser1 xBee 
	_laser1ZBTxRequest = ZBTxRequest(_laser1Addr, _xBeePayload, sizeof(_xBeePayload));

	_laser2Addr = XBeeAddress64(0x0013a200, 0x40c04ef1); //Laser2 xBee 
	_laser2ZBTxRequest = ZBTxRequest(_laser2Addr, _xBeePayload, sizeof(_xBeePayload));

	_laser3Addr = XBeeAddress64(0x0013a200, 0x40c337e0); //Laser3 xBee 
	_laser3ZBTxRequest = ZBTxRequest(_laser3Addr, _xBeePayload, sizeof(_xBeePayload));

	_nfcManagerAddr = XBeeAddress64(0x0013a200, 0x40c04f18); //NFCManager xBee 
	_nfcManagerZBTxRequest = ZBTxRequest(_nfcManagerAddr, _xBeePayload, sizeof(_xBeePayload));
}

bool LaserController::canHandleMessageType(uint8_t messageTypeId){
	return MESSAGETYPEID_LASER_CONTROL == messageTypeId;
}

void LaserController::setLaserPin(int laserId, int pin){
	_localLaserIds[_numRegisteredLasers] = laserId;
	_laserPins[_numRegisteredLasers] = pin;
	_numRegisteredLasers++;
}

bool LaserController::_isLaserIndexLocal(int laserId){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(laserId == _localLaserIds[i]){
			return true;
		}
	}
	return false;
}

void LaserController::_turnOnLocalLaser(int laserId){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(laserId == _localLaserIds[i]){
			digitalWrite(_laserPins[i], HIGH);
			_laserStates[i] = true;
		}
	}
}

void LaserController::_turnOffLocalLaser(int laserId){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(laserId == _localLaserIds[i]){
			digitalWrite(_laserPins[i], LOW);
			_laserStates[i] = false;
		}
	}
}

void LaserController::_switchSensorState(int laserId, bool on){
	int sensorId = GLOBAL_LASER_ID[laserId-1];
	int sensorManagerId = GLOBAL_SENSOR_MANAGER_ID[laserId-1];
	int laserManagerId = GLOBAL_LASER_MANAGER_ID[laserId-1];
	if(sensorManagerId == laserManagerId){
		// same i2c bus, fwd command via i2c
		Wire.beginTransmission(LASER_SENSOR_I2C_ADDR); // transmit to device #100 (NFCDetectorManager
		Wire.write(MESSAGETYPEID_LASER_SENSOR);
		if(on){
			Wire.write(MESSAGETYPEID_LASER_SENSOR_ON); 
		} else {
			Wire.write(MESSAGETYPEID_LASER_SENSOR_OFF); 
		}
		Wire.write(laserId);
		Wire.endTransmission();
	} else {
		// on different i2c bus, fwd comand via xBee	
		_xBeePayload[0] = MESSAGETYPEID_LASER_SENSOR;
		_xBeePayload[1] = on?MESSAGETYPEID_LASER_SENSOR_ON:MESSAGETYPEID_LASER_SENSOR_OFF;
		_xBeePayload[2] = laserId;

		if(0 == sensorManagerId){		
			_xbee->send(_laser1ZBTxRequest);
		} else if (1 == sensorManagerId){
			_xbee->send(_nfcManagerZBTxRequest);
		} else if (2 == sensorManagerId){
			_xbee->send(_laser2ZBTxRequest);
		}
	}
}

void LaserController::_turnOffSensor(int laserId){
	_switchSensorState(laserId,true);
}

void LaserController::_turnOnSensor(int laserId){
	_switchSensorState(laserId,false);
}


void LaserController::handleMessage(uint8_t dataLength, uint8_t data[]){	
	if(canHandleMessageType(data[0])){
		if(_isLaserIndexLocal(data[2])){
			if(MESSAGETYPEID_LASER_CONTROL_ON == data[1]){
				_turnOnLocalLaser(data[2]);
				_turnOnSensor(data[2]);
			} else if (MESSAGETYPEID_LASER_CONTROL_OFF == data[1]){
				_turnOffSensor(data[2]);
				_turnOffLocalLaser(data[2]);
			}
		} else if(_isPrimary) {
			_forwardMessageToOtherControllers(data[2], dataLength, data);
		}
	}
}

void LaserController::setXBeeReference(XBee* xbee_pointer){
	_xbee = xbee_pointer;
}

void LaserController::_forwardMessageToOtherControllers(uint8_t laserId, uint8_t dataLength, uint8_t data[]){
	for(int i=0; i<3; i++){
		_xBeePayload[i] = data[i];
	}

	if(1 == GLOBAL_LASER_MANAGER_ID[laserId-1]){
		_xbee->send(_laser3ZBTxRequest);
	} else {
		_xbee->send(_laser2ZBTxRequest);
	}
}
