//networking includes
#include <sys/select.h> //for fd_set
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <unistd.h> // for close socket

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "netw_common.h"
#include "logging.h"
#include "timer_functions.h" //for millis
#include "defines.h" //for BOOMER_SSID

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

uint16_t packet_len;

extern int errno;

int sockfd; //used to refer to socket once it's opened
struct sockaddr_in si_me, si_rcv_from, si_send_to; //address descriptors
const int addrlen = sizeof(struct sockaddr_in);
char packet[PACKET_MAX];    //buffer used by get_packet; empty string if no packet
char source_ip[IP_ADDR_STRING_LENGTH];  //set by get_packet, human readable string format
uint8_t buffer[PACKET_MAX]; //buffer used to receive packets

bool log_info_flag;
net_error_stats_t net_error_stats = {0};

bool network_init(){
	log_info_flag = false;

	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 ) { //open a socket (similar to file operation)
		LOG_ERROR("socket creation failed"); 
		return true;
	}

	/*
	// set the busy poll as an attempt to reduce latency - doesn't work with the real-time kernel
	int busy_poll = 50;
	int rc = setsockopt(sockfd, SOL_SOCKET, SO_BUSY_POLL, (char *)&busy_poll, sizeof(busy_poll));
	if (rc < 0) {
		LOG_ERROR("set SO_BUSY_POLL failed.");
		exit(1);
	}
	// set timeout in attempt to eliminate intermittent > 16 mSec delays.
	//  shouldn't have any effect, since the recvfrom uses the nonblock option
   struct timeval tout;
	tout.tv_sec = 0;
	tout.tv_usec = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tout, sizeof(tout)) < 0) {
		LOG_ERROR("Set receive timout failed");
	}
	*/

	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = INADDR_ANY;

	if (bind(sockfd, (struct sockaddr*) &si_me, addrlen)==-1)
	{
		LOG_ERROR("socket bind to port %d failed - in use by another program.", PORT);
		return true;
	} 
	// LOG_INFO("OK: Using address: %s, port: %d", ADDR_BOOMER, PORT);

	// set address for future send_packets to send to boomer
	memset((char *) &si_send_to, 0, sizeof(si_send_to));
	si_send_to.sin_family = AF_INET;
	si_send_to.sin_port = htons(PORT);
	si_send_to.sin_addr.s_addr = inet_addr(ADDR_BOOMER);

	memset((char *) &si_rcv_from, 0, sizeof(si_rcv_from));
	return false;
}

void network_close(){
	close(sockfd);
}

//populates char packet[] and source_ip
//packet[0] will be null if nothing received
void get_packet(){
	packet[0] = '\0'; //"erase" the packet data array
	int max_sd = sockfd +1;
	fd_set read_sds;
	struct timeval tout;
	tout.tv_sec = 0;
	tout.tv_usec = 0;   
	FD_ZERO(&read_sds); 
	FD_SET(sockfd, &read_sds); 
	int ret = select(max_sd, &read_sds, NULL, NULL, &tout); 
	if(ret <0)
	{ 
		LOG_ERROR("\nSelect threw an exception\n"); 
		return;
	} 
	else if(!FD_ISSET(sockfd, &read_sds)) return;
	
	socklen_t slen = sizeof(si_rcv_from);
	int blen = recvfrom(sockfd, packet, PACKET_MAX-1, MSG_DONTWAIT, (struct sockaddr*) &si_rcv_from, &slen);
	if (blen == -1) 
   {
      net_error_stats.rcvfrom_failed_count++;
		LOG_WARNING("UDP Recieve error: %s", strerror(errno));
		// TODO: add thresholding
   }
	else if(blen == 0) return;
	packet_len = blen;
  	packet[packet_len] = '\0';
	strcpy(source_ip,  inet_ntoa(si_rcv_from.sin_addr));
	return;
}

void send_packet_no_copy(uint8_t* data, uint16_t length, const char address[])
{
	enum State {CURRENT, PREVIOUS};
	static bool connected_state[2]= {true, true};
	static uint64_t disconnected_at_millis= UINT64_MAX;
	#define SENDTO_NETUNREACH_THRESHOLD 7
	static uint8_t sendto_unreachable_count= 0;
	#define UNREACHABLE_LOG_STATUS_INTERVAL_MILLIS (30*60*1000)   //log wifi status every N minutes
	static uint64_t previous_unreachable_log_status_millis= 0;
    
	// the following check could be used to detect sending to base, vs sending from base
	// if (strcmp(ADDR_BOOMER, address) == 0)

	si_send_to.sin_addr.s_addr = inet_addr(address);
	int n = sendto(sockfd, data, length, 0, (struct sockaddr*) &si_send_to, addrlen);
	//NOTE: sendto errors happen on the cams & speaker; they don't seem to happen on the base
	if (n < 0)
	{
		if (errno == ENETUNREACH)
		{
			// LOG_WARNING("sendto() failed with network unreachable");
			if (++sendto_unreachable_count > SENDTO_NETUNREACH_THRESHOLD)
				connected_state[CURRENT]= false;
		} else {
			/*
				There are other possible errno, like ENETDOWN, ECOMM, EHOSTUNREACH, EHOSTDOWN.
				Only got ENETUNREACH when testing using 'sudo ip link set wlan0 down'
			*/
			if ((++net_error_stats.sendto_failed_count % 300) == 0)
				LOG_WARNING("sendto() failed with return code: errno=%d %s", errno, strerror(errno));
		}
	} else
	{
		connected_state[CURRENT]= true;
		sendto_unreachable_count= 0;
	}

	if (!connected_state[PREVIOUS] && connected_state[CURRENT])
	{
		enum Interval {HOURS, MINUTES, SECONDS};
		uint32_t disconnected[3];
		disconnected[SECONDS]= (millis-disconnected_at_millis)/1000;
		disconnected[MINUTES]= disconnected[SECONDS]/60;
		disconnected[HOURS]= disconnected[MINUTES]/60;

		disconnected[MINUTES]= disconnected[MINUTES]-(disconnected[HOURS]*60);
		disconnected[SECONDS]= disconnected[SECONDS]-(disconnected[MINUTES]*60);
		
		//NOTE: timer_update must be called to have millis updated for the time-based conditionals to work
		LOG_INFO("Network reconnected after %02d:%02d:%02d (HH:MM:SS)",
			disconnected[HOURS], disconnected[MINUTES], disconnected[SECONDS]);
		// LOG_INFO("Re-connected: millis=%llu disconnected_millis=%llu previous_unreachable_log_status_millis=%llu",
		// 	millis, disconnected_at_millis, previous_unreachable_log_status_millis);
	}
	if (connected_state[PREVIOUS] && !connected_state[CURRENT])
	{
		LOG_WARNING("Network disconnected after %d ENETUNREACH errors.", SENDTO_NETUNREACH_THRESHOLD);
		disconnected_at_millis= millis;
		previous_unreachable_log_status_millis= 0; //force log of wifi scan
	}

	if (!connected_state[CURRENT] && (millis > (previous_unreachable_log_status_millis+UNREACHABLE_LOG_STATUS_INTERVAL_MILLIS)))
	{
		//if not connected, then look for problems: is BOOM_NET available?
		previous_unreachable_log_status_millis= millis;
		FILE *fp= popen("iwgetid -s wlan0","r"); 
		#define MAX_RESPONSE_CHARS 192
		char response[MAX_RESPONSE_CHARS]= {0};
		fscanf(fp,"%s",response);
     	fclose(fp);
		// iwgetid returns BOOMNET instead of BOOM_NET, so only compare first 4 chars
		if (strncmp(response, BOOMER_SSID, 4) == 0)
			LOG_INFO("Network unreachable, but connected to network=%s", response);
		else 
		{
			// show available networks.  cams/speaker use wlan0, so this is NOT very useful for the base
			LOG_INFO("Network unreachable, iwgetid=%s", response);
			fp= popen("iwlist wlan0 scanning | awk -F '[ :=]+' '/(ESS|Freq|Qual)/{ printf $3\" \"}'","r"); 
			memset(response, 0, MAX_RESPONSE_CHARS);
			fgets(&response[0], MAX_RESPONSE_CHARS-1, fp);
	     	fclose(fp);
			LOG_INFO("Network_scan=%s", response);
		}
	}
	connected_state[PREVIOUS]= connected_state[CURRENT];
}

void send_packet(uint8_t command_type, uint8_t* data, uint16_t length, const char address[]){
	if(length > PACKET_MAX-1) length = PACKET_MAX-1;
	buffer[0] = command_type;
	memcpy(buffer+1,data,length);
   send_packet_no_copy(buffer, length+1, address);
}

void send_string(uint8_t command_type, char* data, const char address[]){
	send_packet(command_type,(uint8_t*)data,strlen(data),address);
}

void dump_net_error_stats()
{
	LOG_INFO("Network Error statistics:");
	LOG_INFO("  sendto_failed_count:       %d", net_error_stats.sendto_failed_count);
	LOG_INFO("  rcvfrom_failed_count:      %d", net_error_stats.rcvfrom_failed_count);
	LOG_INFO("  unrecognized_cmd_count:    %d", net_error_stats.unrecognized_cmd_count);
	LOG_INFO("  last_unrecognized_cmd:     %d", net_error_stats.last_unrecognized_cmd);
	LOG_INFO("  unrecognized_source_count:  %d", net_error_stats.unrecognized_source_count);
	LOG_INFO("  last_unrecognized_source:  %s", net_error_stats.last_unrecognized_source);
}