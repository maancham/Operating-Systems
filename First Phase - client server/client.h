#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>                      
#include <sys/time.h> 
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include "fcntl.h"

#define TRUE 1 
#define FALSE 0
//#define SERVER_PORT 2000
#define MAXLINE 1025
#define MAX_GROUPS 5
#define LOCALHOST "127.0.0.1"
#define IFCONFIG "127.255.255.255"

int group_ports[MAX_GROUPS] = {2100, 2200, 2300, 2400, 2500};
int pv_ports[MAX_GROUPS] = {3100, 3200, 3300, 3400, 3500};
bool IsHeartBeating;
int server_sock;
int gp_index_global;


#endif