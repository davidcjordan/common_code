#ifndef fault_h
#define fault_h

#include <stdint.h>
#include <time.h>

#define FAULT_INDEX_TABLE_SIZE 255

// the following struct is used by all components, although only the base may populate the location
typedef struct fault_table_entry {
   bool set;
	uint32_t code;  // a system unique error code
	uint32_t location;  // a system unique error code
	time_t  time;   // timestamp of when the fault occurred
} fault_table_entry_t;

typedef struct fault_index_entry {
   uint8_t location[4];
} fault_index_entry_t;

fault_index_entry_t fault_index_table[FAULT_INDEX_TABLE_SIZE];

uint8_t add_fault_entry(uint32_t code, uint8_t location);
void delete_fault_entry(uint32_t code, uint8_t location);
fault_table_entry_t * get_fault(uint32_t index);
void dump_fault_table();

#define set_fault(code, loc) if (fault_index_table[code].location[loc] == 0) add_fault_entry(code, loc);

#define clear_fault(code, loc) if (fault_index_table[code].location[loc] != 0) delete_fault_entry(code, loc);

// there should be a way of automating fault_name generation - use enums?
// refer to: https://stackoverflow.com/questions/9907160/how-to-convert-enum-names-to-string-in-c
#define str(x) #x
#define xstr(x) str(x)

#define DEVICE_FAILURE_ON_INIT 1
#define CAM_FAILURE_ON_SET_MODE 2
#define CAM_FAILURE_ON_SET_CONTROL 3
#define CAM_FAILURE_TO_CAPTURE 4
#define CAM_FAILED_PARAMETER_LOAD 5
#define NOT_RECEIVING_FROM_DEVICE 6
#define UNSUPPORTED_COMMAND 7

//when defining another fault, add to the fault name table in fault.c
extern const char fault_name[FAULT_INDEX_TABLE_SIZE][64];
#endif //fault.h
