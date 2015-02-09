/*
  LaserController.h - Library for Laser Controller module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef LaserController_h
#define LaserController_h

#include "Arduino.h"
#include "Constants.h"

class LaserController
{
  public:
    LaserController(int controllerId, bool isPrimary); 
    bool canHandleI2CMessageType(uint8_t messageTypeId);   
    void handleI2CMessage(uint8_t dataLength, uint8_t data[]);
    void setLaserPin(int laserId, int pin);

  private:
    int _controllerId;
    int _numRegisteredLasers;
    int _localLaserIds[3];
    int _laserPins[3];
    int _laserStates[3];

    bool _isLaserIndexLocal(int laserId);
    void _turnOnLocalLaser(int laserId);
    void _turnOffLocalLaser(int laserId);
    void _forwardMessageToOtherControllers(uint8_t laserId, uint8_t dataLength, uint8_t data[]);
};

#endif