#ifndef SERVER_H
#define SERVER_H

#include <string.h>    
#include <stdlib.h>
#include <errno.h>  
#include <unistd.h>    
#include <arpa/inet.h>      
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <sys/time.h> 
#include <signal.h>
#include <time.h>  
#include <stdbool.h>
#include "fcntl.h"

#define TRUE 1 
#define FALSE 0 
//#define PORT 2000
#define MAX_CLIENTS 5
#define MAX_GROUPS 5
#define MAX_NAME 50
#define MAX_CHAT_LEN 5000
#define MAX_PORT_DIGIT 4
#define LOCALHOST "127.0.0.1"

struct Group {
   char id;
   int port;
   char chat[MAX_CHAT_LEN];
};


int group_ports[MAX_GROUPS] = {2100, 2200, 2300, 2400, 2500};
int hb_ports[MAX_GROUPS] = {3100, 3200, 3300, 3400, 3500};
int alive_groups[MAX_GROUPS];
int max_clients = MAX_CLIENTS;
int client_socket[MAX_CLIENTS];
int current_gp[MAX_GROUPS];
struct Group groups[MAX_GROUPS];
struct sockaddr_in address, *hb_address[MAX_PORT_DIGIT];

char *welcome_message = "Welcome,select from the following commands:\n"
"See list of groups --> glist\n"
"Join a special group --> goin-GroupName\n"
"Create a group --> gmake-GroupName\n"
"Create a private chat --> poin-PersonID\n"
"Exit current group/chat --> exit/quit\n"
"Use -C- before line when talking in a chat!";

char *groups_announcer = "Group created successfully!";
char *error_group_exists = "Error! Group already exists";
char *error_group_join = "Error! No live groups.Create one!";
char *error_group_noexist = "Error! No Group found with this name!";
char *error_user_noexist = "Error! No User found with this name!";
char *error_user_busy = "Error! This User is Busy!";
char *pv_has_started = "------Private chat has been started------";
char *user_joined_group = "User has joined group chat successfully!";

#endif