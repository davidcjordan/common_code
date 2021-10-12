//fault.c

/*
The fault table is a list of active alarms and when they were set.  It is for use by the equipment operator.
Adding and deleting faults requires iterating over the entries in the table to find a empty or
or to find the right fault to clear.

Setting/clearing a fault can occur many times a second, since the detector probably does not have the state
of whether it is aleady set/cleared.

Instead of the state of whether the fault is set/cleared implemented in every detector, it is implemented in
the fault code using the fault_index_table.  The fault index table is referenced by the fault_code (an integer).
This make it fast to detect whether an entry in the fault table has already been created.

The reason to have this fast is because the detectors may check for a condition multiple times a second.

The detcectors will use a macro, e.g. set/clear_fault which will add an entry to the fault_table 
if it doesn't exist (set_fault) or delete if if it does exist (clear_fault)

Because some of the components of the system have 2 instances (cameras, servo-wheels), then a location integer
is included in order to not create a duplicate set of faults, For example, there is only one DEVICE_FAILURE_ON_INIT,
but there are at least 3 instances (2 cameras and a speaker)

*/

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "fault.h"
#include "string.h"
#include "logging.h"

static const char *FAULT_STRING[] = {
    FOREACH_FAULT(GENERATE_STRING)
};

#define FAULT_TABLE_LENGTH 16
fault_table_entry_t fault_table[FAULT_TABLE_LENGTH] = {0}; //table of active faults
fault_index_entry_t fault_index_table[FAULT_INDEX_TABLE_SIZE] = {0}; //used for fast lookup if fault is active


uint8_t add_fault_entry(uint32_t code, uint8_t loc, char * xtra)
{
   bool fault_added = false;
   bool dup_fault = false;
   if (code >= FAULT_END) {
      LOG_ERROR("Out-of-range fault code: %d; highest code is: %d", code, FAULT_END);
      return 0;
   }
   if (loc > FAULT_LOCATIONS) {
      LOG_ERROR("Out-of-range fault location: %d; highest location is: %d", loc, FAULT_LOCATIONS);
      return 0;
   }
   uint8_t i;
   // check if fault is already in the table
   for (i = 1; ((i < FAULT_TABLE_LENGTH) && !dup_fault); i++) 
   {
      if ((fault_table[i].set == true) && (fault_table[i].code == code) && (fault_table[i].location == loc) )
      {
         LOG_WARNING("Duplicate fault on add: %s, location: %d; fault table index: %d", FAULT_STRING[code], loc, i);
         dup_fault = true;
      }
   }
   if (!dup_fault)
   {
      for (i = 1; ((i < FAULT_TABLE_LENGTH) && !fault_added); i++)
      { 
         if (fault_table[i].set == false)
         {
            LOG_INFO("Adding fault: %s, location: %d; fault table index: %d", FAULT_STRING[code], loc, i);
            fault_table[i].set = true;
            fault_table[i].code = code;
            fault_table[i].location = loc;
            fault_table[i].time = time(NULL);
            fault_table[i].xtra_info[0] = '\0';
            if (xtra != NULL) strncpy(fault_table[i].xtra_info, xtra, XTRA_INFO_LENGTH-1);
            fault_added = true;
            fault_index_table[code].location[loc] = i;
         } 
      }
      if (!fault_added) LOG_ERROR("fault %s, location %d not added; fault table full!", FAULT_STRING[code], loc);
   }
   return i;
}

void delete_fault_entry(uint32_t code, uint8_t loc)
{
   if (code >= FAULT_END) {
      LOG_ERROR("Out-of-range fault code: %d; highest code is: %d", code, FAULT_END);
      return;
   }
   if (loc > FAULT_LOCATIONS) {
      LOG_ERROR("Out-of-range fault location: %d; highest location is: %d", loc, FAULT_LOCATIONS);
      return;
   }
   struct tm ts = {0};
   char buf[32];
   bool fault_deleted = false;
   int i;
   for (i = 1; i < FAULT_TABLE_LENGTH && !fault_deleted; i++) 
   {
      if ((fault_table[i].set == true) && (fault_table[i].code == code) && (fault_table[i].location == loc) )
      {
         ts = *localtime(&fault_table[i].time);
         strftime(buf, sizeof(buf), "%Y-%m-%d_%H:%M:%S", &ts);
         LOG_INFO("Removing fault: %s, location: %d, time: %s; fault table index: %d", FAULT_STRING[code], loc, buf, i);
         fault_table[i].set = false;
         fault_table[i].code = 0;
         fault_index_table[code].location[loc] = 0;
         fault_deleted = true;
      }
   }
   if (!fault_deleted) LOG_WARNING("Fault code: %s, location: %d not found on delete", FAULT_STRING[code], loc);
}

void dump_fault_table()
{
   struct tm ts = {0};
   char buf[32];
   char string[96];
   uint32_t fault_count = 0;
   int i;
   for (i = 1; i < FAULT_TABLE_LENGTH; i++) if (fault_table[i].set) fault_count++;
   if (fault_count)
   {
      sprintf(string, "  Fault Table:");
      LOG_DEBUG(string); printf("%s\n", string);
      sprintf(string, "    Index, Code                            , Loc, Timestamp                , extra_info");
      LOG_DEBUG(string); printf("%s\n", string);
      for (i = 1; i < FAULT_TABLE_LENGTH; i++)
      {
         if (fault_table[i].set)
         {
            ts = *localtime(&fault_table[i].time);
            strftime(buf, sizeof(buf), "%Y-%m-%d_%H:%M:%S", &ts);
            sprintf(string, "        %d, %-32s,   %d, %-25s, %s",
               i, FAULT_STRING[fault_table[i].code], fault_table[i].location, buf, fault_table[i].xtra_info);
            LOG_DEBUG(string); printf("%s\n", string);
         }
      }
   } else {
      sprintf(string, "  Fault Table is empty");
      LOG_DEBUG(string); printf("%s\n", string);
   }
}


fault_table_entry_t * get_fault(uint32_t index)
{
   int i = index;
   if (fault_table[i].set == false)
   {
      //look for a fault is that is set, since indexed fault was not set
      for (i = 1; i < FAULT_TABLE_LENGTH; i++)
         if (fault_table[i].set) break;
   }
   if (fault_table[i].set) return &fault_table[i];
   else return NULL;
}
