#include "LCDController.h"
#include "Arduino.h"
#include "Wire.h"
#include "Constants.h"
#include "LiquidCrystal_I2C.h"

LCDController::LCDController(){

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

void LCDController::initLCD(){
    lcd = new LiquidCrystal_I2C (0x3F, 20, 4);
    lcd->init();
    lcd->backlight();
    lcd->noCursor();

    currentPatternName = pattern[0];
    nextPatternIndex = 1;
    nextPatternName = pattern[nextPatternIndex];

    currentEquationIndex = 0;

} //end _initLCD

bool LCDController::canCheckEquation(){
    for (int i=0; i < NUM_NFC_DETECTOR; i++){
        if (detectorNFCValue[i] == 0){
            return false;
        }
    }
    return true;
}

bool LCDController::checkEquation(){
    if (canCheckEquation()){
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
    } else {
        return false;
    }

} //end checkEquation

void LCDController::clearLCD(){
    lcd->clear();
}

void LCDController::displayAllLCD(){
    lcd->clear();

    //displayPatternLCD();
    //displayCounterLCD();
    displayEquationLCD();
   
   lcd->noCursor();
}

void LCDController::displayPatternLCD(){

    //Serial.println ("Debug: displayPatternLCD()");

    displayString (0,0, "Laser Pattern:");
    displayString (15,0, currentPatternName);
    displayString (0,1, "Next Pattern:");
    displayString (14,1, nextPatternName);
}

void LCDController::displayCounterLCD (){
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

void LCDController::displayEquationLCD(){

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
        displayString (0,2, *equation[currentEquationIndex]);
    //}

    /* Display answer */
    int answerValue = desiredEquationValue[currentEquationIndex];
    displayString (answerPosition[currentEquationIndex], 2, answerValue);

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

void LCDController::_updateNFCDetectorLCD (uint8_t NFCDetectorID){
    uint8_t _nfcValue = detectorNFCValue[NFCDetectorID];
    uint8_t _nfcPosition = _detectorNFCPosition[currentEquationIndex][NFCDetectorID];
    char _nfcDetector = pattern[NFCDetectorID];
    /*
    Serial.println ("Debug: _updateNFCDetectorLCD()");
    Serial.print ("NFC_DetectorID Position: ");
    Serial.print (_nfcPosition);
    Serial.print (", NFC_Value: ");
    Serial.println (_nfcValue);
    */

    if (_nfcValue == 0) displayString (_nfcPosition, 2, _nfcDetector);
    else displayString (_nfcPosition, 2, _nfcValue);

} //end _updateNFCDetectorLCD

void LCDController::_clearDetectorNFCValue(){
    for (int i=0; i<NUM_NFC_DETECTOR; i++) detectorNFCValue[i] = 0;
} //end _clearDetectorNFCValue

void LCDController::_clearPattern(){
    for (int i=0; i<NUM_PATTERN; i++) pattern[i] = '-';
} //end _clearPattern

void LCDController::_clearEquation(){
    for (int i=0; i<NUM_EQUATION; i++) *equation[i] = "";
} //end _clearEquation

/* Pattern changed */
void LCDController::notifyPatternChanged(){
    currentPatternName = nextPatternName;
    nextPatternIndex = nextPatternIndex + 1;
    if (nextPatternIndex >= NUM_PATTERN) nextPatternIndex = 0;

    nextPatternName = pattern[nextPatternIndex];

} //end patternChanged()

/* Request change equation */
void LCDController::changeEquation(){
    currentEquationIndex = currentEquationIndex + 1;
    if (currentEquationIndex >= NUM_EQUATION) currentEquationIndex = 0;

} //end changeEquation()

void LCDController::setCounter (int counter){
    _counter = counter;
}

int LCDController::getCounter(){
    return _counter;
}

/* Display string to LCD */
void LCDController::displayString (int col, int row, char message[]){
    
    lcd->setCursor (col, row);
    lcd->print (message);
}

void LCDController::displayString (int col, int row, char message){

    lcd->setCursor (col, row);
    lcd->print (message);

} //end displayString()

void LCDController::displayString (int col, int row, int message){

    lcd->setCursor (col, row);
    lcd->print (message);

} //end displayString()

void LCDController::displayString (int col, int row, uint8_t message){

    lcd->setCursor (col, row);
    lcd->print (message);

} //end displayString()

void LCDController::_loadDesiredNFCValue(){
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

void LCDController::_loadPatternData(){
    pattern[0] = 'A';
    pattern[1] = 'B';
    pattern[2] = 'C';
    pattern[3] = 'D';
    pattern[4] = 'E';
} //end _loadPatternData()

/*
void LCDController::_loadEquationData_old(){
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

void LCDController::_loadEquationData(){
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

void LCDController::_loadNFCPositionData(){
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
void LCDController::_loadEquationPositionData(){
    _equationPosition[0] = 1;
    _equationPosition[1] = 4;
    _equationPosition[2] = 8;
    _equationPosition[3] = 12;
    _equationPosition[4] = 16;
} //end _loadEquationPositionData()
*/

int LCDController::equationOne(){
    if (detectorNFCValue[0]%detectorNFCValue[1] != 0){
        return -1;
    }

    return (
      float(detectorNFCValue[0]/detectorNFCValue[1]) +
      detectorNFCValue[2] - 
      detectorNFCValue[3] +
      detectorNFCValue[4]
    );
}

int LCDController::equationTwo(){
    if ((detectorNFCValue[0]%detectorNFCValue[1] != 0) ||
        (detectorNFCValue[2]%detectorNFCValue[3])){
        return -1;
    }

    return (
        float(detectorNFCValue[0]/detectorNFCValue[1]) +
        float(detectorNFCValue[2]/detectorNFCValue[3]) -
        detectorNFCValue[4]
    );
}

int LCDController::equationThree(){
    if (detectorNFCValue[0]%detectorNFCValue[1] != 0){
        return -1;
    }

    return (
        float(detectorNFCValue[0]/detectorNFCValue[1]) *
        detectorNFCValue[2] -
        detectorNFCValue[3] +
        detectorNFCValue[4]
    );
}
