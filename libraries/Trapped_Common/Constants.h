/*
  Constants.h - Constants for Trapped! modules.
  Created by Billy Wong
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/
#ifndef Constants_h
#define Constants_h

const int NFC_MANAGER_I2C_ADDR = 100;

const int UID_LENGTH = 7;
const uint8_t MESSAGETYPEID_NFC = 6; //enum are ints
const uint8_t MESSAGETYPEID_NFC_FOUNDEXPECTED = 1;
const uint8_t MESSAGETYPEID_NFC_NOTFOUND = 2;
const uint8_t MESSAGETYPEID_NFC_FOUNDTOOL = 3;

/*
NFC message protocol
	[0] messageTypeId (6)
	[1] detectorId
	[2] parameter (1 = found, 2 = not found, 3 = found tool)

	
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