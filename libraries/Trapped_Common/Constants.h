/*
  Constants.h - Constants for Trapped! modules.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef Constants_h
#define Constants_h

const int NFC_MANAGER_I2C_ADDR = 100; // NFCManager, Laser2, Laser1
	// 101 - 105 == NFC detectors
const int SHELF_LOCK_I2C_ADDR = 106;
const int BGM_I2C_ADDR = 107;

	// hardware I2C address
	// - 7-Segment LED 0x70 -- 112
	// - Light sensor 0x29, 0x39, 0x49  -- 41, 57, 73
	// - LCD - 0x3F -- 63

const int UID_LENGTH = 7;
const uint8_t MESSAGETYPEID_NFC = 6; //enum are ints
const uint8_t MESSAGETYPEID_NFC_MANAGE = 7;
const uint8_t MESSAGETYPEID_NFC_LOCK = 8;
const uint8_t MESSAGETYPEID_NFC_TOOL = 9;
const uint8_t MESSAGETYPEID_LASER_SENSOR = 10;
const uint8_t MESSAGETYPEID_LASER_CONTROL = 11;
const uint8_t MESSAGETYPEID_BGM = 12;

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

const uint8_t MESSAGETYPEID_NFC_LOCK_LOCK = 1; 	// Manager -> Lock
const uint8_t MESSAGETYPEID_NFC_LOCK_UNLOCK = 2; // Manager -> Unlock
const uint8_t MESSAGETYPEID_NFC_LOCK_UNLOCK_5SEC = 3; // Manager -> Unlock 5 sec

const uint8_t MESSAGETYPEID_NFC_TOOL_REQUEST = 1; // Tool -> PrimaryManager
const uint8_t MESSAGETYPEID_NFC_TOOL_STATUS = 2; // PrimaryManager -> Tool
const uint8_t MESSAGETYPEID_NFC_TOOL_DEBUG = 3; // Tool -> PrimaryManager set DebugMode

const uint8_t MESSAGETYPEID_LASER_SENSOR_REQUEST = 1; // Tool -> Laser1
const uint8_t MESSAGETYPEID_LASER_SENSOR_STATUS = 2; // Laser1 -> Tool

const uint8_t MESSAGETYPEID_LASER_SENSOR_ON = 1;  // x -> LaserController
const uint8_t MESSAGETYPEID_LASER_SENSOR_OFF = 2; // x -> LaserController

const uint8_t MESSAGETYPEID_BGM_UPDATE = 1;
const uint8_t MESSAGETYPEID_BGM_PLAY = 2;
const uint8_t MESSAGETYPEID_BGM_STOP = 3;
const uint8_t MESSAGETYPEID_BGM_NEXT = 4;

const uint8_t NFC_MESSAGE_MAX_SIZE = 9;
const int GLOBAL_LASER_MANAGER_ID[] = {0,0,0,1,1,1,2,2,2};
/*
Laser Control message protocol
	x -> LaserController
	[0] messageTypeId (11)
	[1] detailedMessageType (1 = on, 2 = off)
	[2] laserId
*/

/*
LASER Sensor message protocol
	Manager <-> Tool
	[0] messageTypeId (10)
	[1] detailedMessageType (1 = enable debug, 2 = request status, 3 = status response, 4 = trip event)
	[2] sensorId
	[3] sensorState (only for detailedMessageType #2) 

	Manager <-> PrimaryManager (next to clock)
	[0] messageTypeId (10)
	[1] detailedMessageType (1 = tripped laser)
	[2] sensorId	
*/

/*
NFC message protocol
 	Detector <-> Manager
	[0] messageTypeId (6)   
	[1] detectorId
	[2] parameter (1 = found, 2 = not found, 3 = found tool, 4 = register with Manager)

	Manager <-> Primary Manager
	[0] messageTypeId (7)
	[1] managerId
	[2] parameter (1 = found, 2 = not found, 3 = register with primary Manager, 4 = request status, 5 = status response, 6 = set debug mode)
	[3] detectorId (for 1 and 2)

	Primary Manager <-> Lock
	[0] messageTypeId (8)
	[1] parameter (1 = lock, 2 = unlock, 3 = unlock for 5 sec)
	
	Primary Manager <-> Tool
	[0] messageTypeId (9)
	[1] parameter (1 = request status, 2 = status response, 3 = set debug mode)
	[2] data (populated for messageType 2)
			uint8_t numDetectors, [uint8_t detectorId, uint8_t state]  - state:{0,1}

	Max Size of 9 uint8_t

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

*/

#endif
