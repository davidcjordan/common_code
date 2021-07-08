//
//TIMING GUIDELINES
// for human-timescale delays compare millis. Note millis is only updated once per cycle
// generally speaking millis is accurate to about 33ms

// For measurements that require greater accuracy use micros()
// This will return the instantanious time in microseconds

#include <errno.h>
#include <time.h> //for nanosleep
#include <sys/time.h> //for CLOCK_... definitions

#include "timer_functions.h"

uint64_t millis = 0; //updated once per cycle. milliseconds since program start

struct timespec millis_start_time;
uint64_t timing_profile[TIME_PROFILE_POINTS];

void timer_init(){
	millis = 0;
	clock_gettime(CLOCK_MONOTONIC, &millis_start_time);
}

void timer_update(){
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	millis = (now.tv_sec - millis_start_time.tv_sec)*1000;
	millis += (now.tv_nsec - millis_start_time.tv_nsec)/1e6;
}

inline double micros() {
	struct timespec now;
	double m;
	clock_gettime(CLOCK_MONOTONIC, &now);
	m = (now.tv_sec - millis_start_time.tv_sec)*1e6;
	m += (now.tv_nsec - millis_start_time.tv_nsec)/1e3;
	return m;
}

uint64_t counter() {
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC_RAW, &now);
	return (uint64_t)now.tv_sec * UINT64_C(1000000000) + (uint64_t)now.tv_nsec;
}

inline void sleepMillis(uint32_t millis) {
	struct timespec sleep;
	sleep.tv_sec = millis / 1000;
	sleep.tv_nsec = (millis % 1000) * 1000000L;
	while(nanosleep(&sleep, &sleep) && errno == EINTR);
	// while(clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, &sleep) && errno == EINTR);
	// clock_nanosleep is recommended per: https://linux.die.net/man/2/nanosleep
}
/*
void dump_timing_profile(){
	LOG_DEBUG("Timing Profile:");
	for(uint8_t i = 0; i<TIME_PROFILE_POINTS;i++){
		if(i == 0) continue;
		if(timing_profile[i] == 0)
		{
			LOG_DEBUG(" Total: %fms", (float)(timing_profile[i-1]-timing_profile[0])/1E6);
			LOG_DEBUG(" w/o capture: %fms", (float)(timing_profile[i-1]-timing_profile[1])/1E6);
			break;
		}
		uint64_t dif = timing_profile[i] - timing_profile[i-1];
		if (dif/1E6 > 128)
		{
			// this should never happen
			// but it will happen if dump_timing_profile is called before all the entries are filled
			uint64_t dif = timing_profile[i-1] - timing_profile[i];
			LOG_DEBUG("  %u-%u: -%fms  > tp[%d]: %lld; tp[%d]: %lld; tp[%d]: %lld; tp[%d]: %lld",i-1,i,(float)dif/1E6, 
				i-2, (long long) timing_profile[i-2], 
				i-1, (long long) timing_profile[i-1],
				i, (long long) timing_profile[i],
				i+1, (long long) timing_profile[i+1]);
		}
		else LOG_DEBUG("  %2u-%u: %8fms",i-1,i,(float)dif/1E6);
	}
}
*/