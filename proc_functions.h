// proc_functions.h
#ifndef proc_functions_h
#define proc_functions_h

#include <stdbool.h>

extern char VERSION_STRING[64];

void kill_existing_procs(char * prog_path);
bool set_scheduling();


#endif /* proc_functions_h */