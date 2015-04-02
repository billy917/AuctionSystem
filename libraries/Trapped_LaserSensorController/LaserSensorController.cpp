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
	//This controller is meant to be used on a Mega where pin D2(0), D3(1), D19(4) are used for interrupts
	
	for (int i=0; i<3; i++){
		_sensorEnabled[i] = false;
		_sensorTimeTick[i] = 1;		
		_interruptIds[i] = 0;
		_interruptFuncs[i] = NULL;
		_sensorPins[i] = 0;
	}

	_laser2Addr = XBeeAddress64(0x0013a200, 0x40c04ef1); //Laser2 xBee 
	_laser2ZBTxRequest = ZBTxRequest(_laser2Addr, _xBeePayload, sizeof(_xBeePayload));
	for (int i=0;i<4;i++){
		_xBeePayload[i] = 0;
	}
}

void LaserSensorController::setXBeeReference(XBee* xbee_pointer){
	_xbee = xbee_pointer;
}

void LaserSensorController::setSensorPin(int sensorId, int interruptId, int pin, uint8_t i2cAddress, void(*interruptFunc)() ){
	_sensorIds[_numRegisteredSensors] = sensorId;
	_sensorPins[_numRegisteredSensors] = pin;
	_sensorI2CAddresses[_numRegisteredSensors] = i2cAddress;
	_sensors[_numRegisteredSensors] = new SFE_TSL2561();
	_sensors[_numRegisteredSensors]->begin(i2cAddress);
	_sensors[_numRegisteredSensors]->setTiming(1,0);	
	_sensors[_numRegisteredSensors]->setInterruptControl(1,1);
	_sensors[_numRegisteredSensors]->clearInterrupt();
	_sensors[_numRegisteredSensors]->setPowerUp();	
	_interruptIds[_numRegisteredSensors] = interruptId;
	_interruptFuncs[_numRegisteredSensors] = interruptFunc;
	_numRegisteredSensors++;
	Serial.print("Set sensorId:"); Serial.print(sensorId); Serial.print(" - pin:"); Serial.println(pin);
}

void LaserSensorController::handleMessage(uint8_t dataLength, uint8_t data[]){
	Serial.print("Handle msg: ");
	Serial.print(data[0]); Serial.print("-");
	Serial.print(data[1]); Serial.print("-");
	Serial.println(data[2]);
	if(dataLength > 0 && MESSAGETYPEID_LASER_SENSOR == data[0]){
		if(MESSAGETYPEID_LASER_SENSOR_ON == data[1]){			
			enableSensorBySensorId(data[2]);
		} else if(MESSAGETYPEID_LASER_SENSOR_OFF == data[1]){
			disableSensorBySensorId(data[2]);
		} else if(MESSAGETYPEID_LASER_SENSOR_CALIBRATE == data[1]){		
			calibrateSensorBySensorId(data[2]);			
		}
	}
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
		if(calibrateSensorByIndex(sensorIndex)){
			_sensorEnabled[sensorIndex] = true;
			attachInterrupt(_interruptIds[sensorIndex], _interruptFuncs[sensorIndex], FALLING);		 
		}
	}
}

void LaserSensorController::disableSensorBySensorId(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);
	if(-1 < sensorIndex){
		if(_sensorEnabled[sensorIndex]){
			detachInterrupt(_interruptIds[sensorIndex]);
			_sensorEnabled[sensorIndex] = false;
			SFE_TSL2561* sensor = _sensors[sensorIndex];
			if(NULL != sensor){
				sensor->setInterruptControl(0,1);
			}
		}	
	}
}

bool LaserSensorController::calibrateSensorBySensorId(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);
	if(-1 < sensorIndex){		
		return calibrateSensorByIndex(sensorIndex);
	}
	return false;
}

bool LaserSensorController::calibrateSensorByIndex(int sensorIndex){
	SFE_TSL2561* sensor = _sensors[sensorIndex];
	if(NULL != sensor){
		return calibrateSensor(sensor);
	}
	return false;
}

bool LaserSensorController::calibrateSensor(SFE_TSL2561* sensor){
	if(NULL != sensor){
		delay(1000);
		unsigned int data0, data1;
		boolean successRead = sensor->getData(data0,data1);
		if(!successRead){
			return false;
		}
		int threshold1 = data0/3;
		
		delay(1000);
		sensor->getData(data0,data1);
		int threshold2 = data0/3;
		
		int threshold = (threshold1 + threshold2)/2;		
		int control = sensor->setInterruptControl(1,1);		
		int setThreshold = sensor->setInterruptThreshold(threshold,threshold*1000);	
		sensor->clearInterrupt();		
		Serial.print("Threshold:");Serial.print(data0);Serial.print("-");
		Serial.print(data1);Serial.print("-");Serial.print(threshold);Serial.print("-");
		Serial.print(control);Serial.print("-");Serial.println(setThreshold);	
	}
	return true;
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
			unsigned long currTime = millis();
			
			if(_lastTrippedTime == 0 || currTime - _lastTrippedTime > 1000){
				_lastTrippedTime = currTime;	
													
				_xBeePayload[0] = MESSAGETYPEID_CLOCK;
				_xBeePayload[1] = MESSAGETYPEID_CLOCK_MODIFY_SUBTRACT; 				
				_xBeePayload[2] = 1;
				_xBeePayload[3] = sensorId;
				_xbee->send(_laser2ZBTxRequest);
			}
		}
		_sensors[sensorIndex]->clearInterrupt();
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
