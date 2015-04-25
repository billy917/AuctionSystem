
#include <EEPROM.h>
#include <SPI.h>
#include <GD2.h>
#include "HackUtil.h"

//HackUtil util = HackUtil();

void setup()
{
  //util.init();
  GD.begin();
  GD.cmd_loadimage(0, 0); 
  GD.load("TrappedLogo.jpg");
}

void loop()
{
  GD.Clear();
  GD.Begin(BITMAPS); 
  GD.Vertex2ii(0, 0);
  GD.swap();
}
