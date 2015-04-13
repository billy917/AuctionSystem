/*
  GameController.h - Library for Game Controller module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef GameController_h
#define GameController_h

#include "Arduino.h"

class GameController
{
  public:
    GameController(); 
    void handleMessage(uint8_t dataLength, uint8_t data[]);
    void pause();
    void start();
    void restart();
    void addTime();
    void overrideConfig();
    void ended();
  private:
    uint8_t _gameState;
    
};

#endif