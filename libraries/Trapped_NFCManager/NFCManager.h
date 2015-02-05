/*
  NFCManager.h - Library for NFC Manager module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef NFCManager_h
#define NFCManager_h

#include "Arduino.h"
#include "Constants.h"
#include <XBee.h>

class NFCManager
{
  public:
    NFCManager(int managerId, bool localLockManager);
    void registerWithShelfLockManager();
    void handleI2CMessage(uint8_t dataLength, volatile uint8_t data[]);
  	void setXBeeReference(XBee* xbee_pointer);
    void setLockManagerI2CAdder(int i2cAddr);

  private:
    int _managerId;
    int _numRegisteredDetector;
    int _registeredDetectorIds[];
    bool _registeredDetectorStates[];
    uint8_t _xBeePayload[];
    uint8_t _detectorIdIndexMap;
    bool _localLockManager;  // if localLockManager is true, then relay messages via I2C
    int _localLockManagerAddr; // if localLockManager is true, set the I2C addr for the LockManager
    XBee* _xbee_pointer;
    
    XBeeAddress64 _laser1Addr;
    ZBTxRequest _laser1ZBTxRequest;
    XBeeAddress64 _toolAddr;
    ZBTxRequest _toolZBTxRequest;

    void _registerDetector(uint8_t detectorId);
    void _updateDetectorStates(uint8_t detectorId, bool detectedExpectedNFC);
    bool _areAllDetectorDetected();
    void _notifyLockManager(bool allDetectedExpectedNFC);
};

#endif