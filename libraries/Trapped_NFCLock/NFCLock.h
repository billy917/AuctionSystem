#ifndef NFCLock_h
#define NFCLock_h

#include "Arduino.h"
#include "Constants.h"
#include "LiquidCrystal_I2C.h"
#include "Timer.h"
#include "Wire.h"

class NFCLock{

    public:
        NFCLock();
        uint8_t detectorNFCValue[5];
        uint8_t detectorNFCPosition[5];
        volatile bool hasReceivedMessage;
        void handleI2CMessage(uint8_t data[]);

        /* LCD variables */
        LiquidCrystal_I2C *lcd;
        char equation[5][9];
        char message[4][20];
        char pattern[5];
        char currentPatternName;
        char nextPatternName;
        uint8_t currentPatternIndex;

        /* LCD methods */
        void notifyPatternChanged();
        void changeEquation();
        void displayString(int col, int row, char message[]);
        void displayString(int col, int row, char message);
        void displayString(int col, int row, int message);
        void displayString(int col, int row, uint8_t message);
        void updateCounter();

        /* Countdown timer */
        Timer *t;
        int countdownEvent;

    private:
        uint8_t _messageID;
        uint8_t _managerID;
        uint8_t _detected;
        uint8_t _detectorID;
        uint8_t _nfcValue;

        int _counter;

        void _initLCD();
        void _populateLocalBuffer(uint8_t data[]);
        void _manageNFC();
        void _updatePatternLCD();
        void _updateCounterLCD();
        void _updateEquationLCD();
        void _updateNFCDetectorLCD(uint8_t NFCDetectorID);

        void _loadPatternData();
        void _loadEquationData();

        void _clearMessage();
        void _clearPattern();
        void _clearEquation();
        void _clearI2CBuffer();
        void _clearLCD();
};

#endif
