#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include "SoftI2C.h"

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield
#define NFC_DETECTOR_ID 0

Adafruit_NFCShield_I2C nfc(IRQ, RESET);

SoftI2C i2c = SoftI2C(12, 13);  //data, clock // green, yellow

uint8_t lastUid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength = 7;  // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
boolean detectedNFCChip = false;

// I2C Message 9 bytes - (sourceId, on/off, 7bit uid)

void setup() {
  Serial.begin(115200);
  Serial.println("Hello");
  delay(50);
  nfc.begin();
  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  nfc.setPassiveActivationRetries(0x01);
  nfc.SAMConfig();
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

  boolean detectedChip = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if(detectedChip){
    nfc.PrintHex(uid, uidLength);Serial.println("");
    detectedNFCChip = true;
    if(!isSameAsLastDetectedChip()){
      notifyFoundNFCChip();
    }
    updateLastUID();
  } else {
    if(detectedNFCChip){
      detectedNFCChip = false;
      clearLastUID();
      notifyCannotFindNFCChip();     
    } else {
      detectedNFCChip = false;
    }
  }
  
  // Wait a bit before reading the card again
  delay(500);
}

void updateLastUID(){
 for(int i=0; i<uidLength; i++){
   lastUid[i] = uid[i];
 } 
}

void clearLastUID(){
 for(int i=0; i<uidLength; i++){
   lastUid[i] = 0;
 } 
}

boolean isSameAsLastDetectedChip(){
  boolean isSame = true;
  for(int i=0; i< uidLength; i++){
    if(uid[i] != lastUid[i]){
      isSame = false;
      break;
    }  
  }
  return isSame;
}

void notifyFoundNFCChip(){
  Serial.println("NFC Found");
  i2c.startWrite(1); // transmit to device #1 (NFCDetectorManager
  i2c.write(NFC_DETECTOR_ID);    // this NFC detector Id
  i2c.write(1);    // detected new NFC (1) or detected missing NFC (0)
  for(int i=0; i< uidLength; i++){
    i2c.write(uid[i]);
  }
  i2c.endWrite();
}

void notifyCannotFindNFCChip(){
  Serial.println("NFC Not Found");
  i2c.startWrite(1); // transmit to device #1 (NFCDetectorManager
  i2c.write(NFC_DETECTOR_ID);    // this NFC detector Id
  i2c.write(0);    // detected new NFC (1) or detected missing NFC (0)
  for(int i=0; i< uidLength; i++){
    i2c.write(0);
  }
  i2c.endWrite();
}

