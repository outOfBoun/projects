// Voicu Alex-Georgian

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "link_emulator/lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

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
char packets_got[MAXPACKETS];

int check_packet(packet *p)
{
	char x = 0;
    int i;
	for (i = 0; i < PLS + 4; ++i)
	{
		x ^= *(((char*)p) + i);
	}
	if(x == (*p).checksum){
		return 1;
	}
	return 0;
}

void packet_to_buffer(packet *p, int *size, char *b)
{
	memcpy(b, (*p).payload, *size);
	*size = (*p).size;
}

void msg_to_packet(msg *m, packet *p)
{
	memcpy((char*)p, (*m).payload, sizeof(packet));
}

void ack_to_msg(acknowledge *a, msg *m)
{
	(*m).len = sizeof(acknowledge);
	memcpy((*m).payload, (char*)a, sizeof(acknowledge));
}

void get_hand(packet *p, int *w, int *packet_num, int *filesize, char *filename)
{
	handshake hs;
	int size;
	size = sizeof(handshake);
	packet_to_buffer(p, &size, (char*)&hs);
	*w = hs.w;
	*filesize = hs.filesize;
	*packet_num = hs.packet_num;
	strncat(filename, hs.filename, sizeof(hs.filename - 5));
}

int main(int argc,char** argv){
  
  	init(HOST,PORT);

    int w;

    msg t, r;
    int size, filesize, packet_num;
    char filename[128] = "recv_";
	packet p;

	int nr = -1, s;
	acknowledge ack;
  	packets_got[0] = 0;
	packet_num = 1;

    while(nr < packet_num) //selective repeat receiver side
    {
        s = recv_message(&r);
  		if(s == -1)
  			continue;
  		msg_to_packet(&r, &p);

  	   	if(check_packet(&p) == 1 && nr < p.num) //accept packet if checksum is ok
  		{
  			size = PLS;
  		    if(p.num > 0)
  				packet_to_buffer(&p, &size, file + (p.num - 1) * PLS);
  			else
  				get_hand(&p, &w, &packet_num, &filesize, filename);
  		
  			ack.num = p.num;
  			packets_got[p.num] = 1; //mark received packet
  			ack.state = 1;
  			ack_to_msg(&ack, &t);
  			send_message(&t);
  		
  		}
  		else
  		{
  			ack.state = 0;
  			ack_to_msg(&ack, &t);
  			send_message(&t);
  		}
  		
  		while(nr < packet_num) //find the position of the packet continously linked to the file start
  		{
  			if(packets_got[nr + 1] == 1)
  				nr++;
  			else
  				break;
  		}
  	}

  	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC , 0777);
	if(fd < 0)
	{
		perror("couldn't create file");
		exit(-1);
	}

	//writing file to disk
	filesize = write(fd, file, filesize);
	if(filesize < 0){
		perror("couldn't write file");
		exit(-1);
	}

	//close file
	fd = close(fd);
	if(fd < 0){
		perror("couldn't close file");
		exit(-1);
	}

    //message sender to announce file was written to disk
	ack.num = -5;
    ack.state = 1;
    ack_to_msg(&ack, &t);
    send_message(&t);

	return 0;
}
