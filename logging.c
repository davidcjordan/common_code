//note this file uses different includes than the others, because it's a C file, not C++
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h> //for timestamping log
#include <libgen.h> //for basename
#include "logging.h"
#include "timer_functions.h"

#define LOG_FILENAME "/run/shm/boomer.log"
#define PREVIOUS_LOG_FILENAME "/run/shm/previous_boomer.log"
#define LOG_FILE_MAX_BYTES 6000000
#define LOG_FLUSH_INTERVAL_MILLIS 1E7
//#define TIME_BASED_FFLUSH

char VERSION_STRING[64] = {0};

FILE *pFile;
uint64_t previous_flush_timestamp;
uint32_t log_start_pos;
const char LEVEL_FATAL[] = "FATL";
const char LEVEL_ERROR[] = "INFO";
const char LEVEL_WARN[] = "WARN";
const char LEVEL_INFO[] = "INFO";
const char LEVEL_DEBUG[] = "DBUG";

uint32_t current_log_level = 5;

void logging_init(void){
//	debug_log_write_ptr = &debug_log[0];
	pFile=fopen(LOG_FILENAME, "a+");
	if(pFile==NULL)
	{
   	perror("Error opening /run/shm log file\n");
		//exit(1);
	}
	fprintf(pFile, "\n");
	// with a+ (append plus read) need to do a write to get a position at the end of the file
	log_start_pos = ftell(pFile);

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	fprintf(pFile, "==== Log opened at: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

	sprintf(VERSION_STRING, "Built: %s %s", __DATE__, __TIME__);
	printf("Version: %s\n", VERSION_STRING);

	previous_flush_timestamp = counter();
}

void log_main(int level, const char * filename, int line_num, const char * format, ...) 
{
	if (level > current_log_level) return;

   const char * level_ptr = LEVEL_DEBUG;
   if (level == LLOG_FATAL) level_ptr = LEVEL_FATAL;
   else if (level == LLOG_ERROR) level_ptr = LEVEL_ERROR;
   else if (level == LLOG_WARN) level_ptr = LEVEL_WARN;
   else if (level == LLOG_INFO) level_ptr = LEVEL_INFO;

   char info_string[96]; 
   struct timespec curTime; struct tm* info; 
   clock_gettime(CLOCK_REALTIME, &curTime); 
   info = localtime(&curTime.tv_sec); 
   sprintf(info_string, "%d-%02d-%02dT%02d:%02d:%02d.%d_%s_%.10s-L%03d", 1900 + info->tm_year, info->tm_mon, info->tm_mday, \
      info->tm_hour, info->tm_min, info->tm_sec, (int) (curTime.tv_nsec/1000000), 
		level_ptr, basename((char * )filename), line_num); 
   
   char log_string[128]; 
   va_list aptr; 
   va_start(aptr, format); 
   vsnprintf(log_string, sizeof(log_string), format, aptr); 
   va_end(aptr);

   if (ftell(pFile) > LOG_FILE_MAX_BYTES){
		fclose(pFile);
		remove(PREVIOUS_LOG_FILENAME);
		if (rename(LOG_FILENAME, PREVIOUS_LOG_FILENAME) != 0) printf("Error: boomer_log rename failed");
		pFile=fopen(LOG_FILENAME, "w+");
		if(pFile==NULL)
		{
			perror("Error opening /run/shm log file\n");
		}
      log_start_pos = ftell(pFile);
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		fprintf(pFile, "==== Log rolled at: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	}
	fprintf(pFile, "%s>%s\n", info_string, log_string);
	// fflush( pFile );

	if((counter() - previous_flush_timestamp) > LOG_FLUSH_INTERVAL_MILLIS){
		// time_t t = time(NULL);
		// struct tm tm = *localtime(&t);
		// printf("==== flushing at: %02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
		fflush( pFile );
		previous_flush_timestamp = counter();
	}
}

void save_debug_log(char*path){
   fflush(pFile);
   uint32_t log_end_pos = ftell(pFile);
   uint32_t log_size = log_end_pos - log_start_pos;
	bool file_write = strcmp(path, "stdout");
	#define MAX_STDOUT_CHARS 32*1024
	//limit printf output
	if (!file_write && log_size > MAX_STDOUT_CHARS) log_size > MAX_STDOUT_CHARS;

   //printf("log_start: %d log_end: %d; size: %d\n", log_start_pos, log_end_pos, log_size);
   char buffer[4096*1024];
   size_t rw_size;
   #define ELEMENTS_TO_READ 1
   if (log_size < sizeof(buffer)) {
      int rc = fseek(pFile, log_start_pos, SEEK_SET);
      if (rc != 0) 
      {
         perror("debug_log_save: seek to log_start failed");
         return;
      }
      rw_size = fread(buffer, log_size, ELEMENTS_TO_READ, pFile);
		fflush(stdout);
      if (rw_size != ELEMENTS_TO_READ) {
         printf("%s read of %d bytes failed.", LOG_FILENAME, log_size);
			fflush(stdout);
      } else
      {
			if (file_write) {
				FILE *f = fopen(path,"w");
				rw_size = fwrite(buffer, log_size, ELEMENTS_TO_READ, f);
				if (rw_size != ELEMENTS_TO_READ) {
					printf("%s write of %d bytes failed.", path, log_size);
					fflush(stdout);
				}
				fclose(f);
			}
			else {
				printf(buffer); printf("\n"); fflush(stdout);
			}
      }
      // restore append position
      rc = fseek(pFile, 0, SEEK_END);
      if (rc != 0) 
      {
         perror("save_debug_log: seek to end failed");
      }
    } else {
		LOG_ERROR("log_size of %d larger than save buffer.\n", log_size);
      printf("log_size of %d larger than save buffer.\n", log_size);
   }
}

void clear_debug_log(){
	log_start_pos = ftell(pFile);
}
void print_debug_log(){
   save_debug_log("stdout");
}