//constants.h
#pragma once

//-=-=- start of exported defines

#define FEET_TO_MM 304.8
#define MM_TO_FEET .00328084
#define INCHES_TO_MM 25.4
#define MPH_TO_MM_PER_TICK 7.4506
#define MM_PER_TICK_TO_MPH .134217
#define MPH_TO_FEET_PER_SECOND (88.0 / 60.0)
#define MM_PER_TICK_TO_FEET_PER_SECOND (MM_PER_TICK_TO_MPH * MPH_TO_FEET_PER_SECOND)

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
