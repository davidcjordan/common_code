#ifndef netw_common_h
#define netw_common_h

#include <stdint.h>

//the first byte of every packet is the "command"
// CMD_START is no longer used; recording starts on CMD_GET_SYNC
// #define CMD_START 3 //command to start recording
#define CMD_SAVE 4 //command to save recording, includes filename as data
#define CMD_GET_SYNC 5 //request for camera reset the frame number
#define CMD_GET_VERSION 6 //request for device (base/camera/speaker) version string
#define CMD_GET_STATUS 7  //request for device status
#define CMD_GET_ERROR 8   //device error info

#define CMD_SAVE_RESULT 34 //this packet is a response to the SAVE command
//#define CMD_SYNC_RESULT 35 //this packet is a response to a GET_SYNC command
#define CMD_VERSION 36     //this packet is a response to a GET_VERSION command
#define CMD_STATUS 37      //this packet is a response to GET_STATUS command
#define CMD_ERROR 38       //this packet is a response to GET_ERROR command
#define CMD_INTERCEPTS 43 //indicates that this packet contains blob intercept data, cameras send data to boomer with this command

#define CMD_LOG_INFO 50 // write statistics or other state info to the log

#define CMD_PLAY_SOUND 55 //sends a file name to the remote speaker, the speaker adds it to it's sound queue
#define CMD_STOP_SOUND 56 //stops sound playback and also clears queue
#define CMD_GET_IS_PLAYING 57 //returns 1 if sound is playing, 0 otherwise
#define CMD_IS_PLAYING 58 //this packet is a response to GET_IS_PLAYING

#define CMD_GET_TIME 60
#define CMD_TIME_RESPONSE 61
#define CMD_SET_CAM_EXPOS_GAIN 62

#define CMD_RESTART 86 // use exit(1) so that systemd will restart the process

#define PORT 11000 //all boomer devices will receive on this port
#define PACKET_MAX 1300 //max length of packet data including null terminator
#define IP_ADDR_STRING_LENGTH 16

extern const char ADDR_BOOMER[];
extern const char ADDR_LEFT_CAM[];
extern const char ADDR_RIGHT_CAM[];
extern const char ADDR_SPEAKER[];

extern uint16_t packet_len;

bool network_init(); //call once at program start, opens receive port
void network_close(); //cleans up before exit, closes port
void get_packet();
void send_packet_no_copy(uint8_t* data, uint16_t length, const char address[]);
void send_packet(uint8_t command_type, uint8_t* data, uint16_t length, const char address[]);
void send_string(uint8_t command_type, char* data, const char address[]);//prototypes
void dump_net_error_stats();

//void network_update(); // these are device specific

typedef struct net_error_stats {
	uint32_t sendto_failed_count;
	uint32_t rcvfrom_failed_count;
   uint32_t unrecognized_cmd_count;
   uint32_t last_unrecognized_cmd;
   uint32_t unrecognized_source_count;
   char last_unrecognized_source[IP_ADDR_STRING_LENGTH];
} net_error_stats_t;

//extern for the cam/speaker/boomer network.c modules
extern char source_ip[16];
extern char packet[255]; 
extern net_error_stats_t net_error_stats;

#endif
