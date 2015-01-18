#include <Wire.h>
#include <XBee.h>
 
//Router xBee address - 40B9DF66  
//I2C address - 1  
  
XBee xbee = XBee();
uint8_t xbeePayload[3] = { 0, 0, 0 };
XBeeAddress64 laser1Addr = XBeeAddress64(0x0013a200, 0x40c0edf);
ZBTxRequest laser1Tx = ZBTxRequest(laser1Addr, xbeePayload, sizeof(xbeePayload));

void setup() {
  Wire.begin(1);
  Wire.onReceive(receiveI2CEvent);
  
  Serial.begin(9600);
  Serial.println("Hello");
}

void receiveI2CEvent(int howMany){
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}
 
char* getString(byte array[], byte len)
{
  char concatString[len+1];
  for(byte i=0; i<len; i++){
    concatString[i] = array[0];
  }
  concatString[len] = '\0';
  return concatString;
}

void loop() {
  
}

void instructXBeeUpdate(int detectorId, int onOff){
  // send instruction to other xBee on mode change
  xbeePayload[0] = 6;
  xbeePayload[1] = de;
  xbeePayload[2] = 0;  
  xbee.send(laser1Tx);
}

