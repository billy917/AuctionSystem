/*
  NFCManager.h - Library for LockManager module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef LockManager_h
#define LockManager_h

#include "Arduino.h"
#include "Constants.h"

class LockManager
{
  public:
    LockManager();
    bool canHandleI2CMessage(uint8_t messageTypeId);
    void handleI2CMessage(uint8_t dataLength, uint8_t data[]);
  	void lock(int lockId);
    void unlock(int lockId);
  private:
    int _getSignalPin(uint8_t lockId);
    bool _isFailSafeLock(uint8_t lockId);    
    void _timedUnlocked(int lockId, int durationInSec);
};

#endif