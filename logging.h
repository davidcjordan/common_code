#ifndef logging_h
#define logging_h

#ifdef __cplusplus
extern "C" {
#endif

/*
  The LOG_xxx messages are written to the file /run/shm/boomer.log, and optionally to stdio.

   /run/shm is an in-memory file system (shm = shared memory). 

  Writing to the shared memory file system is fast and avoids flash file system wear (flash file systems are good for thousands of writes, not unlimited).

  When the save_logs/record function is called, it copies /run/shm/boomer.log to /home/pi/boomer/logs and appends the date-time.  It actually only copies the part since the last bbase start or the last clear_debug().

  Note, however, you don't have to do 'record' to view the log - it's always present.  The record function just persists it, and a script copies it from bbase to Daves.

  boomer.log persists even if bbase (or the cam) crashes or is stopped.  When bbase is run again, it appends to the file.  This is so we can look back at previous bbase runs.

  boomer.log does not persist when the RPi is rebooted.

  When boomer.log reaches a limit (currenly 6 MB), then it renames boomer.log to previous_boomer.log and starts a new boomer.log

  Because boomer.log is a standard file, albiet in-memory, then unix utilities can be used on it:
      * tail -f  will show lines as they get appended.  This is what you will use when supporting a customer remotely, instead of using the console
      * editors can open the file to search for things.  
      * pattern matching (regex) tools can be used to search for patterns, like "Drill_state ball=" to see all (and only) drill state changes.
      * The file can be copied or emailed.
 
*/

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define BOOMER_DIR "/home/pi/boomer"
#define LOG_DIR "logs"
extern char VERSION_STRING[64];
extern int log_to_file_level;
extern int log_to_stdout_level;

void logging_init();
void log_flush();

#define LLOG_FATAL (1)
#define LLOG_ERROR (2)
#define LLOG_WARN  (3)
#define LLOG_INFO  (4)
#define LLOG_DEBUG (5)

void log_main(int level, const char * filename, int line_num, const char * format, ...);

#define LOG_FATAL(...)  log_main(LLOG_FATAL, __BASE_FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)  log_main(LLOG_ERROR, __BASE_FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) log_main(LLOG_WARN, __BASE_FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)   log_main(LLOG_INFO, __BASE_FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)  log_main(LLOG_DEBUG, __BASE_FILE__, __LINE__, __VA_ARGS__)

void print_debug_log();

void clear_debug_log();

void save_debug_log(char*path);

#ifdef __cplusplus
}
#endif

#endif //logging.h