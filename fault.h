#ifndef fault_h
#define fault_h

#include <stdint.h>
#include <time.h>

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
void dump_fault_table();

#define set_fault(code, loc) if (fault_index_table[code].location[loc] == 0) add_fault_entry(code, loc, NULL);
#define set_fault_w_xtra(code, loc, xtra) if (fault_index_table[code].location[loc] == 0) add_fault_entry(code, loc, xtra);
#define clear_fault(code, loc) if (fault_index_table[code].location[loc] != 0) delete_fault_entry(code, loc);

/* the following macros generate a string per fault code, where the fault code is an enum
	refer to: https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c
	The following 2 macros allow printing the name of a define, instead of it's value
#define str(x) #x
#define xstr(x) str(x)
*/

#define FOREACH_FAULT(FAULT) \
	FAULT(FAULT_BEGIN) \
	FAULT(DEVICE_FAILURE_ON_INIT) \
	FAULT(CAM_FAILURE_ON_SET_MODE) \
	FAULT(CAM_FAILURE_ON_SET_CONTROL) \
	FAULT(CAM_FAILURE_TO_CAPTURE) \
	FAULT(CAM_FAILED_PARAMETER_LOAD) \
	FAULT(NOT_RECEIVING_FROM_DEVICE) \
	FAULT(UNRECOGNIZED_IP) \
	FAULT(UNSUPPORTED_COMMAND) \
	FAULT(PRIORITY_CAPABILITY_NOT_SET) \
	FAULT(UNRECOGNIZED_FAULT_CODE) \
	FAULT(UNRECOGNIZED_FAULT_LOCATION) \
	FAULT(FAULT_END)

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum FAULT_ENUM {
    FOREACH_FAULT(GENERATE_ENUM)
};

// the following was moved to fault.c to prevent unused variable warning for each module that includes fault.g
// static const char *FAULT_STRING[] = {
//     FOREACH_FAULT(GENERATE_STRING)
// };

/*
The following code be used to print a string for location, but locations are fault specific
#define FOREACH_DEVICE(DEVICE) \
	DEVICE(NA) \
	DEVICE(BASE) \
	DEVICE(LEFT_CAM) \
	DEVICE(RIGHT_CAM) \
	DEVICE(SPEAKER)

enum DEVICE_ENUM {
    FOREACH_DEVICE(GENERATE_ENUM)
};

static const char *DEVICE_STRING[] = {
    FOREACH_DEVICE(GENERATE_STRING)
};
*/


#endif //fault.h
