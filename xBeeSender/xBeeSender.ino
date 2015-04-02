//Coordinator xBee address - 40c04edf
//I2C address: 1

#include <Wire.h>
#include <XBee.h>
#include "Constants.h"
 
// Pin 13 has an LED connected on most Arduino boards.


XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
uint8_t xbeePayload[4] = { 0, 0, 0, 0 };
XBeeAddress64 laser1Addr = XBeeAddress64(0x0013a200, 0x40c04edf);
ZBTxRequest laser1Tx = ZBTxRequest(laser1Addr, xbeePayload, sizeof(xbeePayload));


// the setup routine runs once when you press reset:
void setup() {           
  Serial.begin(9600);
  
  xbee.begin(Serial);   
  delay(1000);
  
  xbeePayload[0] = MESSAGETYPEID_NFC_MANAGE;
  xbeePayload[1] = 1;
  xbeePayload[2] = MESSAGETYPEID_NFC_MANAGE_FOUND;
  xbeePayload[3] = 3;  
  xbeePayload[4] = 9; 
  xbee.send(laser1Tx);  
  
  delay(1000);
  
  xbeePayload[0] = MESSAGETYPEID_NFC_MANAGE;
  xbeePayload[1] = 1;
  xbeePayload[2] = MESSAGETYPEID_NFC_MANAGE_NOTFOUND;
  xbeePayload[3] = 3;  
  xbeePayload[4] = 0;  
  xbee.send(laser1Tx);
}

void loop(){
}
