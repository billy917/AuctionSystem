#include <Wire.h>
#include <XBee.h>
 
//Router xBee address - 40B9DF66  
//I2C address - 1  
  
XBee xbee = XBee();
uint8_t xbeePayload[3] = { 0, 0, 0 };
XBeeAddress64 laser1Addr = XBeeAddress64(0x0013a200, 0x40c0edf);
ZBTxRequest laser1Tx = ZBTxRequest(laser1Addr, xbeePayload, sizeof(xbeePayload));


volatile uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength = 7;  // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

// need to store expected UID for each detector
volatile uint8_t expectedUid[3][7] = {{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}};


void setup() {
  Serial.begin(9600);

  Wire.begin(1);
  Wire.onReceive(receiveI2CEvent);

  xbee.begin(Serial);  
}

void receiveI2CEvent(int howMany){
  if(9 == howMany){
    char detectorId = Wire.read();
    char mode = Wire.read();
    for(int i=0; i<uidLength; i++){
      uid[i] = Wire.read(); 
    }
    check
  } else {
    Serial.print("Got message with unexpected length:");
    while(1 < Wire.available()) // loop through all but the last
    {
      char c = Wire.read(); // receive byte as a character
      Serial.print(c);         // print the character
    }
  }
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
  //noop
}

void instructXBeeUpdate(int detectorId, int onOff){
  // send instruction to other xBee on mode change
  xbeePayload[0] = 6;
  xbeePayload[1] = 0;
  xbeePayload[2] = 0;  
  xbee.send(laser1Tx);
}

