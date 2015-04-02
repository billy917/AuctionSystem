/*
  PianoLockController.ino
  Includes mock piano keys, and a buzzer to control the state of a
  lock
*/

#include "Arduino.h"
#include "Constants.h"
#include "Wire.h"
#include "pitches.h"

volatile bool receivedI2CMessage = false;
volatile uint8_t i2cDataBuffer[NFC_MESSAGE_MAX_SIZE];

#define LOCK_PIN 2
#define LOCKED true
#define UNLOCKED false
volatile bool lockState = LOCKED;

#define SPEAKER_PIN  13
int keySounds[7] = {NOTE_A4, NOTE_B4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4};

/* Button sound pins are from D3 - D12 */
#define KEY_PIN_OFFSET 3
#define NUMBER_KEYS 7

uint8_t keyPin[NUMBER_KEYS];
int keyState[NUMBER_KEYS];
int lastKeyState[NUMBER_KEYS];

#define NUM_PASSWORD 5
#define MAX_PASSWORD_KEYS 8
int passwordSound[NUM_PASSWORD][MAX_PASSWORD_KEYS];
int inputSound[MAX_PASSWORD_KEYS];

void setup(){
    Serial.begin(9600);
    
    /* Setup Wire I2C */
    Serial.println ("Initialize Wire setup...");
    Wire.begin (PIANO_LOCK_I2C_ADDR);
    Wire.onReceive (receivedEvent);
    Wire.onRequest (requestEvent);
    Serial.println ("---- Wire setup complete");

    Serial.println ("Initialize pins setup...");
    /* Setup lock pin for usage */
    pinMode (LOCK_PIN, INPUT);
    digitalWrite (LOCK_PIN, HIGH);
    Serial.println ("---- Done LOCK_PIN setup...");

    /* Setup speaker pin for usage */
    pinMode (SPEAKER_PIN, OUTPUT);
    digitalWrite (SPEAKER_PIN, LOW);
    Serial.println ("---- Done SPEAKER_PIN setup...");

    /* Initialize key pin and key states */
    for (int i=0; i < NUMBER_KEYS; i++){
        keyPin[i] = i + KEY_PIN_OFFSET;
        keyState[i] = 0;
        lastKeyState[i] = 0;
        pinMode (keyPin[i], INPUT_PULLUP);
    }
    Serial.println ("---- Pins setup complete");
    
    /* Initialize password manager */
    loadPasswordSound();
    initInputSound();

    Serial.println ("Ready");
}

void loop(){

    if (receivedI2CMessage){
        
        receivedI2CMessage = false;
    }

    readKeyState();
    handleKey();

    lock();

    delay(3000);

    unlock();

    delay (3000);

}

void receivedEvent (int numberByte){
    for (int i=0; i<NFC_MESSAGE_MAX_SIZE; i++){
        i2cDataBuffer[i] = Wire.read();
    }

    receivedI2CMessage = true;
}

void requestEvent(){}

/* Key SoundFX */
void pressedKeySound (int numberPressed){
    tone (SPEAKER_PIN, keySounds[numberPressed], 250);
}

void readKeyState(){
    Serial.println ("Reading key states...");
    for(int i=0; i < NUMBER_KEYS; i++){
        keyState[i] = digitalRead (i + KEY_PIN_OFFSET);
    }
}

void handleKey(){
    Serial.println ("Handling key states...");
    for (int i=0; i < NUMBER_KEYS; i++){
        if (isKeyStateChanged (i)){
            
            Serial.println (" Button Pressed ");
            /* Record pressed button */
            recordKey(i);

            pressedKeySound (i);
            
            lastKeyState[i] = keyState[i];

            if (checkPassword){
                unlock();

            } else {
                lock();
            }
        }
    }

}

bool isKeyStateChanged(int _key){
    
    if (keyState[_key] != lastKeyState[_key]){
        Serial.print ("Key: ");
        Serial.print (keyPin[_key]);
        Serial.print (" state changed from ");
        Serial.print (lastKeyState[_key]);
        Serial.print (" to ");
        Serial.println (keyState[_key]);

        return true;

    } else {return false;}
}

void loadPasswordSound(){
    /* Minuet in G Major */
    passwordSound[0][0] = NOTE_D4;
    passwordSound[0][1] = NOTE_G4;
    passwordSound[0][2] = NOTE_A4;
    passwordSound[0][3] = NOTE_B4;
    passwordSound[0][4] = NOTE_C4;
    passwordSound[0][5] = NOTE_D4;
    passwordSound[0][6] = NOTE_G4;
    passwordSound[0][7] = NOTE_G4;

    /* Beethoven's Fifth */
    passwordSound[1][0] = NOTE_E4;
    passwordSound[1][1] = NOTE_E4;
    passwordSound[1][2] = NOTE_E4;
    passwordSound[1][3] = NOTE_C4;
    passwordSound[1][4] = NOTE_D4;
    passwordSound[1][5] = NOTE_D4;
    passwordSound[1][6] = NOTE_D4;
    passwordSound[1][7] = NOTE_B4;

    /* Vivaldi Four Seasons */
    passwordSound[2][0] = NOTE_D4;
    passwordSound[2][1] = NOTE_C4;
    passwordSound[2][2] = NOTE_B4;
    passwordSound[2][3] = NOTE_C4;
    passwordSound[2][4] = NOTE_D4;
    passwordSound[2][5] = NOTE_E4;
    passwordSound[2][6] = NOTE_D4;
    passwordSound[2][7] = 0;

    /* Tchaikovsky Waltz of the Flowers */
    passwordSound[3][0] = NOTE_G4;
    passwordSound[3][1] = NOTE_B4;
    passwordSound[3][2] = NOTE_D4;
    passwordSound[3][3] = NOTE_E4;
    passwordSound[3][4] = NOTE_D4;
    passwordSound[3][5] = NOTE_D4;
    passwordSound[3][6] = 0;
    passwordSound[3][7] = 0;
    
    /* Pachelbel's Canon in D */
    passwordSound[4][0] = NOTE_E4;
    passwordSound[4][1] = NOTE_D4;
    passwordSound[4][2] = NOTE_C4;
    passwordSound[4][3] = NOTE_B4;
    passwordSound[4][4] = NOTE_A4;
    passwordSound[4][5] = NOTE_G4;
    passwordSound[4][6] = NOTE_A4;
    passwordSound[4][7] = NOTE_B4;
}

void initInputSound(){
    for(int i=0; i < MAX_PASSWORD_KEYS; i++){
        inputSound[i] = 0;
    }
}

void recordKey(int _key){
    
    for (int i=0; i < (MAX_PASSWORD_KEYS - 1); i++){
      inputSound[i] = inputSound[i+1];
    }

    inputSound[MAX_PASSWORD_KEYS - 1] = _key;
}

bool checkPassword(){
    
    bool equal = true;
    for (int i=0; i < MAX_PASSWORD_KEYS; i++){
        if (inputSound[i] == passwordSound[i]){
            equal &= true;
        } else if (passwordSound[i] == 0){
            equal &= true;
        } else {
            equal &= false;
        }
    }

    return equal;
}

void lock(){
    if (lockState != LOCKED){
        Serial.println ("Locked...");
        pinMode (LOCK_PIN, INPUT);
        digitalWrite (LOCK_PIN, HIGH);

        lockState = LOCKED;
    }
}

void unlock(){
    if (lockState != UNLOCKED){
        Serial.println ("Unlocked...");
        pinMode (LOCK_PIN, OUTPUT);
        digitalWrite (LOCK_PIN, HIGH);

        lockState = UNLOCKED;
    }
}
