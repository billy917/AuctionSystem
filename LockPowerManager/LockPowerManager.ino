#include "Wire.h"
#include "Constants.h"
#include "LockManager.h"

LockManager manager;
volatile bool receivedMessage = false;
volatile uint8_t i2cDataBuffer[NFC_MESSAGE_MAX_SIZE];
uint8_t localI2CBuffer[NFC_MESSAGE_MAX_SIZE];

void setup() {
  Serial.begin(9600);
  pinMode(LOCK_MANAGER_SHELF_PIN, OUTPUT);
  pinMode(LOCK_MANAGER_INWALL_PIN, OUTPUT);
  pinMode(LOCK_MANAGER_WDOOR_PIN, OUTPUT);
  pinMode(LOCK_MANAGER_MDOOR_PIN, OUTPUT);
  
  Wire.begin(LOCK_MANAGER_I2C_ADDR);
  Wire.onReceive(receiveEvent);
}

void loop() {
  // put your main code here, to run repeatedly: 
  if(receivedMessage){
    for(int i=0; i<NFC_MESSAGE_MAX_SIZE; i++){
      localI2CBuffer[i] = i2cDataBuffer[i];
    }
    if(manager.canHandleI2CMessage(localI2CBuffer[0])){
      manager.handleI2CMessage(NFC_MESSAGE_MAX_SIZE, localI2CBuffer);
    }
    receivedMessage = false; 
  }
}

void receiveEvent(int howMany){
  for(int i=0; i<howMany && i< NFC_MESSAGE_MAX_SIZE; i++){
    i2cDataBuffer[i] = Wire.read();
  }
  receivedMessage = true;
}
