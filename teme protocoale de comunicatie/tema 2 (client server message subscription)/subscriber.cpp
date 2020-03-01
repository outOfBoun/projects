//Voicu Alex-Georgian

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <math.h>
#include <netinet/tcp.h>

#include "msgutils.h"

using namespace std;

#define BUFFLEN 2048

struct fmsg
{
    char topic[51];
    char tip_date;
    char content[1505];
};

int main(int argc, char *argv[])
{
    int sockfd, n; 
    char buff[BUFFLEN]; 

    if(argc != 4)
    {
        printf("\n Usage: %s <ID_Client> <IP_Server> <Port_Server>\n", argv[0]);
        return 1;
    }

    struct sockaddr_in servaddr, udpclient;
    socklen_t addrlen;

    //filling in server addr
    memset(&servaddr, 0, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET;
    inet_aton(argv[2], &(servaddr.sin_addr)); 
    servaddr.sin_port = htons(atoi(argv[3]));
    
    

    //TCP initialization and connection

    //creating socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    }

    int enable = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    enable = 1;
    if(setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(int)) < 0)
        perror("setsockopt(TCP_NODELAY) failed");
    //binding the socket with the server addr 
    if(connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
    { 
        perror("connect failed"); 
        exit(EXIT_FAILURE); 
    }



    //multiplexing setup

    fd_set read_set;
    FD_ZERO(&read_set);
	int maxfd;
    //adding stdin and TCP socket
	FD_SET(STDIN_FILENO, &read_set);
    maxfd = STDIN_FILENO;
	FD_SET(sockfd, &read_set);
	if(sockfd > maxfd)
    	maxfd = sockfd;



    //sending ID

    char myid[11];
    strncpy(myid, argv[1], 10);
    myid[10] = 0;
    tcpsend(sockfd, myid, 11);



    //looping for commands or messages

    fmsg data;
    
    while(1) 
    {
        fd_set tmp_read_set = read_set;
        int rc = select(maxfd + 1, &tmp_read_set, NULL, NULL, NULL);
        if(rc < 0)
        {
            printf("error select\n");
            continue;
        }

        if(FD_ISSET(STDIN_FILENO, &tmp_read_set)) 
        {
            fgets(buff, sizeof(buff), stdin);
            //send(sockfd, buff, strlen(buff) + 1, 0);
            // send commands
            Subscription sb;
            char *p;
            p = strtok(buff, " \n");
            if(strcmp("exit", p) == 0)
            {
                //close the connection 1 way. no longer able to send commands
                shutdown(sockfd, SHUT_WR);
                FD_CLR(STDIN_FILENO, &read_set);
            }
            else if(strcmp("subscribe", p) == 0 || strcmp("unsubscribe", p) == 0)
            {
                if(strcmp("subscribe", p) == 0)
                    sb.subscribe = 1;
                else
                    sb.subscribe = 0;
                p = strtok(0, " \n");
                if(p == 0)
                    continue;
                strncpy(sb.topic, p, 50);
                sb.topic[50] = 0;
                p = strtok(0, " \n");
                if(sb.subscribe == 1)
                {
                    if(p == 0)
                        continue;
                    if(*p == '1')
                    {
                        sb.sf = 1;
                    }
                    else if(*p == '0')
                    {
                        sb.sf = 0;
                    }
                    else
                        continue;
                }
                else
                {
                    if(p != 0)
                        continue;
                }
                tcpsend(sockfd, (char*)&sb, sizeof(Subscription));
                if(sb.subscribe == 1)
                {
                    printf("subscribed %s\n", sb.topic);
                }
                else
                {
                    printf("unsubscribed %s\n", sb.topic);
                }
            }
        }
        else if(FD_ISSET(sockfd, &tmp_read_set))
        {
            int msgsize;
            //read incoming msg size
        	n = tcprecv(sockfd, (char *)&msgsize, sizeof(int), sizeof(int));

       		if(n < 0)
            {
        		perror("recv client error");
        		exit(EXIT_FAILURE);
        	} 
        	else if(n == 0)
            {
           		break;
        	}
      		else
        	{
                //read the incoming message into fmsg structure
                if(msgsize != 52)
                {
                    printf("error recv msg header\n");
                    continue;
                }
                n = tcprecv(sockfd, data.topic, sizeof(data.topic) + sizeof(data.tip_date), msgsize);
                if(n <= 0) {printf("error recv\n"); continue;}
                n = tcprecv(sockfd, (char *)&msgsize, sizeof(int), sizeof(int));
                if(n <= 0) {printf("error recv\n"); continue;}
                n = tcprecv(sockfd, data.content, sizeof(data.content), msgsize);
                if(n <= 0) {printf("error recv\n"); continue;}
                n = tcprecv(sockfd, (char*)&addrlen, sizeof(socklen_t), sizeof(socklen_t));
                if(n <= 0) {printf("error recv\n"); continue;}
                n = tcprecv(sockfd, (char*)&udpclient, addrlen, addrlen);
                if(n <= 0) {printf("error recv\n"); continue;}

                printf("%s:%d - %s", inet_ntoa(udpclient.sin_addr), ntohs(udpclient.sin_port), data.topic);

        		//display func
                if(data.tip_date == 0)
                {
                    unsigned int x, sign;
                    sign = data.content[0];
                    memcpy(&x, data.content + 1, sizeof(unsigned int));
                    x = ntohl(x);
                    printf(" - INT - ");
                    if(sign > 0)
                        printf("-");
                    printf("%u\n", x);
                }
                else if(data.tip_date == 1)
                {
                    unsigned short x;
                    memcpy(&x, data.content, sizeof(unsigned short));
                    x = ntohs(x);
                    printf(" - SHORT_REAL - %.2f\n", (float)x/100);
                }
                else if(data.tip_date == 2)
                {
                    unsigned int x, sign, pow10;
                    sign = data.content[0];
                    pow10 = data.content[5];
                    memcpy(&x, data.content + 1, sizeof(unsigned int));
                    x = ntohl(x);
                    printf(" - FLOAT - ");
                    if(sign > 0)
                        printf("-");
                    if(pow10 == 0)
                    {
                        printf("%u\n", x);
                    }
                    else
                    {
                        unsigned int pow10done = ((unsigned int)pow(10, pow10));
                        unsigned int upper = x / pow10done;
                        unsigned int lower = x % pow10done;
                        if(upper * pow10done == x)
                        {
                            printf("%u\n", upper);
                        }
                        else if(upper == 0)
                        {
                            printf("0.");
                            int i = x;
                            while(pow10)
                            {
                                if(i <= 0)
                                    break;
                                pow10--;
                                i /= 10;
                            }
                            while(pow10)
                            {
                                printf("0");
                                pow10--;
                            }
                            printf("%u\n", lower);
                        }
                        else
                        {
                            printf("%u.", upper);
                            printf("%u\n", lower);
                        }
                    }
                }
                else if(data.tip_date == 3)
                {
                    printf(" - STRING - %s\n", data.content);
                }
                else
                {
                    printf("unknown type\n");
                }             
      		}		
        }
    }

    close(sockfd);

    return 0;
}
