#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <NFCDetector.h>
#include "Constants.h"

#define NFC_DETECTOR_ID 3
#define I2C_ADDR 103
// I2C Message 9 bytes - (MessageTypeId, DetectorId, Parameter)

NFCDetector detector(NFC_DETECTOR_ID, I2C_ADDR);
int led = 13;

void setup(){
  Serial.begin(9600);
  pinMode(led, OUTPUT); 

  Serial.println(F("Hi"));
  
  detector.initNFCCard();
  
  if(!detector.isNFCCardDetected()){
    Serial.println(F("NFC card not detected!")); 
    while (1); // halt
  } else {
    detector.printFirmwareInfo(); 
    while(!detector.isRegistered()){
      detector.registerWithNFCManager();
      if(!detector.isRegistered()){
        delay(15000); //delay 15 sec[
      } else {
        Serial.println(F("Registered with NFCManager")); 
      }
    }
  }
}
 
void loop() {
  detector.detectNFCChanges();
  if(detector.detectedNFC()){
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW); 
  }
  // Wait a bit before reading the card again
  delay(100);
}

