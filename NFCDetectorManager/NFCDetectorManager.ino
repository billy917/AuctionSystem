#include <Wire.h>
#include <XBee.h>
#include "Constants.h"
#include "NFCManager.h"
//Router xBee address - 40c04f18
//I2C address - 1  
  
XBee xbee = XBee();
ZBRxResponse rx = ZBRxResponse();

volatile uint8_t i2cDataBuffer[NFC_MESSAGE_MAX_SIZE];
volatile uint8_t xBeeDataBuffer[I2C_MESSAGE_MAX_SIZE];
volatile boolean receivedI2CMessage = false;
volatile bool receivedXBeeMessage = false;

uint8_t localBuffer[NFC_MESSAGE_MAX_SIZE];
// need to store expected UID for each detector

int led = 13;
NFCManager nfcManager(3, false);

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  Wire.begin(NFC_MANAGER_I2C_ADDR);
  Wire.onReceive(receiveI2CEvent);
  xbee.begin(Serial);  
  nfcManager.setXBeeReference(&xbee);
}

void receiveI2CEvent(int howMany){  
  if(NFC_MESSAGE_MAX_SIZE+1 > howMany){ // assume 9 is the max size of XBee message
    for(int i=0; i<NFC_MESSAGE_MAX_SIZE; i++){
      if(i>=howMany){
        i2cDataBuffer[i] = 0;
      } else {
        i2cDataBuffer[i] = Wire.read(); 
      }
    }
    receivedI2CMessage = true; // TODO: need better way to queue incoming requests
  } else {
    while(1 < Wire.available()) // loop through all but the last
    {
      char c = Wire.read(); // receive byte as a character
      Serial.print(c);         // print the character
    }
  }
}

void handleXBeeMsg(){
  xbee.readPacket();
  if(xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
    xbee.getResponse().getZBRxResponse(rx);
        
    xBeeDataBuffer[0] = rx.getData(0);    
    if( MESSAGETYPEID_LASER_SENSOR == xBeeDataBuffer[0]){
      xBeeDataBuffer[1] = rx.getData(1);
      xBeeDataBuffer[2] = rx.getData(2);
    }

    receivedXBeeMessage = true;
  }   
}

void loop() {
  handleXBeeMsg();
  if(receivedXBeeMessage){
    for(int i=0; i<NFC_MESSAGE_MAX_SIZE; i++){
      localBuffer[i] = xBeeDataBuffer[i];
    }
    handleMessage();
    receivedXBeeMessage = false; 
  }
  
  if(receivedI2CMessage){
    for(int i=0;i<NFC_MESSAGE_MAX_SIZE;i++){
      localBuffer[i] = i2cDataBuffer[i]; 
    }  
    handleMessage();
    receivedI2CMessage = false; 
  }
}

void handleMessage(){
  if (MESSAGETYPEID_LASER_SENSOR == localBuffer[0]){
    forwardI2CMessage(LASER_SENSOR_I2C_ADDR);
  } else {
    nfcManager.handleI2CMessage(9, localBuffer);
  }
}

void forwardI2CMessage(int i2cAddr){
  Wire.beginTransmission(i2cAddr); 
  Wire.write(localBuffer[0]);
  Wire.write(localBuffer[1]);
  Wire.write(localBuffer[2]);
  Wire.write(localBuffer[3]);
  Wire.write(localBuffer[4]);
  Wire.endTransmission();
}

void blinkLED(int times){
  for(int i=0; i<times; i++){
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);               // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(500);     
  }
}



