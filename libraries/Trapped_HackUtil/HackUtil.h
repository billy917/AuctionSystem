/*
  HackUtil.h - Library for HackUtil module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef HackUtil_h
#define HackUtil_h

#include "Arduino.h"
#include "MenuState.h"
#include <XBee.h>

#define QUAD_BORDER 8

class HackUtil
{

  public:
    HackUtil();
    void init();
    void run();
    
  private:
    
    /* XBee protocol */
    XBee _xbee;
    uint8_t _xbeePayload[4];
    ZBTxRequest _laser1Tx;
    ZBTxRequest _laser2Tx;

    int _mode;
    unsigned long int ms;
    void _clearScreen();
    void _displaySplashScreen();
    void _displayMainScreen();
    
    /* Ethernet jack pins */
    #define ETHERNET_PWR_1 4
    #define ETHERNET_READ_1 5
    #define ETHERNET_PWR_2 6
    #define ETHERNET_READ_2 7
    bool _isSystemDetected();

    #define ADMIN_USER 0
    #define NORMAL_USER 1
    uint8_t _user;

    uint8_t _menuState;
    void _displayUserScreen();
    void _handleTouchInput();
    void _handleDisplayScreen();
    void _displayPrevious();
    void _handlePreviousScreen();

    /* ADMIN_USER screen */
    bool _inWallLock;
    bool _mainDoorLock;
    bool _shelfLock;

    void _displayLockScreen();
    void _manageLock(uint8_t tagLock);

    void _displayLaserScreen();
    void _displaySensorScreen();
    void _displayLogScreen();

    /* NORMAL_USER screen */
    bool _isClockKeypadHacked;
    bool _isSafeKeypadHacked;

    void _displayHackSystem();
    void _displayHackFailed();
    void _displayServer();
    void _displayServerAccess();
    void _displayHackLaser();
    void _displayFile();
    void _displayFloorplan(uint8_t floorplan);

    /* Helper methods */
    void _delay (int time);

    void _drawQuadSplit (int colorQ1, int tagQ1,
	    int colorQ2, int tagQ2,
        int colorQ3, int tagQ3,
	    int colorQ4, int tagQ4);
   // void _drawRect (int color, int tag, int point1x, int point1y,
        //int point2x, int point2y);
    void _drawSplit (int colorQ1, int tagQ1, int colorQ2, int tagQ2);
    void _drawPoint (int x, int y, int size, int color, int tag);
    void _drawText (int x, int y, int size, int color, char message[]);

};

#endif
