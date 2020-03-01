//Voicu Alex-Georgian

#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>

#include <unistd.h>
#include <fcntl.h>

#include "helpers.h"
#include "requests.h"
#include "parson.h"

#include <map>
#include <string>

using namespace std;

void parse_cookies(char *response, map<string, string> &cookiesmap)
{
	char rsp[BUFLEN], line[1024], *scookie, *sline, *sval;
	strcpy(rsp, response);
	*(strstr(rsp, "\r\n\r\n")) = 0;
	sline = strtok(rsp, "\n");
	//scookie = strstr
	while(sline)
	{
		if(strncmp("Set-Cookie: ", sline, 12) == 0)
		{
			string s(sline + 12), cname, cval;
			cname = s.substr(0, s.find('='));
			cval = s.substr(s.find('=') + 1, s.find(';') - s.find('=') - 1);
			cookiesmap[cname] = cval;
		}
		sline = strtok(0, "\n");
	}
}

void create_cookie_str(map<string, string> &cookiesmap, char * cookieline)
{
	string cookiesstr;
    map<string, string>::iterator it;
    for(it = cookiesmap.begin(); it != cookiesmap.end(); it++)
    {
        cookiesstr += ' ';
        cookiesstr += it->first;
        cookiesstr += '=';
        cookiesstr += it->second;
        cookiesstr += ';';
    }
    if(!cookiesstr.empty())
    {
        cookiesstr.erase(cookiesstr.length() - 1);
        strcpy(cookieline, "Cookie:");
        strcat(cookieline, cookiesstr.c_str());
    }
}

void create_form_data(JSON_Object *jdata, string &form_data)
{
	for(unsigned int i = 0; i < json_object_get_count(jdata); i++)
	{
	    form_data += json_object_get_name(jdata, i);
	    form_data += '=';
	    form_data += json_value_get_string(json_object_get_value_at(jdata, i));
	    form_data += '&';
    }
	form_data.erase(form_data.length() - 1);
}

int main(int argc, char *argv[])
{
    const char *host = "185.118.200.35";
    const int port = 8081;
    char url[1000], method[10], cookieline[4096];
    string form_data, auth, ctype, query_params;
    int sockfd = -1;
    int count = 0;

    strcpy(url, "/task1/start");
    strcpy(method, "GET");
    strcpy(cookieline, "");

    JSON_Value *root;
    JSON_Object *jobj, *jdata;
    map<string, string> cookiesmap;

    char *message;
    char *response;
    char *json_start;




    //Stage 1 #####################################################################################


    sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
	    perror("socket error");
	    exit(-1);
	}

	message = 0;
    response = 0;	

    printf("\nStage 1\n\n");

	if(strcmp("GET", method) == 0)
	{
		message = compute_get_request(host, url, query_params.c_str(), cookieline, auth.c_str());
	    if(message == 0)
	    {
	        perror("get compute error");
	        close(sockfd);
	        exit(-1);
	    }
	}
	else
	{
	    message = compute_post_request(host, url, form_data.c_str(), ctype.c_str(), cookieline, auth.c_str());
		if(message == 0)
		{
		    perror("post compute error");
		    close(sockfd);
		    exit(-1);
		}
	}


	//send http request and receive response

	printf("\t[message]::\n\n%s\n::[end message]\n\n", message);
	send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
	printf("\t[resp]::\n\n%s\n::[end resp]\n\n", response);


	//save cookies in set in response

	parse_cookies(response, cookiesmap);
	create_cookie_str(cookiesmap, cookieline);
	printf("\t[newly set cookies]::\n\n%s\n\n", cookieline);


	//find json start

	json_start = 0;
	json_start = strstr(response, "\r\n\r\n");
	if(json_start == 0)
	{
		perror("no json found");
	   	delete[] message;
	   	delete[] response;
	   	exit(-1);
	}
	json_start += 4;


	//parse json

	root = 0;
	jobj = 0;
	jdata = 0;
	form_data.clear();
	ctype.clear();
	auth.clear();
	query_params.clear();
   	root = json_parse_string(json_start);
    if(json_value_get_type(root) != JSONObject)
    {
    	printf("error json type\n");
    }
    jobj = json_value_get_object(root);


    //get info from json

    strcpy(url, json_object_get_string(jobj, "url"));
    strcpy(method, json_object_get_string(jobj, "method"));
    if(json_object_get_string(jobj, "type") != 0)
    	ctype += json_object_get_string(jobj, "type");
    jdata = json_object_get_object(jobj, "data");

    //form data type
    if(!ctype.empty() && ctype.compare("application/x-www-form-urlencoded") == 0)
	{
		create_form_data(jdata, form_data);
	}
    

	if(message != 0)
    	delete[] message;
    if(response != 0)
    	delete[] response;
    close(sockfd);
    json_value_free(root);




    //Stage 2 #####################################################################################


    sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
	    perror("socket error");
	    exit(-1);
	}

	message = 0;
    response = 0;

    printf("\nStage 2\n\n");

	if(strcmp("GET", method) == 0)
	{
		message = compute_get_request(host, url, query_params.c_str(), cookieline, auth.c_str());
	    if(message == 0)
	    {
	        perror("get compute error");
	        close(sockfd);
	        exit(-1);
	    }
	}
	else
	{
	    message = compute_post_request(host, url, form_data.c_str(), ctype.c_str(), cookieline, auth.c_str());
		if(message == 0)
		{
		    perror("post compute error");
		    close(sockfd);
		    exit(-1);
		}
	}


	//send http request and receive response

	printf("\t[message]::\n\n%s\n::[end message]\n\n", message);
	send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
	printf("\t[resp]::\n\n%s\n::[end resp]\n\n", response);


	//save cookies in set in response

	parse_cookies(response, cookiesmap);
	create_cookie_str(cookiesmap, cookieline);
	printf("\t[newly set cookies]::\n\n%s\n\n", cookieline);


	//find json start

	json_start = 0;
	json_start = strstr(response, "\r\n\r\n");
	if(json_start == 0)
	{
		perror("no json found");
	   	delete[] message;
	   	delete[] response;
	   	exit(-1);
	}
	json_start += 4;


	//parse json

	root = 0;
	jobj = 0;
	jdata = 0;
	form_data.clear();
	ctype.clear();
	auth.clear();
	query_params.clear();
   	root = json_parse_string(json_start);
    if(json_value_get_type(root) != JSONObject)
    {
    	printf("error json type\n");
    }
    jobj = json_value_get_object(root);


    //get info from json

    strcpy(url, json_object_get_string(jobj, "url"));
    strcpy(method, json_object_get_string(jobj, "method"));
    if(json_object_get_string(jobj, "type") != 0)
    	ctype += json_object_get_string(jobj, "type");
    jdata = json_object_get_object(jobj, "data");

    //form data type
    if(!ctype.empty() && ctype.compare("application/x-www-form-urlencoded") == 0)
	{
		create_form_data(jdata, form_data);
	}
    
    //get auth token
	auth += json_object_get_string(jdata, "token");

	//get query params
	create_form_data(json_object_get_object(jdata, "queryParams"), query_params);
	query_params += "&raspuns1=omul&raspuns2=numele";

	if(message != 0)
    	delete[] message;
    if(response != 0)
    	delete[] response;
    close(sockfd);
    json_value_free(root);




    //Stage 3 #####################################################################################


    sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
	    perror("socket error");
	    exit(-1);
	}

	message = 0;
    response = 0;

    printf("\nStage 3\n\n");

	if(strcmp("GET", method) == 0)
	{
		message = compute_get_request(host, url, query_params.c_str(), cookieline, auth.c_str());
	    if(message == 0)
	    {
	        perror("get compute error");
	        close(sockfd);
	        exit(-1);
	    }
	}
	else
	{
	    message = compute_post_request(host, url, form_data.c_str(), ctype.c_str(), cookieline, auth.c_str());
		if(message == 0)
		{
		    perror("post compute error");
		    close(sockfd);
		    exit(-1);
		}
	}


	//send http request and receive response

	printf("\t[message]::\n\n%s\n::[end message]\n\n", message);
	send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
	printf("\t[resp]::\n\n%s\n::[end resp]\n\n", response);


	//save cookies in set in response

	parse_cookies(response, cookiesmap);
	create_cookie_str(cookiesmap, cookieline);
	printf("\t[newly set cookies]::\n\n%s\n\n", cookieline);


	//find json start

	json_start = 0;
	json_start = strstr(response, "\r\n\r\n");
	if(json_start == 0)
	{
		perror("no json found");
	   	delete[] message;
	   	delete[] response;
	   	exit(-1);
	}
	json_start += 4;


	//parse json

	root = 0;
	jobj = 0;
	jdata = 0;
	form_data.clear();
	ctype.clear();
	//auth.clear();
	query_params.clear();
   	root = json_parse_string(json_start);
    if(json_value_get_type(root) != JSONObject)
    {
    	printf("error json type\n");
    }
    jobj = json_value_get_object(root);


    //get info from json

    strcpy(url, json_object_get_string(jobj, "url"));
    strcpy(method, json_object_get_string(jobj, "method"));
    if(json_object_get_string(jobj, "type") != 0)
    	ctype += json_object_get_string(jobj, "type");
    //jdata = json_object_get_object(jobj, "data");

    //form data type

    if(!ctype.empty() && ctype.compare("application/x-www-form-urlencoded") == 0)
	{
		create_form_data(jdata, form_data);
	}

	if(message != 0)
    	delete[] message;
    if(response != 0)
    	delete[] response;
    close(sockfd);
    json_value_free(root);




	//Stage 4 #####################################################################################


    sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
	    perror("socket error");
	    exit(-1);
	}

	message = 0;
    response = 0;

    printf("\nStage 4\n\n");

	if(strcmp("GET", method) == 0)
	{
		message = compute_get_request(host, url, query_params.c_str(), cookieline, auth.c_str());
	    if(message == 0)
	    {
	        perror("get compute error");
	        close(sockfd);
	        exit(-1);
	    }
	}
	else
	{
	    message = compute_post_request(host, url, form_data.c_str(), ctype.c_str(), cookieline, auth.c_str());
		if(message == 0)
		{
		    perror("post compute error");
		    close(sockfd);
		    exit(-1);
		}
	}


	//send http request and receive response

	printf("\t[message]::\n\n%s\n::[end message]\n\n", message);
	send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
	printf("\t[resp]::\n\n%s\n::[end resp]\n\n", response);


	//save cookies in set in response

	parse_cookies(response, cookiesmap);
	create_cookie_str(cookiesmap, cookieline);
	printf("\t[newly set cookies]::\n\n%s\n\n", cookieline);


	//find json start

	json_start = 0;
	json_start = strstr(response, "\r\n\r\n");
	if(json_start == 0)
	{
		perror("no json found");
	   	delete[] message;
	   	delete[] response;
	   	exit(-1);
	}
	json_start += 4;


	//parse json

	root = 0;
	jobj = 0;
	jdata = 0;
	form_data.clear();
	ctype.clear();
	//auth.clear();
	query_params.clear();
   	root = json_parse_string(json_start);
    if(json_value_get_type(root) != JSONObject)
    {
    	printf("error json type\n");
    }
    jobj = json_value_get_object(root);


    //get info from json

    strcpy(url, json_object_get_string(jobj, "url"));
    strcpy(method, json_object_get_string(jobj, "method"));
    if(json_object_get_string(jobj, "type") != 0)
    	ctype += json_object_get_string(jobj, "type");
    jdata = json_object_get_object(jobj, "data");

    //form data type
    
    if(!ctype.empty() && ctype.compare("application/x-www-form-urlencoded") == 0)
	{
		create_form_data(jdata, form_data);
	}

	//get weather request params
	string weather_url_full, weather_queryParams, weather_method, weather_dns, weather_url;
	//get weather query params
	create_form_data(json_object_get_object(jdata, "queryParams"), weather_queryParams);
	weather_url_full += json_object_get_string(jdata, "url");
    weather_method += json_object_get_string(jdata, "method");
    weather_dns += weather_url_full.substr(0, weather_url_full.find("/"));
    weather_url += weather_url_full.substr(weather_url_full.find("/"), string::npos);
    printf("\n%s\n%s\n", weather_dns.c_str(), weather_url.c_str());

	if(message != 0)
    	delete[] message;
    if(response != 0)
    	delete[] response;
    close(sockfd);
    json_value_free(root);


    //resolve weather dns

    struct addrinfo hints, *res;
  	char ip[100];
  	void *ptr;

  	memset(&hints, 0, sizeof (hints));
  	hints.ai_family = AF_INET;
  	hints.ai_socktype = SOCK_STREAM;
  	hints.ai_flags |= AI_CANONNAME;

  	if(getaddrinfo(weather_dns.c_str(), "80", &hints, &res) != 0)
  	{
  		perror("could not resolve dns of weather service");
  		return 0;
  	}
  	strcpy(ip, inet_ntoa(((sockaddr_in *)(res->ai_addr))->sin_addr));
  	freeaddrinfo(res);




    //Stage 5 #####################################################################################
    //get weather 


    sockfd = open_connection(ip, 80, AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
	    perror("socket error");
	    exit(-1);
	}

	message = 0;
    response = 0;

    printf("\nStage 5 - get weather\n\n");

	message = compute_get_request(weather_dns.c_str(), weather_url.c_str(), weather_queryParams.c_str(), "", "");
	if(message == 0)
	{
	    perror("get compute error");
	    close(sockfd);
	    exit(-1);
	}


	//send http request and receive response

	printf("\t[message]::\n\n%s\n::[end message]\n\n", message);
	send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
	printf("\t[resp]::\n\n%s\n::[end resp]\n\n", response);


	//get json string

	//find json start

	json_start = 0;
	json_start = strstr(response, "\r\n\r\n");
	if(json_start == 0)
	{
		perror("no json found");
	   	delete[] message;
	   	delete[] response;
	   	exit(-1);
	}
	json_start += 4;

	//create form data of type json
	if(!ctype.empty() && ctype.compare("application/json") == 0)
	{
		form_data += json_start;
	}


	if(message != 0)
    	delete[] message;
    if(response != 0)
    	delete[] response;
    close(sockfd);
    json_value_free(root);




	//Stage 5 #####################################################################################
    //send weather report


    sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
	    perror("socket error");
	    exit(-1);
	}

	message = 0;
    response = 0;

    printf("\nStage 5 - post weather\n\n");

	if(strcmp("GET", method) == 0)
	{
		message = compute_get_request(host, url, query_params.c_str(), cookieline, auth.c_str());
	    if(message == 0)
	    {
	        perror("get compute error");
	        close(sockfd);
	        exit(-1);
	    }
	}
	else
	{
	    message = compute_post_request(host, url, form_data.c_str(), ctype.c_str(), cookieline, auth.c_str());
		if(message == 0)
		{
		    perror("post compute error");
		    close(sockfd);
		    exit(-1);
		}
	}


	//send http request and receive response

	printf("\t[message]::\n\n%s\n::[end message]\n\n", message);
	send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
	printf("\t[resp]::\n\n%s\n::[end resp]\n\n", response);


	//save cookies in set in response

	parse_cookies(response, cookiesmap);
	create_cookie_str(cookiesmap, cookieline);
	printf("\t[newly set cookies]::\n\n%s\n\n", cookieline);

	if(message != 0)
    	delete[] message;
    if(response != 0)
    	delete[] response;
    close(sockfd);
    json_value_free(root);

    return 0;
}
