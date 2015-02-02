#include <Wire.h>
#include <XBee.h>
 
//Router xBee address - 40c04f18
//I2C address - 1  
  
XBee xbee = XBee();
uint8_t xbeePayload[3] = { 0, 0, 0 };
XBeeAddress64 laser1Addr = XBeeAddress64(0x0013a200, 0x40c0edf);
ZBTxRequest laser1Tx = ZBTxRequest(laser1Addr, xbeePayload, sizeof(xbeePayload));

XBeeAddress64 toolAddr = XBeeAddress64(0x0013a200, 0x40b9df66);
ZBTxRequest toolTx = ZBTxRequest(toolAddr, xbeePayload, sizeof(xbeePayload));


volatile uint8_t detectorId = 0; //(0,1,2)
volatile uint8_t msgType = 0; // //(0 - missingNFCChip, 1 - NewNFCChip)
volatile uint8_t param = 0;
volatile boolean receivedMessage = false;
// need to store expected UID for each detector

int led = 13;
void setup() {
  pinMode(led, OUTPUT);  
    
  Serial.begin(9600);

  Wire.begin(100);
  Wire.onReceive(receiveI2CEvent);

  xbee.begin(Serial);  
}

void receiveI2CEvent(int howMany){
  if(3 == howMany){
    msgType = Wire.read();
    detectorId = Wire.read(); 
    param = Wire.read();
    receivedMessage = true; // TODO: need better way to queue incoming requests
  } else {
    while(1 < Wire.available()) // loop through all but the last
    {
      char c = Wire.read(); // receive byte as a character
      Serial.print(c);         // print the character
    }
  }
}

void loop() {
  if(receivedMessage){
    blinkLED(1);
    if(6 == msgType && 1 == param){
      instructXBeeUpdate(false);
    } else if(6 == msgType && 2 == param){
      instructXBeeUpdate(true);
    }
    receivedMessage = false; 
  }
}

void instructXBeeUpdate(boolean detectedCorrectNFC){
  // send instruction to other xBee on mode change
  xbeePayload[0] = 6; // message typeId
  xbeePayload[1] = detectorId;
  xbeePayload[2] = detectedCorrectNFC;  
  xbee.send(laser1Tx);
  xbee.send(toolTx);
}

void blinkLED(int times){
  for(int i=0; i<times; i++){
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);               // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(500);     
  }
}



