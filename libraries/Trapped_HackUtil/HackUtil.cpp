/*
  HackUtil.cpp - Library for HackUtil module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#include "Arduino.h"
#include <Digits.h>
#include <HackUtil.h>
#include "MenuState.h"
#include <Constants.h>
#include <EEPROM.h>
#include <SPI.h>
#include <GD2.h>
#include "asset/trapped_logo_full.h"
#include "XBee.h"

HackUtil::HackUtil(){
  _mode = 1;

    /*
    _xbee = XBee();
    _xbeeResponse = XBeeResponse();
    _xbeeRx = ZBRxResponse();
    _xbeePayload[4] = { 0, 0, 0, 0 };
    _laser2Addr = XBeeAddress64(0x0013a200, 0x40c04ef1);
    _laser2Tx = ZBTxRequest(_laser2Addr, _xbeePayload, sizeof(_xbeePayload));
    */

  _inWallLock = true;
  _mainDoorLock = true;
  _shelfLock = true;
}

void HackUtil::init(){
  // display splash screen
  _displaySplashScreen();

  // connect to game 

}

void HackUtil::run(){
    // get input
    GD.get_inputs();

    // process input
    _handleTouchInput();

    // update menu
    _handleDisplayScreen();
    
}

void HackUtil::_clearScreen(){
  GD.begin(0);
  GD.ClearColorRGB(0x191919);
  GD.Clear();
}

void HackUtil::_displaySplashScreen(){
  //_clearScreen();
  GD.begin();
  LOAD_ASSETS();

  GD.ClearColorRGB(0x404042);
  GD.Clear();
  GD.Begin(BITMAPS);
  GD.Vertex2ii(0, 0, TRAPPED_LOGO_FULL_HANDLE);
  GD.swap();

  delay (3000);

  _menuState = SCREEN_USER;
  _user = ADMIN_USER;
}

/* Assuming GD.get_inputs() has already been called beforehand */
void HackUtil::_handleTouchInput(){
    switch (GD.inputs.tag){
        
        /* Admin -> Lock */
        case SCREEN_ADMIN_LOCK: _menuState = SCREEN_ADMIN_LOCK; break;
        
        /* Admin -> Lock -> In_Wall_Safe */
        case TAG_LOCK_INWALL: 
            _manageLock(TAG_LOCK_INWALL);
            _menuState = SCREEN_ADMIN_LOCK;
            break;
        /* Admin -> Lock -> Main_Door */
        case TAG_LOCK_MAIN_DOOR: 
            _manageLock(TAG_LOCK_MAIN_DOOR);
            _menuState = SCREEN_ADMIN_LOCK;
            break;
        /* Admin -> LOCK -> Shelf */
        case TAG_LOCK_SHELF: 
            _manageLock(TAG_LOCK_SHELF);
            _menuState = SCREEN_ADMIN_LOCK;
            break;

        /* Admin -> Laser */
        case SCREEN_ADMIN_LASER: _menuState = SCREEN_ADMIN_LASER; break;

        /* Admin -> Sensor */
        case SCREEN_ADMIN_SENSOR: _menuState = SCREEN_ADMIN_SENSOR; break:

        /* Admin -> Log */
        case SCREEN_ADMIN_LOG: _menuState = SCREEN_ADMIN_LOG; break;

        /* Previous Menu */
        case SCREEN_PREVIOUS: _handlePreviousScreen(); break;

        default: break;
        
    }
}

void HackUtil::_handleDisplayScreen(){
    switch (_menuState){
        case SCREEN_SPLASH: _displaySplashScreen(); break;
        case SCREEN_USER: _displayUserScreen(); break;

        case SCREEN_ADMIN_LOCK: _displayLockScreen(SCREEN_ADMIN_LOCK); break;
        //case TAG_LOCK_INWALL: _displayLockScreen(TAG_LOCK_INWALL); break;
        //case TAG_LOCK_MAIN_DOOR: _displayLockScreen(TAG_LOCK_MAIN_DOOR); break;
        //case TAG_LOCK_SHELF: _displayLockScreen(TAG_LOCK_SHELF); break;


        case SCREEN_ADMIN_LASER: _displayLaserScreen(); break;
        case SCREEN_ADMIN_SENSOR: _displaySensorScreen(); break;
        case SCREEN_ADMIN_LOG: _displayLogScreen(); break;

        default: break;
        
    }

    GD.swap();
}


/* Assuming _user has already been defined beforehand */
void HackUtil::_displayUserScreen(){
    if (_user == ADMIN_USER){
        _drawQuadSplit (0xff4b76, SCREEN_ADMIN_LOCK,
                        0xffaf4b, SCREEN_ADMIN_LASER,
                        0xf94bff, SCREEN_ADMIN_SENSOR,
                        0x6a4bff, SCREEN_ADMIN_LOG);
        //TODO: _drawQuadText
    }

    else if (_user == NORMAL_USER){
        //_drawQuadSplit (0xc0ff1b, 0xff881b, 0x1bff2d, 0x1c66ff);
        _drawSplit (0xc0ff1b, TAG_N1,
                    0x1c66ff, TAG_N2);
        //TODO: _drawQuadText
    }

    else{}

}

void HackUtil::_handlePreviousScreen(){
    if (_menuState == SCREEN_ADMIN_LOCK ||
        _menuState == SCREEN_ADMIN_LASER ||
        _menuState == SCREEN_ADMIN_SENSOR ||
        _menuState == SCEEN_ADMIN_LOG)
    {
        _menuState = SCREEN_ADMIN;
    }
    
}

void HackUtil::_drawQuadSplit (int colorQ1, int tagQ1, int colorQ2, int tagQ2,
int colorQ3, int tagQ3, int colorQ4, int tagQ4){
    GD.Begin (RECTS);

    /* Two opposite points make a rectangle */

    /* Q1: Top-Left */
    GD.ColorRGB (colorQ1);
    GD.Tag (tagQ1);
    GD.Vertex2ii (0,0);
    GD.Vertex2ii (239,135);

    /* Q2: Top-Right */
    GD.ColorRGB (colorQ2);
    GD.Tag (tagQ2);
    GD.Vertex2ii (240,0);
    GD.Vertex2ii (479,135);

    /* Q3: Bottom-Left */
    GD.ColorRGB (colorQ3);
    GD.Tag (tagQ3);
    GD.Vertex2ii (0,136);
    GD.Vertex2ii (239,271);

    /* Q4: Bottom-Right */
    GD.ColorRGB (colorQ4);
    GD.Tag (tagQ4);
    GD.Vertex2ii (240,136);
    GD.Vertex2ii (479,271);

}

void HackUtil::_drawSplit (int colorQ1, int tagQ1, int colorQ2, int tagQ2){
    /* Q1: Top */
    GD.ColorRGB (colorQ1);
    GD.Tag (tagQ1);
    GD.Vertex2ii (0,0);
    GD.Vertex2ii (479,135);

    /* Q2: Bottom */
    GD.ColorRGB (colorQ2);
    GD.Tag (tagQ2);
    GD.Vertex2ii (0,136);
    GD.Vertex2ii (479,271);

}

void HackUtil::_drawPoint (int x, int y, int size, int color, int tag){
    GD.PointSize = 16 * size;
    GD.ColorRGB (color);
    GD.Begin (POINTS);
    GD.Tag (tag);
    GD.Vertex2ii (x, y);

}

void HackUtil::_drawText (int x, int y, int size, int color, char message[]){
    GD.ColorRGB (color);
    GD.cmd_text (x, y, size, OPT_CENTER, message);
    
}
