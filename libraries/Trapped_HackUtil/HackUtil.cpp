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
#include <XBee.h>

HackUtil::HackUtil(){
  _mode = 1;

    _xbee = XBee();
    for (int i=0; i < 4; i++){
	_xbeePayload[i] = 0;
    }
    XBeeResponse _xbeeResponse = XBeeResponse();
    ZBRxResponse _xbeeRx = ZBRxResponse();
    XBeeAddress64 _laser1Addr = XBeeAddress64(0x0013a200, 0x40c04edf);
    XBeeAddress64 _laser2Addr = XBeeAddress64(0x0013a200, 0x40c04ef1);
    _laser1Tx = ZBTxRequest(_laser1Addr, _xbeePayload, sizeof(_xbeePayload));
    _laser2Tx = ZBTxRequest(_laser2Addr, _xbeePayload, sizeof(_xbeePayload));

    /* set up pins for ethernet */
    pinMode (ETHERNET_PWR_1, OUTPUT);
    digitalWrite (ETHERNET_PWR_1, HIGH);

    pinMode (ETHERNET_PWR_2, OUTPUT);
    digitalWrite (ETHERNET_PWR_2, HIGH);  _isClockKeypadHacked = false;

  _isSafeKeypadHacked = false;
  _inWallLock = true;
  _mainDoorLock = true;
  _shelfLock = true;
}

void HackUtil::init(){
  GD.begin();

  // display splash screen
  _menuState = SCREEN_SPLASH;
  //_displaySplashScreen();

  // connect to game 
  
  ms = 0;

}

void HackUtil::run(){
    if ((long)millis() >= (long)(ms + 100)){
        // get input
        GD.get_inputs();
   
	// process input
	_handleTouchInput();
	    
	// update menu
	_handleDisplayScreen();
	ms = (long)millis();
    }
}

void HackUtil::_clearScreen(){
  GD.begin(0);
  GD.ClearColorRGB(0x191919);
  GD.Clear();
}

void HackUtil::_displaySplashScreen(){
  LOAD_ASSETS();

  GD.ClearColorRGB(0x404042);
  GD.Clear();
  GD.Begin(BITMAPS);
  GD.Vertex2ii(0, 0, TRAPPED_LOGO_FULL_HANDLE);
  GD.swap();

  ms = (long)millis();
  while ((long)millis() <= (long)(ms + 3000)){
      // get input for admin mode
      _user = ADMIN_USER;
  }

  _menuState = SCREEN_USER;
  _user = NORMAL_USER;

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
        case SCREEN_ADMIN_SENSOR: _menuState = SCREEN_ADMIN_SENSOR; break;

        /* Admin -> Log */
        case SCREEN_ADMIN_LOG: _menuState = SCREEN_ADMIN_LOG; break;

        /* User -> Hack System */
        case SCREEN_USER_HACK_SYSTEM: _menuState = SCREEN_USER_HACK_SYSTEM; break;

        /* User -> Hack System -> Server */
        case SCREEN_USER_SERVER: _menuState = SCREEN_USER_SERVER; break;

        /* User -> Hack System -> Server -> Access */
        case SCREEN_USER_SERVER_ACCESS: _menuState = SCREEN_USER_SERVER_ACCESS; break;
        
        /* User -> Hack System -> Server -> Access -> Hack Laser */
        case SCREEN_USER_HACK_LASER: _menuState = SCREEN_USER_HACK_LASER; break;

        /* User -> Downloaded Files */
        case SCREEN_USER_FILE: _menuState = SCREEN_USER_FILE; break;

        /* Downloaded Files -> Floorplan */
        case SCREEN_USER_FLOORPLAN: _menuState = SCREEN_USER_FLOORPLAN; break;

        /* Previous Menu */
        case SCREEN_PREVIOUS: _handlePreviousScreen(); break;

    }

}

void HackUtil::_handleDisplayScreen(){
    GD.ClearColorRGB (0x404042);
    GD.Clear();

    /* clearing AND swapping screens MUST be in each method */
    switch (_menuState){
        case SCREEN_SPLASH: _displaySplashScreen(); break;
        case SCREEN_USER: _displayUserScreen(); break;

        case SCREEN_ADMIN_LOCK: _displayLockScreen(); break;
        case SCREEN_ADMIN_LASER: _displayLaserScreen(); break;
        case SCREEN_ADMIN_SENSOR: _displaySensorScreen(); break;
        case SCREEN_ADMIN_LOG: _displayLogScreen(); break;

        case SCREEN_USER_HACK_SYSTEM: _displayHackSystem(); break;
        case SCREEN_USER_SERVER: _displayServer(); break;
        case SCREEN_USER_SERVER_ACCESS: _displayServerAccess(); break;
        case SCREEN_USER_HACK_LASER: _displayHackLaser(); break;
        case SCREEN_USER_FILE: _displayFile(); break;
        case SCREEN_USER_FLOORPLAN: _displayFloorplan(); break;

    }

}

/* Assuming _user has already been defined beforehand */
void HackUtil::_displayUserScreen(){
    GD.ClearColorRGB (0x404042);
    GD.Clear();

    if (_user == ADMIN_USER){
        _drawQuadSplit (0xff4b76, SCREEN_ADMIN_LOCK,
                        0xffaf4b, SCREEN_ADMIN_LASER,
                        0xf94bff, SCREEN_ADMIN_SENSOR,
                        0x612a9f, SCREEN_ADMIN_LOG);

    GD.Tag (SCREEN_ADMIN_LOCK);
	_drawText (119, 67, 24, 0xffffff, "Lock");

    GD.Tag (SCREEN_ADMIN_LASER);
	_drawText (359, 67, 24, 0xffffff, "Laser");

    GD.Tag (SCREEN_ADMIN_SENSOR);
	_drawText (119, 203, 24, 0xffffff, "Sensor");

    GD.Tag (SCREEN_ADMIN_LOG);
	_drawText (359, 203, 24, 0xffffff, "Log");
    }

    if (_user == NORMAL_USER){
        if (!_isClockKeypadHacked){
            GD.Begin (RECTS);
            GD.ColorRGB (0xff0000);
            GD.Tag (SCREEN_USER_HACK_SYSTEM);
            GD.Vertex2ii (0 + 16, 0 + 16);
            GD.Vertex2ii (479 - (16 / 2), 271 - (16 / 2));

            GD.Tag (SCREEN_USER_HACK_SYSTEM);
            _drawText (239, 135, 24, 0xffffff, "Hack Security System");
        }else{
            GD.Begin (RECTS);
            GD.ColorRGB (0x99ffff);
            GD.Tag (SCREEN_USER_HACK_SYSTEM);
            GD.Vertex2ii (0 + QUAD_BORDER, 0 + QUAD_BORDER);
            GD.Vertex2ii (479 - (QUAD_BORDER / 2), 135 - (QUAD_BORDER / 2));

            GD.Tag (SCREEN_USER_HACK_SYSTEM);
            _drawText (239, 67, 24, 0xffffff, "Hack Security System");
            
            GD.Begin (RECTS);
            GD.ColorRGB (0x0000ff);
            GD.Tag (SCREEN_USER_FILE);
            GD.Vertex2ii (0 + QUAD_BORDER, 136 + QUAD_BORDER);
            GD.Vertex2ii (479 - (QUAD_BORDER / 2), 271 - (QUAD_BORDER / 2));

            GD.Tag (SCREEN_USER_FILE);
            _drawText (239, 203, 24, 0xffffff, "View Downloaded Files");
        }
    }

    GD.swap();

}

void HackUtil::_displayLockScreen(){
    GD.ClearColorRGB (0x404042);
    GD.Clear();

    /* in_wall_lock */
    GD.Begin (RECTS);
    (_inWallLock)? GD.ColorRGB (0xff0000) : GD.ColorRGB (0xff00ff);
    GD.Tag (TAG_LOCK_INWALL);
    GD.Vertex2ii (0 + QUAD_BORDER, 0 + QUAD_BORDER);
    GD.Vertex2ii (239 - (QUAD_BORDER / 2), 135 - (QUAD_BORDER / 2));
    GD.Tag (TAG_LOCK_INWALL);
    if (_inWallLock){
	_drawText (119, 67, 24, 0xffffff, "In Wall (ON)");
    }else{
	_drawText (119, 67, 24, 0xffffff, "In Wall (OFF)");
    }

    /* main_door_lock */
    GD.Begin (RECTS);
    (_mainDoorLock)? GD.ColorRGB (0x00ff00) : GD.ColorRGB (0x00ffff);
    GD.Tag (TAG_LOCK_MAIN_DOOR);
    GD.Vertex2ii (240 + (QUAD_BORDER / 2), 0 + QUAD_BORDER);
    GD.Vertex2ii (479 - QUAD_BORDER, 135 - (QUAD_BORDER / 2));
    GD.Tag (TAG_LOCK_MAIN_DOOR);
    if (_mainDoorLock){
	_drawText (359, 67, 24, 0xffffff, "Main Door (ON)");
    }else{
	_drawText (359, 67, 24, 0xffffff, "Main Door (OFF)");
    }

    /* shelf_lock */
    GD.Begin (RECTS);
    (_shelfLock)? GD.ColorRGB (0x0000ff) : GD.ColorRGB (0xffff00);
    GD.Tag (TAG_LOCK_SHELF);
    GD.Vertex2ii (0 + QUAD_BORDER, 136 + (QUAD_BORDER / 2));
    GD.Vertex2ii (239 - (QUAD_BORDER / 2), 271 - QUAD_BORDER);
    GD.Tag (TAG_LOCK_SHELF);
    if (_shelfLock){
	_drawText (119, 203, 24, 0xffffff, "Shelf (ON)");
    }else{
	_drawText (119, 203, 24, 0xffffff, "Shelf (OFF)");
    }

    /* back */
    GD.Begin (RECTS);
    GD.ColorRGB (0xe4e4e4);
    GD.Tag (SCREEN_PREVIOUS);
    GD.Vertex2ii (240 + (QUAD_BORDER / 2), 136 + (QUAD_BORDER / 2));
    GD.Vertex2ii (479 - QUAD_BORDER, 271 - QUAD_BORDER);
    GD.Tag (SCREEN_PREVIOUS);
    _drawText (359, 203, 24, 0xffffff, "Back");

    GD.swap();

}

void HackUtil::_manageLock (uint8_t tagLock){
    if (tagLock == TAG_LOCK_INWALL){
        if (_inWallLock){
            // disable in_wall_lock
	    _xbeePayload[0] = MESSAGETYPEID_LOCK;
            _xbeePayload[1] = MESSAGETYPEID_LOCK_LOCKID_INWALL;
            _xbeePayload[2] = MESSAGETYPEID_LOCK_UNLOCK;
	    
            //_xbee.send (_laser1Tx);

            _inWallLock = false;
        }
	else {
            // enable in_wall_lock
            _xbeePayload[0] = MESSAGETYPEID_LOCK;
            _xbeePayload[1] = MESSAGETYPEID_LOCK_LOCKID_INWALL;
            _xbeePayload[2] = MESSAGETYPEID_LOCK_LOCK;
	    
            //_xbee.send (_laser1Tx);

            _inWallLock = true;
        }
        
    }

    if (tagLock == TAG_LOCK_MAIN_DOOR){
        if (_mainDoorLock){
            // disable main_door_lock
            _xbeePayload[0] = MESSAGETYPEID_LOCK;
            _xbeePayload[1] = MESSAGETYPEID_LOCK_LOCKID_MAINDOOR;
            _xbeePayload[2] = MESSAGETYPEID_LOCK_UNLOCK;
	    
            //_xbee.send (_laser1Tx);

            _mainDoorLock = false;
        }
	else {
            // enable main_door_lock
            _xbeePayload[0] = MESSAGETYPEID_LOCK;
            _xbeePayload[1] = MESSAGETYPEID_LOCK_LOCKID_MAINDOOR;
            _xbeePayload[2] = MESSAGETYPEID_LOCK_LOCK;
	    
            //_xbee.send (_laser1Tx);

            _mainDoorLock = true;
        }

    }

    if (tagLock == TAG_LOCK_SHELF){
        if (_shelfLock){
            // disable shelf_lock
            _xbeePayload[0] = MESSAGETYPEID_LOCK;
            _xbeePayload[1] = MESSAGETYPEID_LOCK_LOCKID_SHELF;
            _xbeePayload[2] = MESSAGETYPEID_LOCK_UNLOCK;
	    
            //_xbee.send (_laser1Tx);

            _shelfLock = false;
        }
	else {
            // enable shelf_lock
            _xbeePayload[0] = MESSAGETYPEID_LOCK;
            _xbeePayload[1] = MESSAGETYPEID_LOCK_LOCKID_SHELF;
            _xbeePayload[2] = MESSAGETYPEID_LOCK_LOCK;
	    
            //_xbee.send (_laser1Tx);

            _shelfLock = true;
        }
        
    }

}

void HackUtil::_displayLaserScreen(){
    
}

void HackUtil::_displaySensorScreen(){
    
}

void HackUtil::_displayLogScreen(){
    
}

void HackUtil::_displayHackSystem(){
    bool _isConnected = false;
    unsigned long int _connectTime = 0;

    /* for a total of 500ms, each 100ms does:
     *  increment loading bar,
     *  check connection to ethernet jack
     */
    _connectTime = (long) millis();
    for (int i=0; i < 6; i++){
        while ((long)millis() <= _connectTime + 60){}
        GD.ClearColorRGB (0x404042);
        GD.Clear();
    
        _drawText (239, 67, 24, 0xffffff, "Connect device to security system.");

        if (i == 0){
            _drawText (239, 203, 24, 0xffffff,
                "[=>                    ]");
        }

        if (i == 1){
            _drawText (239, 203, 24, 0xffffff,
                "[====>                 ]");
        }

        if (i == 2){
            _drawText (239, 203, 24, 0xffffff,
                "[========>             ]");
        }

        if (i == 3){
            _drawText (239, 203, 24, 0xffffff,
                "[================>     ]");
            _isConnected = _isSystemDetected();
        }

        if (i == 4){
            if (_isConnected){
                _drawText (239, 203, 24, 0xffffff,
                    "[====================>]");
            } else {
                _drawText (239, 203, 24, 0xffffff,
                "[==================>   ]");
            }
        }

        GD.swap();
        _connectTime = (long)millis();
    }

    if (_isConnected){
        _menuState = SCREEN_USER_SERVER;
        
    } else {
        GD.ClearColorRGB (0x404042);
        GD.Clear();

        //TODO: back button
        GD.Begin (RECTS);
        GD.Tag (SCREEN_USER);
        GD.Vertex2ii (0 + QUAD_BORDER, 0 + QUAD_BORDER);
        GD.Vertex2ii (479 - QUAD_BORDER, 271 - QUAD_BORDER);
        
        GD.Tag (SCREEN_USER);
        _drawText (239, 135, 36, 0xffffff, "Could not detect security system");

        GD.swap();

    }

}

bool _isSystemDetected(){
    /* read pins if there is current or not */
    if ((digitalRead (ETHERNET_READ_1) == HIGH) &&
            (digitalRead (ETHERNET_READ_2) == HIGH)){
        return true;
    } else {
        return false;
    }

}

void HackUtil::_displayPrevious(){}

void HackUtil::_handlePreviousScreen(){
    if (_menuState == SCREEN_ADMIN_LOCK ||
        _menuState == SCREEN_ADMIN_LASER ||
        _menuState == SCREEN_ADMIN_SENSOR ||
        _menuState == SCREEN_ADMIN_LOG ||
        
        _menuState == SCREEN_USER_HACK_SYSTEM ||
        _menuState == SCREEN_USER_FILE)
    {
        _menuState = SCREEN_USER;
    }
    
}

void HackUtil::_drawQuadSplit (int colorQ1, int tagQ1,
	int colorQ2, int tagQ2, 
	int colorQ3, int tagQ3,
	int colorQ4, int tagQ4){
    GD.ClearColorRGB(0x404042);
    GD.Clear();
    GD.Begin (RECTS);

    /* Two opposite points make a rectangle */

    /* Q1: Top-Left */
    GD.ColorRGB (colorQ1);
    GD.Tag (tagQ1);
    GD.Vertex2ii (0 + QUAD_BORDER, 0 + QUAD_BORDER);
    GD.Vertex2ii (239 - (QUAD_BORDER / 2), 135 - (QUAD_BORDER / 2));

    /* Q2: Top-Right */
    GD.ColorRGB (colorQ2);
    GD.Tag (tagQ2);
    GD.Vertex2ii (240 + (QUAD_BORDER / 2), 0 + QUAD_BORDER);
    GD.Vertex2ii (479 - QUAD_BORDER, 135 - (QUAD_BORDER / 2));

    /* Q3: Bottom-Left */
    GD.ColorRGB (colorQ3);
    GD.Tag (tagQ3);
    GD.Vertex2ii (0 + QUAD_BORDER, 136 + (QUAD_BORDER / 2));
    GD.Vertex2ii (239 - (QUAD_BORDER / 2), 271 - QUAD_BORDER);

    /* Q4: Bottom-Right */
    GD.ColorRGB (colorQ4);
    GD.Tag (tagQ4);
    GD.Vertex2ii (240 + (QUAD_BORDER / 2), 136 + (QUAD_BORDER / 2));
    GD.Vertex2ii (479 - QUAD_BORDER, 271 - QUAD_BORDER);

}

void HackUtil::_drawSplit (int colorQ1, int tagQ1, int colorQ2, int tagQ2){
    GD.Begin (RECTS);

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
    GD.PointSize (16 * size);
    GD.ColorRGB (color);
    GD.Begin (POINTS);
    GD.Tag (tag);
    GD.Vertex2ii (x, y);

}

void HackUtil::_drawText (int x, int y, int size, int color, char message[]){
    GD.ColorRGB (color);
    GD.cmd_text (x, y, size, OPT_CENTER, message);
    
}
