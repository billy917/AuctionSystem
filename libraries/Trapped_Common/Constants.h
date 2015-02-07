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

const int UID_LENGTH = 7;
const uint8_t MESSAGETYPEID_NFC = 6; //enum are ints
const uint8_t MESSAGETYPEID_NFC_MANAGE = 7;
const uint8_t MESSAGETYPEID_NFC_LOCK = 8;
const uint8_t MESSAGETYPEID_NFC_TOOL = 9;

const uint8_t MESSAGETYPEID_NFC_FOUNDEXPECTED = 1;
const uint8_t MESSAGETYPEID_NFC_NOTFOUND = 2;
const uint8_t MESSAGETYPEID_NFC_FOUNDTOOL = 3;
const uint8_t MESSAGETYPEID_NFC_REGISTER = 4; // detector registering with manager

const uint8_t MESSAGETYPEID_NFC_MANAGE_FOUND = 1;
const uint8_t MESSAGETYPEID_NFC_MANAGE_NOTFOUND = 2;
const uint8_t MESSAGETYPEID_NFC_MANAGE_REGISTER = 3; // register with primary manager
const uint8_t MESSAGETYPEID_NFC_MANAGE_REQUEST = 4; // PrimaryManager -> Manager
const uint8_t MESSAGETYPEID_NFC_MANAGE_STATUS = 5; // Manager -> PrimaryManager

const uint8_t MESSAGETYPEID_NFC_LOCK_LOCK = 1; 	// Manager -> Lock
const uint8_t MESSAGETYPEID_NFC_LOCK_UNLOCK = 2; // Manager -> Unlock
const uint8_t MESSAGETYPEID_NFC_LOCK_UNLOCK_5SEC = 3; // Manager -> Unlock 5 sec

const uint8_t MESSAGETYPEID_NFC_TOOL_REQUEST = 1; // Tool -> PrimaryManager
const uint8_t MESSAGETYPEID_NFC_TOOL_STATUS = 2; // PrimaryManager -> Tool

const uint8_t NFC_MESSAGE_MAX_SIZE = 9;
/*
NFC message protocol
 	Detector <-> Manager
	[0] messageTypeId (6)   
	[1] detectorId
	[2] parameter (1 = found, 2 = not found, 3 = found tool, 4 = register with Manager)

	Manager <-> Primary Manager
	[0] messageTypeId (7)
	[1] managerId
	[2] parameter (1 = found, 2 = not found, 3 = register with primary Manager, 4 = request status, 5 = status response)
	[3] detectorId (for 1 and 2)

	Primary Manager <-> Lock
	[0] messageTypeId (8)
	[1] parameter (1 = lock, 2 = unlock, 3 = unlock for 5 sec)
	
	Primary Manager <-> Tool
	[0] messageTypeId (9)
	[1] parameter (1 = request status, 2 = status response)
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

#endif