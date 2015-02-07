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
    bool detectedNFC();

    void initNFCCard();
    void registerWithNFCManager();
    void printFirmwareInfo();
    void detectNFCChanges();

  private:
    int _detectorId;   // from 0 to 4
    bool _isRegistered; 
    bool _lastDetectedChip;
    uint32_t _nfcFirmwareVersion;
    uint8_t _uid[UID_LENGTH]; 
    uint8_t _lastUid[UID_LENGTH];
    uint8_t _expectedUid[UID_LENGTH];
    uint8_t _expectedToolUid[UID_LENGTH];

    void _updateLastUID();
    void _clearLastUID();

    bool _isSameAsLastDetectedChip();
    bool _detectedExpectedChip();
    bool _detectedToolChip();
    
    void _notifyFoundToolChip();
    void _notifyFoundNFCChip();
    void _notifyCannotFindNFCChip();


};

#endif