/*
  HackUtil.h - Library for HackUtil module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef HackUtil_h
#define HackUtil_h

#include "Arduino.h"
#include "XBee.h"

class HackUtil
{
  /* Admin screen tags */
  #define TAG_A1 110 // enable/disable Locks
  #define TAG_A2 120 // enable/disable Lasers
  #define TAG_A3 130 // enable/disable Sensors
  #define TAG_A4 140 // ten recent received Messages

  /* Normal User screen tags */
  #define TAG_N1 210
  #define TAG_N2 220

  public:
    HackUtil();
    void init();
    void run();
    
  private:
    /*
    XBee _xbee;
    XBeeResponse _xbeeResponse;
    ZBRxResponse rx = ZBRxResponse();
    uint8_t xbeePayload[4];
    XBeeAddress64 laser2Addr;
    ZBTxRequest laser2Tx;
    */

    int _mode;
    void _clearScreen();
    void _displaySplashScreen();
    void _displayMainScreen();

    bool _inWallLock;
    bool _mainDoorLock;
    bool _shelfLock;

    #define ADMIN_USER 0
    #define NORMAL_USER 1
    uint8_t _user;

    uint8_t _menuState;
    void _displayUserScreen();
    void _handleTouchInput();
    void _handleDisplayScreen();
    void _handlePreviousScreen();

    /* ADMIN_USER screen */
    void _displayLockScreen();
    void _manageLock(uint8_t tagLock);

    void _displayLaserScreen();
    void _displaySensorScreen();
    void _displayLogScreen();

    /* NORMAL_USER screen */

    /* Helper methods */
    void _drawQuadSplit (int colorQ1, int tagQ1, int colorQ2, int tagQ2,
        int colorQ3, int tagQ3, int colorQ4, int tagQ4);
    void _drawSplit (int colorQ1, int tagQ1, int colorQ2, int tagQ2);
    void _drawPoint (int x, int y, int size, int color, int tag);
    void _drawText (int x, int y, int size, int color, char message[])

};

#endif
