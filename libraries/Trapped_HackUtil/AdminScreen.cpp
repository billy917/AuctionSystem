/*
  AdminScreen.cpp - Library for Admin screen
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#include "Arduino.h"
#include "HackUtil.h"
#include "Constants.h"
#include "MenuState.h"
#include "EEPROM.h"
#include "SPI.h"
#include "GD2.h"
#include "XBee.h"


void HackUtil::_displayLockScreen(){
    GD.Begin (RECTS);

    (_inWallLock)? GD.ColorRGB (0xff00ff) : GD.ColorRGB (0xff0000);
    GD.Tag (TAG_LOCK_INWALL);
    GD.Vertex2ii (0,0);
    GD.Vertex2ii (239,135);

    (_mainDoorLock)? GD.ColorRGB (0x00ffff) : GD.ColorRGB (0x00ff00);
    GD.Tag(TAG_LOCK_MAIN_DOOR);
    GD.Vertex2ii (240,0);
    GD.Vertex2ii (479,135);

    (_shelfLock)? GD.ColorRGB (0xffff00) : GD.ColorRGB (0x0000ff);
    GD.Tag(TAG_LOCK_SHELF);
    GD.Vertex2ii (0,136);
    GD.Vertex2ii (239, 271);

    Gd.ColorRGB (0xe4e4e4);
    GD.Tag(SCREEN_PREVIOUS);
    GD.Vertex2ii (240,136);
    GD.Vertex2ii (479,271);

    _drawText (119, 67, 24, 0xffffff, "In Wall");
    _drawText (359, 67, 24, 0xffffff, "Main Door");
    _drawText (119, 203, 24, 0xffffff, "Shelf");
    _drawText (359, 203, 24, 0xffffff, "Back");


}

void HackUtil::_manageLock (uint8_t tagLock){
    if (tagLock == TAG_LOCK_INWALL){
        if (_inWallLock){
            // disable in_wall_lock
            /*Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
            Wire.write (MESSAGETYPEID_LOCK);
            Wire.write (MESSAGETYPEID_LOCK_LOCKID_INWALL);
            Wire.write (MESSAGETYPEID_LOCK_UNLOCK);
            Wire.endTransmission();
            */

            _inWallLock = false;
        }

        if (!_inWallLock){
            // enable in_wall_lock
            /*Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
            Wire.write (MESSAGETYPEID_LOCK);
            Wire.write (MESSAGETYPEID_LOCK_LOCKID_INWALL);
            Wire.write (MESSAGETYPEID_LOCK_LOCK);
            Wire.endTransmission();
            */

            _inWallLock = true;
        }
        
    }

    if (tagLock == TAG_LOCK_MAIN_DOOR){
        if (_mainDoorLock){
            // disable main_door_lock
            /*
            Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
            Wire.write (MESSAGETYPEID_LOCK);
            Wire.write (MESSAGETYPEID_LOCK_LOCKID_MAINDOOR);
            Wire.write (MESSAGETYPEID_LOCK_UNLOCK);
            Wire.endTransmission();
            */

            _mainDoorLock = false;
        }

        if (!_mainDoorLock){
            // enable main_door_lock
            /*
            Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
            Wire.write (MESSAGETYPEID_LOCK);
            Wire.write (MESSAGETYPEID_LOCK_LOCKID_MAINDOOR);
            Wire.write (MESSAGETYPEID_LOCK_LOCK);
            Wire.endTransmission();
            */

            _mainDoorLock = true;
        }

    }

    if (tagLock == TAG_LOCK_SHELF){
        if (_shelfLock){
            // disable shelf_lock
            /*
            Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
            Wire.write (MESSAGETYPEID_LOCK);
            Wire.write (MESSAGETYPEID_LOCK_LOCKID_SHELF);
            Wire.write (MESSAGETYPEID_LOCK_UNLOCK);
            Wire.endTransmission();
            */

            _shelfLock = false;
        }

        if (!_shelfLock){
            // enable shelf_lock
            /*
            Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
            Wire.write (MESSAGETYPEID_LOCK);
            Wire.write (MESSAGETYPEID_LOCK_LOCKID_SHELF);
            Wire.write (MESSAGETYPEID_LOCK_LOCK);
            Wire.endTransmission();
            */

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
