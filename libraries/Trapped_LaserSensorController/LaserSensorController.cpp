/*
  LaserSensorController.cpp - Library for Laser Sensor Controller module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/

#include "Arduino.h"
#include <LaserSensorController.h>
#include <Constants.h>
#include <Wire.h>
#include <XBee.h>

LaserSensorController::LaserSensorController(int controllerId, bool isPrimary){
	_isPrimary = isPrimary;
	_controllerId = controllerId; 
	_numRegisteredSensors = 0;
	_lastTrippedTime = 0;
	_debugMode = true;
	//This controller is meant to be used on a Mega where pin D2(0), D3(1), D19(4) are used for interrupts
	
	for (int i=0; i<3; i++){
		_sensorEnabled[i] = false;
		_sensorTimeTick[i] = 1;		
		_interruptIds[i] = 0;
		_interruptFuncs[i] = NULL;
		_sensorPins[i] = 0;
		_sensorState[i] = SENSOR_STATE_NONE;
		_lastTripTime[i] = 0;
		_trippedFlag[i] = false;
		_sensorData[i] = -1;
		_sensorPersistConfig[i] = 1;
	}

	_laser2Addr = XBeeAddress64(0x0013a200, 0x40c04ef1); //Laser2 xBee 
	_laser2ZBTxRequest = ZBTxRequest(_laser2Addr, _xBeePayload, sizeof(_xBeePayload));

	_toolAddr = XBeeAddress64(0x0013a200, 0x40b9df66); //HackTool xBee 
	_toolZBTxRequest = ZBTxRequest(_toolAddr, _xBeePayload, sizeof(_xBeePayload));

	for (int i=0;i<9;i++){
		_xBeePayload[i] = 0;
	}
}

void LaserSensorController::clearTrippedFlag(){
	for(int i=0; i<3; i++){
		if(_trippedFlag[i]){
			unsigned long currTime = millis();
			if(currTime - _lastTripTime[i] > 2000){				
				_sensors[i]->clearInterrupt();
				_trippedFlag[i] = false;				
			}
		}
	}
}

void LaserSensorController::setXBeeReference(XBee* xbee_pointer){
	_xbee = xbee_pointer;
}

void LaserSensorController::setSensorPin(int sensorId, int interruptId, int pin, int persistConfig, uint8_t i2cAddress, void(*interruptFunc)() ){
	_sensorIds[_numRegisteredSensors] = sensorId;
	_sensorPins[_numRegisteredSensors] = pin;
	_sensorI2CAddresses[_numRegisteredSensors] = i2cAddress;
	_sensors[_numRegisteredSensors] = new SFE_TSL2561();
	_sensors[_numRegisteredSensors]->begin(i2cAddress);
	_sensors[_numRegisteredSensors]->setTiming(1,0);
	_sensorPersistConfig[_numRegisteredSensors] = persistConfig;
		// gain, time
		// If time = 0, integration will be 13.7ms
		// If time = 1, integration will be 101ms
		// If time = 2, integration will be 402ms	
	_sensors[_numRegisteredSensors]->setInterruptControl(1,persistConfig);		
		// control, persist
		// If control = 0, interrupt output disabled
		// If control = 1, use level interrupt, see setInterruptThreshold()
		// If persist = 0, every integration cycle generates an interrupt
		// If persist = 1, any value outside of threshold generates an interrupt
		// If persist = 2 to 15, value must be outside of threshold for 2 to 15 integration cycles
	_sensors[_numRegisteredSensors]->clearInterrupt();
	_sensors[_numRegisteredSensors]->setPowerUp();	
	_sensorState[_numRegisteredSensors] = SENSOR_STATE_INIT;
	_interruptIds[_numRegisteredSensors] = interruptId;
	_interruptFuncs[_numRegisteredSensors] = interruptFunc;
	_numRegisteredSensors++;
	Serial.print("Set sensorId:"); Serial.print(sensorId); Serial.print(" - pin:"); Serial.println(pin);
}

void LaserSensorController::handleMessage(uint8_t dataLength, uint8_t data[]){
	if(_debugMode){
		Serial.print("DEBUG::");
	}
	Serial.print("Handle msg: ");
	Serial.print(data[0]); Serial.print("-");
	Serial.print(data[1]); Serial.print("-");
	Serial.println(data[2]);
	if(dataLength > 0 && MESSAGETYPEID_LASER_SENSOR == data[0]){
		if(MESSAGETYPEID_LASER_SENSOR_ON == data[1]){			
			enableSensorBySensorId(data[2]);
		} else if(MESSAGETYPEID_LASER_SENSOR_OFF == data[1]){
			disableSensorBySensorId(data[2]);
		} else if(MESSAGETYPEID_LASER_SENSOR_OFF_ALL == data[1]){
			pauseAllSensors();
		} else if(MESSAGETYPEID_LASER_SENSOR_ON_ALL == data[1]){
			reenableAllSensors();
		} else if(MESSAGETYPEID_LASER_SENSOR_CALIBRATE == data[1]){		
			calibrateSensorBySensorId(data[2]);			
		} else if(MESSAGETYPEID_LASER_SENSOR_DEBUG == data[1]){
			_debugMode = !_debugMode;
		} else if (!_debugMode && MESSAGETYPEID_LASER_SENSOR_REQUEST == data[1]){
			updateToolStatus();
		}
	}
	if(_debugMode){
		//update hack tool on sensor details
		updateToolStatus();
	}
}

void LaserSensorController::updateToolStatus(){
	Serial.println("== Sensor State ==");
	_xBeePayload[0] = MESSAGETYPEID_LASER_SENSOR;
	_xBeePayload[1] = MESSAGETYPEID_LASER_SENSOR_STATUS;	
	Serial.print("Num sensors:");Serial.println(_numRegisteredSensors);
	for(int i=0; i<_numRegisteredSensors; i++){
		_xBeePayload[2] = _sensorIds[i];		
		_xBeePayload[3] = _sensorState[i];
		_xBeePayload[4]	= _sensorData[i];	
		_xbee->send(_toolZBTxRequest);
		delay(500);
		Serial.print(_sensorIds[i]); Serial.print("-");Serial.print(_sensorEnabled[i]); Serial.print("-");Serial.println(_sensorState[i]);
	}
	

	Serial.println("============");
}

void LaserSensorController::pinInterrupted(int pin){
	Serial.print("Interrupted:");Serial.println(pin);
	int sensorIndex = _getSensorIndexByPin(pin);
	if(-1 < sensorIndex){		
		int sensorId = _sensorIds[sensorIndex];
		trippedWire(sensorId);
	}
}

int LaserSensorController::_getSensorIndexByPin(int sensorPin){
	for(int i=0;i<3;i++){
		if(sensorPin == _sensorPins[i]){
			return i;
		}
	}
	return -1;
}

int LaserSensorController::_getSensorIndexById(int sensorId){
	for(int i=0;i<3;i++){
		if(sensorId == _sensorIds[i]){
			return i;
		}
	}
	return -1;
}

void LaserSensorController::enableSensorBySensorId(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);	
	if(-1 < sensorIndex){		
		if(calibrateSensorByIndex(sensorId, sensorIndex)){
			attachInterrupt(_interruptIds[sensorIndex], _interruptFuncs[sensorIndex], FALLING);		 
			_sensorEnabled[sensorIndex] = true;			
			Serial.print("Sensor state:"); Serial.println(_sensorState[sensorIndex]);
		}
	}
}

void LaserSensorController::disableSensorBySensorId(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);
	if(-1 < sensorIndex){		
		if(_sensorEnabled[sensorIndex]){
			_sensorEnabled[sensorIndex] = false;
			_sensorState[sensorIndex] = SENSOR_STATE_OFF;	
			detachInterrupt(_interruptIds[sensorIndex]);								
			SFE_TSL2561* sensor = _sensors[sensorIndex];
			if(NULL != sensor){
				sensor->setInterruptControl(0,_sensorPersistConfig[sensorIndex]);
			}
		}	
	}
}

void LaserSensorController::pauseAllSensors(){
	for(int sensorIndex=0; sensorIndex<_numRegisteredSensors; sensorIndex++){
		detachInterrupt(_interruptIds[sensorIndex]);								
		SFE_TSL2561* sensor = _sensors[sensorIndex];
		if(NULL != sensor){
			sensor->setInterruptControl(0,_sensorPersistConfig[sensorIndex]);
		}
	}
}

void LaserSensorController::reenableAllSensors(){
	for(int sensorIndex=0; sensorIndex<_numRegisteredSensors; sensorIndex++){
		if (_sensorEnabled[sensorIndex]){
			if(calibrateSensorByIndex(_sensorIds[sensorIndex], sensorIndex)){
				attachInterrupt(_interruptIds[sensorIndex], _interruptFuncs[sensorIndex], FALLING);		 
			}			
		}
	}
}

bool LaserSensorController::calibrateSensorBySensorId(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);
	if(-1 < sensorIndex){		
		return calibrateSensorByIndex(sensorId, sensorIndex);
	}
	return false;
}

bool LaserSensorController::calibrateSensorByIndex(int sensorId, int sensorIndex){
	SFE_TSL2561* sensor = _sensors[sensorIndex];
	if(NULL != sensor){
		uint8_t returnCode = calibrateSensor(sensorId, sensorIndex, sensor);
		_sensorState[sensorIndex] = returnCode;
		if(SENSOR_STATE_ON == returnCode){
			return true;
		}
	}
	return false;
}

uint8_t LaserSensorController::calibrateSensor(int sensorId, int sensorIndex, SFE_TSL2561* sensor){
	if(NULL != sensor){
		delay(1000);
		unsigned int data0, data1;		
		if(!sensor->getData(data0,data1)){
			Serial.println("Unable to read data 1 when calibrating sensor");
			return SENSOR_STATE_CANNOT_READ_DATA;
		}
		int threshold1 = data0/3;
		
		delay(1000);
		if(!sensor->getData(data0,data1)){
			Serial.println("Unable to read data 2 when calibrating sensor");
			return SENSOR_STATE_CANNOT_READ_DATA;
		}
		int threshold2 = data0/3;
		
		int threshold = (threshold1 + threshold2)/2;	
		_sensorData[sensorIndex] = threshold;

		if(!AUTO_CALIBRATE_SENSOR[sensorId-1]){
			threshold = SENSOR_TRESHOLD[sensorId-1];
			_sensorData[sensorIndex] = data0;
		}
		
		if(!sensor->setInterruptThreshold(threshold,-1)){
				//-1 unsigned int == max possible value
			Serial.println("Unable to set threshold");
			return SENSOR_STATE_CANNOT_SET_THRESHOLD;
		}

		if(!sensor->setInterruptControl(1,_sensorPersistConfig[sensorIndex])){
			Serial.println("Unable to set interrupt control");
			return SENSOR_STATE_CANNOT_SET_INTERRUPT_CONTROL;
		}
		
		if(!sensor->clearInterrupt()){
			Serial.println("Unable to clear interrupt");
			return SENSOR_STATE_CANNOT_CLEAR_INTERRUPT;
		}

		Serial.print("Threshold:");Serial.print(data0);Serial.print("-");
		Serial.print(data1);Serial.print("-");Serial.println(threshold);
		
	}
	return SENSOR_STATE_ON;
}

unsigned int LaserSensorController::getReadings(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);
	if(-1 < sensorIndex){		
		SFE_TSL2561* sensor = _sensors[sensorIndex];
		if(NULL != sensor){
			unsigned int data0, data1;
			sensor->getData(data0,data1);
			return data0;
		}
	}
	return 0;
}

void LaserSensorController::trippedWire(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);
	if(sensorIndex > -1){		
		if(_sensorEnabled[sensorIndex]){
			_trippedFlag[sensorIndex] = true;
			_lastTripTime[sensorIndex] = millis();

			_xBeePayload[0] = MESSAGETYPEID_CLOCK;
			_xBeePayload[1] = MESSAGETYPEID_CLOCK_MODIFY_SUBTRACT; 				
			_xBeePayload[2] = 1;
			_xBeePayload[3] = sensorId;
			_xbee->send(_laser2ZBTxRequest);
		}		
	}
}

void LaserSensorController::printState(){
	Serial.print("State[");
	for(int i=0; i<_numRegisteredSensors; i++){
		Serial.print(i);Serial.print("-");
		Serial.print(_sensorIds[i]);Serial.print("-");
		Serial.print(_sensorEnabled[i]);
	}	
	Serial.println("]");
}
