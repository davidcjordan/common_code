//fault.c

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "fault.h"
#include "string.h"

fault_t fault_table[FAULT_TABLE_LENGTH] = {0};

void add_fault(char * location, uint32_t code, uint32_t sub_code)
{
   bool fault_added = false;
   for (int i = 0; i < FAULT_TABLE_LENGTH; i++)
   { 
      if (fault_table[i].set == false)
      {
         printf("Adding fault code %d at index: %d\n", code, i);
         fault_table[i].set = true;
         if (location) strncpy(fault_table[i].location, location, FAULT_LOCATION_SIZE);
         fault_table[i].code = code;
         fault_table[i].time = time(NULL);
         fault_table[i].sub_code = sub_code;
         fault_added = true;
         break;
      }
   }
   if (!fault_added) printf("fault not added!\n");
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
      printf("  Fault Table:\n");
      printf("    Index,  Code, subC, Timestamp\n");
      for (i = 0; i < FAULT_TABLE_LENGTH; i++)
      {
         if (fault_table[i].set)
         {
            ts = *localtime(&fault_table[i].time);
            strftime(buf, sizeof(buf), "%Y-%m-%d_%H:%M:%S", &ts);
            printf("    FLT %d, %05d, %04d, %s\n", i, fault_table[i].code, fault_table[i].sub_code, buf);
         }
      }
   } else printf("  Fault Table is empty\n");
   fflush(stdout);
}
