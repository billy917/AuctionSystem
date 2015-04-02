//Coordinator xBee address - 40b9df66

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <XBee.h>
#include "Constants.h"

bool isConnected = false;

#define TFT_CS     10
#define TFT_RST    9  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!

#define Neutral 0
#define Press 1
#define Up 2
#define Down 3
#define Right 4
#define Left 5

#define MainMenu 0
#define ToogleLaser 1
#define ToggleLock 2
#define ToggleLaserPattern 3
#define CalibrateLaserSensor 4
#define NFCDetector 5
#define ToggleMusic 6
#define ResetSafeKeypad 7
#define ResetClock 8

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
uint8_t commandData[] = {0,0};

int patternIndex = 0;

bool laserState[9] = {true,true,true,true,true,true,true,true,true};
bool lockState[4] = {true, false, false, false};
uint8_t xbeePayload[4] = { 0, 0, 0, 0 };
XBeeAddress64 laser2Addr = XBeeAddress64(0x0013a200, 0x40c04ef1); // send commands to LaserDriver2
ZBTxRequest laser2Tx = ZBTxRequest(laser2Addr, xbeePayload, sizeof(xbeePayload));
XBeeAddress64 laser1Addr = XBeeAddress64(0x0013a200, 0x40c04edf); // send commands to LaserDriver2
ZBTxRequest laser1Tx = ZBTxRequest(laser1Addr, xbeePayload, sizeof(xbeePayload));
XBeeAddress64 laser3Addr = XBeeAddress64(0x0013a200, 0x40c337e0); // send commands to LaserDriver2
ZBTxRequest laser3Tx = ZBTxRequest(laser3Addr, xbeePayload, sizeof(xbeePayload));
XBeeAddress64 sensor1Addr = XBeeAddress64(0x0013a200, 0x40cab3f1); // send commands to LaserDriver2
ZBTxRequest sensor1Tx = ZBTxRequest(sensor1Addr, xbeePayload, sizeof(xbeePayload));

volatile boolean enableSensor = false;
volatile boolean laserEnabled = false;
volatile boolean musicOn = true;
volatile uint8_t nfcState[6] = {0,0,0,0,0,0};

volatile boolean sensorOn = false;
volatile uint8_t sensorId = -1;

void setup(void) {  
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(3);
  
  // Trapped logo splash screen
  //drawSplash();

  //drawInitialization();
  
  drawMenu();
  
  Serial.begin(9600);
  xbee.begin(Serial);   
}

int currentMenu = MainMenu;
// 0 - Main Menu
// 1 - ToogleLaser
// 2 - ToggleLock
// 3 - ToggleLaserPattern
// 4 - CalibrateLaserSensor
// 5 - NFCDetectors
// 6 - ToggleMusic
// 7 - ResetSafeKeypad
// 8 - ResetClock

int currentRowSelection = 0;
int maxRows[] = {9, 9, 4, 6, 10, 1};

void drawMenu(){
  tft.fillScreen(ST7735_BLACK);
  switch(currentMenu){
    case MainMenu:
      drawMainMenu();
      break;
    case ToogleLaser:
      drawToggleLaser();
      break;
    case ToggleLock:
      drawToggleLock();
      break;
    case ToggleLaserPattern:
      drawToggleLaserPattern();
      break;
    case NFCDetector:
      drawNFCDetector();
      break;
    case CalibrateLaserSensor:
      drawSensorDetails();
  }
}

void drawNFCDetector(){
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("\nNFC Detectors");
  
  tft.setTextSize(1); 
  tft.println("-> Refresh");
  for(int i=1;i<=5;i++){
    tft.print("Detector"); tft.print(i); tft.print(") "); tft.println(nfcState[i]);
  }  
}

void drawToggleLaserPattern(){
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("\nToggle Laser\n");
  
  tft.setTextSize(1); 
  if(0 == currentRowSelection){
    tft.print("->");
  }
  tft.println("1) Full On");
  tft.setTextSize(1); 
  if(1 == currentRowSelection){
    tft.print("->");
  }
  tft.println("2) Deactivated");
  tft.setTextSize(1); 
  if(2 == currentRowSelection){
    tft.print("->");
  }
  tft.println("3) Pattern1 ");
  tft.setTextSize(1); 
  if(3 == currentRowSelection){
    tft.print("->");
  }
  tft.println("4) Pattern2");
  if(4 == currentRowSelection){
    tft.print("->");
  }
  tft.println("5) Pattern3");
  if(5 == currentRowSelection){
    tft.print("->");
  }
  tft.println("6) Pattern4");
}

void handleToggleLaserPattern(){
  patternIndex = currentRowSelection;
  xbeePayload[0] = MESSAGETYPEID_LASER_CONTROL;
  if(patternIndex == 0){    
    xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_ON;
    for(int i=0; i<9; i++){
      xbeePayload[2] = i;  
      xbee.send(laser1Tx); 
    }

  } else if (patternIndex == 1){
    for(int i=0; i<9; i++){
      xbeePayload[2] = i;  
      if(i == 2 || i == 3 || i==6 || i==7){
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_OFF;
      } else {
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_ON;
      }
      xbee.send(laser1Tx); 
    }
  } else if (patternIndex == 2){
    for(int i=0; i<9; i++){
      xbeePayload[2] = i;  
      if(i == 3){
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_OFF;
      } else {
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_ON;
      }
      xbee.send(laser1Tx); 
    }
  } else if (patternIndex == 3){
    for(int i=0; i<9; i++){
      xbeePayload[2] = i;  
      if(i == 4 || i == 3){
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_OFF;
      } else {
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_ON;
      }
      xbee.send(laser1Tx); 
    }
  } else if (patternIndex == 4){
    for(int i=0; i<9; i++){
      xbeePayload[2] = i;  
      if(i == 0 ){
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_OFF;
      } else {
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_ON;
      }
      xbee.send(laser1Tx); 
    }
  } else if (patternIndex == 5){
    for(int i=0; i<9; i++){
      xbeePayload[2] = i;  
      if(i == 2){
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_OFF;
      } else {
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_ON;
      }
      xbee.send(laser1Tx); 
    }
  }
}

void drawToggleLaser(){
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("\nToggle Laser\n");
  
  tft.setTextSize(1); 
  if(0 == currentRowSelection){
    tft.print("->");
  }
  tft.print("1) Laser1");
  tft.print("(");
  if(laserState[0]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  if(1 == currentRowSelection){
    tft.print("->");
  }
  tft.print("2) Laser2");
   tft.print("(");
  if(laserState[1]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  if(2 == currentRowSelection){
    tft.print("->");
  }
  tft.print("3) Laser3");
   tft.print("(");
  if(laserState[2]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  if(3 == currentRowSelection){
    tft.print("->");
  }
  tft.print("4) Laser4");
   tft.print("(");
  if(laserState[3]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  if(4 == currentRowSelection){
    tft.print("->");
  }
  tft.print("5) Laser5");
   tft.print("(");
  if(laserState[4]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  if(5 == currentRowSelection){
    tft.print("->");
  }
  tft.print("6) Laser6");
   tft.print("(");
  if(laserState[5]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  if(6 == currentRowSelection){
    tft.print("->");
  }
  tft.print("7) Laser7");
   tft.print("(");
  if(laserState[6]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  if(7 == currentRowSelection){
    tft.print("->");
  }
  tft.print("8) Laser8");
   tft.print("(");
  if(laserState[7]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  if(8 == currentRowSelection){
    tft.print("->");
  }
  tft.print("9) Laser9");
   tft.print("(");
  if(laserState[8]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
}

void drawToggleLock(){
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("\nToggle Lock");
  
  tft.setTextSize(1); 
  if(0 == currentRowSelection){
    tft.print("->");
  }
  tft.print("1) Shelf Lock");
  tft.print("(");
  if(lockState[0]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  if(1 == currentRowSelection){
    tft.print("->");
  }
  tft.print("2) InWall Safe");
  tft.print("(");
  if(lockState[1]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  
  if(2 == currentRowSelection){
    tft.print("->");
  } 
  tft.print("3) White Door");
  tft.print("(");
  if(lockState[2]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
  
  if(3 == currentRowSelection){
    tft.print("->");
  }
  tft.print("4) Main Door");
  tft.print("(");
  if(lockState[3]){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.print(")\n");
}

uint8_t sensorState[9]={0,0,0,0,0,0,0,0,0};

void drawSensorDetails(){
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("Sensors");
  
  tft.setTextSize(1); 
  if (0 == currentRowSelection) tft.print("->");
  tft.println("Refresh data");
  for(int i=1;i<=9;i++){
    if (i == currentRowSelection) tft.print("->");
    tft.print("Sensor "); tft.print(i); tft.print(": "); tft.println(sensorState[i-1]);
  }
}

void drawMainMenu(){
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(29,0);
  tft.println("\nMain Menu\n");
 
  tft.setTextSize(1); 
  if(0 == currentRowSelection){
    tft.print("->");
  }
  tft.println("1) Toogle Lasers");
  
  if(1 == currentRowSelection){
    tft.print("->"); 
  }
  tft.println("2) Toogle Locks");
  
  if(2 == currentRowSelection){
    tft.print("->");
  }
  tft.println("3) Toogle Laser Pattern");

  if(3 == currentRowSelection){
    tft.print("->");
  }
  tft.println("4) Sensor Details");
  
  if(4 == currentRowSelection){
    tft.print("->");
  }
  tft.println("5) NFC Detectors");
   
  if(5 == currentRowSelection){
    tft.print("->");
  }
  tft.print("6) Toggle Music (");
  if(musicOn){
    tft.setTextColor(ST7735_GREEN);
    tft.print("ON");
  } else {
    tft.setTextColor(ST7735_RED);
    tft.print("OFF");
  }
  tft.setTextColor(ST7735_WHITE);
  tft.println(")");
  
  if(6 == currentRowSelection){
    tft.print("->");
  }
  tft.println("7) Reset Safe Keypad");
  
  if(7 == currentRowSelection){
    tft.print("->");
  }
  tft.println("8) Reset Clock");
  
  if(8 == currentRowSelection){
    tft.print("->");
  }
  tft.println("9) Toggle sensor");
  
  tft.print("10) Sensor:"); tft.println(sensorId);
  tft.print(" value:"); tft.println(sensorOn);
  
  
}

void loop() {
  boolean needRefresh = handleXBeeMsg();
  
  int joystickPosition = CheckJoystick();
  if(Neutral != joystickPosition){
     needRefresh = needRefresh || handleJoystickAction(joystickPosition);
  }
  
  if(needRefresh){
    drawMenu();
  }
}

boolean handleJoystickAction(int joystickState){
  boolean updated = false;
  switch(joystickState){
    case Up:
      currentRowSelection -= 1;
      if(currentRowSelection <= -1){
        currentRowSelection = maxRows[currentMenu] - 1;
      } 
      updated = true;
      break;
    case Down:
      currentRowSelection += 1;
      if(currentRowSelection >= maxRows[currentMenu]){
        currentRowSelection = 0;
      } 
      updated = true;
      break;
    case Left:
      updated = handleMenuBack();
      break;
    case Right:
    case Press:
      updated = handleMenuSelection();
  }
  return updated;
}

void handleSensorInfo(){
    if(0 == currentRowSelection){
        xbeePayload[0] = MESSAGETYPEID_LASER_SENSOR;
        xbeePayload[1] = MESSAGETYPEID_LASER_SENSOR_REQUEST;
        xbee.send (sensor1Tx);
    } 
}

void calibrateLaserSensor (uint8_t laserSensorId){
    xbeePayload[0] = MESSAGETYPEID_LASER_SENSOR;
    xbeePayload[1] = MESSAGETYPEID_LASER_SENSOR_CALIBRATE;
    xbeePayload[2] = laserSensorId;
    xbee.send (laser1Tx);
}

void toggleMusic(){
  bool currState = musicOn;
    if(currState){
      xbeePayload[1] = MESSAGETYPEID_BGM_STOP_SONG;
      musicOn = false;
    } else {
      xbeePayload[1] = MESSAGETYPEID_BGM_PLAY_SONG;
      musicOn = true;
    }
    xbeePayload[0] = MESSAGETYPEID_BGM;
    xbeePayload[2] = 0;
    xbee.send(laser1Tx); 
}

void resetSafeKeyPad(){
    xbeePayload[0] = MESSAGETYPEID_KEYPAD_LOCK;
    xbeePayload[1] = MESSAGETYPEID_KEYPAD_LOCK_RESET;

    //xbeePayload[0] = MESSAGETYPEID_BGM;
    //xbeePayload[1] = MESSAGETYPEID_BGM_UPDATE;
    xbeePayload[2] = 0;
    xbee.send(laser1Tx);

    delay (1500);

    xbeePayload[0] = MESSAGETYPEID_NFC_MANAGE;
    xbeePayload[1] = 0;
    xbeePayload[2] = MESSAGETYPEID_NFC_MANAGE_FOUND;
    xbeePayload[3] = 1;
    xbeePayload[4] = 5;
    xbee.send(laser1Tx);
    
}

boolean handleMenuSelection(){
  boolean updated = false;
  if(currentMenu == MainMenu){
    if(currentRowSelection == 0){
      currentMenu = ToogleLaser;
      currentRowSelection = 0;
      updated = true;
    } else if (currentRowSelection == 1){
      currentMenu = ToggleLock;
      currentRowSelection = 0;
      updated = true;
    } else if(currentRowSelection == 2){
      currentMenu = ToggleLaserPattern;
      currentRowSelection = 0;
      drawToggleLaserPattern();      
      updated = true;
    } else if (currentRowSelection == 3){
        currentMenu = CalibrateLaserSensor;
        currentRowSelection = 0;
        drawSensorDetails();
        updated = true;
    } else if(currentRowSelection == 4){
      currentMenu = NFCDetector;
      currentRowSelection = 0;
      drawNFCDetector();
      updated = true;
    } else if(currentRowSelection == 5){
      toggleMusic();
      updated = true; 
    } else if(currentRowSelection == 6){
      resetSafeKeyPad();
      updated = true;
    } else if(currentRowSelection == 7){
      resetClock();
      updated = true; 
    } else if(currentRowSelection == 8){
      toggleSensor(); 
    }
  } else if (currentMenu == ToogleLaser){
      bool currState = laserState[currentRowSelection];
      if(currState){
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_OFF;
        laserState[currentRowSelection] = false;
      } else {
        xbeePayload[1] = MESSAGETYPEID_LASER_CONTROL_ON;
        laserState[currentRowSelection] = true;
      }
      xbeePayload[0] = MESSAGETYPEID_LASER_CONTROL;
      xbeePayload[2] = currentRowSelection+1;  
      xbee.send(laser1Tx); 
      
      //
      //xbee.send(laser3Tx); 
      updated = true;
  } else if (currentMenu == ToggleLock){
      bool currState = lockState[currentRowSelection];
      if(currState){
        xbeePayload[2] = MESSAGETYPEID_LOCK_UNLOCK;
        lockState[currentRowSelection] = false;
      } else {
        xbeePayload[2] = MESSAGETYPEID_LOCK_LOCK;
        lockState[currentRowSelection] = true;
      }
      xbeePayload[0] = MESSAGETYPEID_LOCK;
      xbeePayload[1] = currentRowSelection + 1;
      xbee.send(laser1Tx); 
      updated = true;
  } else if (currentMenu == ToggleLaserPattern){
      handleToggleLaserPattern();
      updated = true;    
  } else if (currentMenu == CalibrateLaserSensor){
      handleSensorInfo();
      updated=true;
  } else if (currentMenu == NFCDetector){
      requestNewNFCData(); 
      updated = true;
  }
  return updated;
}

void toggleSensor(){
  if(enableSensor){    
    enableSensor = false;
  } else {
    enableSensor = true;
  }
  xbeePayload[0] = MESSAGETYPEID_LASER_SENSOR;
  if(enableSensor){
    xbeePayload[1] = MESSAGETYPEID_LASER_SENSOR_ON;
  } else {
    xbeePayload[1] = MESSAGETYPEID_LASER_SENSOR_OFF;
  }
  xbeePayload[2] = 2;
  xbee.send(laser1Tx);
}

void requestNewNFCData(){
  xbeePayload[0] = MESSAGETYPEID_NFC_TOOL;
  xbeePayload[1] = MESSAGETYPEID_NFC_TOOL_REQUEST;
  xbee.send(laser1Tx);
}

void resetClock(){
  xbeePayload[0] = MESSAGETYPEID_CLOCK;
  xbeePayload[1] = MESSAGETYPEID_CLOCK_RESET;
  xbee.send(laser2Tx);  
 
  xbeePayload[0] = MESSAGETYPEID_CLOCK;
  xbeePayload[1] = MESSAGETYPEID_CLOCK_PAUSE;
  xbee.send(laser2Tx);

  delay (1500);

  xbeePayload[0] = MESSAGETYPEID_CLOCK;
  xbeePayload[1] = MESSAGETYPEID_CLOCK_START;
  xbee.send(laser2Tx);
}

boolean handleMenuBack(){
  boolean updated = false;
  if(currentMenu == ToogleLaser || 
      currentMenu == ToggleLock || 
      currentMenu == ToggleLaserPattern || 
      currentMenu == CalibrateLaserSensor ||
      currentMenu == NFCDetector){
    currentRowSelection = 0;
    currentMenu = MainMenu; 
    updated = true;
  }
  return updated;
}

// Check the joystick position
int CheckJoystick()
{
  if(isConnected){
    int joystickState = analogRead(3);
    if (joystickState < 50) return Left;
    if (joystickState < 250) return Down;
    if (joystickState < 400) return Press;
    if (joystickState < 550) return Right;
    if (joystickState < 950) return Up;
  } else {
    // battery
    int joystickState = analogRead(3);
    if (joystickState < 50) return Left;
    if (joystickState < 250) return Down;
    if (joystickState < 400) return Press;
    if (joystickState < 600) return Right;
    if (joystickState < 1020) return Up;
  }
  return Neutral;
}

void drawSplash(){
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(35,60);
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);
  tft.println("Trapped!");
  delay(2000);
}

void drawInitialization(){
  tft.setTextSize(1);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(3,0);
  
  tft.setTextColor(ST7735_WHITE);
  tft.print("Loading bootloader.");
  for(int i=0; i<4; i++){
    delay(750);
    tft.print(".");
  }
  tft.setTextColor(ST7735_GREEN);
  tft.println("OK");
   
  tft.setTextColor(ST7735_WHITE);
  tft.print("Detecting network.");
  for(int i=0; i<4; i++){
    delay(750);
    tft.print(".");
  }
  tft.setTextColor(ST7735_GREEN);
  tft.println("OK");
  
  tft.setTextColor(ST7735_WHITE);
  tft.print("Bypassing firewall.");
  for(int i=0; i<4; i++){
    delay(750);
    tft.print(".");
  }
  tft.setTextColor(ST7735_GREEN);
  tft.println("OK");
  delay(2000);
}

boolean handleXBeeMsg(){
  xbee.readPacket();
  if(xbee.getResponse().isAvailable() && xbee.getResponse().getApiId() == ZB_RX_RESPONSE){
    xbee.getResponse().getZBRxResponse(rx);
    if(MESSAGETYPEID_NFC_MANAGE ==  rx.getData(0)){
      int nfcId = rx.getData(3);
      bool nfcDetected = MESSAGETYPEID_NFC_MANAGE_FOUND == rx.getData(2) ? true:false;
      uint8_t nfcValue = nfcDetected?rx.getData(4):0;
      nfcState[nfcId] = nfcValue;
      return true;
    } else if (MESSAGETYPEID_NFC_TOOL ==  rx.getData(0)
                && MESSAGETYPEID_NFC_TOOL_STATUS == rx.getData(1)){
      /* Clear local memory */
      for(int i=1;i<= rx.getData(2);i++){
        nfcState[i] = 0;
      }
    
      for(int i=1;i<= rx.getData(2);i++){
        nfcState[i] = rx.getData(2+i);
      }
      return true;
    } else if (MESSAGETYPEID_LASER_SENSOR == rx.getData(0)){
      if(MESSAGETYPEID_LASER_SENSOR_ON == rx.getData(1)){
        sensorOn = true; 
        sensorId = rx.getData(2);
      } else if(MESSAGETYPEID_LASER_SENSOR_OFF == rx.getData(1)){
        sensorOn = false; 
        sensorId = rx.getData(2);
      } else if (MESSAGETYPEID_LASER_SENSOR_STATUS == rx.getData(1)){
        int numPoints = rx.getData(2);
        for (int i=0; i<numPoints; i++){
          uint8_t sensorId = rx.getData((i*2)+3);
          uint8_t sensorValue = rx.getData((i*2)+4);
          sensorState[sensorId-1] = sensorValue;
        }
      }
      return true;
    }
  }   
  return false;
}

void unlockShelf(){
 instructXBeeModeChange(1); 
}

void instructXBeeModeChange(int nextMode){
  // send instruction to other xBee on mode change
  xbeePayload[0] = nextMode;
  xbeePayload[1] = 1;
  xbeePayload[2] = 1;  
  xbee.send(laser2Tx);
}

/*
void tftPrintTest() {
  float p = 3.1415;
  
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST7735_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST7735_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST7735_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST7735_WHITE);
  tft.print(" seconds.");
}
*/

