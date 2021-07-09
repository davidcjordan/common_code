#pragma once

void logging_init();

void LOG_ERROR(char*message,...);
void LOG_WARNING(char*message,...);
void LOG_DEBUG(char*message,...);
void LOG_INFO(char*message,...);

void print_debug(); //prints out the debug log

void clear_debug(); //clears the debugging log

void save_debug(char*path);
