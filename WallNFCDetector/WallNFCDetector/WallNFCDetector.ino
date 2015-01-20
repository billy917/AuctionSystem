#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include "SoftI2C.h"

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield
#define NFC_DETECTOR_ID 0

Adafruit_NFCShield_I2C nfc(IRQ, RESET);

SoftI2C i2c = SoftI2C(12, 13);  //data, clock

uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength = 7;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
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
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  boolean detectedNFCChip = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if(detectedNFCChip & 7 == uidLength){
    readNFCChip();
    sendI2CMessage();
  }
}

void readNFCChip(){
  // Display some basic information about the card
  Serial.println("Found an ISO14443A card");
  Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
  Serial.print("  UID Value: ");
  nfc.PrintHex(uid, uidLength);
  Serial.println("");
  
  if (uidLength == 7)
  {
    
  }   
}
  
void sendI2CMessage(){
  i2c.startWrite(1); // transmit to device #1 (NFCDetectorManager)
  i2c.write(NFC_DETECTOR_ID);        // sends five bytes
  i2c.write(1);              // sends one byte  
  i2c.endWrite();    // stop transmitting
  delay(500);
}

