#include "NFCLock.h"
#include "Arduino.h"
#include "Wire.h"
#include "Constants.h"
#include "LiquidCrystal_I2C.h"
#include "Timer.h"

NFCLock::NFCLock(){
    hasReceivedMessage = false;
    for (int i=0; i<5; i++) detectorNFCValue[i] = 0;

    detectorNFCPosition[0] = 1;
    detectorNFCPosition[1] = 5;
    detectorNFCPosition[2] = 9;
    detectorNFCPosition[3] = 13;
    detectorNFCPosition[4] = 17;

    _messageID, _managerID, _detected, _detectorID, _nfcValue = 0;

    lcd = new LiquidCrystal_I2C (0x3F, 20, 4);
    t = new Timer();

    _counter = 30;

    _clearMessage();
    _clearPattern();
    _clearEquation();

    _initLCD();
}

void NFCLock::_initLCD(){
    lcd->init();
    lcd->backlight();

    currentPatternIndex = 0;
    currentPatternName = pattern [currentPatternIndex];
    nextPatternName = pattern [((currentPatternIndex + 1) % 5) -1];

    _updatePatternLCD();
    _updateCounterLCD();
    _updateEquationLCD();

    //countdownEvent = t->every(1000, updateCounter);

} //end _initLCD

void NFCLock::_populateLocalBuffer(uint8_t data[]){
    _messageID = data[0];
    _managerID = data[1];
    _detected = data[2];
    _detectorID = data[3];
    _nfcValue = data[4];

} //end _populateLocalBuffer

void NFCLock::handleI2CMessage (uint8_t data[]){
    _populateLocalBuffer (data);

    hasReceivedMessage = true;

    if (_messageID == MESSAGETYPEID_NFC_MANAGE){
        _manageNFC();

    } else {}

} //end handleI2CMessage

void NFCLock::_manageNFC(){
    if (_managerID == MESSAGETYPEID_NFC_MANAGE_FOUND){
        detectorNFCValue[_detectorID] = _nfcValue;

    } else if (_managerID == MESSAGETYPEID_NFC_MANAGE_NOTFOUND){
        detectorNFCValue[_detectorID] = 0;

    }

        _updateNFCDetectorLCD (_detectorID);

} //end _manageNFC

void NFCLock::_updatePatternLCD(){

    displayString (0,0, "Pattern:");
    displayString (9,0, "A"); //currentPatternName
    displayString (0,1, "Next Pattern:");
    displayString (14,1, "C"); //nextPatternName
}

void NFCLock::_updateCounterLCD(){
    displayString (18,0, _counter);
}

void NFCLock::_updateEquationLCD(){
    _updateNFCDetectorLCD (0);
    displayString (3,3, "+");
    _updateNFCDetectorLCD (1);
    displayString (7,3, "-");
    _updateNFCDetectorLCD (2);
    displayString (11,3, "x");
    _updateNFCDetectorLCD (3);
    displayString (15,3, "/");
    _updateNFCDetectorLCD (4);
}

void NFCLock::_updateNFCDetectorLCD (uint8_t NFCDetectorID){
    uint8_t nfcValue = detectorNFCValue[NFCDetectorID];
    uint8_t nfcPosition = detectorNFCPosition[NFCDetectorID];

    if (nfcValue == 0) displayString (nfcPosition, 3, "?");
    else displayString (nfcPosition, 3, nfcValue);

}

void NFCLock::_clearPattern(){
    for (int i=0; i<5; i++){
        pattern[i] = '-';
    }
} //end _clearPattern

void NFCLock::_clearMessage(){
    for (int i=0; i<4; i++){
        for (int j=0; j < 20; j++){
            message[i][j] = '-';
        }
    }
} //end _clearMessage

void NFCLock::_clearEquation(){
    for (int i=0; i<5; i++){
        for (int j=0; j < 9; j++){
            equation[i][j] = '-';
        }
    }
} //end _clearEquation

/*
void NFCLock::_clearI2CBuffer(){
    for (int i=0; i < I2C_MESSAGE_MAX_SIZE; i++){
        i2cDataBuffer[i] = 0;
    }
}
*/

/* Pattern changed */
void NFCLock::notifyPatternChanged(){
    t->stop (countdownEvent);
    currentPatternName = nextPatternName;
    nextPatternName = pattern[++currentPatternIndex];

    _updatePatternLCD();

} //end patternChanged()

/* Request change equation */
void NFCLock::changeEquation(){


} //end changeEquation()

/* Display string to LCD */
void NFCLock::displayString (int col, int row, char message[]){

    lcd->setCursor (col, row);
    lcd->print (message);

} //end displayString()

void NFCLock::displayString (int col, int row, char message){

    lcd->setCursor (col, row);
    lcd->print (message);

} //end displayString()

void NFCLock::displayString (int col, int row, int message){

    lcd->setCursor (col, row);
    lcd->print (message);

} //end displayString()

void NFCLock::displayString (int col, int row, uint8_t message){

    lcd->setCursor (col, row);
    lcd->print (message);

} //end displayString()

void NFCLock::updateCounter(){
    /*
    _counter = _counter - 1;
    */
}

void NFCLock::_loadPatternData(){

}

void NFCLock::_loadEquationData(){

}
