#include "NFCLock.h"
#include "Arduino.h"
#include "Wire.h"
#include "Constants.h"
#include "LiquidCrystal_I2C.h"

NFCLock::NFCLock(){

    _messageID, _managerID, _detected, _detectorID, _nfcValue = 0;

    _clearDetectorNFCValue();
    _clearPattern();
    _clearEquation();

    /* For testing only: */
    //_loadDesiredNFCValue();

    _loadPatternData();
    _loadEquationData();
    //_loadEquationPositionData();
    _loadNFCPositionData();

}

void NFCLock::initLCD(){
    lcd = new LiquidCrystal_I2C (0x3F, 20, 4);
    lcd->init();
    lcd->backlight();
    lcd->noCursor();

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

    if (_messageID == MESSAGETYPEID_NFC_MANAGE){
        _manageNFC();

    } else {}

} //end handleI2CMessage

void NFCLock::_manageNFC(){

    uint8_t offsetPosition = _detectorID - 1;

    if (_detected == MESSAGETYPEID_NFC_MANAGE_FOUND){
        detectorNFCValue[offsetPosition] = _nfcValue;

    } else if (_detected == MESSAGETYPEID_NFC_MANAGE_NOTFOUND){
        detectorNFCValue[offsetPosition] = 0;

    }
        _updateNFCDetectorLCD (offsetPosition);

} //end _manageNFC

bool NFCLock::checkEquation(){
    switch (currentEquationIndex){
        case 0: return (equationOne() == 
            desiredEquationValue[currentEquationIndex]);
            break;
        case 1: return (equationTwo() == 
            desiredEquationValue[currentEquationIndex]);
            break;
        case 2: return (equationThree() == 
            desiredEquationValue[currentEquationIndex]);
            break;

        default: return false; break;
    }


} //end checkEquation

void NFCLock::displayAllLCD(){
    lcd->clear();

    displayPatternLCD();
    displayCounterLCD();
    displayEquationLCD();
   
   lcd->noCursor();
}

void NFCLock::displayPatternLCD(){

    //Serial.println ("Debug: displayPatternLCD()");

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

    /*
    Serial.println ("Debug: displayCounter()");
    Serial.print ("Counter: ");
    Serial.println (_counter);
    */
}

void NFCLock::displayEquationLCD(){

    //Serial.println ("Debug: displayEquationLCD()");

    /* Display equation characters */
    //for (int i=0; i < NUM_EQUATION_POSITION; i++){
        //int position = _equationPosition[i];
        
        /*
        Serial.print ("Equation Position: ");
        Serial.print (position);
        Serial.print (", Message: ");
        Serial.println (message);
        */

        //displayString (position, 3, message);
        displayString (0,3, *equation[currentEquationIndex]);
    //}

    /* Display answer */
    int answerValue = desiredEquationValue[currentEquationIndex];
    displayString (answerPosition[currentEquationIndex], 3, answerValue);

    /*
    Serial.print ("Answer position: ");
    Serial.print (answerPosition);
    Serial.print (", Answer value: ");
    Serial.println (answerValue);
    */

    /* Display NFC values */
    for (int i=0; i < NUM_NFC_DETECTOR; i++) {
        _updateNFCDetectorLCD (i);
    }
}

void NFCLock::_updateNFCDetectorLCD (uint8_t NFCDetectorID){
    uint8_t _nfcValue = detectorNFCValue[NFCDetectorID];
    uint8_t _nfcPosition = _detectorNFCPosition[currentEquationIndex][NFCDetectorID];
    
    /*
    Serial.println ("Debug: _updateNFCDetectorLCD()");
    Serial.print ("NFC_DetectorID Position: ");
    Serial.print (_nfcPosition);
    Serial.print (", NFC_Value: ");
    Serial.println (_nfcValue);
    */

    if (_nfcValue == 0) displayString (_nfcPosition, 3, '?');
    else displayString (_nfcPosition, 3, _nfcValue);

} //end _updateNFCDetectorLCD

void NFCLock::_clearDetectorNFCValue(){
    for (int i=0; i<NUM_NFC_DETECTOR; i++) detectorNFCValue[i] = 0;
} //end _clearDetectorNFCValue

void NFCLock::_clearPattern(){
    for (int i=0; i<NUM_PATTERN; i++) pattern[i] = '-';
} //end _clearPattern

void NFCLock::_clearEquation(){
    for (int i=0; i<NUM_EQUATION; i++) *equation[i] = "";
} //end _clearEquation

/* Pattern changed */
void NFCLock::notifyPatternChanged(){
    currentPatternName = nextPatternName;
    nextPatternIndex = nextPatternIndex + 1;
    if (nextPatternIndex >= NUM_PATTERN) nextPatternIndex = 0;

    nextPatternName = pattern[nextPatternIndex];

    displayAllLCD();

} //end patternChanged()

/* Request change equation */
void NFCLock::changeEquation(){
    currentEquationIndex = currentEquationIndex + 1;
    if (currentEquationIndex >= NUM_EQUATION) currentEquationIndex = 0;

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
}

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
    /* For testing only: 
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
    */

} //end desiredNFCValue()

void NFCLock::_loadPatternData(){
    pattern[0] = 'A';
    pattern[1] = 'B';
    pattern[2] = 'C';
    pattern[3] = 'D';
    pattern[4] = 'E';
} //end _loadPatternData()

/*
void NFCLock::_loadEquationData_old(){
    desiredEquationValue [0] = 11;
    equation[0][0] = '/';
    equation[0][1] = '+';
    equation[0][2] = '-';
    equation[0][3] = '+';
    equation[0][4] = '=';

    desiredEquationValue[1] = 2;
    equation[1][0] = '/';
    equation[1][1] = '+';
    equation[1][2] = '/';
    equation[1][3] = '-';
    equation[1][4] = '=';

    desiredEquationValue[2] = 16;
    equation[2][0] = '/';
    equation[2][1] = 'x';
    equation[2][2] = '-';
    equation[2][3] = '+';
    equation[2][4] = '=';

    *
    equation[3][0] = '-';
    equation[3][1] = '/';
    equation[3][2] = 'x';
    equation[3][3] = '+';

    equation[4][0] = 'x';
    equation[4][1] = '/';
    equation[4][2] = '-';
    equation[4][3] = '+';
    *

} //end _loadEquationData()
*/

void NFCLock::_loadEquationData(){
    desiredEquationValue[0] = 11;
    desiredEquationValue[1] = 2;
    desiredEquationValue[2] = 16;

    answerPosition[0] = 14;
    answerPosition[1] = 16;
    answerPosition[2] = 14;

    *equation[0] = "( / )+ - +  =";
    *equation[1] = "( / )+( / )-  =";
    *equation[2] = "( / )x - +  =";

}

void NFCLock::_loadNFCPositionData(){
    /*
    _detectorNFCPosition[0] = 0;
    _detectorNFCPosition[1] = 2;
    _detectorNFCPosition[2] = 6;
    _detectorNFCPosition[3] = 10;
    _detectorNFCPosition[4] = 14;
    */

    _detectorNFCPosition[0][0] = 1;
    _detectorNFCPosition[0][1] = 3;
    _detectorNFCPosition[0][2] = 6;
    _detectorNFCPosition[0][3] = 8;
    _detectorNFCPosition[0][4] = 10;

    _detectorNFCPosition[1][0] = 1;
    _detectorNFCPosition[1][1] = 3;
    _detectorNFCPosition[1][2] = 7;
    _detectorNFCPosition[1][3] = 9;
    _detectorNFCPosition[1][4] = 12;

    _detectorNFCPosition[2][0] = 1;
    _detectorNFCPosition[2][1] = 3;
    _detectorNFCPosition[2][2] = 6;
    _detectorNFCPosition[2][3] = 8;
    _detectorNFCPosition[2][4] = 10;

} //end _loadLCDPositionData()

/*
void NFCLock::_loadEquationPositionData(){
    _equationPosition[0] = 1;
    _equationPosition[1] = 4;
    _equationPosition[2] = 8;
    _equationPosition[3] = 12;
    _equationPosition[4] = 16;
} //end _loadEquationPositionData()
*/

int NFCLock::equationOne(){
    return (
      (detectorNFCValue[0]/detectorNFCValue[1]) +
      detectorNFCValue[2] - 
      detectorNFCValue[3] +
      detectorNFCValue[4]
    );
}

int NFCLock::equationTwo(){
    return (
        (detectorNFCValue[0]/detectorNFCValue[1]) +
        (detectorNFCValue[2]/detectorNFCValue[3]) -
        detectorNFCValue[4]
    );
}

int NFCLock::equationThree(){
    return (
        (detectorNFCValue[0]/detectorNFCValue[1]) *
        detectorNFCValue[2] -
        detectorNFCValue[3] +
        detectorNFCValue[4]
    );
}
