#ifndef netw_common_h
#define netw_common_h

#include <stdint.h>

//the first byte of every packet is the "command"
#define CMD_START 3 //command to start recording
#define CMD_SAVE 4 //command to save recording, includes filename as data
#define CMD_GET_SYNC 5 //request for camera to respond on next SYNC signal
#define CMD_GET_VERSION 6 //request for device (base/camera/speaker) version string
#define CMD_GET_STATUS 7  //request for device status
#define CMD_GET_ERROR 8   //device error info

#define CMD_SAVE_RESULT 34 //this packet is a response to the SAVE command
//#define CMD_SYNC_RESULT 35 //this packet is a response to a GET_SYNC command
#define CMD_VERSION 36     //this packet is a response to a GET_VERSION command
#define CMD_STATUS 37      //this packet is a response to GET_STATUS command
#define CMD_ERROR 38       //this packet is a response to GET_ERROR command

#define CMD_INTERCEPTS_MODE 40 //puts a camera into mode where it will transmit blob intercepts every frame
#define CMD_IMAGE_MODE 41 //puts a camera into image mode, where it transmits frame images instead of blobs
#define CMD_AUTO_EXP 42 //turns auto exposure on and off, off if first byte is 0, on if it's 1

#define CMD_INTERCEPTS 43 //indicates that this packet contains blob intercept data, cameras send data to boomer with this command

#define CMD_LOG_INFO 50 // write statistics or other state info to the log

#define CMD_PLAY_SOUND 55 //sends a file name to the remote speaker, the speaker adds it to it's sound queue
#define CMD_STOP_SOUND 56 //stops sound playback and also clears queue
#define CMD_GET_IS_PLAYING 57 //returns 1 if sound is playing, 0 otherwise

#define CMD_RESTART 86 // use exit(1) so that systemd will restart the process

//#define USE_ENET 1
#ifdef USE_ENET
const char ADDR_BOOMER[] = "192.168.0.42";
const char ADDR_LEFT_CAM[] = "192.168.0.43";
const char ADDR_RIGHT_CAM[] = "192.168.0.44";
const char ADDR_SPEAKER[] = "192.168.0.46";
#else
const char ADDR_BOOMER[] = "192.168.27.2";
const char ADDR_LEFT_CAM[] = "192.168.27.3";
const char ADDR_RIGHT_CAM[] = "192.168.27.4";
const char ADDR_SPEAKER[] = "192.168.27.6";
#endif

#define PORT 11000 //all boomer devices will receive on this port
#define PACKET_MAX 1300 //max length of packet data including null terminator
#define IP_ADDR_STRING_LENGTH 16

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
