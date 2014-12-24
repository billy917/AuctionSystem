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

int passwordLength = 0;

void setup() {
  Serial.begin(9600);
  matrix.begin(0x70);  //0x70 is the 7-Segment address
  
  keypad.addEventListener(keypadEvent);
  keypad.setDebounceTime(20);
  
  // try to print a number thats too long
  matrix.print(0x0257,HEX);
  //matrix.writeDigitNum(1,2);
  //matrix.writeDigitNum(2,4);
  //matrix.writeDigitNum(3,5);
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
      default: password.append(eKey); passwordLength++; updateLED(eKey);
    }
         
    if(4 == passwordLength){
      checkPassword(); 
    }
  }
}

void pressedKeySound(){
  
}

void updateLED(char e){
  
}

void sucessUnlockSound(){
  
}

void failUnlockSound(){
  
}

void checkPassword(){
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
}

void loop() {
  keypad.getKey();
}
