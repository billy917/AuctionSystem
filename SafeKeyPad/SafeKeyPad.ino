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
//#include "pitches.h"

char currentPassword[4] = {};
char password[4] = {};
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

    /* Tell BGM CONTROLLER to start playing the song */
    Wire.beginTransmission (BGM_I2C_ADDR);
    Wire.write (MESSAGETYPEID_BGM_PLAY_SONG);
    Wire.endTransmission();
    
    delay (10);

    Serial.print ("Current Password: ");
    Serial.println (currentPassword);

} //end setup()

void loop(){

    /* Get key input from keypad */
    keypad.getKey():
    handleCommands();

} //end loop()

/* I2C onReceive interrupt */
void Received (int noBytes){
    ((String)Wire.read()).toCharArray (currentPassword, 4);

} //end Received()

/* I2C onRequest interrupt */
void Request(){}

void handleCommands(){}

void initLEDs(){
  pinMode(9, OUTPUT); // 1-Red
  pinMode(10, OUTPUT); // 1-Green
  pinMode(11, OUTPUT); // 2-Red
  pinMode(12, OUTPUT); // 2-Green
  pinMode(A0, OUTPUT); // 3-Red
  pinMode(A1, OUTPUT); // 3-Green
}

void turnOnRed(int num){
  switch (num){
    case 3: analogWrite(A0,HIGH);
    case 2: digitalWrite(11,HIGH);
    case 1: digitalWrite(9,HIGH); 
  } 
}

void turnOffLEDs(){
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  analogWrite(A0, 0);
  analogWrite(A1, 0);
}

void turnOnGreenLEDs(){
  turnOffLEDs();
  digitalWrite(10,HIGH);
  digitalWrite(12,HIGH);
  analogWrite(A1,HIGH);
}

void keypadEvent(KeypadEvent eKey){
  if(PRESSED == keypad.getState()){
    if(locked){
      Serial.print("Pressed: "); Serial.println(eKey);
      switch (eKey){
        case '*': 
        case '#': passwordLength = 0; break; 
        default: password[passwordLength]=eKey; passwordLength++; updateLED(); 
      } 
      
      if(4 == passwordLength){
        if(checkPassword()){         
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
  if(length == 4){
   length = 3; 
  }
  
  matrix.clear();
  int j=0;
  for(int i=3-length;i<4;i++){
    int k = i;
    if(i>1){k++;}
    matrix.writeDigitNum(k,password[j]-'0',false);
    j++;
  }
  matrix.writeDisplay();  
  Serial.println(password);
}

bool checkPassword(){
    /* Check key input to song password */
        /* If key input IS song password */

        if (password == currentPassword){
            /* Stop playing song */
            Wire.beginTransmission (BGM_I2C_ADDR);
            Wire.write (MESSAGETYPEID_BGM_STOP_SONG);
            Wire.endTransmission();

            return true;

        } else {
        /* If key input is NOT song password */
        /* If fail three times */
            if (fail >= 3){
                
                /* Change to next song */
                Wire.beginTransmission (BGM_I2C_ADDR);
                Wire.write (MESSAGETYPEID_BGM_NEXT_SONG);
                Wire.endTransmission();

                /* Delay for one milisecond to get new song's password */
                delay (10);

                Serial.print ("New Password: ");
                Serial.println (currentPassword);

                /* Reset fail counter */
                fail = 0;

            } else {
            /* Increment fail counter by 1 */
                fail += 1;
            }

            return false;
        }

} //end checkPassword()
