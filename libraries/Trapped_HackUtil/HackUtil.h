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
  /* Admin screen tags */
  #define TAG_A1 11
  #define TAG_A2 12
  #define TAG_A3 13
  #define TAG_A4 14

  /* Normal User screen tags */
  #define TAG_N1 21
  #define TAG_N2 22

  public:
    HackUtil();
    void init();
    void run();
    
  private:
  	int _mode;
    void _clearScreen();
    void _displaySplashScreen();
    void _displayMainScreen();

    #define ADMIN_USER 0
    #define NORMAL_USER 1
    uint8_t _user;

    void _displayUserScreen();
    void _handleTouchInput();

    /* Helper methods */
    void _drawQuadSplit (int q1, int tagQ1, int q2, int tagQ2,
        int q3, int tagQ3, int q4, int tagQ4);
    void _drawSplit (int q1, int tagQ1, int q2, int tagQ2);

};

#endif
