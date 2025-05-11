//constants.h
#pragma once

//-=-=- start of exported defines

#define BOOMER_SSID "BOOM_NET"
//#define FRAME_NUM_MAX (UINT16_MAX >> 5)
#define FRAME_NUM_MAX (INT32_MAX - 1)
#define FEET_TO_METERS .3048
#define FEET_TO_MM 304.8
#define MM_TO_FEET .00328084
#define INCHES_TO_MM 25.4
#define FEET_PER_SEC_TO_MPH (60/88.0)
#define FEET_PER_FRAME_TO_MPH (60*60/88.0)
// A tennis ball hit at 100 mph will be going roughly 73 mph 60 feet later
// This next value guesses the initial hit velocity based on 70 Feet between hit and bounce
// So if the bounce velocity is 50 mph, assuming it was hit from 70 feet away,
// it was going 1.47*50 = 73.5 mph when hit.
#define BOUNCE_VEL_TO_INIT 1.47
#define MPH_TO_MM_PER_FRAME 7.4506
#define MM_PER_FRAME_TO_MPH .134217
#define MPH_TO_FEET_PER_SECOND (88.0 / 60.0)
#define MM_PER_FRAME_TO_FEET_PER_SECOND (MM_PER_FRAME_TO_MPH * MPH_TO_FEET_PER_SECOND)

//2 previous defines to be deleted after merge:
#define MM_PER_TICK_TO_MPH .134217
#define MM_PER_TICK_TO_FEET_PER_SECOND (MM_PER_TICK_TO_MPH * MPH_TO_FEET_PER_SECOND)

#define HALF_BALL_SIZE_MM 33.5 /*  radius of tennis ball in millimeters. */
#define HALF_BALL_SIZE_FT .1099 // radius of tennis ball in feet.
#define RADIANS_TO_DEGREES 57.29577951
#define DEGREES_TO_RADIANS 0.017453293
// CAMERA STUFF:

// This define, FRAME_MICROS is the only value that needs to be set for the launcher
// code to match the camera set pulse rate.
// This is the only value that needs to be set for a particular camera set.
// All other values like FRAME_NANOS, FRAME_TIME, TICK_INTERVAL, INTEGRATION_INTERVAL,
// TWO_FRAME_TIME_MIN, TWO_FRAME_TIME_MAX, ERROR_CHECK_INTERVAL_MILLIS
//#define FRAME_MICROS 16682 May 10, 2025: Values for CCC camera set
//                     16655: May 10, 2025: Values for camera set #1
// 						  16652: May 11, 2025: Values for camera set #2
// CHANGE THIS ONE VALUE TO WORK WITH A DIFFERENT CAMERA SET. You can find the correct
// number of micros by doing a ssh left (from the base) and then typing "blog".
// This will show the typical value of a pulse in microseconds.
#define FRAME_MICROS 16652
#define FRAME_NANOS  1000*FRAME_MICROS
#define FRAME_TIME .000001*FRAME_MICROS

#define GRAVITY_IN_FEET_PER_SECOND_SQUARED 32.174
#define CX_MID (FRAME_WIDTH / 2 - 0.5)
#define CY_MID (FRAME_HEIGHT / 2 - 0.5)
#define PIXEL_SIZE_MM 0.003 /* pixel size is .003 mm. */

#define LEFT_DOUBLE_SIDELINE_X  -1371.6       // doubles sideline
#define LEFT_SINGLE_SIDELINE_X  0.0           // origin of X
#define RIGHT_SINGLE_SIDELINE_X 8226.6        // 27*FEET_TO_MM
#define RIGHT_DOUBLE_SIDELINE_X 9601.2        // 31.5*FEET_TO_MM
#define CENTER_SERVICE_LINE_X   4114.8        // 13.5*FEET_TO_MM

// Y values (depth along court, net is origin, towards Boomer is positive )
#define NET_Y                         0.0      // origin of Y, 0
#define BOOMER_SERVICE_LINE_Y      6400.8      // 21*FEET_TO_MM
#define BOOMER_BASELINE_Y         11887.2      // 39*FEET_TO_MM
#define PLAYER_BASELINE_Y		    -11887.2      // -39*FEET_TO_MM
#define PLAYER_SERVICE_LINE_Y     -6400.8      // -21*FEET_TO_MM

#define LEFT_WIDE_X             2057.4        // 6.75*FEET_TO_MM, 1/2 way between sideline and center
#define LEFT_THIRD_X            2743.2        // 9*FEET_TO_MM, 1/3 way across court
#define RIGHT_THIRD_X			  5486.4        // 18*FEET_TO_MM, 2/3 way across court
#define RIGHT_WIDE_X            6172.2        // 20.25*FEET_TO_MM, 1/2 way between center and sideline
#define VERY_DEEP_Y				  9144.0         // 30*FEET_TO_MM

// TOTAL_Y is the # of feet from where the ball leaves the wheels
// to the far baseline. It assume the center of the wheel is one foot
// behind the near baseline. total_y=1+39+39=79 feet
#define TOTAL_Y 79


#define FOREACH_COURT_POINT(COURT_POINT) \
	COURT_POINT(FBL) \
	COURT_POINT(FBC) \
	COURT_POINT(FBR) \
	COURT_POINT(NBL) \
	COURT_POINT(NBC) \
	COURT_POINT(NBR) \
	COURT_POINT(NSR) \
	COURT_POINT(NSC) \
	COURT_POINT(NSL)
#define NUM_COURT_POINTS 9

#define NUM_CAMS 2
#define FRAME_WIDTH 1280
#define FRAME_HEIGHT 800
#define TOTPIX (FRAME_WIDTH * FRAME_HEIGHT)

#define NUM_2D_AXIS 2
#define FOREACH_AXIS_NAME(AXIS_NAME) \
	AXIS_NAME(X) \
	AXIS_NAME(Y) \
	AXIS_NAME(Z)
#define NUM_3D_AXIS 3

#define FOREACH_FRAME_TYPE(FRAME_TYPE_NAME) \
	FRAME_TYPE_NAME(EVEN) \
	FRAME_TYPE_NAME(ODD)

#define FOREACH_BLOB_TYPE(BLOB_TYPE_NAME) \
	BLOB_TYPE_NAME(BALL_BLOB) \
	BLOB_TYPE_NAME(PLAYER_BLOB) \
	BLOB_TYPE_NAME(SPECTATOR_BLOB) \
	BLOB_TYPE_NAME(TEST_BLOB)

//-=-=- end of exported defines

typedef struct point { float axis[2];} point_t;
typedef struct line { point_t * end[2]; } line_t;

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum FRAME_TYPE_ENUM {
    FOREACH_FRAME_TYPE(GENERATE_ENUM)
};

enum COURT_POINT_ENUM {
    FOREACH_COURT_POINT(GENERATE_ENUM)
};

enum COURT_AXIS_NAME_ENUM {
    FOREACH_AXIS_NAME(GENERATE_ENUM)
};

enum BLOB_TYPE_ENUM {
    FOREACH_BLOB_TYPE(GENERATE_ENUM)
};


/*
// The following is not necessary - defined by NET_DEVICE in faults.h
#define FOREACH_CAMERA_NAME(CAMERA_NAME) \
	CAMERA_NAME(LEFT) \
	CAMERA_NAME(RIGHT)
*/

#ifndef MIN
#  define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#  define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

