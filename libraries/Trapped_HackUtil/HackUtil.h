/*
  HackUtil.h - Library for HackUtil module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef HackUtil_h
#define HackUtil_h

#include "Arduino.h"  

class HackUtil
{
  public:
    HackUtil();
    void init();
    void run();
    
  private:
  	int _mode;
    void _clearScreen();
    void _displaySplashScreen();
    void _displayMainScreen();

};

#endif
