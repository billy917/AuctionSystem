#ifndef NFCLock_h
#define NFCLock_h

#include "Arduino.h"
#include "Constants.h"
#include "LiquidCrystal_I2C.h"
#include "Wire.h"

#define MAX_COL 20
#define MAX_ROW 4
#define NUM_PATTERN 5
#define NUM_EQUATION 3
//#define NUM_EQUATION_POSITION 5
#define NUM_NFC_DETECTOR 5

class NFCLock{

    public:
        NFCLock();
        uint8_t detectorNFCValue[NUM_NFC_DETECTOR];
        uint8_t desiredNFCValue[NUM_PATTERN][NUM_NFC_DETECTOR];
        
        volatile bool hasReceivedMessage;
        void handleI2CMessage (uint8_t data[]);
        bool checkEquation();
        int equationOne(), equationTwo(), equationThree();

        /* LCD variables */
        LiquidCrystal_I2C *lcd;
        //char equation[NUM_EQUATION][NUM_EQUATION_POSITION];
        char *equation[NUM_EQUATION][MAX_COL];
        char pattern[NUM_PATTERN];
        char currentPatternName;
        char nextPatternName;
        int nextPatternIndex;
        int currentEquationIndex;
        int desiredEquationValue[NUM_EQUATION];
        int answerPosition[NUM_EQUATION];

        /* LCD methods */
        void initLCD();
        void displayAllLCD();

        void displayString (int col, int row, char message[]);
        void displayString (int col, int row, char message);
        void displayString (int col, int row, int message);
        void displayString (int col, int row, uint8_t message);
        void displayCounterLCD();
        void displayPatternLCD();
        void displayEquationLCD();

        void notifyPatternChanged();
        void changeEquation();
        
        /* Countdown timer counter */
        void setCounter (int counter);
        int getCounter();

    private:
        uint8_t _messageID;
        uint8_t _managerID;
        uint8_t _detected;
        uint8_t _detectorID;
        uint8_t _nfcValue;
        //int _detectorNFCPosition[NUM_NFC_DETECTOR];
        int _detectorNFCPosition[NUM_EQUATION][NUM_NFC_DETECTOR];
        //int _equationPosition[NUM_EQUATION_POSITION];

        int _counter;

        void _updateNFCDetectorLCD (uint8_t NFCDetectorID);
        void _manageNFC();

        void _loadDesiredNFCValue();
        void _loadPatternData();
        void _loadEquationData();
        void _loadNFCPositionData();
        //void _loadEquationPositionData();

        void _clearDetectorNFCValue();
        void _clearPattern();
        void _clearEquation();
        void _clearI2CBuffer();
};

#endif
