/*
  NFCManager.h - Library for NFC Manager module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef NFCManager_h
#define NFCManager_h

#include "Arduino.h"
#include <XBee.h>

class NFCManager
{
  public:
    NFCManager(int managerId, bool localLockManager);
    void registerWithShelfLockManager();
    void handleI2CMessage(uint8_t dataLength, uint8_t data[]);
  	void setXBeeReference(XBee* xbee_pointer);
  private:
    int _managerId;
    int _numRegisteredDetector;
    uint8_t _detectorIdIndexMap;
    bool _localLockManager;    
    XBee* _xbee_pointer;
};

#endif