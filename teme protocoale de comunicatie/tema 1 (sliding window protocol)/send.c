// Voicu Alex-Georgian

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

#define PLS (MSGSIZE - 5)
#define MAXFILESIZE 11534336
#define MAXPACKETS 8000

typedef struct {
	int w, filesize, packet_num;
	char filename[128];
}handshake;

typedef struct {
	short num, size;
	char payload[PLS];
	char checksum;
}packet;

typedef struct {
	short num, state; //1 for ack, 0 for nack
}acknowledge;

char file[MAXFILESIZE];
char packets_sent[MAXPACKETS];

void buffer_to_packet(char *b, int size, packet *p, int num)
{
	(*p).num = num;
	(*p).size = size;
	memcpy((*p).payload, b, size);
	(*p).checksum = 0;
  int i;
	for (i = 0; i < PLS + 4; ++i)
	{
		(*p).checksum ^= *(((char*)p) + i);
	}
}

void packet_to_msg(packet *p, msg *m)
{
	(*m).len = sizeof(packet);
	memcpy((*m).payload, (char*)p, sizeof(packet));
}

void msg_to_ack(msg *m, acknowledge *a)
{
	memcpy((char*)a, (*m).payload, sizeof(acknowledge));
}

int main(int argc,char** argv){

	if(argc < 4 || argc > 4){
		perror("wrong number or arguments");
		exit(-1);
	}

	char filename[512];
	int connection_speed, connection_delay, 
		filesize, packet_num, timeout;

	strncpy(filename, argv[1], sizeof(filename));
	connection_speed = atoi(argv[2]);
	connection_delay = atoi(argv[3]);
	timeout = connection_delay/2; 

	if(connection_delay == 0 || connection_speed == 0){
		perror("invalid connection speed or delay");
		exit(-1);
	}

	int fd = open(filename, O_RDONLY);
	if(fd < 0){
		perror("couldn't open file");
		exit(-1);
	}

	//loading file from disk
	filesize = read(fd, file, MAXFILESIZE);
	if(filesize < 0){
		perror("couldn't read file");
		exit(-1);
	}

	//close file
	fd = close(fd);
	if(fd < 0){
		perror("couldn't close file");
		exit(-1);
	}

	//computing number of packets
	packet_num = filesize / PLS;
	if(packet_num * PLS < filesize)
		packet_num++;

	//deriving windows size from BDP
	//checking if the num of packets is actually lower
	//lower bounding w size at 1
	int w;
	w = (1000 * connection_speed * connection_delay) / (sizeof(msg) * 8);
	w = w < packet_num ? w : packet_num;
	if(w < 1)
		w = 1;

	//ready to initialize link
	init(HOST,PORT);
	
	//initial data communication
	handshake hs;

	hs.w = w;
	hs.filesize = filesize;
	hs.packet_num = packet_num;
	strncpy(hs.filename, filename, sizeof(hs.filename));

	packet p;
	msg t, r, hsmsg;
    //creating handshake for packet 0
	buffer_to_packet(((char*)&hs), sizeof(hs), &p, 0);
	packet_to_msg(&p, &hsmsg);

	int s = -1;
	acknowledge ack;

  	int na = -1, nt = -1, count;
  	s = -1;
  	while(na < packet_num) //selective retransmission
  	{	
  		if(s == -1) //send packet burst to fill window
  		{
  			nt = na;
  			count = 0;
  			while(nt < packet_num && count < w)
  			{
  				nt++;
  				if(packets_sent[nt] == 1) //skip already sent packets
  					continue;
                if(nt > 0)
                {
                    buffer_to_packet(file + PLS*(nt-1), PLS, &p, nt);
                    packet_to_msg(&p, &t);
                    send_message(&t);
                }
                else //handshake msg special case
                {
                    send_message(&hsmsg);
                }
  				count++;
  			}
  		}

  		s = recv_message_timeout(&r, timeout);
  		if(s == -1)
  		{
  			continue;
  		}
  		else msg_to_ack(&r, &ack);

        if(ack.state == 1) //if packet is acked then remember
  			packets_sent[ack.num] = 1;

        while(na < packet_num) //increase na to the last acked packed 
        //that is continously linked to the file start (not skipping unacked packets)
  	 	{
  			  if(packets_sent[na + 1] == 1)
  			  	 na++;
  			  else
  				   break;
  		}

  		count = packet_num;
  		nt++;

  		if(nt > count)
  			nt = na + 1;

  		while(nt <= count) //get the next packed cycling through the window
  		{
  			if(packets_sent[nt] == 1)
  			{
  				nt++;
  				if(nt > count)
  					nt = na + 1;
  			}
  			else
  				break;
  		}
  			
        if(nt > 0) //send the respective packet
        {
            buffer_to_packet(file + PLS*(nt-1), PLS, &p, nt);
            packet_to_msg(&p, &t);
            send_message(&t);
        }
        else
        {
            send_message(&hsmsg);
        }
  	}

  	//wait a bit to write on disk at receiver
  	s = recv_message(&r);
  

  return 0;
}
