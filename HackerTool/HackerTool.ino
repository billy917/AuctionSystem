#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

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
#define HackSystem 1
#define DetectSensor 2
#define ScrewPeter 3

void setup(void) {
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(3);
  
  // Trapped logo splash screen
  drawSplash();

  drawInitialization();
  
  drawMenu();
}

int currentMenu = MainMenu;
// 0 - Main Menu
// 1 - Hack System
// 2 - Detect Sensor
// 3 - Screw Peter

int currentRowSelection = 0;
int maxRows[] = {3};

void drawMenu(){
  tft.fillScreen(ST7735_BLACK);
  switch(currentMenu){
    case MainMenu:
      drawMainMenu();
      break;
    case HackSystem:
      drawHackSystem();
      break;
    case DetectSensor:
      drawDetectSensor();
      break;
    case ScrewPeter:
      drawScrewPeter();
      break;
  }
}

void drawHackSystem(){
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("\nHack System\n");
  
  tft.setTextSize(1); 
  tft.println("Before hacking, connect device to the security system first...");
}

void drawDetectSensor(){
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("\nDetect Sensor");
  
  tft.setTextSize(1); 
  tft.println("Connect to security system and download the system's wireless frequency first...");
}

void drawScrewPeter(){
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10,0);
  tft.println("\nFuck Peter\n");
  
  tft.setTextSize(1); 
  tft.println("Just tell Maggie that he  likes men");
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
  tft.println("1)Hack security network\n");
  if(1 == currentRowSelection){
    tft.print("->"); 
  }
  tft.println("2)Detect hidden sensor\n");
  if(2 == currentRowSelection){
    tft.print("->");
  }
  tft.println("3)Fuck Peter?\n");
}

void loop() {
  int joystickPosition = CheckJoystick();
  if(Neutral != joystickPosition){
    if(handleJoystickAction(joystickPosition)){
      drawMenu();
    }
  }
}

boolean handleJoystickAction(int joystickState){
  boolean updated = false;
  switch(joystickState){
    case Up:
      currentRowSelection -= 1;
      if(currentRowSelection == -1){
        currentRowSelection = maxRows[currentMenu] - 1;
      } 
      updated = true;
      break;
    case Down:
      currentRowSelection += 1;
      if(currentRowSelection == maxRows[currentMenu]){
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

boolean handleMenuSelection(){
  boolean updated = false;
  if(currentMenu == MainMenu){
    if(currentRowSelection == 0){
      currentMenu = HackSystem;
      updated = true;
    } else if (currentRowSelection == 1){
      currentMenu = DetectSensor;
      updated = true;
    } else if(currentRowSelection == 2){
      currentMenu = ScrewPeter;
      updated = true;
    }
  }
  return updated;
}

boolean handleMenuBack(){
  boolean updated = false;
  if(currentMenu == HackSystem || currentMenu == DetectSensor || currentMenu == ScrewPeter){
    currentMenu = MainMenu; 
    updated = true;
  }
  return updated;
}

// Check the joystick position
int CheckJoystick()
{
  int joystickState = analogRead(3);
  if (joystickState < 50) return Left;
  if (joystickState < 200) return Down;
  if (joystickState < 400) return Press;
  if (joystickState < 600) return Right;
  if (joystickState < 1000) return Up;
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

