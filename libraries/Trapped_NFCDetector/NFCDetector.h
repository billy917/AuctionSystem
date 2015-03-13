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
    uint8_t _data[32];

    void _updateLastUID();
    void _clearLastUID();

    bool _isSameAsLastDetectedChip();
    bool _detectedExpectedChip(uint8_t firstByte, uint8_t lastByte);
    bool _detectedToolChip(uint8_t firstByte, uint8_t lastByte);
    
    void _notifyFoundToolChip();
    void _notifyFoundNFCChip(uint8_t firstByte);
    void _notifyCannotFindNFCChip();


};

#endif