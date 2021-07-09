//note this file uses different includes than the others, because it's a C file, not C++
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> //for exit
#include <time.h> //for timestamping log
#include "logging.h"
#include "timer_functions.h"

#define LOG_TO_SHM 1
#define LOG_FILENAME "/run/shm/boomer.log"
#define PREVIOUS_LOG_FILENAME "/run/shm/previous_boomer.log"
#define LOG_FILE_MAX_BYTES 6000000
#define LOG_FLUSH_INTERVAL_NANOS 1E9
//#define TIME_BASED_FFLUSH

FILE *pFile;
uint64_t previous_flush_timestamp;

//the debug log is a text buffer holding 1MB of text
//print to it as you would to the console using LOG_DEBUG()
#define debug_log_length 1000000
char debug_log[debug_log_length];
char* debug_log_write_ptr; //points to the next space to write to in the debug log
const char* debug_log_end_ptr = &debug_log[debug_log_length-1]; //last valid debug log pointer
bool debug_log_filled = false; //lets us know when the whole log is full of valid data

//prototypes
void VA_LOG_DEBUG(char * message,va_list ap);

void logging_init(void){
	debug_log_write_ptr = &debug_log[0];
	#ifdef LOG_TO_SHM
	pFile=fopen(LOG_FILENAME, "a");
	if(pFile==NULL)
	{
   	perror("Error opening /run/shm log file\n");
		exit(1);
	}
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	fprintf(pFile, "==== Log opened at: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	previous_flush_timestamp = counter();
	#endif
}

void LOG_ERROR(char *  message,...) {
	va_list ap; //create pointer for argument list
	va_start(ap,message); //initialize the pointer?
	printf("ERROR: ");
	vprintf(message,ap);
	VA_LOG_DEBUG(message,ap); //make sure this message appears in log debug as well
	va_end(ap);
}
void LOG_WARNING(char *  message,...) {
	va_list ap; //create pointer for argument list
	va_start(ap,message); //initialize the pointer?
	printf("WARN: ");
	vprintf(message,ap);
	VA_LOG_DEBUG(message,ap); //make sure this message appears in log debug as well
	va_end(ap);
}
void LOG_INFO(char * message,...){
	va_list ap; //create pointer for argument list
	va_start(ap,message); //initialize the pointer?  
	VA_LOG_DEBUG(message,ap); //pass through to VA_LOG_DEBUG, which contains the actual logic
	va_end(ap);
}
void LOG_DEBUG(char * message,...){
	va_list ap; //create pointer for argument list
	va_start(ap,message); //initialize the pointer?  
	VA_LOG_DEBUG(message,ap); //pass through to VA_LOG_DEBUG, which contains the actual logic
	va_end(ap);
}

void VA_LOG_DEBUG(char * message,va_list ap){ //same as log debug, but takes a va_list of arguments instead of multiple arguments
	char buf[255];
	char * buf_ptr = buf;
	vsnprintf(buf,255,message,ap);
	while(true){
		if(*buf_ptr == '\0'){
			*debug_log_write_ptr = '\n';
			debug_log_write_ptr++;
			break;
		}
		*debug_log_write_ptr = *buf_ptr; //copy from the message into the buffer
		debug_log_write_ptr++;
		buf_ptr++;
		if(debug_log_write_ptr > debug_log_end_ptr){
			debug_log_write_ptr = &debug_log[0];
			debug_log_filled = true;
		}
	}
	#ifdef LOG_TO_SHM
	// roll log if over max
	if (ftell(pFile) > LOG_FILE_MAX_BYTES){
		fclose(pFile);
		remove(PREVIOUS_LOG_FILENAME);
		if (rename(LOG_FILENAME, PREVIOUS_LOG_FILENAME) != 0) printf("Error: boomer_log rename failed");
		pFile=fopen(LOG_FILENAME, "w");
		if(pFile==NULL)
		{
			perror("Error opening /run/shm log file\n");
		}
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		fprintf(pFile, "==== Log rolled at: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}
	// write log
	fprintf(pFile, "%s\n", buf);
	#ifdef TIME_BASED_FFLUSH
	if((counter() - previous_flush_timestamp) > LOG_FLUSH_INTERVAL_NANOS){
		// time_t t = time(NULL);
		// struct tm tm = *localtime(&t);
		// printf("==== flushing at: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
		fflush( pFile );
		previous_flush_timestamp = counter();
	}
	#else
	fflush( pFile );
	#endif

	#endif //log to shared memory file
}

//clears the debug log
void clear_debug(){
	debug_log_filled = false;
	debug_log_write_ptr = &debug_log[0];
}

void print_debug(){
	char* read_ptr;
	if(!debug_log_filled){
		read_ptr = &debug_log[0];
		while(true){
			if(read_ptr == debug_log_write_ptr) break;
			putc(*read_ptr,stdout);
			read_ptr++;
		}
	} else { //debug log is filled
		read_ptr = debug_log_write_ptr;
		while(true){
			putc(*read_ptr,stdout);
			read_ptr++;
			if(read_ptr > debug_log_end_ptr) read_ptr = &debug_log[0];
			if(read_ptr == debug_log_write_ptr) break;
		}
	}
}

void save_debug(char *path)
{
	char *read_ptr;
	FILE *f = fopen(path, "w");
	if (f == NULL)
	{
		printf("Error on fopen of %s", read_ptr);
		return;
	}
	int byte_count = 0;
	if (!debug_log_filled)
	{
		read_ptr = &debug_log[0];
		while (true)
		{
			if (read_ptr == debug_log_write_ptr)
				break;
			fputc(*read_ptr, f);
			read_ptr++;
			byte_count++;
		}
	}
	else
	{ //debug log is filled
		read_ptr = debug_log_write_ptr;
		while (true)
		{
			fputc(*read_ptr, f);
			read_ptr++;
			byte_count++;
			if (read_ptr > debug_log_end_ptr)
				read_ptr = &debug_log[0];
			if (read_ptr == debug_log_write_ptr)
				break;
		}
	}
	fclose(f);
}
