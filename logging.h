#pragma once

void logging_init();

void LOG_ERROR(char*message,...);
void LOG_WARNING(char*message,...);
void LOG_DEBUG(char*message,...);
void LOG_INFO(char*message,...);

void VA_LOG_DEBUG(char * message,va_list ap); //takes an argument list, instead of multiple arguments, called be LOG_DEBUG

void print_debug(); //prints out the debug log

void clear_debug(); //clears the debugging log

void save_debug(char*path);

void logging_update();

char plog_string[256]; //buffer for composing error messages
