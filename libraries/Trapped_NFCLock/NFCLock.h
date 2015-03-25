#ifndef NFCLock_h
#define NFCLock_h

#include "Arduino.h"
#include "Constants.h"
#include "LiquidCrystal_I2C.h"
#include "Wire.h"

class NFCLock{

    public:
        NFCLock();
        uint8_t detectorNFCValue[5];
        uint8_t desiredNFCValue[5][5];
        
        volatile bool hasReceivedMessage;
        void handleI2CMessage (uint8_t data[]);
        bool checkEquation();

        /* LCD variables */
        LiquidCrystal_I2C *lcd;
        char equation[5][4];
        char pattern[5];
        char currentPatternName;
        char nextPatternName;
        int nextPatternIndex;
        int currentEquationIndex;

        /* LCD methods */
        void initLCD();

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
        int _detectorNFCPosition[5];
        int _equationPosition[4];

        int _counter;

        void _updateNFCDetectorLCD (uint8_t NFCDetectorID);
        void _manageNFC();

        void _loadDesiredNFCValue();
        void _loadPatternData();
        void _loadEquationData();
        void _loadNFCPositionData();
        void _loadEquationPositionData();

        void _clearDetectorNFCValue();
        void _clearPattern();
        void _clearEquation();
        void _clearI2CBuffer();
        void _clearLCD();
};

#endif
