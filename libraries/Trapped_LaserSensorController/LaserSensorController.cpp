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
	_lastTrippedTime = 0;
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
	_sensors[_numRegisteredSensors]->setTiming(0,0);	
	//Serial.print("Set interrupt control:");Serial.println();	
	_sensors[_numRegisteredSensors]->setInterruptControl(1,1);
	_sensors[_numRegisteredSensors]->clearInterrupt();
	_sensors[_numRegisteredSensors]->setPowerUp();
	_numRegisteredSensors++;

	calibrateSensorBySensorId(sensorId);
	enableSensorBySensorId(sensorId);		
}

void LaserSensorController::handleMessage(uint8_t dataLength, uint8_t data[]){
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
		calibrateSensorBySensorId(sensorId);
		_sensorEnabled[sensorIndex] = true;
	}
}

void LaserSensorController::disableSensorBySensorId(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);
	if(-1 < sensorIndex){
		_sensorEnabled[sensorIndex] = false;
	}
}

void LaserSensorController::calibrateSensorBySensorId(int sensorId){
	int sensorIndex = _getSensorIndexById(sensorId);
	if(-1 < sensorIndex){
		//Serial.print("Calibrating SensorId:");Serial.println(sensorId);
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
		int threshold1 = data0/3;
		//Serial.print("Sensor values:");Serial.print(data0);Serial.print("-"); 
		//Serial.print(data1);Serial.print("-"); Serial.println(threshold1);

		delay(1000);
		sensor->getData(data0,data1);
		int threshold2 = data0/3;
		//Serial.print("Sensor values:");Serial.print(data0);Serial.print("-"); 
		//Serial.print(data1);Serial.print("-"); Serial.println(threshold2);

		int threshold = (threshold1 + threshold2)/2;
		sensor->setInterruptThreshold(threshold,threshold*1000);		
	}
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
		_sensors[sensorIndex]->clearInterrupt();
		if(_sensorEnabled[sensorIndex]){
			unsigned long currTime = millis();
			//Serial.print("SensorId:"); Serial.print(sensorId); 
			//Serial.print(" tripped @ "); Serial.print(currTime);

			if(_lastTrippedTime == 0 || currTime - _lastTrippedTime > 1000){
				//Serial.print(" <--");
				_lastTrippedTime = currTime;	
			
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
				Wire.write(1);
				Wire.endTransmission();
			}
		}
		//Serial.println("");
	}
}