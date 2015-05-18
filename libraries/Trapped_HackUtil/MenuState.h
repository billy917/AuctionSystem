/*
  MenuState.h - library for changing different screens
  Copyright 2014 - Unauthorized users cannot run, copy, distribute, study, change, modify, and/or improve the software.
*/

#define SCREEN_SPLASH 0
#define SCREEN_USER 1
#define SCREEN_PREVIOUS 9


/* Admin User screen state and touch tags */
#define SCREEN_ADMIN_LOCK 11 // enable/disable Locks
#define TAG_LOCK_INWALL 111 // in_wall_safe_lock
#define TAG_LOCK_MAIN_DOOR 112 // main_door_lock
#define TAG_LOCK_SHELF 113 // shelf_lock
#define TAG_A14 114

/* Enable/Disable Lasers */
#define SCREEN_ADMIN_LASER 12
#define TAG_LASER_ALL 120 // all_lasers
#define TAG_LASER_1 121 // laser_1
#define TAG_LASER_2 122 // laser_2
#define TAG_LASER_3 123 // laser_3
#define TAG_LASER_4 124 // laser_4
#define TAG_LASER_5 125 // laser_5
#define TAG_LASER_6 126 // laser_6
#define TAG_LASER_7 127 // laser_7
#define TAG_LASER_8 128 // laser_8
#define TAG_LASER_9 129 // laser_9

/* Enable/Disable Sensors */
#define SCREEN_ADMIN_SENSOR 13
#define TAG_SENSOR_ALL 130 // all_sensors
#define TAG_SENSOR_1 131 // sensor_1
#define TAG_SENSOR_1 132 // sensor_2
#define TAG_SENSOR_3 133 // sensor_3
#define TAG_SENSOR_4 134 // sensor_4
#define TAG_SENSOR_5 135 // sensor_5
#define TAG_SENSOR_6 136 // sensor_6
#define TAG_SENSOR_7 137 // sensor_7
#define TAG_SENSOR_8 138 // sensor_8
#define TAG_SENSOR_9 139 // sensor_9


/* 10 most recent received Commands */
#define SCREEN_ADMIN_LOG 14

/* Modify Timer values */
#define SCREEN_ADMIN_TIMER 15

/* Normal User screen state and touch tags */
#define SCREEN_USER_HACK_SYSTEM 20
#define SCREEN_USER_HACK_FAIL 21
#define SCREEN_USER_SERVER 200
#define SCREEN_USER_SERVER_ACCESS 210
#define TAG_DOWNLOAD_FLOORPLAN 211
#define SCREEN_USER_HACK_LASER 220
#define SCREEN_USER_FILE 21
#define SCREEN_USER_FLOORPLAN 22
