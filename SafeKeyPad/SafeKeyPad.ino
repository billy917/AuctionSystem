/*
 * SafeKeypad.ino
 * Gets input from keypad, transmits to BGM CONTROLLER,
 *  controls the lock state
*/

#include "Wire.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Constants.h"
#include "Keypad.h"
#include "NFCLock.h"
//#include "pitches.h"

/* Initialize variables */
uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile bool receivedI2CMessage = false;

NFCLock nfcLock = new NFCLock();

char songPassword[5] = {};
int fail = 0;

// Setup lock settings
Adafruit_7segment matrix = Adafruit_7segment();

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};
byte rowPins[ROWS] = {5, 6, 7, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
char password[4] = {'-','-','-','-'};
int passwordLength = 0;
bool locked = true;

void setup(){
    Serial.begin (9600);
    Wire.begin (KEYPAD_LOCK_I2C_ADDR); // this Keypad-Lock Controller I2C port
    Wire.onReceive (Received);
    Wire.onRequest (Request);

    keypad.addEventListener (keypadEvent);
    keypad.setDebounceTime(20);
    initLEDs();

    // try to print a number thats too long
    matrix.begin(0x70);  //0x70 is the 7-Segment address

    clearPassword();

    delay (5000);

    /* Tell BGM CONTROLLER to start playing the song */
    Wire.beginTransmission (BGM_I2C_ADDR);
    Wire.write (MESSAGETYPEID_BGM);
    Wire.write (MESSAGETYPEID_BGM_STOP_SONG);
    Wire.endTransmission();

    delay (500);

    Wire.beginTransmission (BGM_I2C_ADDR);
    Wire.write (MESSAGETYPEID_BGM);
    Wire.write (MESSAGETYPEID_BGM_PLAY_SONG);
    Wire.endTransmission();

    Serial.println ("Sent PLAY_SONG message.");

} //end setup()

void loop(){

    /* Get key input from keypad */
    keypad.getKey();
    if (receivedI2CMessage) handleCommands();

} //end loop()

/* I2C onReceive interrupt */
void Received (int noBytes){

    for (int i=0; i < noBytes && i < I2C_MESSAGE_MAX_SIZE; i++){
        i2cDataBuffer[i] = Wire.read();
        //Serial.println(i2cDataBuffer[i]);
    }

    receivedI2CMessage = true;
    //Serial.print ("Received command: ");
    //Serial.println (receivedI2CMessage);

} //end Received()

/* I2C onRequest interrupt */
void Request(){}

void handleCommands(){
    if (i2cDataBuffer[0] == MESSAGETYPEID_KEYPAD_LOCK){
        /* Received reset command */
        if (i2cDataBuffer[1] == MESSAGETYPEID_KEYPAD_LOCK_RESET){

            resetAll();

        } else if (i2cDataBuffer[1] ==
            MESSAGETYPEID_KEYPAD_LOCK_GET_PASSWORD){

            /* Get song password */
            int i;
            for (i = 0; i < 4; i++) {
                songPassword[i] = i2cDataBuffer[i+2];
            }

            Serial.print ("Current password is ");
            Serial.println (songPassword);

        } else {}

    } else if (i2cDataBuffer[0] == MESSAGETYPEID_NFC_MANAGE){
        nfcLock.handleI2CMessage (i2cDataBuffer);

    } else {}

    /* Reset receivedi2cmessage */
    receivedI2CMessage = false;
    clearI2CBuffer();
}

void initLEDs(){
  pinMode(9, OUTPUT); // 1-Red
  pinMode(10, OUTPUT); // 1-Green
  pinMode(11, OUTPUT); // 2-Red
  pinMode(12, OUTPUT); // 2-Green
  pinMode(A2, OUTPUT); // 3-Red
  pinMode(A3, OUTPUT); // 3-Green
}

void turnOnRed(int num){
  switch (num){
    case 3: analogWrite(A2,255);
    case 2: digitalWrite(11,HIGH);
    case 1: digitalWrite(9,HIGH);
  }
}

void turnOffLEDs(){
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  analogWrite(A2, 0);
  analogWrite(A3, 0);
}

void turnOnGreenLEDs(){
  turnOffLEDs();
  digitalWrite(10,HIGH);
  digitalWrite(12,HIGH);
  analogWrite(A3,255);
}

void clearI2CBuffer(){
    for (int i=0; i < I2C_MESSAGE_MAX_SIZE; i++){
        i2cDataBuffer[i] = 0;
    }
}

void keypadEvent(KeypadEvent eKey){
  if(PRESSED == keypad.getState()){
    if(locked){
      Serial.print("Pressed: "); Serial.println(eKey);
      switch (eKey){
        case '*':
        case '#': clearPassword(); break;
        default: password[passwordLength]=eKey; passwordLength++; updateLED();
      }

      if(4 == passwordLength){
        bool passwordMatch = checkPassword();
        delay(2000);

        if(passwordMatch){
          locked = false;
          Serial.println("Unlocked!");
        } else {
          clearPassword();
          Serial.println("Failed");
        }
      }
    } else {
      if ('*' == eKey || '#' == eKey){
        locked = true;
        clearPassword();
        Serial.println("Lock!");
      }
    }
  }
}

void clearPassword(){
  passwordLength = 0;
  password[0] = '-';
  password[1] = '-';
  password[2] = '-';
  password[3] = '-';
  matrix.clear();
  matrix.writeDisplay();
}

void updateLED(){
  int length = passwordLength;

  matrix.clear();
  int j=0;

  for (int i=5-length; i<5; i++){
      int k=i;
      if(i<3){k--;}
      matrix.writeDigitNum (k, password[j]-'0',false);
      j++;
  }

  matrix.writeDisplay();

  Serial.println(password);

}

bool checkPassword(){
    /* Check key input to song password */
        /* If key input IS song password */

        if (charArrayCompare (songPassword, password)){
            /* Stop playing song */
            Wire.beginTransmission (BGM_I2C_ADDR);
            Wire.write (MESSAGETYPEID_BGM);
            Wire.write (MESSAGETYPEID_BGM_STOP_SONG);
            Wire.endTransmission();

            Serial.println ("Correct password!");

            turnOnGreenLEDs();

            /* Unlock LOCK_MANAGER */
            Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
            Wire.write (MESSAGETYPEID_LOCK);
            Wire.write (MESSAGETYPEID_LOCK_LOCKID_INWALL);
            Wire.write (MESSAGETYPEID_LOCK_UNLOCK);
            Wire.endTransmission();

            return true;

        } else {
        /* If key input is NOT song password */
            /* Increment fail counter by 1 */
            fail += 1;
            turnOnRed(fail);

            /* If fail three times */
            if (fail >= 3){

                /* Change to next song */
                Wire.beginTransmission (BGM_I2C_ADDR);
                Wire.write (MESSAGETYPEID_BGM);
                Wire.write (MESSAGETYPEID_BGM_NEXT_SONG);
                Wire.endTransmission();

                delay (2000);

                Serial.print ("New Password: ");
                Serial.println (songPassword);

                /* Reset fail counter */
                fail = 0;
                turnOffLEDs();

            }

            Serial.print (3 - fail);
            Serial.print (" time(s) left! The password is ");
            Serial.println (songPassword);

            return false;
        }

} //end checkPassword()

/* Reset to initial settings */
void resetAll(){
    delay(200);

    /* Clear song password */
    songPassword[0] = '-';
    songPassword[1] = '-';
    songPassword[2] = '-';
    songPassword[3] = '-';
    songPassword[4] = '-';

    /* Clear fail count */
    fail = 0;

    /* Clear matrix */
    clearPassword();

    /* Clear LED */
    turnOffLEDs();

    /* Lock safe */
    locked = true;
}

bool charArrayCompare (char one[], char two[]){

    int i;
    bool equal = true;
    for (i = 0; i < 4; i++){
        (one[i] == two[i])? equal &= true: equal &= false;
    }

    return equal;
}
