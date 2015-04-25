/*
  HackUtil.cpp - Library for HackUtil module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#include "Arduino.h"
#include <Digits.h>
#include <HackUtil.h>
#include <Constants.h>
#include <EEPROM.h>
#include <SPI.h>
#include <GD2.h>
#include "asset/trapped_logo_full.h"

HackUtil::HackUtil(){
  _mode = 1;
}

void HackUtil::init(){
  // display splash screen
  _displaySplashScreen(); 

  // connect to game 

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
}

void HackUtil::run(){

}
