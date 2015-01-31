#include <Wire.h>
#include <Adafruit_PN532.h>

#define SCK  (8)
#define MOSI (11)
#define SS   (10)
#define MISO (9)

#define NFC_DETECTOR_ID 2

Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);


uint8_t lastUid[] = { 0, 0, 0, 0, 0, 0, 0 };
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength = 7;  // Length of theUID (4 or 7 bytes depending on ISO14443A card type)
boolean detectedNFCChip = false;

// I2C Message 9 bytes - (sourceId, on/off, 7bit uid)

int led = 13;
int ledMode = LOW;
void setup() {
//  pinMode(led, OUTPUT); 
  
  Serial.begin(9600);
  Serial.println("Hello");
  delay(50);
  Wire.begin(2);
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

  boolean success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if(success){
    nfc.PrintHex(uid, uidLength);Serial.println("");
    detectedNFCChip = true;
    
    if(!isSameAsLastDetectedChip()){
      Serial.println("Is not same as last chip");
      notifyFoundNFCChip();
    } else {
      Serial.println("Is same as last chip"); 
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
  Wire.beginTransmission(10); // transmit to device #1 (NFCDetectorManager
  Wire.write(NFC_DETECTOR_ID);    // this NFC detector Id
  Wire.write(1);    // detected new NFC (1) or detected missing NFC (0)
  for(int i=0; i< uidLength; i++){
    Wire.write(uid[i]);
  }
  Wire.endTransmission();
  Serial.println("Done writing Found");
}

void notifyCannotFindNFCChip(){
  Serial.println("NFC Not Found");
  Wire.beginTransmission(10); // transmit to device #1 (NFCDetectorManager
  Wire.write(NFC_DETECTOR_ID);    // this NFC detector Id
  Wire.write(0);    // detected new NFC (1) or detected missing NFC (0)
  for(int i=0; i< uidLength; i++){
    Wire.write(0);
  }
  Wire.endTransmission();
  Serial.println("Done writing Mot Found");
  
}

