/*
  LaserController.h - Library for Laser Controller module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef LaserController_h
#define LaserController_h

#include "Arduino.h"
#include "Constants.h"
#include <XBee.h>

class LaserController
{
  public:
    LaserController(int controllerId, bool isPrimary, bool enableSensor); 
    bool canHandleMessageType(uint8_t messageTypeId);   
    void setXBeeReference(XBee* xbee_pointer);
    void handleMessage(uint8_t dataLength, uint8_t data[]);
    void setLaserPin(int laserId, int pin);
    void turnOnAllLaser();
    void turnOffAllLaser();

  private:
    bool _isPrimary;
    bool _enableSensor;
    int _controllerId;
    int _numRegisteredLasers;
    int _localLaserIds[3];
    int _laserPins[3];
    int _laserStates[3];
    uint8_t _xBeePayload[3];
    XBee* _xbee;
    XBeeAddress64 _laser1Addr;
    ZBTxRequest _laser1ZBTxRequest;
    XBeeAddress64 _laser2Addr;
    ZBTxRequest _laser2ZBTxRequest;
    XBeeAddress64 _laser3Addr;
    ZBTxRequest _laser3ZBTxRequest;
    XBeeAddress64 _nfcManagerAddr;
    ZBTxRequest _nfcManagerZBTxRequest;
    XBeeAddress64 _toolAddr;
    ZBTxRequest _toolZBTxRequest;

    XBeeAddress64 _sensor1Addr;
    ZBTxRequest _sensor1ZBTxRequest;

    XBeeAddress64 _sensor2Addr;
    ZBTxRequest _sensor2ZBTxRequest;

    XBeeAddress64 _sensor3Addr;
    ZBTxRequest _sensor3ZBTxRequest;

    bool _isLaserIndexLocal(int laserId);
    void _turnOnLocalLaser(int laserId);
    void _turnOnLocalLaserByIndexId(int indexId);
    void _turnOffLocalLaser(int laserId);
    void _turnOffLocalLaserByIndexId(int indexId);
    void _turnOnSensor(int laserId);
    void _turnOffSensor(int laserId);
    void _switchSensorState(int laserId, bool on);
    void _forwardMessageToOtherControllers(uint8_t laserId, uint8_t dataLength, uint8_t data[]);
};

#endif