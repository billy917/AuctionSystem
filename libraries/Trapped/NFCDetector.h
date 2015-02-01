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
    
    bool isRegistered();
    bool isNFCCardDetected();

    void registerWithNFCManager();
    void printFirmwareInfo();
    void detectNFCChanges();
  private:
    int _detectorId;   
    bool _isRegistered; 
    uint32_t _nfcFirmwareVersion;
    uint8_t _uid[7]; 
    uint8_t _lastUid[7];
    uint8_t _uidLength;
};

#endif