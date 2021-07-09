#ifndef timer_functions_h
#define timer_functions_h

#include <stdint.h>

extern uint64_t millis;
#define TIME_PROFILE_POINTS 20
extern uint64_t timing_profile[TIME_PROFILE_POINTS]; //used to keep track of the execution time of code segments in real time

void dump_timing_profile(); //outputs the time differences between time points in the timing profile

void sleepMillis(uint32_t millis);

void timer_init();

void timer_update();

double micros();

uint64_t counter();

#endif /* timer_functions_h */
