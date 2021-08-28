//fault.c

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "fault.h"
#include "string.h"
#include "logging.h"

fault_t fault_table[FAULT_TABLE_LENGTH] = {0};

void add_fault(char * location, uint32_t code, uint32_t sub_code)
{
   bool fault_added = false;
   for (int i = 0; i < FAULT_TABLE_LENGTH; i++)
   { 
      if (fault_table[i].set == false)
      {
         LOG_DEBUG("Adding fault code %d at index: %d", code, i);
         fault_table[i].set = true;
         if (location) strncpy(fault_table[i].location, location, FAULT_LOCATION_SIZE);
         fault_table[i].code = code;
         fault_table[i].time = time(NULL);
         fault_table[i].sub_code = sub_code;
         fault_added = true;
         break;
      }
   }
   if (!fault_added) LOG_ERROR("fault %d not added; fault table full!\n", code);
}

fault_t * get_fault(uint32_t index)
{
   int i = index;
   if (fault_table[i].set == false)
   {
      //look for a fault is that is set, since indexed fault was not set
      for (i = 0; i < FAULT_TABLE_LENGTH; i++)
         if (fault_table[i].set) break;
   }
   if (fault_table[i].set) return &fault_table[i];
   else return NULL;
}

void dump_faults()
{
   struct tm ts = {0};
   char buf[32];
   uint32_t fault_count = 0;
   int i;
   for (i = 0; i < FAULT_TABLE_LENGTH; i++) if (fault_table[i].set) fault_count++;
   if (fault_count)
   {
      LOG_DEBUG("  Fault Table:");
      LOG_DEBUG("    Index,  Code, subC, Timestamp");
      for (i = 0; i < FAULT_TABLE_LENGTH; i++)
      {
         if (fault_table[i].set)
         {
            ts = *localtime(&fault_table[i].time);
            strftime(buf, sizeof(buf), "%Y-%m-%d_%H:%M:%S", &ts);
            LOG_DEBUG("    FLT %d, %05d, %04d, %s\n", i, fault_table[i].code, fault_table[i].sub_code, buf);
         }
      }
   } else LOG_DEBUG("  Fault Table is empty");
}
