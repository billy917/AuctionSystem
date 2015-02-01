/*
  ShelfLockManager.h - Library for Shelf Lock Manager module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef ShelfLockManager_h
#define ShelfLockManager_h

#include "Arduino.h"

class ShelfLockManager
{
  public:
    ShelfLockManager();
    void registerWithShelfLockManager();
  private:
    int _detectorId;    
};

#endif