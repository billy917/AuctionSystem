#include "NFCLock.h"
#include "Arduino.h"
#include "Wire.h"
#include "Constants.h"
#include "LiquidCrystal_I2C.h"

NFCLock::NFCLock(){
    hasReceivedMessage = false;

    _messageID, _managerID, _detected, _detectorID, _nfcValue = 0;

    _clearDetectorNFCValue();
    _clearPattern();
    _clearEquation();

    _loadDesiredNFCValue();
    _loadPatternData();
    _loadEquationData();
    _loadEquationPositionData();
    _loadNFCPositionData();
    
}

void NFCLock::initLCD(){
    lcd = new LiquidCrystal_I2C (0x3F, 20, 4);
    lcd->init();
    lcd->backlight();
    lcd->noAutoscroll();
    lcd->noCursor();
    lcd->noBlink();

    currentPatternName = pattern[0];
    nextPatternIndex = 1;
    nextPatternName = pattern[nextPatternIndex];

    currentEquationIndex = 0;

    lcd->clear();

    displayPatternLCD();
    displayCounterLCD();
    displayEquationLCD();

} //end _initLCD

void NFCLock::handleI2CMessage (uint8_t data[]){
    _messageID = data[0];
    _managerID = data[1];
    _detected = data[2];
    _detectorID = data[3];
    _nfcValue = data[4];

    hasReceivedMessage = true;

    if (_messageID == MESSAGETYPEID_NFC_MANAGE){
        _manageNFC();

    } else {}

} //end handleI2CMessage

void NFCLock::_manageNFC(){

    uint8_t offsetPosition = _detectorID - (uint8_t)1;

    if (_detected == MESSAGETYPEID_NFC_MANAGE_FOUND){
        detectorNFCValue[offsetPosition] = _nfcValue;

    } else if (_detected == MESSAGETYPEID_NFC_MANAGE_NOTFOUND){
        detectorNFCValue[offsetPosition] = 0;

    }
        _updateNFCDetectorLCD (offsetPosition);

} //end _manageNFC

bool NFCLock::checkEquation(){
    bool check = true;
    for (int i=0; i < 5; i++){
        if (detectorNFCValue[i] ==
            desiredNFCValue[currentEquationIndex][i]){

            check &= true;
        } else {
            check &= false;
        }
    }

    return check;

} //end checkEquation

void NFCLock::displayAllLCD(){
    lcd->clear();

    displayPatternLCD();
    displayCounterLCD();
    displayEquationLCD();
    
}

void NFCLock::displayPatternLCD(){

    displayString (0,0, "Pattern:");
    displayString (9,0, currentPatternName);
    displayString (0,1, "Next Pattern:");
    displayString (14,1, nextPatternName);
}

void NFCLock::displayCounterLCD (){
    /* 
     Cannot just display _counter,
     when it gets down to single digit, the lcd will
     display it as the first position like '9_' instead of
     '09'
    */
    if (_counter < 10) {
        displayString (18,0, 0);
        displayString (19,0, _counter);

    }else {
        displayString (18,0, _counter); 
    }

    //Serial.println ("Debug: displayCounter()");
    //Serial.print ("Counter: ");
    //Serial.println (_counter);
}

void NFCLock::displayEquationLCD(){

    for (int i=0; i < 4; i++){
        int position = _equationPosition[i];
        char message = equation[currentEquationIndex][i];
        
        
        //Serial.println ("Debug: displayEquationLCD()");
        //Serial.print ("Equation Position: ");
        //Serial.print (position);
        //Serial.print (", Message: ");
        //Serial.println (message);
        

        displayString (position, 3, message);
    }

    for (int i=0; i < 5; i++) {
        _updateNFCDetectorLCD (i);
    }
}

void NFCLock::_updateNFCDetectorLCD (uint8_t NFCDetectorID){
    uint8_t _nfcValue = detectorNFCValue[NFCDetectorID];
    uint8_t _nfcPosition = _detectorNFCPosition[NFCDetectorID];
    
    //Serial.println ("Debug: _updateNFCDetectorLCD()");
    //Serial.print ("NFC_DetectorID Position: ");
    //Serial.print (_nfcPosition);
    //Serial.print (", NFC_Value: ");
    //Serial.println (_nfcValue);
    

    if (_nfcValue == 0) displayString (_nfcPosition, 3, '?');
    else displayString (_nfcPosition, 3, _nfcValue);

} //end _updateNFCDetectorLCD

void NFCLock::_clearDetectorNFCValue(){
    for (int i=0; i<5; i++) detectorNFCValue[i] = 0;
} //end _clearDetectorNFCValue

void NFCLock::_clearPattern(){
    for (int i=0; i<5; i++) pattern[i] = '-';
} //end _clearPattern

void NFCLock::_clearEquation(){
    for (int i=0; i<5; i++){
        for (int j=0; j < 4; j++){
            equation[i][j] = '-';
        }
    }
} //end _clearEquation

/* Pattern changed */
void NFCLock::notifyPatternChanged(){
    currentPatternName = nextPatternName;
    nextPatternIndex += 1;
    if (nextPatternIndex > 4) nextPatternIndex = 0;

    nextPatternName = pattern[nextPatternIndex];

    // Send I2CMessage to notify LaserDriver
    //Wire.beginTransmission ();

    //displayPatternLCD();

    displayAllLCD();

} //end patternChanged()

/* Request change equation */
void NFCLock::changeEquation(){
    currentEquationIndex += 1;
    if (currentEquationIndex > 4) currentEquationIndex = 0;

    //displayEquationLCD();
    displayAllLCD();

} //end changeEquation()

void NFCLock::setCounter (int counter){
    _counter = counter;
}

int NFCLock::getCounter(){
    return _counter;
}

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

void NFCLock::_loadDesiredNFCValue(){
    desiredNFCValue[0][0] = 1;
    desiredNFCValue[0][1] = 2;
    desiredNFCValue[0][2] = 3;
    desiredNFCValue[0][3] = 4;
    desiredNFCValue[0][4] = 5;

    desiredNFCValue[1][0] = 1;
    desiredNFCValue[1][1] = 2;
    desiredNFCValue[1][2] = 3;
    desiredNFCValue[1][3] = 4;
    desiredNFCValue[1][4] = 5;

    desiredNFCValue[2][0] = 1;
    desiredNFCValue[2][1] = 2;
    desiredNFCValue[2][2] = 3;
    desiredNFCValue[2][3] = 4;
    desiredNFCValue[2][4] = 5;

    desiredNFCValue[3][0] = 1;
    desiredNFCValue[3][1] = 2;
    desiredNFCValue[3][2] = 3;
    desiredNFCValue[3][3] = 4;
    desiredNFCValue[3][4] = 5;

    desiredNFCValue[4][0] = 1;
    desiredNFCValue[4][1] = 2;
    desiredNFCValue[4][2] = 3;
    desiredNFCValue[4][3] = 4;
    desiredNFCValue[4][4] = 5;

} //end desiredNFCValue()

void NFCLock::_loadPatternData(){
    pattern[0] = 'A';
    pattern[1] = 'B';
    pattern[2] = 'C';
    pattern[3] = 'D';
    pattern[4] = 'E';
} //end _loadPatternData()

void NFCLock::_loadEquationData(){
    equation[0][0] = '+';
    equation[0][1] = '-';
    equation[0][2] = 'x';
    equation[0][3] = '/';

    equation[1][0] = 'x';
    equation[1][1] = '+';
    equation[1][2] = '/';
    equation[1][3] = '-';

    equation[2][0] = '/';
    equation[2][1] = 'x';
    equation[2][2] = '+';
    equation[2][3] = '-';

    equation[3][0] = '-';
    equation[3][1] = '/';
    equation[3][2] = 'x';
    equation[3][3] = '+';

    equation[4][0] = 'x';
    equation[4][1] = '/';
    equation[4][2] = '-';
    equation[4][3] = '+';

} //end _loadEquationData()

void NFCLock::_loadNFCPositionData(){
    _detectorNFCPosition[0] = 1;
    _detectorNFCPosition[1] = 5;
    _detectorNFCPosition[2] = 9;
    _detectorNFCPosition[3] = 13;
    _detectorNFCPosition[4] = 17;
} //end _loadLCDPositionData()

void NFCLock::_loadEquationPositionData(){
    _equationPosition[0] = 3;
    _equationPosition[1] = 7;
    _equationPosition[2] = 11;
    _equationPosition[3] = 15;
} //end _loadEquationPositionData()
