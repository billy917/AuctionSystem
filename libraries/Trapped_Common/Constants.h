/*
  Constants.h - Constants for Trapped! modules.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef Constants_h
#define Constants_h

const int NFC_MANAGER_I2C_ADDR = 100; // NFCManager, Laser2, Laser1
	// 101 - 105 == NFC detectors
const int LOCK_MANAGER_I2C_ADDR = 106;
const int BGM_I2C_ADDR = 107;
const int KEYPAD_LOCK_I2C_ADDR = 108; //SafeKeypad
const int KEYPAD_I2C_ADDR = 109;
const int LASER_SENSOR_I2C_ADDR = 110;
const int TRIP_LASER_I2C_ADDR = 111;
/* NOTE: SKIP I2C_ADDR 112, see below */
const int SOUNDFX_I2C_ADDR = 113;
const int CLOCK_I2C_ADDR = 114; //same as keypad on purpose
const int PIANO_LOCK_I2C_ADDR = 115;
const int WIRELESS_I2C_ADDR = 116;


	// hardware I2C address
	// - 7-Segment LED 0x70 -- 112
	// - Light sensor 0x29, 0x39, 0x49  -- 41, 57, 73
	// - LCD - 0x3F -- 63

const int UID_LENGTH = 7;
const uint8_t MESSAGETYPEID_NFC = 6; //enum are ints
const uint8_t MESSAGETYPEID_NFC_MANAGE = 7;
const uint8_t MESSAGETYPEID_LOCK = 8;
const uint8_t MESSAGETYPEID_NFC_TOOL = 9;
const uint8_t MESSAGETYPEID_LASER_SENSOR = 10;
const uint8_t MESSAGETYPEID_LASER_CONTROL = 11;
const uint8_t MESSAGETYPEID_BGM = 12;
const uint8_t MESSAGETYPEID_CLOCK = 13;
const uint8_t MESSAGETYPEID_KEYPAD_LOCK = 14; //SafeKeyPad
const uint8_t MESSAGETYPEID_TRIP_LASER = 15;
const uint8_t MESSAGETYPEID_LCD = 16;
const uint8_t MESSAGETYPEID_GAME = 17;

const uint8_t MESSAGETYPEID_NFC_FOUNDEXPECTED = 1;
const uint8_t MESSAGETYPEID_NFC_NOTFOUND = 2;
const uint8_t MESSAGETYPEID_NFC_FOUNDTOOL = 3;
const uint8_t MESSAGETYPEID_NFC_REGISTER = 4; // detector registering with manager

const uint8_t MESSAGETYPEID_NFC_MANAGE_FOUND = 1;
const uint8_t MESSAGETYPEID_NFC_MANAGE_NOTFOUND = 2;
const uint8_t MESSAGETYPEID_NFC_MANAGE_REGISTER = 3; // register with primary manager
const uint8_t MESSAGETYPEID_NFC_MANAGE_REQUEST = 4; // PrimaryManager -> Manager
const uint8_t MESSAGETYPEID_NFC_MANAGE_STATUS = 5; // Manager -> PrimaryManager
const uint8_t MESSAGETYPEID_NFC_MANAGE_DEBUG = 6; // PrimaryManager -> set DebugMode

const uint8_t MESSAGETYPEID_LOCK_LOCKID_SHELF = 1;
const uint8_t MESSAGETYPEID_LOCK_LOCKID_INWALL = 2;
const uint8_t MESSAGETYPEID_LOCK_LOCKID_WHITEDOOR = 3;
const uint8_t MESSAGETYPEID_LOCK_LOCKID_MAINDOOR = 4;

const uint8_t MESSAGETYPEID_LOCK_LOCK = 1; 	// Manager -> Lock
const uint8_t MESSAGETYPEID_LOCK_UNLOCK = 2; // Manager -> Unlock
const uint8_t MESSAGETYPEID_LOCK_UNLOCK_5SEC = 3; // Manager -> Unlock 5 sec

const uint8_t MESSAGETYPEID_NFC_TOOL_REQUEST = 1; // Tool -> PrimaryManager
const uint8_t MESSAGETYPEID_NFC_TOOL_STATUS = 2; // PrimaryManager -> Tool
const uint8_t MESSAGETYPEID_NFC_TOOL_DEBUG = 3; // Tool -> PrimaryManager set DebugMode


const uint8_t MESSAGETYPEID_LASER_SENSOR_REQUEST = 1; // Tool -> Sensor1
const uint8_t MESSAGETYPEID_LASER_SENSOR_STATUS = 2; // Sensor1 -> Tool
const uint8_t MESSAGETYPEID_LASER_SENSOR_DEBUG = 3; // Tool -> Sensor1

const uint8_t MESSAGETYPEID_LASER_SENSOR_ON = 5;  // x -> SensorController
const uint8_t MESSAGETYPEID_LASER_SENSOR_OFF = 6; // x -> SensorController
const uint8_t MESSAGETYPEID_LASER_SENSOR_CALIBRATE = 7;

const uint8_t MESSAGETYPEID_LASER_CONTROL_ON = 1;  // Tool -> Laser1
const uint8_t MESSAGETYPEID_LASER_CONTROL_OFF = 2; // Tool -> Laser1
const uint8_t MESSAGETYPEID_LASER_CONTROL_ON_ALL = 3; // GameController -> Laser1
const uint8_t MESSAGETYPEID_LASER_CONTROL_OFF_ALL = 4; // GameController -> Laser1
const uint8_t MESSAGETYPEID_LASER_CONTROL_ON_ALL_NO_SENSOR = 5; // GameController -> Laser1
const uint8_t MESSAGETYPEID_LASER_CONTROL_ON_ALL_ACTIVE = 6; // GameController -> Laser1
const uint8_t MESSAGETYPEID_LASER_CONTROL_OFF_ALL_ACTIVE = 7;
const uint8_t MESSAGETYPEID_LASER_CONTROL_OFF_MODE_1 = 8;

const uint8_t MESSAGETYPEID_BGM_UPDATE = 1;
const uint8_t MESSAGETYPEID_BGM_PLAY_SONG = 2;
const uint8_t MESSAGETYPEID_BGM_STOP_SONG = 3;
const uint8_t MESSAGETYPEID_BGM_NEXT_SONG = 4;

const uint8_t MESSAGETYPEID_KEYPAD_LOCK_RESET = 1; // SafeKeyPad
const uint8_t MESSAGETYPEID_KEYPAD_LOCK_GET_PASSWORD = 2;

const uint8_t MESSAGETYPEID_LCD_CHANGE_EQUATION = 1;
const uint8_t MESSAGETYPEID_LCD_CHANGE_PATTERN = 2;

const uint8_t MESSAGETYPEID_TRIP_LASER_ON = 1;
const uint8_t MESSAGETYPEID_TRIP_LASER_OFF = 2;
const uint8_t MESSAGETYPEID_TRIP_LASER_BEEP = 3;


const uint8_t MESSAGETYPEID_CLOCK_RESET = 1;
const uint8_t MESSAGETYPEID_CLOCK_START = 2;
const uint8_t MESSAGETYPEID_CLOCK_PAUSE = 3;
const uint8_t MESSAGETYPEID_CLOCK_MODIFY_ADD = 4;
const uint8_t MESSAGETYPEID_CLOCK_MODIFY_SUBTRACT = 5;
const uint8_t MESSAGETYPEID_CLOCK_PLAY_LAST_TRACK = 6;
const uint8_t MESSAGETYPEID_CLOCK_STOP_LAST_TRACK = 7;
const uint8_t MESSAGETYPEID_CLOCK_STOP = 8;

const uint8_t MESSAGETYPEID_GAME_INIT_LASER_CONFIG = 1;
const uint8_t MESSAGETYPEID_GAME_INIT_SENSOR_CONFIG = 2;

const uint8_t NFC_MESSAGE_MAX_SIZE = 9;
const uint8_t I2C_MESSAGE_MAX_SIZE = 9; // should be same as NFC Message Max Size

const int LOCK_MANAGER_SHELF_PIN = 12;
const int LOCK_MANAGER_INWALL_PIN = 11;
const int LOCK_MANAGER_WDOOR_PIN = 10;
const int LOCK_MANAGER_MDOOR_PIN = 9;
const int GLOBAL_LASER_ID[9] = {1,2,3,4,5,6,7,8,9};
const int GLOBAL_LASER_MANAGER_ID[9] = {0,0,0,1,1,1,2,2,2};
const int GLOBAL_SENSOR_ID[9] = {6,3,2,4,1,5,7,8,9};
const int GLOBAL_SENSOR_MANAGER_ID[9] = {1,0,0,1,0,1,2,2,2};
const bool GLOBAL_ENABLE_LASER[9] = {true,true,true,true,true,true,true,true,true}; // if laser is false, then sensor will not turn on
const bool GLOBAL_ENABLE_SENSOR[9] = {true,true,true,true,true,true,true,true,true};
const int SENSOR_TRESHOLD[9] = {5,10,10,10,15,25,5,15,15}; 
	// 7,8,9 - 40,10,40
const bool AUTO_CALIBRATE_SENSOR[9] = {false,false,false,false,false,false,false,false,false};
const bool GLOBAL_LASER_PATTERN_1[9] = {true, true, true, false, true, false, false, true, true};
/*
1 151
2 122
3
4 15, 58
5 45, 48
6
7 89, 77
8 136, 23, 193
9 82, 118
*/
const int CLOCK_MODE_PAUSE = 1;
const int CLOCK_MODE_COUNTDOWN = 2;

const uint8_t SENSOR_STATE_NONE = 0;
const uint8_t SENSOR_STATE_INIT = 1;
const uint8_t SENSOR_STATE_ON = 2;
const uint8_t SENSOR_STATE_OFF = 3;
const uint8_t SENSOR_STATE_CANNOT_READ_DATA = 4;
const uint8_t SENSOR_STATE_CANNOT_SET_INTERRUPT_CONTROL = 5;
const uint8_t SENSOR_STATE_CANNOT_SET_THRESHOLD = 6;
const uint8_t SENSOR_STATE_CANNOT_CLEAR_INTERRUPT = 7;

const uint8_t GAME_STATE_RESTART = 1;
const uint8_t GAME_STATE_RUNNING = 2;
const uint8_t GAME_STATE_PAUSED = 3;
const uint8_t GAME_STATE_STOPPED = 4;

/*
Game Control init config protocol
	[0] messageTypeId (MESSAGETYPEID_GAME_INIT_LASER_CONFIG)
	[1] detailedMessageType (1 = Laser config, 2 = Sensor config)
	[2] laserId [1..9]
	[3] enabled [0,1]

/*
Clock Control message protocol
	[0] messageTypeId (13)
	[1] detailedMessageType (1 = reset, 2 = start, 3 = pause, 4 = add time, 5 = subtract time)
	[2] quantity (1)
	[3] sensorId (1)
*/

/*
Lock Control message protocol
	x -> LockController
	[0] messageTypeId (8)
	[1] lockType (1 = Shelf, 2 = inwall, 3 = white door, 4 = main door)
	[2] action (1 = lock, 2 = unlock, 3 = unlock for 5sec)
*/

/*
Laser Control message protocol
	x -> LaserController
	[0] messageTypeId (11)
	[1] detailedMessageType (1 = on, 2 = off)
	[2] laserId
*/

/*
LASER Sensor message protocol
	LaserSensorController <-> Tool
	[0] messageTypeId (10)
	[1] detailedMessageType (1 = enable debug, 2 = request status, 3 = status response, 4 = trip event)
	[2] sensorId
	[3] sensorState (only for detailedMessageType #2) 

	LaserSensorController <- LaserController 
	[0] messageTypeId (10)
	[1] detailedMessageType (5 = Calibrate & ON, 6 = OFF, 7 = request to calibrate)
	[2] sensorId
*/

/*
NFC message protocol
 	Detector <-> Manager
	[0] messageTypeId (6)   
	[1] detectorId
	[2] parameter (1 = found, 2 = not found, 3 = found tool, 4 = register with Manager)
	[3] NFC chip value

	Manager <-> Primary Manager
	[0] messageTypeId (7)
	[1] managerId
	[2] parameter (1 = found, 2 = not found, 3 = register with primary Manager, 4 = request status, 5 = status response, 6 = set debug mode)
	[3] detectorId (for 1 and 2)
	[4] NFC chip value

	Primary Manager <-> Lock
	[0] messageTypeId (8)
	[1] parameter (1 = lock, 2 = unlock, 3 = unlock for 5 sec)
	
	Primary Manager <-> Tool
	[0] messageTypeId (9)
	[1] parameter (1 = request status, 2 = status response, 3 = set debug mode)
	[2] data (populated for messageType 2)
			uint8_t numDetectors, [uint8_t state] 

	Max Size of 9 uint8_t

*/

/*
BGM Controller message protocol
    BGM Controller <- SafeKeyPad
    [0] messageTypeId (12)
    [1] parameter (1 = update, 2 = play song, 3 = stop song, 4 = next song)

    BGM Controller -> SafeKeyPad
    [0] messageTypeId (14)
    [1] parameter (2 = get password)
    [2] password[0]
    [3] password[1]
    [4] password[2]
    [5] password[3]
*/

/*
SafeKeyPad Controller message protocol
    x -> SafeKeyPad
    [0] messageTypeId (14)
    [1] parameter (1 = reset keypad, 2 = get password from BGM)
    
    SafeKeyPad -> LCD Controller
    [0] messageTypeId (16)
    [1] parameter (1 = request change equation, 
        2 = notify pattern changed)
*/

/*
HackTool1
0x04 0x8B 0x82 0xEA 0xC2 0x23 0x80

Pic 1 - 0x04 0x8A 0x82 0xEA 0xC2 0x23 0x80
Pic 2 - 0x04 0xEC 0x82 0xEA 0xC2 0x23 0x80
Pic 3 - 0x04 0x04 0x82 0xEA 0xC2 0x23 0x81
Pic 4 - 0x04 0xA2 0x82 0xEA 0xC2 0x23 0x80
Pic 5 - 0x04 0xF6 0x87 0x82 0x1C 0x23 0x80
*/

/*
XBee
0x0013a200, 0x40c04f18 - NFC Manager
0x0013a200, 0x40c337e0 - laser3
0x0013a200, 0x40c04ef1 - laser2
0x0013a200, 0x40c04edf - laser1
0x0013a200, 0x40b9df66 - Tool
0x0013a200, 0x40cab3f1 - Sensor1 (Diamond wall)
0x0013a200, 0x40bef834 - Sensor2 (Window wall)
0x0013a200, 0x40bf36b4 - Sensor3 (Claustrophobia wall)

*/

#endif
