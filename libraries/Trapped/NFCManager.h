/*
  NFCManager.h - Library for NFC Manager module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef NFCManager_h
#define NFCManager_h

#include "Arduino.h"

class NFCManager
{
  public:
    NFCManager(int managerId);
    void registerWithShelfLockManager();
  private:
    int _detectorId;    
};

#endif