/*
  LaserSensorController.cpp - Library for Laser Sensor Controller module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/

#include "Arduino.h"
#include <LaserSensorController.h>
#include <Constants.h>
#include <Wire.h>

LaserSensorController::LaserSensorController(int controllerId, bool isPrimary){
	_isPrimary = isPrimary;
	_controllerId = controllerId; 
	_numRegisteredSensors = 0;
	//This controller is meant to be used on a Mega where pin D2(0), D3(1), D19(4) are used for interrupts
	
	for (int i=0; i<3; i++){
		_sensorEnabled[i] = false;
		_sensorTimeTick[i] = 1;
	}
}

void LaserSensorController::setSensorPin(int sensorId, int pin, uint8_t i2cAddress){
	_sensorIds[_numRegisteredSensors] = sensorId;
	_sensorPins[_numRegisteredSensors] = pin;
	_sensorI2CAddresses[_numRegisteredSensors] = i2cAddress;
	_sensors[_numRegisteredSensors] = new SFE_TSL2561();
	_sensors[_numRegisteredSensors]->begin(i2cAddress);
	unsigned int ms;
	_sensors[_numRegisteredSensors]->setTiming(0,0,ms);
	_sensors[_numRegisteredSensors]->setInterruptControl(1,1);
	_sensors[_numRegisteredSensors]->setPowerUp();
	calibrateSensor(_sensors[_numRegisteredSensors]);
	_sensorEnabled[_numRegisteredSensors] = true;
	_numRegisteredSensors++;
}

void LaserSensorController::handleMessage(uint8_t dataLength, uint8_t data[]){
	if(dataLength > 0 && MESSAGETYPEID_LASER_SENSOR == data[0]){
		//TODO: handle message

		// calibrate sensor
		// disable/enable
		// get reading
	}
}

void LaserSensorController::pinInterrupted(int pin){
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

void LaserSensorController::calibrateSensorBySensorId(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);
	if(-1 < sensorIndex){
		Serial.print("Calibrating SensorId:");Serial.println(sensorId);
		SFE_TSL2561* sensor = _sensors[sensorIndex];
		if(NULL != sensor){
			calibrateSensor(sensor);
		}
	}
}

void LaserSensorController::calibrateSensor(SFE_TSL2561* sensor){
	if(NULL != sensor){
		unsigned int data0, data1;
		sensor->getData(data0,data1);
		int threshold = data0/2;
		Serial.print("Sensor values:");Serial.print(data0);Serial.print("-");
		Serial.print(data1);Serial.print("-");
		Serial.println(threshold);
		sensor->setInterruptThreshold(threshold,10000);
	}
}

void LaserSensorController::trippedWire(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);
	if(sensorIndex > -1){
		if(_sensorEnabled[sensorIndex]){
			byte i2cAddress = 0;
			if(_isPrimary){
				i2cAddress = CLOCK_I2C_ADDR;
			} else {
				i2cAddress = NFC_MANAGER_I2C_ADDR;
			}
			Wire.beginTransmission(i2cAddress); // transmit to device #100 (NFCDetectorManager
			Wire.write(MESSAGETYPEID_CLOCK);
			Wire.write(MESSAGETYPEID_CLOCK_MODIFY); 
			Wire.write(MESSAGETYPEID_CLOCK_MODIFY_SUBTRACT);
			Wire.write(_sensorTimeTick[sensorIndex]);
			Wire.endTransmission();
		}
	}
}