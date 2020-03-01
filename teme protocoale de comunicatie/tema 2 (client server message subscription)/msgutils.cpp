//Voicu Alex-Georgian

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

using namespace std;

int tcpsend(int sock, char *buff, int size)
{
    int sent = 0, crt_sent;

    while(sent < size)
    {
        crt_sent = send(sock, buff + sent, size - sent, 0);
        if(crt_sent <= 0)
            return crt_sent;
        sent += crt_sent;
    }
    return sent;
}

int tcprecv(int sock, char *buff, int buff_size, int expected_size)
{
    int read = 0, crt_read;
    if(buff_size < expected_size)
        expected_size = buff_size;

    while(read < expected_size)
    {
        crt_read = recv(sock, buff + read, expected_size - read, 0);
        if(crt_read <= 0)
            return crt_read;
        read += crt_read;
    }
    return read;
}

