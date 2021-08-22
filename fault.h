#ifndef fault_h
#define fault_h

#include <stdint.h>
#include <time.h>

#define FAULT_LOCATION_SIZE 16
#define FAULT_DESCRIPTION_SIZE 80
#define FAULT_TABLE_LENGTH 3

// the following struct is used by all components, although only the base may populate the location
typedef struct fault {
   bool set;
	char location[FAULT_LOCATION_SIZE]; // which component failed, e.g. left camera, which motor, etc.
	uint32_t code;  // a system unique error code
	time_t  time;   // timestamp of when the fault occurred
	char description[FAULT_DESCRIPTION_SIZE];  // a string describing the error; could be replaced by an error code lookup
	uint32_t sub_code;  // if the error has an additional error info (like the code returned on a function call)
} fault_t;

void add_fault(char * location, uint32_t code, uint32_t sub_code);
fault_t * get_fault(uint32_t index);
void dump_faults();

#define FAULT_TABLE_EMPTY 2
#define CAM_FAILURE_ON_INIT 1001
#define CAM_FAILURE_ON_SET_MODE 1002
#define CAM_FAILURE_ON_SET_CONTROL 1003
#define CAM_FAILURE_TIMEOUTS 1004
#define CAM_FAILED_PARAMETER_LOAD 1005


#endif //fault.h
