#include <Wire.h> // Enable this line if using Arduino Uno, Mega, etc.
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

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

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

Password password = Password( "9999" );
char passChar[4] = {};
int passwordLength = 0;

void setup() {
  Serial.begin(9600);
  matrix.begin(0x70);  //0x70 is the 7-Segment address
  
  keypad.addEventListener(keypadEvent);
  keypad.setDebounceTime(20);
  
  // try to print a number thats too long
  matrix.print(0x0254,HEX);
  matrix.writeDisplay();
}

void keypadEvent(KeypadEvent eKey){
  if(PRESSED == keypad.getState()){
    Serial.print("Pressed: ");
    Serial.println(eKey);
    pressedKeySound();
    switch (eKey){
      case '*': checkPassword(); break;
      case '#': resetPassword(); break;
      default: password.append(eKey); passChar[passwordLength]=eKey; updateLED(); passwordLength++; 
    }
         
    if(4 == passwordLength){
      checkPassword(); 
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
    if(i >1){k++;}
    matrix.writeDigitNum(k,passChar[j]-'0',false);
    j++;
  }
  matrix.writeDisplay();
}

void blinkLED(int delayDuration, int times){
  for(int i=0; i<times; i++){
    matrix.clear();
    matrix.writeDisplay();
    delay(delayDuration);
    updateLED();
    delay(delayDuration);
  }
}

void resetLED(){
/** Empties LED **/
  matrix.clear();
  matrix.writeDisplay();
}

/** SoundFX **/
void pressedKeySound(){}
void sucessUnlockSound(){}
void failUnlockSound(){}

void checkPassword(){
  blinkLED(250,4);
  if (password.evaluate()){
    Serial.println("Success");
    //Add code to run if it works
    sucessUnlockSound();
  }else{
    Serial.println("Wrong");
    failUnlockSound();
  }
  resetPassword();
}

void resetPassword(){
  password.reset();
  passwordLength = 0;
  resetLED();
}

void loop() {
  keypad.getKey();
}
