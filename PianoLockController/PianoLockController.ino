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

const uint8_t PIANO_LOCK_I2C_ADDR = 120;
volatile bool receivedI2CMessage = false;
volatile uint8_t i2cDataBuffer[NFC_MESSAGE_MAX_SIZE];

#define LOCK_PIN 2
#define LOCKED true
#define UNLOCKED false
volatile bool lockState = LOCKED;

#define SPEAKER_PIN  13
#define ERROR_KEY -9
int keySounds[7] = {NOTE_F3, NOTE_G3, NOTE_A4, NOTE_B4, NOTE_C4, NOTE_D4, NOTE_E4};

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
int inputSound[MAX_PASSWORD_KEYS];
int fail = 0;

LiquidCrystal_I2C lcd = LiquidCrystal_I2C (0x3F, 20, 4);
long int debounceTime[NUMBER_KEYS];

void setup(){
    Serial.begin(9600);
    
    /* Setup Wire I2C */
    Serial.println ("Initialize Wire setup...");
    Wire.begin (PIANO_LOCK_I2C_ADDR);
    Wire.onReceive (receivedEvent);
    Wire.onRequest (requestEvent);
    Serial.println ("---- Wire setup complete");

    Serial.println ("Initialize LCD setup...");
    lcd.init();
    lcd.backlight();
    lcd.noCursor();
    lcd.clear();
    Serial.print ("---- LCD setup complete");

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
        keyState[i] = 1;
        lastKeyState[i] = 1;
        pinMode (keyPin[i], INPUT_PULLUP);
        debounceTime[i] = 0;
    }
    Serial.println ("---- Pins setup complete");
    
    /* Initialize password manager */
    currentPasswordIndex = 0;
    loadPasswordSound();
    initInputSound();

    delay(250);

    showInput();

    Serial.println ("Ready");
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

    handleKey();
    
    if (inputSound[0] != ERROR_KEY){
        if (checkPassword()){
            fail = 0;
            unlock();

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

            Serial.print (TRIES - fail);
            Serial.println (" time(s) left!");

        }
    }
}

void readKeyState(){
    //Serial.println ("Reading key states...");
   //Serial.print ("Key states: ");
    
    for(int i=0; i < NUMBER_KEYS; i++){

        keyState[i] = digitalRead (i + KEY_PIN_OFFSET);

        //Serial.print (keyState[i]);
        //Serial.print (" ");
        
    }

    //Serial.println (".");

}

void handleKey(){
    //Serial.println ("Handling key states...");

    for (int i=0; i < NUMBER_KEYS; i++){
        if (debounceTime[i] + 125 <= millis()){
            if (isKeyStateChanged (i)){
               
               /* Get new debounce time */
               debounceTime[i] = millis();

                /* Record pressed button */
                recordKey(i);

                /* Display inputted sequence on LCD */
                showInput();

                /* Make the corresponding sound */
                pressedKeySound (i);

            }
        }
        
        /* Update key state */
        lastKeyState[i] = keyState[i];
    }

}

/* Key SoundFX */
void pressedKeySound (int numberPressed){
    tone (SPEAKER_PIN, keySounds[numberPressed], 250);
}

bool isKeyStateChanged (int _key){
    
    /* Count only on press event, NOT on release event */
    if ((keyState[_key] == LOW) && (lastKeyState[_key] == HIGH)){

        /*
        Serial.print ("Pin: ");
        Serial.print (keyPin[_key]);
        Serial.print (" state changed from ");
        Serial.print (lastKeyState[_key]);
        Serial.print (" to ");
        Serial.println (keyState[_key]);
        */

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
    
    for (int i=0; i < (MAX_PASSWORD_KEYS - 1); i++){
      inputSound[i] = inputSound[i+1];
    }

    inputSound[MAX_PASSWORD_KEYS - 1] = _key;

    Serial.print ("Key pressed: ");
    Serial.println (_key);
}

void showInput(){
    
    Serial.print ("Input: ");
    for (int i=0; i < MAX_PASSWORD_KEYS; i++){
        Serial.print (inputSound[i]);
    }
    Serial.println (".");

    lcd.clear();

    /* Display song name */
    switch (currentPasswordIndex){
        case 0: displaySong1(); break;
        case 1: displaySong2(); break;
        case 2: displaySong3(); break;
        case 3: displaySong4(); break;
        case 4: displaySong4(); break;
        default: break;
    }

    /* Display input */
    for (int i=0; i<MAX_PASSWORD_KEYS; i++){
        lcd.setCursor (2,i+1);

        if (inputSound[i] == ERROR_KEY) {
            lcd.print ('-');
        } else {
            lcd.print (inputSound[i]);
        }
    }

}

bool checkPassword(){
    
    bool equal = true;
    for (int i=0; i < MAX_PASSWORD_KEYS; i++){
        if (inputSound[i] == ERROR_KEY) return false;

        if (inputSound[i] == passwordSound[currentPasswordIndex][i]){
            equal &= true;
        }else if (passwordSound[currentPasswordIndex][i] == 0){
            //Serial.print ("passwordSound :");
            //Serial.println (passwordSound[currentPasswordIndex][i]);
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

void displaySong1(){
    lcd.setCursor(0,0);
    lcd.print ("Minuet");

    lcd.setCursor(6,1);
    lcd.print ("in G Major");
}

void displaySong2(){
    lcd.setCursor(0,0);
    lcd.print ("Beethoven's");

    lcd.setCursor(11,1);
    lcd.print ("Fifth");
}

void displaySong3(){
    lcd.setCursor(0,0);
    lcd.print ("Vivaldi");

    lcd.setCursor(7,1);
    lcd.print ("Four Seasons");
}

void displaySong4(){
    lcd.setCursor(0,0);
    lcd.print ("Tchaikovsky");
    
    lcd.setCursor(11,1);
    lcd.print ("Waltz of the Flowers");

}

void displaySong5(){
    lcd.setCursor(0,0);
    lcd.print ("Pachelbel's");
    
    lcd.setCursor(11,1);
    lcd.print ("Canon in D");
}

void loadPasswordSound(){
    /* Minuet in G Major */
    passwordSound[0][0] = 5; //D,G,A,B,C,D,G,G
    passwordSound[0][1] = 1;
    passwordSound[0][2] = 2;
    passwordSound[0][3] = 3;
    passwordSound[0][4] = 4;
    passwordSound[0][5] = 5;
    passwordSound[0][6] = 1;
    passwordSound[0][7] = 1;

    /* Beethoven's Fifth */
    passwordSound[1][0] = 6; //E,E,E,C,D,D,D,B
    passwordSound[1][1] = 6;
    passwordSound[1][2] = 6;
    passwordSound[1][3] = 4;
    passwordSound[1][4] = 5;
    passwordSound[1][5] = 5;
    passwordSound[1][6] = 5;
    passwordSound[1][7] = 3;

    /* Vivaldi Four Seasons */
    passwordSound[2][0] = 5; //D,C,B,C,D,E,D
    passwordSound[2][1] = 4;
    passwordSound[2][2] = 3;
    passwordSound[2][3] = 4;
    passwordSound[2][4] = 5;
    passwordSound[2][5] = 6;
    passwordSound[2][6] = 5;
    passwordSound[2][7] = 0;

    /* Tchaikovsky Waltz of the Flowers */
    passwordSound[3][0] = 1; //G,B,D,E,D,D
    passwordSound[3][1] = 3;
    passwordSound[3][2] = 5;
    passwordSound[3][3] = 6;
    passwordSound[3][4] = 5;
    passwordSound[3][5] = 5;
    passwordSound[3][6] = 0;
    passwordSound[3][7] = 0;
    
    /* Pachelbel's Canon in D */
    passwordSound[4][0] = 6; //E,D,C,B,A,G,A,B
    passwordSound[4][1] = 5;
    passwordSound[4][2] = 4;
    passwordSound[4][3] = 3;
    passwordSound[4][4] = 2;
    passwordSound[4][5] = 1;
    passwordSound[4][6] = 2;
    passwordSound[4][7] = 3;
}
