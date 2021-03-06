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


LaserController::LaserController(int controllerId, bool isPrimary, bool enableSensor){
	_controllerId = controllerId;
	_isPrimary = isPrimary;
	_enableSensor = enableSensor;
	_numRegisteredLasers=0;

	for(int i=0; i<3; i++){
		_localLaserIds[i] = 0;
		_laserPins[i] = 0;
		_xBeePayload[i] = 0;
		_laserStates[i] = false;
		_enableLaser[i] = false;
	}

	_laser1Addr = XBeeAddress64(0x0013a200, 0x40c04edf); //Laser1 xBee 
	_laser1ZBTxRequest = ZBTxRequest(_laser1Addr, _xBeePayload, sizeof(_xBeePayload));

	_laser2Addr = XBeeAddress64(0x0013a200, 0x40c04ef1); //Laser2 xBee 
	_laser2ZBTxRequest = ZBTxRequest(_laser2Addr, _xBeePayload, sizeof(_xBeePayload));

	_laser3Addr = XBeeAddress64(0x0013a200, 0x40c337e0); //Laser3 xBee 
	_laser3ZBTxRequest = ZBTxRequest(_laser3Addr, _xBeePayload, sizeof(_xBeePayload));

	_nfcManagerAddr = XBeeAddress64(0x0013a200, 0x40c04f18); //NFCManager xBee 
	_nfcManagerZBTxRequest = ZBTxRequest(_nfcManagerAddr, _xBeePayload, sizeof(_xBeePayload));

	_toolAddr = XBeeAddress64(0x0013a200, 0x40b9df66); //HackTool xBee 
	_toolZBTxRequest = ZBTxRequest(_toolAddr, _xBeePayload, sizeof(_xBeePayload));

	_sensor1Addr = XBeeAddress64(0x0013a200, 0x40cab3f1); //Sensor1 xBee  (1,2,3)
	_sensor1ZBTxRequest = ZBTxRequest(_sensor1Addr, _xBeePayload, sizeof(_xBeePayload));

	_sensor2Addr = XBeeAddress64(0x0013a200, 0x40bef834); //Sensor2 xBee  (4,5,6)
	_sensor2ZBTxRequest = ZBTxRequest(_sensor2Addr, _xBeePayload, sizeof(_xBeePayload));

	_sensor3Addr = XBeeAddress64(0x0013a200, 0x40bf36b4); //Sensor3 xBee  (7,8,9)
	_sensor3ZBTxRequest = ZBTxRequest(_sensor3Addr, _xBeePayload, sizeof(_xBeePayload));

}

bool LaserController::canHandleMessageType(uint8_t messageTypeId){
	return MESSAGETYPEID_LASER_CONTROL == messageTypeId || MESSAGETYPEID_GAME == messageTypeId;
}

void LaserController::setLaserPin(int laserId, int pin){
	_localLaserIds[_numRegisteredLasers] = laserId;
	_laserPins[_numRegisteredLasers] = pin;
	_enableLaser[_numRegisteredLasers] = GLOBAL_ENABLE_LASER[laserId-1];
	_numRegisteredLasers++;
}

int LaserController::_getIndexByLaserId(int laserId){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(laserId == _localLaserIds[i]){
			return i;
		}
	}
	return -1;
}

bool LaserController::_isLaserIndexLocal(int laserId){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(laserId == _localLaserIds[i]){
			return true;
		}
	}
	return false;
}

void LaserController::turnOnAllLaser(bool turnOnSensor){
	for(int i=0;i<_numRegisteredLasers; i++){		
		_turnOnLocalLaserByIndexId(i);		
	}
	if(turnOnSensor){
		delay(1000);
		for(int i=0;i<_numRegisteredLasers; i++){		
			_turnOnSensor(_localLaserIds[i]);				
		}
	}
}

void LaserController::turnOffAllLaser(){
	for(int i=0;i<_numRegisteredLasers; i++){		
		_turnOffSensor(_localLaserIds[i]);
	}
	delay(2500);
	for(int i=0;i<_numRegisteredLasers; i++){
		_turnOffLocalLaserByIndexId(i);		
	}
}

void LaserController::_turnOnLocalLaser(int laserId){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(laserId == _localLaserIds[i]){
			_turnOnLocalLaserByIndexId(i);
		}
	}
}

void LaserController::_turnOnLocalLaserByIndexId(int indexId){
	if(_enableLaser[indexId]){
		digitalWrite(_laserPins[indexId], HIGH);
		_laserStates[indexId] = true;
	}	
}

void LaserController::_turnOffLocalLaser(int laserId){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(laserId == _localLaserIds[i]){
			_turnOffLocalLaserByIndexId(i);
		}		
	}
}

void LaserController::_turnOffLocalLaserByIndexId(int indexId){
	if(_enableLaser[indexId]){
		digitalWrite(_laserPins[indexId], LOW);
		_laserStates[indexId] = false;
	}
}

void LaserController::_switchSensorState(int laserId, bool on){
	int laserIndex = _getIndexByLaserId(laserId);
	if(laserIndex > -1 && _enableSensor && GLOBAL_ENABLE_SENSOR[laserId-1] && _enableLaser[laserIndex]){
		int sensorId = GLOBAL_SENSOR_ID[laserId-1];
		int sensorManagerId = GLOBAL_SENSOR_MANAGER_ID[laserId-1];		
		_xBeePayload[0] = MESSAGETYPEID_LASER_SENSOR;
		_xBeePayload[1] = on?MESSAGETYPEID_LASER_SENSOR_ON:MESSAGETYPEID_LASER_SENSOR_OFF;
		_xBeePayload[2] = sensorId;

		if(sensorManagerId == 0){			
			_xbee->send(_sensor1ZBTxRequest);			
		} else if( sensorManagerId == 1 ) {
			_xbee->send(_sensor2ZBTxRequest);
		} else if( sensorManagerId == 2 ) {
			_xbee->send(_sensor3ZBTxRequest);
		}
	}
}

void LaserController::_turnOffSensor(int laserId){
	_switchSensorState(laserId,false);
}

void LaserController::_turnOnSensor(int laserId){
	_switchSensorState(laserId,true);
}


void LaserController::handleMessage(uint8_t dataLength, uint8_t data[]){	
	Serial.print("Handling Message:");Serial.print(data[0]);Serial.print(data[1]);Serial.println(data[2]);
	if(canHandleMessageType(data[0])){
		if(MESSAGETYPEID_GAME == data[0]){
			if(MESSAGETYPEID_GAME_INIT_LASER_CONFIG == data[1]){				
				int laserIndex = _getIndexByLaserId(data[2]);
				if(-1 < laserIndex){
					_enableLaser[laserIndex] = data[3]==0?false:true;
				} else if(_isPrimary) {
					_forwardMessageToOtherControllers(data[2], dataLength, data);
				}
			}
		} else if (MESSAGETYPEID_LASER_CONTROL == data[0]){
			if(MESSAGETYPEID_LASER_CONTROL_OFF_MODE_1 == data[1]){
				// forward to other laserControllers
				if(2 == _controllerId){ // only laser2 will recieve message from keypad
					_forwardMessageToOtherControllers(1, dataLength, data);
					_forwardMessageToOtherControllers(4, dataLength, data);
				}

				// turn off lasers
				for(int i=0; i<_numRegisteredLasers; i++){
					int localLaserId = _localLaserIds[i];
					if(!GLOBAL_LASER_PATTERN_1[localLaserId-1]){ // if laser is ON
						_turnOffSensor(localLaserId);
						delay(1000);
						_turnOffLocalLaser(localLaserId);
					}		
				}


			} else if (MESSAGETYPEID_LASER_CONTROL_ON_ALL == data[1]){
				turnOnAllLaser(true);
				if(_isPrimary){
					_forwardMessageToOtherControllers(4, dataLength, data);
					_forwardMessageToOtherControllers(7, dataLength, data);
				}
			} else if(MESSAGETYPEID_LASER_CONTROL_ON_ALL_NO_SENSOR == data[1]){
				turnOnAllLaser(false);
				if(_isPrimary){
					_forwardMessageToOtherControllers(4, dataLength, data);
					_forwardMessageToOtherControllers(7, dataLength, data);
				}
			} else if (MESSAGETYPEID_LASER_CONTROL_ON_ALL_ACTIVE == data[1]){ // turn on all active laser sensor
				_enableActiveLaserSensor();
				if(_isPrimary){
					_forwardMessageToOtherControllers(4, dataLength, data);
					_forwardMessageToOtherControllers(7, dataLength, data);
				}
			} else if (MESSAGETYPEID_LASER_CONTROL_OFF_ALL_ACTIVE == data[1]){ // turn off all active laser sensor
				_disableActiveLaserSensor();
				if(_isPrimary){
					_forwardMessageToOtherControllers(4, dataLength, data);
					_forwardMessageToOtherControllers(7, dataLength, data);
				}
			} else if (MESSAGETYPEID_LASER_CONTROL_OFF_ALL == data[1]){
				turnOffAllLaser();
				if(_isPrimary){
					_forwardMessageToOtherControllers(4, dataLength, data);
					_forwardMessageToOtherControllers(7, dataLength, data);
				}
			} else if(_isLaserIndexLocal(data[2])){
				if(MESSAGETYPEID_LASER_CONTROL_ON == data[1]){
					_turnOnLocalLaser(data[2]);
					delay(2500);
					_turnOnSensor(data[2]);
				} else if (MESSAGETYPEID_LASER_CONTROL_OFF == data[1]){
					_turnOffSensor(data[2]);
					delay(2500);
					_turnOffLocalLaser(data[2]);
				}
			} else if(_isPrimary) {
				_forwardMessageToOtherControllers(data[2], dataLength, data);
			}
		}
	}
}

void LaserController::_enableActiveLaserSensor(){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(_laserStates[i]){ // if laser is ON
			_switchSensorState(_localLaserIds[i],true);		
			delay(250);	
		}		
	}
}

void LaserController::_disableActiveLaserSensor(){
	for(int i=0; i<_numRegisteredLasers; i++){
		if(_laserStates[i]){ // if laser is ON
			_switchSensorState(_localLaserIds[i],false);	
			delay(250);		
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

	if(0 == GLOBAL_LASER_MANAGER_ID[laserId-1]){
		_xbee->send(_laser1ZBTxRequest);
	} else if(1 == GLOBAL_LASER_MANAGER_ID[laserId-1]){
		_xbee->send(_laser3ZBTxRequest);
	} else if (2 == GLOBAL_LASER_MANAGER_ID[laserId-1]) {
		_xbee->send(_laser2ZBTxRequest);
	}
}
