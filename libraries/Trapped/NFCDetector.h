/*
  NFCDetector.h - Library for NFC detector module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef NFCDetector_h
#define NFCDetector_h

#include "Arduino.h"
#include "Constants.h"

class NFCDetector
{
  public:
    NFCDetector(int detectorId, int i2cAddressId);
    boolean isRegistered();
    void registerWithNFCManager();
  private:
    int _detectorId;   
    bool _isRegistered; 
};

#endif