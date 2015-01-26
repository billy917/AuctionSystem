//Coordinator xBee address - 40b9df66

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

volatile int joystickState = 0;
void setup(void) {  
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(3);
}

void drawMenu(){
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(0,0);
  tft.println(joystickState);
  
}

void loop() {
  int joystickPosition = CheckJoystick();
  drawMenu();
}

// Check the joystick position
int CheckJoystick()
{
  joystickState = analogRead(3);
  if (joystickState < 50) return Left;
  if (joystickState < 250) return Down;
  if (joystickState < 400) return Press;
  if (joystickState < 600) return Right;
  if (joystickState < 1020) return Up;
  return Neutral;
}


