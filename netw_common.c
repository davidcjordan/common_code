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
char packet[255];    //buffer used by get_packet; empty string if no packet
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
	LOG_INFO("OK: Using address: %s, port: %d", ADDR_BOOMER, PORT);

	// set address for future send_packets to send to boomer
	memset((char *) &si_send_to, 0, sizeof(si_send_to));
	si_send_to.sin_family = AF_INET;
	si_send_to.sin_port = htons(PORT);
	si_send_to.sin_addr.s_addr = inet_addr(ADDR_BOOMER);

	memset((char *) &si_rcv_from, 0, sizeof(si_rcv_from));
	LOG_DEBUG("Socket setup with no errors.");
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
   si_send_to.sin_addr.s_addr = inet_addr(address);
	int n = sendto(sockfd, data, length, 0, (struct sockaddr*) &si_send_to, addrlen);
	if (n < 0)
	{
		net_error_stats.sendto_failed_count++;
		if ((net_error_stats.sendto_failed_count % 128) == 0) {
			LOG_WARNING("sendto() failed with return code: %d\n", n);
		}
      /* TODO: add error count thresholding
		if (net_error_stats.sendto_failed_count > SENDTO_EXCESSIVE_ERROR_THRESHOLD) {
			LOG_ERROR("Exiting due to > %d sendto() failures.\n", SENDTO_EXCESSIVE_ERROR_THRESHOLD);
		}
      */
	}
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
	LOG_INFO("  nrecognized_source_count:  %d", net_error_stats.unrecognized_source_count);
	LOG_INFO("  last_unrecognized_source:  %s", net_error_stats.last_unrecognized_source);
}