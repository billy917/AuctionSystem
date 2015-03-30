/*
  LaserSensorController.h - Library for Laser SensorController module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef LaserSensorController_h
#define LaserSensorController_h

#include "Arduino.h"
#include "Constants.h"
#include <SFE_TSL2561.h>
#include <Wire.h>

class LaserSensorController
{
  public:
    LaserSensorController(int controllerId, bool isPrimary); 
    void handleMessage(uint8_t dataLength, uint8_t data[]);
    void setSensorPin(int sensorId, int pin, uint8_t i2cAddress);
    unsigned int getReadings(int sensorId);
    void pinInterrupted(int pin);

    void calibrateSensorByIndex(int sensorIndex);
    void calibrateSensorBySensorId(int sensorId);
    void calibrateSensor(SFE_TSL2561* sensor);
    void enableSensorBySensorId(int sensorId);
    void disableSensorBySensorId(int sensorId);
    void trippedWire(int sensorId);
    void printState();

  private:
    bool _isPrimary;
    int _controllerId;
    int _numRegisteredSensors;
    unsigned long _lastTrippedTime;
    byte _sensorI2CAddresses[3];
    int _sensorIds[3];
    int _sensorPins[3];
    bool _sensorEnabled[3];
    SFE_TSL2561* _sensors[3];
    int _sensorTimeTick[3];

    int _getSensorIndexByPin(int sensorPin);
    int _getSensorIndexById(int sensorId);

};

#endif