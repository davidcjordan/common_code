#ifndef fault_h
#define fault_h

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#define FAULT_TABLE_LENGTH 16
#define FAULT_INDEX_TABLE_SIZE 255
#define XTRA_INFO_LENGTH 32

// the following struct is used by all components, although only the base may populate the location
typedef struct fault_table_entry {
   bool set;
	uint32_t code;  // a system unique error code
	uint32_t location;  // a system unique error code
	time_t  time;   // timestamp of when the fault occurred
	char xtra_info[XTRA_INFO_LENGTH];
} fault_table_entry_t;

#define FAULT_LOCATIONS 8
typedef struct fault_index_entry {
   uint8_t location[FAULT_LOCATIONS];
} fault_index_entry_t;

extern fault_index_entry_t fault_index_table[FAULT_INDEX_TABLE_SIZE];

uint8_t add_fault_entry(uint32_t code, uint8_t location, char * xtra);
void delete_fault_entry(uint32_t code, uint8_t location);
fault_table_entry_t * get_fault(uint32_t index);
void dump_fault_table(void);
uint8_t get_fault_count(void);
bool is_fault_code_set(uint32_t code);  //returns 0 if not set, 1 if set;  location is ignored

#define set_fault(code, loc) { \
	assert(code < FAULT_END); \
	assert(loc <= FAULT_LOCATIONS); \
	if (fault_index_table[code].location[loc] == 0) add_fault_entry(code, loc, NULL); \
}
#define set_fault_w_xtra(code, loc, xtra) { \
	assert(code < FAULT_END); \
	assert(loc <= FAULT_LOCATIONS); \
	if (fault_index_table[code].location[loc] == 0) add_fault_entry(code, loc, xtra); \
}
#define clear_fault(code, loc) { \
	assert(code < FAULT_END); \
	assert(loc <= FAULT_LOCATIONS); \
	if (fault_index_table[code].location[loc] != 0) delete_fault_entry(code, loc); \
}

/* the following macros generate a string per fault code, where the fault code is an enum
	refer to: https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c
	The following 2 macros allow printing the name of a define, instead of it's value
#define str(x) #x
#define xstr(x) str(x)
*/
#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

//-=-=- start of exported defines

#define FOREACH_FAULT(FAULT) \
	FAULT(FAULT_BEGIN) \
	FAULT(DEVICE_FAILURE_ON_INIT) \
	FAULT(CAM_FAILURE_ON_SET_MODE) \
	FAULT(CAM_FAILURE_ON_SET_CONTROL) \
	FAULT(CAM_FAILURE_TO_CAPTURE) \
	FAULT(CAM_FAILED_PARAMETER_LOAD) \
	FAULT(NOT_RECEIVING_FROM_NETWORK_DEVICE) \
	FAULT(UNRECOGNIZED_IP) \
	FAULT(UNSUPPORTED_COMMAND) \
	FAULT(PRIORITY_CAPABILITY_NOT_SET) \
	FAULT(UNRECOGNIZED_FAULT_CODE) \
	FAULT(UNRECOGNIZED_FAULT_LOCATION) \
	FAULT(NOT_RECEIVING_DATA_FROM_CAM) \
	FAULT(CAM_SET_GAIN_FAILED) \
	FAULT(CAM_SET_EXPOSURE_FAILED) \
	FAULT(CAMERAS_NOT_SYNCHRONIZED) \
	FAULT(I2C_ERROR_ON_INITIALIZATION) \
	FAULT(GPIO_ERROR_ON_INITIALIZATION) \
	FAULT(DAC_ERROR_ON_INITIALIZATION) \
	FAULT(DAC_WRITE_ERROR) \
	FAULT(GPIO_ACCESS_ERROR) \
	FAULT(ADC_ERROR_ON_INITIALIZATION) \
	FAULT(ADC_EXCESSIVE_READ_ERRORS) \
	FAULT(TACH_ERROR_ON_INITIALIZATION) \
	FAULT(TACH_EXCESSIVE_READ_ERRORS) \
	FAULT(BALL_SWITCH_STUCK_DOWN) \
	FAULT(BOTTOM_WHEEL_FAILED_TO_REACH_SPEED) \
	FAULT(TOP_WHEEL_FAILED_TO_REACH_SPEED) \
	FAULT(LEFT_RIGHT_FAILED_TO_REACH_TARGET) \
	FAULT(UP_DOWN_SERVO_FAILED_TO_REACH_TARGET) \
	FAULT(CONTROL_PROGRAM_NOT_RUNNING) \
	FAULT(CONTROL_PROGRAM_GET_STATUS_FAILED) \
	FAULT(CONTROL_PROGRAM_FAILED) \
	FAULT(NOT_TRACKING_BALL) \
	FAULT(READ_CAMERA_CONFIG_FILE_ERROR) \
	FAULT(CAMERA_VECTOR_GENERATION_FAILED) \
	FAULT(SETTING_CAMERA_EXPOSURE_FAILED) \
	FAULT(CAMERA_FOCAL_LENGTH_CALC_FAILED) \
	FAULT(READ_CONFIG_FILE_ERROR) \
	FAULT(NOT_PAIRED) \
	FAULT(NOT_CONNECTED) \
	FAULT(MOTOR_DRIVER_ERROR_ON_INITIALIZATION) \
	FAULT(MOTOR_DRIVER_EXCESSIVE_READ_ERRORS) \
	FAULT(MOTOR_CALIBRATION_FAILED) \
	FAULT(MOTOR_CONFIG_FILE_ERROR) \
	FAULT(MOTOR_NOT_DETECTED) \
	FAULT(FAULT_END)

#define FOREACH_NET_DEVICE(NET_DEVICE) \
	NET_DEVICE(LEFT) \
	NET_DEVICE(RIGHT) \
	NET_DEVICE(SPEAKER) \
	NET_DEVICE(BASE) \
	NET_DEVICE(TRACKING) \
	NET_DEVICE(UNRECOGNIZED_DEVICE)

//-=-=- end of exported defines

enum FAULT_ENUM {
    FOREACH_FAULT(GENERATE_ENUM)
};

enum NET_DEVICE_ENUM {
    FOREACH_NET_DEVICE(GENERATE_ENUM)
};

#endif //fault.h
