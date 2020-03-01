//Voicu Alex-Georgian

#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

#include <string>

char *compute_get_request(const char *host, const char *url, const char *url_params, const char * cookiesline, 
    const char * authorization)
{
    char *message = new char[BUFLEN];
    message[0] = 0;
    char *line = new char[LINELEN];
    if(url_params[0] != 0)
    {
        sprintf(line, "GET %s?%s HTTP/1.1", url, url_params);
    }
    else
    {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    //set auth
    if(authorization[0] != 0)
    {
        sprintf(line, "Authorization: Basic %s", authorization);
        compute_message(message, line);
    } 

    //set cookies
    if(cookiesline[0] != 0)
        compute_message(message, cookiesline);

    compute_message(message, "");
    delete[] line;
    return message;
}
char *compute_post_request(const char *host, const char *url, const char *form_data, const char *ctype, const char * cookiesline, 
    const char * authorization)
{

    char *message = new char[BUFLEN];
    message[0] = 0;
    char *line = new char[LINELEN];
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    //set auth
    if(authorization[0] != 0)
    {
        sprintf(line, "Authorization: Basic %s", authorization);
        compute_message(message, line);
    } 

    //set cookies
    if(cookiesline[0] != 0)
        compute_message(message, cookiesline);

    sprintf(line, "Content-Type: %s", ctype);
    compute_message(message, line);
    sprintf(line, "Content-Length: %u", (unsigned int)strlen(form_data));
    compute_message(message, line);
    compute_message(message, "");
    sprintf(line, "%s", form_data);
    compute_message(message, line);
    compute_message(message, "");

    delete[] line;
    return message;
}
