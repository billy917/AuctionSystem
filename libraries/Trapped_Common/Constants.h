/*
  Constants.h - Constants for Trapped! modules.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef Constants_h
#define Constants_h

const int NFC_MANAGER_I2C_ADDR = 100; // NFCManager, Laser2, Laser1

const int UID_LENGTH = 7;
const uint8_t MESSAGETYPEID_NFC = 6; //enum are ints
const uint8_t MESSAGETYPEID_NFC_MANAGE = 7;

const uint8_t MESSAGETYPEID_NFC_FOUNDEXPECTED = 1;
const uint8_t MESSAGETYPEID_NFC_NOTFOUND = 2;
const uint8_t MESSAGETYPEID_NFC_FOUNDTOOL = 3;
const uint8_t MESSAGETYPEID_NFC_REGISTER = 4; // detector registering with manager

const uint8_t MESSAGETYPEID_NFC_MANAGE_FOUND = 1;
const uint8_t MESSAGETYPEID_NFC_MANAGE_NOTFOUND = 2;

/*
NFC message protocol
 	Detector <-> Manager
	[0] messageTypeId (6)   
	[1] detectorId
	[2] parameter (1 = found, 2 = not found, 3 = found tool)

	Manager <-> Lock
	[0] messageTypeId (7)
	[1] managerId
	[2] parameter (1 = all found, 2 = not found)
	
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