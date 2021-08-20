//fault.c

#include <stdbool.h>
#include <time.h>
#include "fault.h"
#include "string.h"

fault_t fault_table[FAULT_TABLE_LENGTH] = {0};

void add_fault(char * location, uint32_t code, uint32_t sub_code)
{
   for (int i = 0; i < FAULT_TABLE_LENGTH; i++)
   { 
      if (fault_table[i].set == false)
      {
         fault_table[i].set = true;
         if (location) strncpy(fault_table[i].location, location, FAULT_LOCATION_SIZE);
         fault_table[i].code = code;
         fault_table[i].time = time(NULL);
         fault_table[i].sub_code = sub_code;
      }
   }
}

fault_t * get_fault(uint32_t index)
{
   if (fault_table[index].set == false)
   {
      //look for a fault is that is set
      bool fault_found = false;
      int i;
      for (i = 0; i < FAULT_TABLE_LENGTH; i++)
         if (fault_table[i].set == true)
         {
            fault_found = true; 
            break;
         }
      if (fault_found) return &fault_table[i];
      else return NULL;
   } else return &fault_table[index];
}
