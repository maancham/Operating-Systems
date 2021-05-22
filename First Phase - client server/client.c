#include "client.h"

void beat() 
{
	char str[1];
	sprintf(str, "%d", gp_index_global);
	char *temp = malloc(2);
	temp[0] = '@';
	temp[1] = str[0];
    write(server_sock, temp, strlen(temp));
	alarm(5);

}

int max(int num1, int num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

char *make_partner_header(char p_id)
{
	int num = 7;
	char *ret = malloc(7);
	ret[0] = 'U';
	ret[1] = 'S';
	ret[2] = 'E';
	ret[3] = 'R';
	ret[4] = ' ';
	ret[5] = p_id;
	ret[6] = ':';
    return ret;
}


int main(int argc , char *argv[])
{
	int SERVER_PORT = atoi(argv[1]);
	int opt = TRUE;
	int gp_sock, sock, val_read, master_socket, chat_port, max_sd, max_gp_sd;
	struct sockaddr_in server, group_adr, pv;
	char first_message[274] , server_reply[1025], message[250], gp_chat[250], gp_reply[1025];
	char pv_chat[250];
	char *partner_tag;
	char *myself_tag;
	char id;
	int key = 0;
	char right_now;
	int partner_id;
	// 0 - first command after seeing welcome!
	// 1 - command is glist!
	// 2 - command is gmake!
	// 3 - command is to join a group!
	// 4 - command is to join a pv chat!



	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket!\n");
	}
	puts("-->Socket created<--");
	
	fd_set readfds, gpfds, sigfds, pvfds;
	max_sd = sock;

    server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(LOCALHOST);
	server.sin_port = htons( SERVER_PORT );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server, sizeof(server)) != 0)
	{
		perror("NO SERVER");
		return 1;
	}
	server_sock = sock;
	puts("-->Connected<--"); 
	val_read = read(sock , first_message, sizeof(first_message));
    puts(first_message);
	val_read = read(sock , server_reply, sizeof(server_reply));
	puts(server_reply);
	id = server_reply[24];
	myself_tag = make_partner_header(id);
	right_now = '0';

	//keep communicating with server
	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		FD_SET(0, &readfds);
		int activity = select(max_sd + 1 , &readfds , NULL , NULL , NULL);
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			perror("select error"); 
		} 
		if (FD_ISSET(sock, &readfds))
		{
			memset(server_reply, '\0', sizeof(server_reply));
			val_read = read(sock , server_reply, sizeof(server_reply));
			if (val_read == 0)
			{
				puts("Server has been killed, Adios!");
				break;
			}
			else
			{
				server_reply[val_read] = '\0';
				if (server_reply[0] == '4')
				{
					partner_tag = make_partner_header(server_reply[1]);
					partner_id = server_reply[1] - '0';
					key = 1;
					right_now = '4';
					puts("------Private chat has been started------");
				}
				else if (right_now == '1' || right_now == '2')
				{
					puts(server_reply);
					right_now = '0';
				}
				else if(right_now == '3')
				{
					if(server_reply[0] == 'E')
					{
						puts(server_reply);
						right_now = '0';
					}
					else
					{
						puts("------Group chat has been started------");
						int gp_index = server_reply[0] - '0';
						gp_index_global = gp_index;
						int gp_port = group_ports[gp_index];
						gp_sock = socket(AF_INET, SOCK_DGRAM, 0);
						if (gp_sock == -1)
							perror("Group Socket falied!");
						group_adr.sin_family = AF_INET;
						group_adr.sin_addr.s_addr = INADDR_ANY;
						group_adr.sin_port = htons( gp_port );
						int reuse = 1;
						if (setsockopt(gp_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
						{
							perror("setsockopt"); 
							exit(EXIT_FAILURE);
						}
						int broadcast = 1;
						if (setsockopt(gp_sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) < 0)
						{
							perror("setsockopt"); 
							exit(EXIT_FAILURE);
						}
						
						max_gp_sd = gp_sock;
						struct sockaddr_in bc_adr;
						//memset(&bc_adr, '\0', sizeof(bc_adr));
						bc_adr.sin_family = AF_INET;  
						bc_adr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
						bc_adr.sin_port = htons(gp_port);
						bind(gp_sock, (struct sockaddr *)&bc_adr, sizeof(bc_adr));
						signal(SIGALRM, beat);
						alarm(1);
						while(1)
						{
							FD_ZERO(&sigfds);
							FD_SET(sock, &sigfds);
							FD_SET(1, &sigfds);
							FD_ZERO(&gpfds);
							FD_SET(gp_sock, &gpfds);
							FD_SET(0, &gpfds);
							int sd = max(max_gp_sd , sock) + 1;
							int activity = select(sd , &gpfds , &sigfds , NULL , NULL);
							if ((activity < 0) && (errno!=EINTR)) 
							{ 
								perror("select error"); 
							} 
							if (FD_ISSET(gp_sock, &gpfds))
							{
								memset(gp_reply, '\0', sizeof(gp_reply));
								val_read = recvfrom(gp_sock, gp_reply, sizeof(gp_reply),  MSG_WAITALL, 
									( struct sockaddr *) &group_adr, sizeof(bc_adr)); 
								if (gp_reply[5] != id)
								{
									puts(gp_reply);
								}
								
								
							}
							if (FD_ISSET(0, &gpfds))
							{
								memset(gp_chat, '\0', sizeof(gp_chat));
								read(0, gp_chat, sizeof(gp_chat));
								if(gp_chat[0] == 'q')
									break;
								else
								{
									char *result = malloc(strlen(gp_chat) + strlen(myself_tag) + 1);
									strcpy(result, myself_tag);
									strcat(result, gp_chat);
									int a = sendto(gp_sock, result, strlen(result), 0, 
										(struct sockaddr *) &bc_adr, sizeof(bc_adr));		
								}
								
							}
						}
						alarm(0);
						right_now = '0';
						close(gp_sock);
						if(write(sock, "quit", strlen("quit")) == -1)
							puts("error");
						else
							puts("------Group chat abandoned!------");				
					}
					
				}
				else if ((right_now == '4') || (key == 1))
				{
					if(server_reply[0] == 'E')
					{
						puts(server_reply);
						right_now = '0';
					}
					else if(server_reply[0] == '#')
					{
						int pv_sock;
						int client_socket = 0;
						pv_sock = socket(AF_INET , SOCK_STREAM , 0);
						if (pv_sock == -1)
						{
							printf("Could not create socket!\n");
						}
						puts("-->Pv Socket created<--");
						pv.sin_family = AF_INET;
						pv.sin_addr.s_addr = inet_addr(LOCALHOST);
						pv.sin_port = htons( pv_ports[0] );
						if (bind(pv_sock, (struct sockaddr *)&pv, sizeof(pv))<0) 
						{ 
							perror("bind failed"); 
							exit(EXIT_FAILURE); 
						} 
						if (listen(pv_sock, 5) < 0) 
						{ 
							perror("listen"); 
							exit(EXIT_FAILURE); 
						} 
						while(TRUE)
						{
							FD_ZERO(&pvfds); 
							FD_SET(pv_sock, &pvfds); 
							FD_SET(0, &pvfds);
							int max_sd = pv_sock;
							int sd = client_socket;
							if (sd > 0)
								FD_SET( sd , &pvfds);
							if (sd > max_sd)
								max_sd = sd;
							int activity = select( max_sd + 1 , &pvfds , NULL , NULL , NULL); 
							if ((activity < 0) && (errno!=EINTR)) 
							{ 
								puts("select error"); 
							}
							int new_socket;
							int addrlen = sizeof(pv);
							if (FD_ISSET(pv_sock, &pvfds)) 
							{
								if ((new_socket = accept(pv_sock, (struct sockaddr *)&pv, (socklen_t*)&addrlen)) < 0) 
								{ 
									perror("accept"); 
									exit(EXIT_FAILURE); 
								} 
								client_socket = new_socket;
							}
							if (FD_ISSET(client_socket , &pvfds)) 
							{

								int addrlen = sizeof(pv); 
								memset(pv_chat, '\0', sizeof(pv_chat));
								int val_read;
								val_read = read( client_socket , pv_chat, sizeof(pv_chat));
								if ((val_read == 0) || (pv_chat[0] == 'e')) 
								{  
									puts("Your friend is disconnected! Adios...");
									write(sock, "&", sizeof("&"));
									close(client_socket);
									close(pv_sock);
									break;
								}
								else
								{
									//puts("kir");
									pv_chat[val_read] = '\0';
									char *result = malloc(strlen(pv_chat) + strlen(partner_tag) + 1);
									strcpy(result, partner_tag);
									strcat(result, pv_chat);
									puts(result);
								}
								  
							}
							if (FD_ISSET(0, &pvfds))
							{
								memset(message, '\0', sizeof(message));
								read(0, message, sizeof(message));
								if(message[0] == 'e')
								{
									write(sock, "&", sizeof("&"));
									write(client_socket, "exit", sizeof("exit"));
									break;
								}
								else if(sizeof(message) != 0)
								{
									write(client_socket, message, sizeof(message));		
								}
							}
						} 
						right_now = '0';
					}
					else
					{
						int pv_sock;
						pv_sock = socket(AF_INET , SOCK_STREAM , 0);
						if (pv_sock == -1)
						{
							printf("Could not create socket!\n");
						}
						puts("-->Pv Socket created<--");
						pv.sin_family = AF_INET;
						pv.sin_addr.s_addr = inet_addr(LOCALHOST);
						pv.sin_port = htons( pv_ports[0] );
						if (connect(pv_sock , (struct sockaddr *)&pv, sizeof(pv)) != 0)
						{
							perror("NO SERVER");
							return 1;
						}
						while(TRUE)
						{
							FD_ZERO(&pvfds);
							FD_SET(pv_sock, &pvfds);
							FD_SET(0, &pvfds);
							int max_sd = pv_sock;
							int activity = select(max_sd + 1 , &pvfds , NULL , NULL , NULL);
							if ((activity < 0) && (errno!=EINTR)) 
							{ 
								perror("select error"); 
							} 
							if (FD_ISSET(sock, &readfds))
							{
								memset(pv_chat, '\0', sizeof(pv_chat));
								int val_read = read(pv_sock , pv_chat, sizeof(pv_chat));
								if ((val_read == 0) || (pv_chat[0] == 'e')) 
								{  
									puts("Your friend is disconnected! Adios...");
									write(sock, "&", sizeof("&"));
									close(pv_sock);
									break;
								}
								else
								{
									pv_chat[val_read] = '\0';
									char *result = malloc(strlen(pv_chat) + strlen(partner_tag) + 1);
									strcpy(result, partner_tag);
									strcat(result, pv_chat);
									puts(result);
								}
							}
							if (FD_ISSET(0, &pvfds))
							{
								memset(pv_chat, '\0', sizeof(pv_chat));
								read(0, pv_chat, sizeof(pv_chat));
								if(pv_chat[0] == 'e')
								{
									write(sock, "&", sizeof("&"));
									write(pv_sock, "exit", sizeof("exit"));
									break;
								}
								else
								{
									write(pv_sock, pv_chat, sizeof(pv_chat));		
								}
							}
						}
						right_now = '0';	
					}
					
				}
			}
			
		}
		if (FD_ISSET(0, &readfds))
		{
			memset(message, '\0', sizeof(message));
			read(0, message, sizeof(message));
			if (right_now == '0')
			{
				if (message[1] == 'l')
				{
					right_now = '1';
				}
				else if (message[1] == 'm')
				{
					right_now = '2';
				}
				else if (message[1] == 'o' && message[0] == 'g')
				{
					right_now = '3';
				}
				else if (message[0] == 'p')
				{
					right_now = '4';
					partner_tag = make_partner_header(message[5]);
				}
				else if(key == 1)
					right_now = '4';
				if( write(sock, message, strlen(message)) == -1)
				{
					puts("Sending failed");
				}
			}
			else if (right_now == '4')
			{
				if (message[0] == 'e')
				{
					key = 0;
					right_now = '0';
				}
				if( write(sock, message, strlen(message)) == -1)
				{
					puts("Sending failed");
				}
			}

		}
		
	}
	
	close(sock);
	return 0;
}