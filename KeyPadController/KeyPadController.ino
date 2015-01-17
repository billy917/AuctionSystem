//Coordinator xBee address - 40c04F18
//I2C address: 1
// 1) http:/itp.nyu.edu/physcomp/labs/labs-arduino-digital-and-analog/tone-output-using-an-arduino/
// 2) http://code.google.com/p/rogue-code/wiki/ToneLibraryDocumentation

#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "pitches.h"

#include <Keypad.h>
#include <Password.h>

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
int speakerPin = 9;
int keySounds[10] = {NOTE_A4, NOTE_AS4, NOTE_B4, NOTE_C4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4};
int errorSound = NOTE_B0;

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
Password password = Password( "9999" );
Password adminPassword = Password( "8888" );
char passChar[4] = {};
int passwordLength = 0;
volatile int mode, nextMode = 1;
volatile boolean adminMode = false;
volatile boolean servoMode = false;

void setup() {
  Serial.begin(9600);
  matrix.begin(0x70);  //0x70 is the 7-Segment address
  
  keypad.addEventListener(keypadEvent);
  keypad.setDebounceTime(20);
  
  // try to print a number thats too long
  matrix.print(0x0255,HEX);
  matrix.writeDisplay();
  
  Wire.begin(1);
}

int servoIndex = -1;
void keypadEvent(KeypadEvent eKey){
  if(PRESSED == keypad.getState()){
    Serial.print("Pressed: ");
    Serial.println(eKey);
    Serial.println(adminMode);
    if(adminMode) {
      if(servoMode){
        switch (eKey){
          case '*': servoIndex = -1; break; 
          case '#': servoMode = false;  servoIndex = -1; break; 
          default: passChar[0]=eKey; updateLED(); passwordLength=1; servoIndex = eKey - '0';
        } 
      } else {
        switch (eKey){
          case '*': servoIndex = -1; break; 
          case '#': adminMode=false; resetPassword(); servoIndex = -1; break; 
          default: passChar[0]=eKey; updateLED(); passwordLength=1; servoMode = true; servoIndex = eKey - '0';
        }
      }
    } else {
      switch (eKey){
        case '*': checkPassword(); break;
        case '#': resetPassword(); break;
        default: password.append(eKey); adminPassword.append(eKey); passChar[passwordLength]=eKey; updateLED(); pressedKeySound(eKey-'0'); passwordLength++; 
      }
    } 
    
    Serial.println(passwordLength);
    if(adminMode && 2 == passwordLength){
      nextMode = 5;
      adminMode = false;
      resetPassword();
    } else if(4 == passwordLength){
      checkAdminPassword();
      if(!adminMode){
        checkPassword(); 
      }
    }
  }
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
    matrix.writeDigitNum(k,passChar[j]-'0',false);
    j++;
  }
  matrix.writeDisplay();
}

void blinkLED(int delayDuration, int times){
  for(int i=0; i<times; i++){
    matrix.clear();
    matrix.writeDisplay();
    tone(speakerPin, errorSound, delayDuration);
    delay(delayDuration);
    updateLED();
    tone(speakerPin, errorSound, delayDuration);
    delay(delayDuration);
  }
}

void resetLED(){
/** Empties LED **/
  matrix.clear();
  matrix.writeDisplay();
}

/** SoundFX **/
void pressedKeySound(int numberPressed){
  tone(speakerPin, keySounds[numberPressed], 250);
}
void sucessUnlockSound(){}
void failUnlockSound(){}

void checkAdminPassword(){
  Serial.println("Check admin");
  if(adminPassword.evaluate()){
    Serial.println("admin true");
    adminMode = true;
    resetPassword();
  } else {
     Serial.println("admin false"); 
  }
}

void checkPassword(){
  blinkLED(250,4);
  if (password.evaluate()){
    Serial.println("Success");
    //Add code to run if it works
    if(1 == mode){
      nextMode = 3;
    } else if (3 == mode){
      nextMode = 1; 
    }
    sucessUnlockSound();
  }else{
    Serial.println("Wrong");
    failUnlockSound();
  }
  resetPassword();
}

void resetPassword(){
  password.reset();
  adminPassword.reset();
  passwordLength = 0;
  resetLED();
}

void loop() {
  keypad.getKey();
  
  handleCommands();
}

void handleCommands(){
  if(nextMode != mode){
    if(nextMode == 1){
      //Turn Off  
      instructModeChange(nextMode);
    } else if (nextMode == 2){
      //Arm System
    } else if (nextMode == 3){
      //Turn On
      instructModeChange(nextMode);
    } else if (nextMode == 5){
       //Reposition laser
      //instructRepositionLaser(); 
    }
    mode = nextMode;
  } 
}

void instructModeChange(int nextMode){
  // send instructions to internal I2C nodes on mode change
  Wire.beginTransmission(2); //2 == LaserDriver2 addr
  Wire.write(mode);
  Wire.endTransmission();
}

void instructRepositionLaser(int laserIndex, int moveDirectionChar){
  
  uint8_t moveDirection = 0;
  if(moveDirectionChar == '2'){
    moveDirection = 0;  
  } else if(moveDirectionChar == '4'){
    moveDirection = 2;  
  } else if(moveDirectionChar == '6'){
    moveDirection = 3;  
  } else if(moveDirectionChar == '8'){
    moveDirection = 1;  
  }
  xbeePayload[0] = 5;
  xbeePayload[1] = laserIndex;
  xbeePayload[2] = moveDirection;
  
  //xbee.send(laser1Tx);  
}
