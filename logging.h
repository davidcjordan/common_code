#ifndef logging_h
#define logging_h

#define LOG_PATH "/home/pi/boomer/logs/"
extern char VERSION_STRING[64];

void logging_init();

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

#endif //logging.h