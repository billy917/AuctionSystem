/*
  GameController.cpp - Library for Game Controller module.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/

#include "Arduino.h"
#include <GameController.h>
#include "Constants.h"
#include <Wire.h>


GameController::GameController(){
	_gameState = GAME_STATE_STOPPED;
}

void GameController::start(){
	if(GAME_STATE_PAUSED == _gameState || GAME_STATE_RESTART == _gameState){
		// lock main door
		Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
	    Wire.write(MESSAGETYPEID_LOCK);
	    Wire.write(MESSAGETYPEID_LOCK_LOCKID_MAINDOOR);
	    Wire.write(1);
		Wire.endTransmission();

		delay(250);

		// start sensor	that are active	
		Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
	    Wire.write(MESSAGETYPEID_LASER_CONTROL);
	    Wire.write (MESSAGETYPEID_LASER_CONTROL_ON_ALL_ACTIVE);
		Wire.endTransmission();

		delay(1000);
		// start timer
		Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
		Wire.write(MESSAGETYPEID_CLOCK);
	    Wire.write (MESSAGETYPEID_CLOCK_START);
		Wire.endTransmission();

		_gameState = GAME_STATE_RUNNING;	
	}	
}

void GameController::pause(){
	if(GAME_STATE_RUNNING == _gameState){
		// pause timer
		Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
	    Wire.write(MESSAGETYPEID_CLOCK);
	    Wire.write (MESSAGETYPEID_CLOCK_PAUSE);
		Wire.endTransmission();		

		delay(250);
		
		// pause sensor for active laser
		Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
	    Wire.write(MESSAGETYPEID_LASER_CONTROL);
	    Wire.write (MESSAGETYPEID_LASER_CONTROL_OFF_ALL_ACTIVE);
		Wire.endTransmission();

		delay(250);

		// unlock main door
		Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
	    Wire.write(MESSAGETYPEID_LOCK);
	    Wire.write(MESSAGETYPEID_LOCK_LOCKID_MAINDOOR);
	    Wire.write(2);
		Wire.endTransmission();

		delay(250);

		//stop soundFx
		Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
        Wire.write (MESSAGETYPEID_CLOCK);
        Wire.write (MESSAGETYPEID_CLOCK_STOP_LAST_TRACK);
        Wire.endTransmission();

		_gameState = GAME_STATE_PAUSED;
	}
}

void GameController::restart(){
	if(GAME_STATE_RESTART == _gameState){ // turn off
		// turn off laser and sensor
		Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
	    Wire.write(MESSAGETYPEID_LASER_CONTROL);
	    Wire.write (MESSAGETYPEID_LASER_CONTROL_OFF_ALL);
		Wire.endTransmission();

		delay(250);

		Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
	    Wire.write(MESSAGETYPEID_CLOCK);
	    Wire.write (MESSAGETYPEID_CLOCK_STOP);
		Wire.endTransmission();

		_gameState = GAME_STATE_STOPPED;

	} else if (GAME_STATE_PAUSED == _gameState || GAME_STATE_STOPPED == _gameState){ // refresh game
		// restart & pause timer
		Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
	    Wire.write(MESSAGETYPEID_CLOCK);
	    Wire.write (MESSAGETYPEID_CLOCK_RESET);
		Wire.endTransmission();

		// enable all laser
		delay(500);
		Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
	    Wire.write(MESSAGETYPEID_LASER_CONTROL);
	    Wire.write (MESSAGETYPEID_LASER_CONTROL_ON_ALL_NO_SENSOR);
		Wire.endTransmission();

		_gameState = GAME_STATE_RESTART;
	}	
}

void GameController::ended(){
	// pause game
	_gameState = GAME_STATE_PAUSED;	

	//stop soundFx
	Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
    Wire.write (MESSAGETYPEID_CLOCK);
    Wire.write (MESSAGETYPEID_CLOCK_STOP_LAST_TRACK);
    Wire.endTransmission();
    
}

void GameController::addTime(){
	if(GAME_STATE_PAUSED == _gameState || GAME_STATE_RUNNING == _gameState){
		// send msg to timer to add timer
		Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
	    Wire.write(MESSAGETYPEID_CLOCK);
	    Wire.write(MESSAGETYPEID_CLOCK_MODIFY_ADD);
	    Wire.write(2);
		Wire.endTransmission();
	}
}

void GameController::overrideConfig(){
	Wire.beginTransmission (NFC_MANAGER_I2C_ADDR);
	Wire.write(MESSAGETYPEID_GAME);
	Wire.write(MESSAGETYPEID_GAME_INIT_LASER_CONFIG);
	Wire.write(4);
	Wire.write(0);
	Wire.endTransmission();
}