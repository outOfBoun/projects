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
#include <netinet/tcp.h>

#include <string>
#include <unordered_map>
#include <set>
#include <list>

#include "msgutils.h"

using namespace std;

#define BUFFLEN 2048

class ClientsDB
{
public:
    struct Client
    {
        set<string> sftopics, nosftopics;
        string id;
        int sockfd;
        bool connected;
    };
    unordered_map<string, Client*> clByID;
    unordered_map<int, Client*> clBySOCK;

    void new_client(int sockfd, char *id)
    {
        Client *cl = new Client;
        cl->id = string(id);
        cl->sockfd = sockfd;
        cl->connected = true;
        clByID[id] = cl;
        clBySOCK[sockfd] = cl;
    }

    ~ClientsDB()
    {
        unordered_map<int, Client*>::iterator it;
        for(it = clBySOCK.begin(); it != clBySOCK.end(); it++)
        {
            close(it->first);
        }
        unordered_map<string, Client*>::iterator it2;
        for(it2 = clByID.begin(); it2 != clByID.end(); it2++)
        {
            delete it2->second;
        }
    }
};

class Msg
{
    public:
    struct smsg
    {
        char topic[51];
        char tip_date;
        char *content;   
    };
    set<ClientsDB::Client *> depends;
    smsg data;
    struct sockaddr_in udpclient;
    socklen_t addrlen;
    int payload_len, header_len;

    Msg(char *buff, struct sockaddr_in udpcl, socklen_t addrl)
    {
        header_len = 52;
        int poz;
        strncpy(data.topic, buff, 50);
        data.topic[50] = 0;

        poz = 50;

        data.tip_date = buff[poz];
        poz++;

        if(data.tip_date == 0)
        {
            data.content = new char[5];
            memcpy(data.content, buff + poz, 5);
            payload_len = 5;
        }
        else if(data.tip_date == 1)
        {
            data.content = new char[2];
            memcpy(data.content, buff + poz, 2);
            payload_len = 2;
        }
        else if(data.tip_date == 2)
        {
            data.content = new char[6];
            memcpy(data.content, buff + poz, 6);
            payload_len = 6;
        }
        else if(data.tip_date == 3)
        {
            char auxbuf[1501];
            strncpy(auxbuf, buff + poz, 1500);
            auxbuf[1500] = 0;
            payload_len = strlen(auxbuf) + 1;
            data.content = new char[payload_len];
            strcpy(data.content, auxbuf);
        }
        else
        {
             perror("type not expected!");
             return;
        }   
        udpclient = udpcl;
        addrlen = addrl;
    }

    ~Msg()
    {
        delete []data.content;
    }

    int sendMsg(int sockfd)
    {
        int n;
        n = tcpsend(sockfd, (char*)&header_len, 4);
        if(n <= 0) return n;
        n = tcpsend(sockfd, data.topic, header_len - 1);
        if(n <= 0) return n;
        n = tcpsend(sockfd, &data.tip_date, 1);
        if(n <= 0) return n;
        n = tcpsend(sockfd, (char*)&payload_len, 4);
        if(n <= 0) return n;
        n = tcpsend(sockfd, data.content, payload_len);
        if(n <= 0) return n;
        n = tcpsend(sockfd, (char*)&addrlen, sizeof(socklen_t));
        if(n <= 0) return n;
        n = tcpsend(sockfd, (char*)&udpclient, addrlen);
        return n;
    }
};

int main(int argc, char **argv) 
{
    int tcplistenfd, udplistenfd, rc;
    char buff[BUFFLEN];

    if(argc != 2) 
    {
        printf("\n Usage: %s <PORT_DORIT>\n", argv[0]);
        return 1;
    }

    struct sockaddr_in servaddr, clientaddr;
    socklen_t addrlen;

    //filling in server addr info
    memset(&servaddr, 0, sizeof(servaddr)); 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(atoi(argv[1]));



    //TCP initialization
    
    //creating listening socket for TCP clients
    if((tcplistenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        perror("listen socket creation failed");
        exit(EXIT_FAILURE);
    }

    int enable = 1;
    if(setsockopt(tcplistenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    enable = 1;
    if(setsockopt(tcplistenfd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(int)) < 0)
        perror("setsockopt(TCP_NODELAY) failed");
    
    //linking the socket to the server addr
    if(bind(tcplistenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
    { 
        perror("bind failed");
        exit(EXIT_FAILURE);
    } 

    //starting to listen on the socket for TCP connections
    rc = listen(tcplistenfd, 100);
    if(rc < 0) 
    {
        perror("listen failed");
        close(tcplistenfd);
        exit(EXIT_FAILURE);
    }



    //UDP initialization

    if((udplistenfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
    { 
        perror("listen socket creation failed");
        exit(EXIT_FAILURE);
    }

    enable = 1;
    if(setsockopt(udplistenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    if(bind(udplistenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
    { 
        perror("bind failed");
        exit(EXIT_FAILURE);
    }



    //multiplexing setup

    fd_set read_set;
    FD_ZERO(&read_set);
    int maxfd;
    //adding stdin, TCP and UDP sockets
    FD_SET(STDIN_FILENO, &read_set);
    maxfd = STDIN_FILENO;
    FD_SET(tcplistenfd, &read_set);
    if(tcplistenfd > maxfd)
        maxfd = tcplistenfd;
    FD_SET(udplistenfd, &read_set);
    if(udplistenfd > maxfd)
        maxfd = udplistenfd;



    //data structures for storing client's info and topics

    ClientsDB cldb;
    unordered_map<string, list<Msg *>> topicsToMsg;
    unordered_map<string, set<ClientsDB::Client *>> topicsToCl;



    //Main program loop

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
            if(strcmp("exit\n", buff) == 0)
            {
                //cleanup of sockets and left messages in ClientsDB destructor
                break;
            }
        }


        else if(FD_ISSET(tcplistenfd, &tmp_read_set)) //new connection or returning client#########
        {
            //process new connection
            addrlen = sizeof(clientaddr);
            int new_sock = accept(tcplistenfd, (struct sockaddr *)&clientaddr, &addrlen);
            enable = 1;
            if(setsockopt(new_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
                perror("setsockopt(SO_REUSEADDR) failed");
            enable = 1;
            if(setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(int)) < 0)
                perror("setsockopt(TCP_NODELAY) failed");
            if(new_sock < 0)         
            {
                perror("accept error");
                exit(EXIT_FAILURE);
            }

            //get client id
            if(tcprecv(new_sock, buff, sizeof(buff), 11) <= 0)
                continue;

            //send sf topics if reconnected
            unordered_map<string, ClientsDB::Client *>::iterator it;
            it = cldb.clByID.find(string(buff));
            if(it != cldb.clByID.end())
            {
                //existing client reconnecting
                if((it->second)->connected == true)
                {
                    //if already connected don't allow new connection on same id
                    close(new_sock);
                    continue;
                }

                //client reconnected. insert into db
                cldb.clBySOCK[new_sock] = it->second;
                (it->second)->connected = true;
                (it->second)->sockfd = new_sock;

                //send sf topics missed while disconnected
                for(auto &cltopic : it->second->sftopics)
                {
                    list<Msg*> *msgl = &(topicsToMsg[cltopic]);
                    list<Msg*>::iterator imsg;
                    for(imsg = msgl->begin(); imsg != msgl->end();)
                    {
                        if((*imsg)->depends.find(it->second) != (*imsg)->depends.end())
                        {
                            int n;
                            n = (*imsg)->sendMsg(it->second->sockfd);
                            if(n <= 0)
                            {
                                printf("error sending message to client\n");
                                continue;
                            }
                            (*imsg)->depends.erase(it->second);
                        }
                        if((*imsg)->depends.size() == 0)
                        {
                            //if all dependencies are lost then erase msg
                            delete (*imsg);
                            imsg = msgl->erase(imsg);
                        }
                        else
                            imsg++;
                    }       
                }

            }
            else //else the client is new
            {
                cldb.new_client(new_sock, buff);
            }

            FD_SET(new_sock, &read_set);
            if(maxfd < new_sock)
                maxfd = new_sock;
            printf("New client %s connected from %s:%d.\n", cldb.clBySOCK[new_sock]->id.c_str(),
                inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        }


        else if(FD_ISSET(udplistenfd, &tmp_read_set)) //incoming message from UDP client ##########
        {
            int n;
            addrlen = sizeof(clientaddr);
            n = recvfrom(udplistenfd, buff, sizeof(buff), 0, (struct sockaddr *)&clientaddr, 
                &addrlen);
            buff[n] = 0;
            Msg *mpointer = new Msg(buff, clientaddr, addrlen);
            set<ClientsDB::Client *> missed_cl;
            for(auto client : topicsToCl[mpointer->data.topic])
            {
                if(client->connected == true)
                {
                    int n;
                    n = mpointer->sendMsg(client->sockfd);
                    if(n <= 0)
                    {
                        printf("error sending message to client\n");
                        continue;
                    }
                }
                else if(client->sftopics.find(string(mpointer->data.topic)) != client->sftopics.end())
                {
                    missed_cl.insert(client);
                }
            }

            if(missed_cl.size() != 0) //if all the clients got the message then remove it directly
            {
                mpointer->depends = missed_cl;
                topicsToMsg[mpointer->data.topic].push_back(mpointer);
            }
            else
            {
                delete mpointer;
            }
        }


        else //command from TCP client ###########################################################
        {
            //msg from client close or subscribe
            unordered_map<int, ClientsDB::Client *>::iterator it;
            for(it = cldb.clBySOCK.begin(); it != cldb.clBySOCK.end();)
            {
                if(FD_ISSET(it->first, &tmp_read_set))
                {
                    Subscription sb;
                    int n = tcprecv(it->first, (char*)&sb, sizeof(Subscription), sizeof(Subscription));
                    if(n == 0)
                    {
                        //client left. remove from selection set and from db
                        printf("Client %s disconnected.\n", (it->second)->id.c_str());
                        FD_CLR(it->first, &read_set);
                        close(it->first);
                        (it->second)->connected = false;
                        it = cldb.clBySOCK.erase(it);
                    }
                    else if(n < 0)
                    {
                        perror("recv error");
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        //interpret command
                        unordered_map<string, set<ClientsDB::Client *>>::iterator itopic;
                        itopic = topicsToCl.find(sb.topic);
                        //printf("got %d %s %d", sb.subscribe, sb.topic, sb.sf);
                        if(sb.subscribe == 1)
                        {
                            topicsToCl[sb.topic].insert(it->second);
                            if(sb.sf == 1)
                            {
                                (it->second)->sftopics.insert(string(sb.topic));
                            }
                            else
                            {
                                (it->second)->nosftopics.insert(string(sb.topic));
                            }
                        }
                        else if(sb.subscribe == 0 && (itopic != topicsToCl.end()))
                        {
                            topicsToCl[sb.topic].erase(it->second);
                            (it->second)->sftopics.erase(string(sb.topic));
                            (it->second)->nosftopics.erase(string(sb.topic));
                        }
                        it++;
                    }
                    break;
                }
                else
                    it++;
            }
        }


    }

    //free unsent messages
    for(auto &topic: topicsToMsg)
    {
        for(auto &msg : topic.second)
        {
            delete msg;
        }
    }

    close(tcplistenfd);
    close(udplistenfd);
    return 0;
}
