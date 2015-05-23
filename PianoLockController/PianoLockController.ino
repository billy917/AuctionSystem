/*
  PianoLockController.ino
  Includes mock piano keys, and a buzzer to control the state of a
  lock
*/

#include "Arduino.h"
#include "Constants.h"
#include "Wire.h"
#include "pitches.h"
#include "LiquidCrystal_I2C.h"

volatile bool receivedI2CMessage = false;
volatile uint8_t i2cDataBuffer[NFC_MESSAGE_MAX_SIZE];

#define LOCK_PIN 2
#define LOCKED true
#define UNLOCKED false
volatile bool lockState = LOCKED;

#define SPEAKER_PIN  12
#define ERROR_KEY -9
int keySounds[7] = {NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5};

/* Button sound pins are from D3 - D12 */
#define KEY_PIN_OFFSET 3
#define NUMBER_KEYS 7

uint8_t keyPin[NUMBER_KEYS];
int keyState[NUMBER_KEYS];
int lastKeyState[NUMBER_KEYS];

#define NUM_PASSWORD 5
#define MAX_PASSWORD_KEYS 8
#define TRIES 3
int currentPasswordIndex;
int passwordSound[NUM_PASSWORD][MAX_PASSWORD_KEYS];
int passwordMaxKeys[NUM_PASSWORD];
int inputSound[MAX_PASSWORD_KEYS];
int fail = 0;

LiquidCrystal_I2C lcd = LiquidCrystal_I2C (0x3F, 20, 4);
#define DEBOUNCE_TIME 700
volatile unsigned long debounceTime[NUMBER_KEYS];

void setup(){
    Serial.begin(9600);
    
    /* Setup Wire I2C */
    Serial.println (F("Initialize Wire setup..."));
    Wire.begin (PIANO_LOCK_I2C_ADDR);
    Wire.onReceive (receivedEvent);
    Wire.onRequest (requestEvent);
    Serial.println (F("---- Wire setup complete"));

    Serial.println (F("Initialize LCD setup..."));
    lcd.init();
    lcd.backlight();
    lcd.noCursor();
    lcd.clear();
    showInput();
    Serial.println (F("---- LCD setup complete"));

    Serial.println (F("Initialize pins setup..."));
    /* Setup lock pin for usage */
    lock();
    Serial.println (F("---- Done LOCK_PIN setup..."));

    /* Setup speaker pin for usage */
    pinMode (SPEAKER_PIN, OUTPUT);
    digitalWrite (SPEAKER_PIN, LOW);
    Serial.println (F("---- Done SPEAKER_PIN setup..."));

    /* Initialize key pin and key states */
    for (int i=0; i < NUMBER_KEYS; i++){
        keyPin[i] = i + KEY_PIN_OFFSET;
        keyState[i] = 1;
        lastKeyState[i] = 1;
        pinMode (keyPin[i], INPUT_PULLUP);
        debounceTime[i] = 0;
    }
    Serial.println (F("---- Pins setup complete"));
    
    /* Initialize password manager */
    currentPasswordIndex = 0;
    loadPasswordSound();
    initInputSound();

    delay(250);

    showInput();

    Serial.println (F("Ready"));
}

void receivedEvent (int numberByte){
    for (int i=0; i<NFC_MESSAGE_MAX_SIZE; i++){
        i2cDataBuffer[i] = Wire.read();
    }

    receivedI2CMessage = true;
}

void requestEvent(){}

void loop(){
    readKeyState();

    /* Use below only if handleKey() is not in readKeyState() */
    //handleKey();
    
    if (inputSound[0] != ERROR_KEY){
        checkPassword();
    }

    /* Every x time, refresh button pin modes
        in attempt to prevent hanging

        x = 900000 millisecond = 15 min
        x = 600000 millisecond = 10 min
        x = 300000 millisecond = 5 min
    */
    if ((millis() % 600000) == 0) {
        for (int i=0; i < NUMBER_KEYS; i++){
            long int tmpTime = millis();
                pinMode (keyPin[i], OUTPUT);
            while (millis() - tmpTime < 110){}
                pinMode (keyPin[i], INPUT_PULLUP);
            tmpTime = millis();
                digitalWrite (keyPin[i], LOW);
            while (millis() - tmpTime < 110){}
                digitalWrite (keyPin[i], HIGH);
        }
    }
    
    /* Every 30 seconds, change song */
    if ((millis()) % 30000 == 0){
        nextPassword();
    }
    
}

void readKeyState(){
    //Serial.println (F("Reading key states..."));
    //Serial.print (F("Key states: "));
    
    for(int i=0; i < NUMBER_KEYS; i++){

        keyState[i] = digitalRead (i + KEY_PIN_OFFSET);
        handleKey(i);
        
        //Serial.print (keyState[i]);
        //Serial.print (F(" "));
        
    }

    //Serial.println (F("."));

}

void handleKey (int _key){
    //Serial.println (F("Handling key states..."));

    /* Use the below for loop when handling outside readKeyState */
    //for (int i=0; i < NUMBER_KEYS; i++){

        Serial.print (F("micros(): "));
        Serial.println (micros());

        if ((long)(micros()-debounceTime[_key]) >= (DEBOUNCE_TIME*1000)){
           
            if (isKeyStateChanged (_key)){
               
               /*
               Serial.print (F("Key pressed: "));
               Serial.println (_key);
               Serial.print (F("Debounce time: "));
               Serial.println (debounceTime[_key]);
               Serial.print (F("Last key state: "));
               Serial.print (lastKeyState[_key]);
               Serial.print (F(", Key state: "));
               Serial.println (keyState[_key]);
               */

                /* Record pressed button */
                recordKey(_key);

                /* Display inputted sequence on LCD */
                showInput();

                /* Make the corresponding sound */
                pressedKeySound (_key);

                /* Get new debounce time */
                debounceTime[_key] = (long)micros();
            }
        }

        /* Update key state */
        lastKeyState[_key] = keyState[_key];
    //}

}

/* Key SoundFX */
void pressedKeySound (int numberPressed){
    tone (SPEAKER_PIN, keySounds[numberPressed], 250);
}

bool isKeyStateChanged (int _key){
    
    /*
        Serial.print (F("Pin: "));
        Serial.print (keyPin[_key]);
        Serial.print (F(" state changed from "));
        Serial.print (lastKeyState[_key]);
        Serial.print (F(" to "));
        Serial.println (keyState[_key]);
    */

    if ((keyState[_key] == HIGH) && (lastKeyState[_key] == LOW) ||
        (keyState[_key] == LOW) && (lastKeyState[_key] == LOW) ||
        (keyState[_key] == HIGH) && (lastKeyState[_key] == HIGH)
       ){
        return false;
    }
    
    /* Count only one event, NOT both onPress and onRelease event */
    if ((keyState[_key] == LOW) && (lastKeyState[_key] == HIGH)){
        return true;

    }

    return false;
    
}

void initInputSound(){
    for(int i=0; i < MAX_PASSWORD_KEYS; i++){
        inputSound[i] = ERROR_KEY;
    }

}

void recordKey(int _key){
    
    for (int i=0; i < (passwordMaxKeys[currentPasswordIndex]- 1); i++){
      inputSound[i] = inputSound[i+1];
    }

    inputSound[passwordMaxKeys[currentPasswordIndex]- 1] = _key;

    
}

void showInput(){
    
    /*
    Serial.print (F("Input: "));
    for (int i=0; i < MAX_PASSWORD_KEYS; i++){
        Serial.print (inputSound[i]);
    }
    Serial.println (F("."));
    */

    lcd.clear();

    /* Display song name */
    switch (currentPasswordIndex){
        case 0: displaySong1(); break;
        case 1: displaySong2(); break;
        case 2: displaySong3(); break;
        case 3: displaySong4(); break;
        case 4: displaySong5(); break;
        default: break;
    }

    /* Display fail times */
    lcd.setCursor(19,0);
    lcd.print (3 - fail);

    /* Display input */
    for (int i=0; i<passwordMaxKeys[currentPasswordIndex]; i++){
        lcd.setCursor ((i+1)*2, 3);

        if (inputSound[i] == ERROR_KEY) {
            lcd.print (F("-"));
        } else {
            switch (inputSound[i]){
                case 0: lcd.print (F("F")); break;
                case 1: lcd.print (F("G")); break;
                case 2: lcd.print (F("A")); break;
                case 3: lcd.print (F("B")); break;
                case 4: lcd.print (F("C")); break;
                case 5: lcd.print (F("D")); break;
                case 6: lcd.print (F("E")); break;
                default: break;
            }
        }
    }

    lcd.noCursor();

}

void checkPassword(){
    
    if (matchPassword()){
        fail = 0;

        lcd.clear();
        lcd.setCursor(6,1);
        lcd.print (F("Unlocked!"));
        lcd.setCursor(1,2);
        lcd.print (F("Exit through doors"));

        unlock();

        while(true){}

    } else {
        fail += 1;

        if (fail >= TRIES){
            /* Reset fail counter */
            fail = 0;

            /* Change next song */
            nextPassword();

            /* Keep lock locked */
            lock();

        }
        
        /* Reset input */
        initInputSound();

        /* Display updates on LCD */
        showInput();

        /*
        Serial.print (TRIES - fail);
        Serial.println (F(" time(s) left!"));
        */

    }
}

bool matchPassword(){
    
    bool equal = true;
    for (int i=0; i < passwordMaxKeys[currentPasswordIndex]; i++){
        if (inputSound[i] == ERROR_KEY) return false;

        if (inputSound[i] == passwordSound[currentPasswordIndex][i]){
            equal &= true;
        } else {
            equal &= false;
        }
    }

    return equal;
}

void nextPassword(){
    currentPasswordIndex = currentPasswordIndex + 1;

    if (currentPasswordIndex >= NUM_PASSWORD){
        currentPasswordIndex = 0;
    }
}

void lock(){
    if (lockState != LOCKED){
        Serial.println (F("Locked..."));
        pinMode (LOCK_PIN, INPUT);
        digitalWrite (LOCK_PIN, HIGH);

        lockState = LOCKED;
    }
}

void unlock(){
    if (lockState != UNLOCKED){
        Serial.println (F("Unlocked..."));
        pinMode (LOCK_PIN, OUTPUT);
        digitalWrite (LOCK_PIN, HIGH);

        lockState = UNLOCKED;
    }
}

void displaySong1(){
    lcd.setCursor(0,0);
    lcd.print (F("Minuet"));

    lcd.setCursor(5,1);
    lcd.print (F("in G Major"));
}

void displaySong2(){
    lcd.setCursor(0,0);
    lcd.print (F("Beethoven's"));

    lcd.setCursor(5,1);
    lcd.print (F("Fifth"));
}

void displaySong3(){
    lcd.setCursor(0,0);
    lcd.print (F("Vivaldi"));

    lcd.setCursor(5,1);
    lcd.print (F("Four Seasons"));
}

void displaySong4(){
    lcd.setCursor(0,0);
    lcd.print (F("Tchaikovsky"));
    
    lcd.setCursor(0,1);
    lcd.print (F("Waltz of the Flowers"));

}

void displaySong5(){
    lcd.setCursor(0,0);
    lcd.print (F("Pachelbel's"));
    
    lcd.setCursor(5,1);
    lcd.print (F("Canon in D"));
}

void loadPasswordSound(){
    /* Minuet in G Major */
    passwordMaxKeys[0] = 8;
    passwordSound[0][0] = 5; //D,G,A,B,C,D,G,G
    passwordSound[0][1] = 1;
    passwordSound[0][2] = 2;
    passwordSound[0][3] = 3;
    passwordSound[0][4] = 4;
    passwordSound[0][5] = 5;
    passwordSound[0][6] = 1;
    passwordSound[0][7] = 1;

    /* Beethoven's Fifth */
    passwordMaxKeys[1] = 8;
    passwordSound[1][0] = 6; //E,E,E,C,D,D,D,B
    passwordSound[1][1] = 6;
    passwordSound[1][2] = 6;
    passwordSound[1][3] = 4;
    passwordSound[1][4] = 5;
    passwordSound[1][5] = 5;
    passwordSound[1][6] = 5;
    passwordSound[1][7] = 3;

    /* Vivaldi Four Seasons */
    passwordMaxKeys[2] = 7;
    passwordSound[2][0] = 5; //D,C,B,C,D,E,D
    passwordSound[2][1] = 4;
    passwordSound[2][2] = 3;
    passwordSound[2][3] = 4;
    passwordSound[2][4] = 5;
    passwordSound[2][5] = 6;
    passwordSound[2][6] = 5;
    passwordSound[2][7] = 0;

    /* Tchaikovsky Waltz of the Flowers */
    passwordMaxKeys[3] = 6;
    passwordSound[3][0] = 1; //G,B,D,E,D,D
    passwordSound[3][1] = 3;
    passwordSound[3][2] = 5;
    passwordSound[3][3] = 6;
    passwordSound[3][4] = 5;
    passwordSound[3][5] = 5;
    passwordSound[3][6] = 0;
    passwordSound[3][7] = 0;
    
    /* Pachelbel's Canon in D */
    passwordMaxKeys[4] = 8;
    passwordSound[4][0] = 6; //E,D,C,B,A,G,A,B
    passwordSound[4][1] = 5;
    passwordSound[4][2] = 4;
    passwordSound[4][3] = 3;
    passwordSound[4][4] = 2;
    passwordSound[4][5] = 1;
    passwordSound[4][6] = 2;
    passwordSound[4][7] = 3;
}
