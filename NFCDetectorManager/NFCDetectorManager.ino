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


volatile uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
volatile uint8_t detectorId = 0; //(0,1,2)
volatile uint8_t mode = 0; // //(0 - missingNFCChip, 1 - NewNFCChip)
uint8_t uidLength = 7;  // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
volatile boolean receivedMessage = false;
// need to store expected UID for each detector
volatile uint8_t expectedUid[3][7] = {{0x04, 0x8A, 0x82, 0xEA, 0xC2, 0x23, 0x80}, {0x04, 0xEC, 0x82, 0xEA, 0xC2, 0x23, 0x80},{0x04, 0x04, 0x82, 0xEA, 0xC2, 0x23, 0x81}};
uint8_t hackToolUid[1][7] = {{0x04, 0xBB, 0x82, 0xEA, 0xC2, 0x23, 0x80}};

int led = 13;
void setup() {
  pinMode(led, OUTPUT);  
    
  Serial.begin(9600);

  Wire.begin(10);
  Wire.onReceive(receiveI2CEvent);

  xbee.begin(Serial);  
}

void receiveI2CEvent(int howMany){
  if(9 == howMany){
    detectorId = Wire.read(); 
    mode = Wire.read(); 
    for(int i=0; i<uidLength; i++){
      uid[i] = Wire.read(); 
    }
    receivedMessage = true; // TODO: need better way to queue incoming requests
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
  if(receivedMessage){
    blinkLED(1);
    if(0 == mode){
      instructXBeeUpdate(false);
    } else if(detectedNFCMatchExpectedId()){
      instructXBeeUpdate(true);
    }
    receivedMessage = false; 
  }
}

boolean detectedNFCMatchExpectedId(){
  boolean matchExpected = true;

  if(detectorId >= 0 && detectorId < 3){
    for( int i=0; i < uidLength; i++){
      if(uid[i] != expectedUid[detectorId][i]){
        matchExpected = false;
        break; 
      }      
    }
  }

  return matchExpected;  
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



