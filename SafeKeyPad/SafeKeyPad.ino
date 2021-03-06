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
#include "LiquidCrystal_I2C.h"
#include "Timer.h"
#include "LCDController.h"
//#include "MemoryFree.h"

const int MAX_COUNTER_VALUE = 30; //in seconds

/* Initialize variables */
volatile uint8_t i2cDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile bool receivedI2CMessage = false;
uint8_t localBuffer[I2C_MESSAGE_MAX_SIZE];
volatile bool i2cDetectorLock = true;
volatile bool i2cKeypadLock = true;

LCDController lcdController;
volatile bool lcdPrint = false;
volatile bool i2cPrintLock = false;
bool bookLock = true;

Timer t;
volatile int countdownEvent;
volatile int counter = MAX_COUNTER_VALUE;
/* Counter lock used by other methods */
volatile bool counterFlag = false;
/* Counter lock used by Timer */
volatile bool counterLock = true;

char songPassword[5] = {};
int fail = 0;

/* Setup lock settings */
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
volatile bool locked = true;
long int lastKeyPress;
volatile bool delayKeyPress = false;

volatile bool checkEquationState = false;

void setup(){
    Serial.begin (9600);

    Serial.println (F("Begin Wire setup..."));
    Wire.begin (KEYPAD_LOCK_I2C_ADDR);
    Wire.onReceive (Received);
    Wire.onRequest (Request);
    Serial.println (F("---- Wire setup complete"));

    delay (500);

    Serial.println (F("Initializing LCD_Controller..."));
    lcdController.setCounter (counter);
    lcdController.initLCD();

    /* Show random equation at start-up */
    //lcdController.currentEquationIndex = randomEquation();

    lcdController.displayAllLCD();
    Serial.println (F("---- LCD_Controller initialize complete"));
    
    delay (200);

    Serial.println (F("Initializing Keypad..."));
    keypad.addEventListener (keypadEvent);
    keypad.setDebounceTime(20);
    initLEDs();
    // try to print a number thats too long
    matrix.begin(0x70);  //0x70 is the 7-Segment address
    clearPassword();
    Serial.println (F("---- Keypad initialize complete"));

    delay (200);

    Serial.println (F("Initializing magnetic lock..."));
    /* Lock LOCK_MANAGER */
    Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
    Wire.write (MESSAGETYPEID_LOCK);
    Wire.write (MESSAGETYPEID_LOCK_LOCKID_INWALL);
    Wire.write (MESSAGETYPEID_LOCK_LOCK);
    Wire.endTransmission();
    Serial.println (F("---- Mag-Lock initialize complete"));

    delay (200);

    /* Tell BGM CONTROLLER to start playing the song */
    Wire.beginTransmission (BGM_I2C_ADDR);
    Wire.write (MESSAGETYPEID_BGM);
    Wire.write (MESSAGETYPEID_BGM_STOP_SONG);
    Wire.endTransmission();

    i2cDetectorLock = false;
    i2cKeypadLock = false;
    
    delay (1500);

    Wire.beginTransmission (BGM_I2C_ADDR);
    Wire.write (MESSAGETYPEID_BGM);
    Wire.write (MESSAGETYPEID_BGM_PLAY_SONG);
    Wire.endTransmission();
    Serial.println (F("Sent PLAY_SONG message"));

    turnOnGreenLEDs();
    delay (200);
    turnOffLEDs();

    Serial.println (F("Begin LCD countdown event"));
    /* BELOW BREAKS I2C BUS */
    /* BUT ALSO BREAKS KEYPAD */
    //countdownEvent = t.every (1000, updateCounter);

} //end setup()

void loop(){

    /* Free RAM */
    //Serial.println(freeRam());

    /* Get key input from keypad */
    if (locked){
    if ((!delayKeyPress)) keypad.getKey();

    if (delayKeyPress){
        if ((long)(millis() - lastKeyPress) >= 2000){
            delayKeyPress = false;
            clearPassword();
        }
    }
    }

    if (receivedI2CMessage){
        //counterFlag = true;
        //counterLock = true;

        noInterrupts();
        for(int i=0; i<I2C_MESSAGE_MAX_SIZE; i++){
            localBuffer[i] = i2cDataBuffer[i];
        }
        interrupts();
        
        handleCommands();

        /* Reset receivedi2cmessage */
        receivedI2CMessage = false;
        //counterFlag = false;
    }

    if (!locked){
        if (!counterFlag && !counterLock) handleCounter();

        if (lcdController.canCheckEquation()){
            if (lcdController.checkEquation()) {
                unlockShelf();
                
                /* アンロック メインドーア */
                Wire.beginTransmission(LOCK_MANAGER_I2C_ADDR);
                Wire.write (MESSAGETYPEID_LOCK);
                Wire.write (MESSAGETYPEID_LOCK_LOCKID_MAINDOOR);
                Wire.write (2);
                Wire.endTransmission();
                
            } else {
                lockShelf();
            }
        } else {
            lockShelf();
        }

        if (lcdPrint){
            /* the code below may cause lcd corruption */
            //if (((long)millis())%300000 == 0) lcdController.lcd->init();
            if (bookLock) lcdController.displayAllLCD();
            lcdPrint = false;
        }
    }

    //t.update();

} //end loop()

/* I2C onReceive interrupt */
void Received (int noBytes){
    uint8_t temp = Wire.read();

    if ((temp == MESSAGETYPEID_KEYPAD_LOCK) && (!i2cKeypadLock)){

        i2cDataBuffer[0] = temp;
        for (int i=1; i < noBytes && i < I2C_MESSAGE_MAX_SIZE; i++){
           i2cDataBuffer[i] = Wire.read();
        }

        receivedI2CMessage = true;

    } else if ((temp == MESSAGETYPEID_NFC_MANAGE) &&
                (!i2cDetectorLock) &&
                (!locked)){

        i2cDataBuffer[0] = temp;
        for (int i=1; i < noBytes && i < I2C_MESSAGE_MAX_SIZE; i++){
           i2cDataBuffer[i] = Wire.read();
        }

        receivedI2CMessage = true;

    } else{
        /* MUST KEEP BELOW or else cannot receive BGM password */
        for (int i=1; i<noBytes; i++){
            temp = Wire.read();
        }
        
    }

} //end Received()

/* I2C onRequest interrupt */
void Request(){}

void handleCommands(){
    if (localBuffer[0] == MESSAGETYPEID_KEYPAD_LOCK){
        i2cKeypadLock = true;
        /* Received reset command */
        if (localBuffer[1] == MESSAGETYPEID_KEYPAD_LOCK_RESET){

            resetAll();

        } else if (localBuffer[1] ==
            MESSAGETYPEID_KEYPAD_LOCK_GET_PASSWORD){

            /* Get song password */
            int i;
            for (i = 0; i < 4; i++) {
                songPassword[i] = localBuffer[i+2];
            }

            Serial.print (F("Current password is "));
            Serial.println (songPassword);

        } else {}

        i2cKeypadLock = false;

    } else if (localBuffer[0] == MESSAGETYPEID_NFC_MANAGE){
        i2cDetectorLock = true;

        if (localBuffer[2] == MESSAGETYPEID_NFC_MANAGE_FOUND){
            lcdController.detectorNFCValue[localBuffer[3] - 1] = localBuffer[4];

        } else if (localBuffer[2] == MESSAGETYPEID_NFC_MANAGE_NOTFOUND){
            lcdController.detectorNFCValue[localBuffer[3] - 1] = 0;
        }

        lcdPrint = true;
        
        i2cDetectorLock = false;
    } else if (localBuffer[0] == MESSAGETYPEID_LCD) {

        // TODO: Have potential problems

        counterFlag = true;
        if (localBuffer[1] == MESSAGETYPEID_LCD_CHANGE_PATTERN){
            lcdController.notifyPatternChanged();

        } else if (localBuffer[1] == MESSAGETYPEID_LCD_CHANGE_EQUATION){
            lcdController.changeEquation();
        }

        counterFlag = false;

        lcdPrint = true;

    } else {}

    
} //end handleCommands()

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
      Serial.print(F("Pressed: ")); Serial.println(eKey);
      switch (eKey){
        case '*':
        case '#': clearPassword(); break;
        default: password[passwordLength]=eKey; passwordLength++; updateLED();
      }

      if(4 == passwordLength){

        /* If key input IS song password */
        if(checkPassword()){
          Serial.println (F("Correct password!"));

          delayKeyPress = false;

          /* Stop playing song */
          /*
          Wire.beginTransmission (BGM_I2C_ADDR);
          Wire.write (MESSAGETYPEID_BGM);
          Wire.write (MESSAGETYPEID_BGM_STOP_SONG);
          Wire.endTransmission();
          */

          turnOnGreenLEDs();

          /* アンロック セイフ */
          Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
          Wire.write (MESSAGETYPEID_LOCK);
          Wire.write (MESSAGETYPEID_LOCK_LOCKID_INWALL);
          Wire.write (MESSAGETYPEID_LOCK_UNLOCK);
          Wire.endTransmission();

          /* 全てのレーザーを遮断する */
          Wire.beginTransmission(NFC_MANAGER_I2C_ADDR);
          Wire.write(MESSAGETYPEID_LASER_CONTROL);
          Wire.write(MESSAGETYPEID_LASER_CONTROL_OFF_ALL);
          Wire.endTransmission();

          locked = false;

          Serial.println(F("Unlocked!"));

        /* If key input is NOT song password */
        } else {
          Serial.println(F("Failed"));

          //delayKeyPressEvent = t.after (2000, canKeyPress);

          /* Increment fail counter by 1 */
          fail += 1;
          turnOnRed(fail);

          delayKeyPress = true;
          lastKeyPress = (long)millis();
          
          //delay(2000);
          //clearPassword();

          /* If fail three times */
          if (fail >= 3){

            /* Change to next song */
            Wire.beginTransmission (BGM_I2C_ADDR);
            Wire.write (MESSAGETYPEID_BGM);
            Wire.write (MESSAGETYPEID_BGM_NEXT_SONG);
            Wire.endTransmission();

            delay (100);

            Serial.print (F("New Password: "));
            Serial.println (songPassword);

            /* Reset fail counter */
            fail = 0;
            turnOffLEDs();

          }

          Serial.print (3 - fail);
          Serial.print (F(" time(s) left! The password is "));
          Serial.println (songPassword);

          locked = true;
        }
      }

      if ('*' == eKey || '#' == eKey){
        locked = true;
        clearPassword();
        Serial.println(F("Lock!"));
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

/* Check key input to song password */
bool checkPassword(){
    return (charArrayCompare (songPassword, password));
} //end checkPassword()

/*
void canKeyPress(){
    clearPassword();
    delayKeyPress = false;
    t.stop (delayKeyPressEvent);
}
*/

void updateCounter(){
    /* if no other methods are running and
        counter lock is "locked" (true), unlock
        to update counter on LCD
    */

    if (!counterFlag && counterLock) counterLock = false;
}

void handleCounter(){
    counterFlag = true;
    counterLock = true;

    counter = counter - 1;

    if (counter <= 0) {

        //Serial.println("Before counter reset");
        counter = MAX_COUNTER_VALUE;
        lcdController.setCounter(counter);

        lcdController.notifyPatternChanged();
        //lcdController.changeEquation();

    } else {

        lcdController.setCounter(counter);

    }

    lcdPrint = true;

    //Serial.println ("Debug: updateCounter()");
    Serial.print (F("CountdownEvent: "));
    Serial.print (countdownEvent);
    Serial.print (F(", Counter: "));
    Serial.println (counter);
    
    counterFlag = false;

}

void lockShelf(){
    if (checkEquationState){
        /* Lock SHELF_LOCK */
        Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
        Wire.write (MESSAGETYPEID_LOCK);
        Wire.write (MESSAGETYPEID_LOCK_LOCKID_SHELF);
        Wire.write (MESSAGETYPEID_LOCK_LOCK);
        Wire.endTransmission();

        bookLock = true;
        checkEquationState = false;
    }
}

void unlockShelf(){
    if (!checkEquationState){
        /* Unlock SHELF_LOCK */
        Wire.beginTransmission (LOCK_MANAGER_I2C_ADDR);
        Wire.write (MESSAGETYPEID_LOCK);
        Wire.write (MESSAGETYPEID_LOCK_LOCKID_SHELF);
        Wire.write (MESSAGETYPEID_LOCK_UNLOCK);
        Wire.endTransmission();

        bookLock = false;

        lcdController.clearLCD();
        lcdController.displayString (7, 1, "Bookcase");
        lcdController.displayString (7, 2, "Unlocked");

        checkEquationState = true;
    }
}

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

int randomEquation(){
    Serial.print (F("Random equation "));

    randomSeed(analogRead(A0));
    int randomNumber = random(NUM_EQUATION - 1);

    Serial.print (randomNumber);
    Serial.println (F(" selected."));

    return randomNumber;
}

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
