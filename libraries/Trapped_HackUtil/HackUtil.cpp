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

/* Assuming _user has already been defined beforehand */
void HackUtil::_displayUserScreen(){
    if (_user == ADMIN_USER){
        _drawQuadSplit (0xff4b76, TAG_A1, 0xffaf4b, TAG_A2, 
            0xf94bff, TAG_A3, 0x6a4bff, TAG_A4);
        //TODO: _drawQuadText
    }

    else if (_user == NORMAL_USER){
        //_drawQuadSplit (0xc0ff1b, 0xff881b, 0x1bff2d, 0x1c66ff);
        _drawSplit (0xc0ff1b, TAG_N1, 0x1c66ff, TAG_N2);
        //TODO: _drawQuadText
    }

    else{}

}

/* Assuming GD.get_inputs() has already been called beforehand */
void HackUtil::_handleTouchInput(){
    switch (GD.inputs.tag){
        
        case tagA1: break;
        case tagA2: break;
        case tagA3: break:
        case tagA4: break;

        case tagN1: break;
        case tagN2: break;

        
        default: break;
        
    }
}

void HackUtil::_drawQuadSplit (int q1, int tagQ1, int q2, int tagQ2,
int q3, int tagQ3, int q4, int tagQ4){
    GD.Begin (RECTS);

    /* Two opposite points make a rectangle */

    /* Q1: Top-Left */
    GD.ColorRGB (q1);
    GD.Tag (tagQ1);
    GD.Vertex2ii (0,0);
    GD.Vertex2ii (239,135);

    /* Q2: Top-Right */
    GD.ColorRGB (q2);
    GD.Tag (tagQ2);
    GD.Vertex2ii (240,0);
    GD.Vertex2ii (480,135);

    /* Q3: Bottom-Left */
    GD.ColorRGB (q3);
    GD.Tag (tagQ3);
    GD.Vertex2ii (0,136);
    GD.Vertex2ii (239,272);

    /* Q4: Bottom-Right */
    GD.ColorRGB (q4);
    GD.Tag (tagQ4);
    GD.Vertex2ii (240,136);
    GD.Vertex2ii (480,272);

    GD.swap();

}

void HackUtil::_drawSplit (int q1, int tagQ1, int q2, int tagQ2){
    /* Q1: Top */
    GD.ColorRGB (q1);
    GD.Tag (tagQ1);
    GD.Vertex2ii (0,0);
    GD.Vertex2ii (135, 480);

    /* Q2: Bottom */
    GD.ColorRGB (q2);
    GD.Tag (tagQ2);
    GD.Vertex2ii (136,0);
    GD.Vertex2ii (272,480);

}

void HackUtil::run(){

}
