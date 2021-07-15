// proc_functions.c

#include <stdlib.h> //system
#include <unistd.h> //getpid
#include <libgen.h> //for basename

#include <sys/resource.h> //setpriority
#include <sched.h> //setsched
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "proc_functions.h"
#include "logging.h"

void kill_existing_procs(char * prog_path) {
   //kill any boomer copies that are currently running
   // if copies were running, then network port bind will fail, and other resource problems.
	#define KILL_STRING_SIZE 80
	char buffer[KILL_STRING_SIZE];
	char * program_name;
	char temp[48];
	strcpy(temp, prog_path);
	program_name = basename(temp);
	snprintf(buffer, KILL_STRING_SIZE, "pgrep %s | grep -v %d | xargs -r kill", program_name, getpid());
	system(buffer);
	//pgrep returns process id's that have the name "Boomer2.0", output is piped to grep
	//grep searchs that list for PIDs that are not the PID of THIS process, output is passed to kill
	//kill kills any processes it receives
}

bool set_scheduling()
{
   char err_string[80];
	int rc = setpriority(PRIO_PROCESS, 0, -20);
	if (rc)
	{
      sprintf(err_string, "Error on setpriority %s\n", strerror(errno));
		fprintf(stderr, err_string);
      LOG_ERROR(err_string);
		return true;
	}
	//lock memory
		//mlockall();//not needed swap file disabled
		//in the future we may allow swapping with a thrown error

	//set real-time(ish) task scheduling mode and process priority (range 0 to 99)
	// running at priority 99 was not recommended: https://rt.wiki.kernel.org/index.php/HOWTO:_Build_an_RT-application
	const struct sched_param priority = { 32 };
	rc = sched_setscheduler(0, SCHED_FIFO, &priority);
	if (rc)
	{
      sprintf(err_string, "Error on setscheduler %s\n", strerror(errno));
		fprintf(stderr, err_string);
      LOG_ERROR(err_string);
	}
	return false;
}
