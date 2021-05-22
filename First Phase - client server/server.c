#include "server.h"

void check_beat()
{
	for(int i=0; i< MAX_GROUPS; i++)
	{
		if((alive_groups[i] == 0) && (groups[i].id != '0'))
		{
			puts("Group terminated!");
			groups[i].id = '0';
		}	
	}
	for(int i=0; i< MAX_GROUPS; i++)
		alive_groups[i] = 0;
	alarm(15);
}


char *show_groups()
{
	int num = 2 * MAX_GROUPS;
	char *ret = malloc(num);
	for (int i=0; i<MAX_GROUPS; i++)
	{
		ret[2 * i] = groups[i].id;
	}
	for (int i=0; i<MAX_GROUPS; i++)
	{
		ret[(2 * i) + 1] = '|';
	}
    return ret;
}

char *trim_chat(char *buffer)
{
	int size = sizeof(buffer);
	char *ret = malloc(size-5);
	for (int i=5; i<size; i++)
	{
		ret[i-5] = buffer[i];
	}
	return ret;

}

int check_join_group(char group_id)
{
	int check = 0;
	for (int i=0; i<MAX_GROUPS; i++)
	{
		if (groups[i].id == '0')
			check ++;
	}
	if (check == 5)
		return -2;
	for (int i=0; i<MAX_GROUPS; i++)
	{
		if (group_id == groups[i].id)
			return i;
	}
	return -1;
}

int check_first_empty_group()
{
	for (int i=0; i<MAX_GROUPS; i++)
	{
		if (groups[i].id == '0')
			return i;
	}
}

bool check_group_duplicate(char c)
{
	for (int i=0; i<MAX_GROUPS; i++)
	{
		if (groups[i].id == c)
			return TRUE;
	}
	return FALSE;
}

int main(int argc , char *argv[]) 
{ 
	struct timespec start,end;
	int opt = TRUE; 
	int master_socket , addrlen , new_socket, activity, i , valread , sd;
	int p1_sock, p2_sock, p1_index, p2_index; 
	int max_sd; 
	int PORT = atoi(argv[1]);
	 
	char buffer[1025];
	//set of socket descriptors 
	fd_set readfds, sigfds; 
		 
	
	//initialise all client_socket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0;
		current_gp[i] = -1;
		groups[i].id = '0';
		groups[i].port = 0;	
		alive_groups[i] = 0;
		strcpy(groups[i].chat,"NOT YET STARTED");
	}

	group_ports[0] = "2100";
	group_ports[1] = "2200";
	group_ports[2] = "2300";
	group_ports[3] = "2400";
	group_ports[4] = "2500";

	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
		sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = inet_addr(LOCALHOST); 
	address.sin_port = htons( PORT ); 
		
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 

	printf("Listener on port %d \n", PORT); 
		
	if (listen(master_socket, 5) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
		
	//accept the incoming connection 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
	signal(SIGALRM, check_beat);
	alarm(1);
	while(TRUE) 
	{ 
		FD_ZERO(&sigfds);
		FD_SET(master_socket, &sigfds);
		FD_SET(1, &sigfds);
		FD_ZERO(&readfds); 
		FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			sd = client_socket[i]; 
			if(sd > 0) 
				FD_SET( sd , &readfds); 
			if(sd > max_sd) 
				max_sd = sd; 
		} 
		activity = select( max_sd + 1 , &readfds , &sigfds , NULL , NULL); 

		if ((activity < 0) && (errno == EINTR)) 
		{
            continue;
        }

		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
			
		if (FD_ISSET(master_socket, &readfds)) 
		{
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
    		printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
        		(address.sin_port)); 

    		if( write(new_socket, welcome_message, strlen(welcome_message)) == -1) 
    		{ 
        		perror("send"); 
    		} 
    		else
			{	
				puts("Welcome message sent successfully"); 
			}
			for (int i = 0; i < max_clients; i++) 
			{ 
				if (client_socket[i] == 0)
				{ 
					client_socket[i] = new_socket;
					printf("User ID would be: %d\n" , i);
					char buf[22];
					snprintf(buf, 28, "Your User ID would be: -%d-\n", i);
					write(new_socket, buf , strlen(buf));	
					break; 
				} 
			} 
		} 
			
		//else its some IO operation on some other socket 
		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i];
			int person_id = i; 
				
			if (FD_ISSET( sd , &readfds)) 
			{ 
				memset(buffer, '\0', sizeof(buffer)); 
				if ((valread = read( sd , buffer, 1024)) == 0) 
				{ 
					getpeername(sd , (struct sockaddr*)&address , \ 
						(socklen_t*)&addrlen); 
					printf("Host disconnected , ip %s , port %d \n" , 
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
					close( sd ); 
					client_socket[i] = 0;
				} 
					
				else
				{ 
					buffer[valread] = '\0';
					if (buffer[0] == 'p')
					{
						char partner = buffer[5];
						for (int j = 0; j < max_clients; j++) 
						{
							char temp[1];
							sprintf(temp, "%d", j);
							if (partner == temp[0])
							{
								if (client_socket[j] == 0)
									write(sd, error_user_noexist, strlen(error_user_noexist));
								else if(current_gp[j] != -1)
									write(sd, error_user_busy, strlen(error_user_busy));
								else
								{
									int second_sd = client_socket[j];
									char str[1];
									char sec_str[1];
									sprintf(str, "%d", i);
									sprintf(sec_str, "%d", j);
									char *temp = malloc(2);
									temp[0] = '4';
									temp[1] = str[0];
									char *for_one = malloc(2);
									for_one[0] = '#';
									for_one[1] = str[1];
									char *for_sec = malloc(2);
									for_sec[0] = '$';
									for_sec[1] = sec_str[1];
									write(sd, for_one, strlen(for_one));
									write(second_sd, temp, strlen(temp));
									sleep(1);
									write(second_sd, for_sec, sizeof(for_sec));
									current_gp[i] = MAX_GROUPS - 1;
									current_gp[j] = MAX_GROUPS - 1;
									break;
								}
								
							}
							
						}
					}
					// else if (buffer[0] == 'C')
					// {
					// 	buffer[0] = ' ';
					// 	if (sd == p1_sock)
					// 	{
					// 		write(p2_sock, buffer, strlen(buffer));
					// 	}
					// 	else
					// 	{
					// 		write(p1_sock, buffer, strlen(buffer));
					// 	}
						
					// }
					else if (buffer[0] == '@')
					{
						int live_gp_index = buffer[1] - '0';
						alive_groups[live_gp_index] = 1;
					}
					else if (buffer[0] == '&')
					{
						current_gp[person_id] = -1;
					}
					else if (buffer[0] == 'q')
					{
						current_gp[person_id] = -1;
					}
					else if(buffer[0] == 'g')
					{
						if(buffer[1] == 'l')  //see list of groups!
						{
							if( write(sd, show_groups(), strlen(show_groups())) == -1) 
							{ 
								perror("send"); 
							}
							else
								puts("Group list sent to User!");
							
						}
						else if(buffer[1] == 'o')  //join a group!
						{
							int number = check_join_group(buffer[5]);
							if(number == -2)
								write(sd, error_group_join, strlen(error_group_join));
							else if(number == -1)
								write(sd, error_group_noexist, strlen(error_group_noexist));
							else if(number >= 0)
							{
								current_gp[i] = number;
								char str[1];
								sprintf(str, "%d", number);
								puts(user_joined_group);
								write(sd, str, strlen(str));
							}
						}
						else if(buffer[1] == 'm')    //make a new group!
						{
							if (check_group_duplicate(buffer[6]) == TRUE)
							{
								puts(error_group_exists);
								if( write(sd, error_group_exists , strlen(error_group_exists)) == -1) 
								{ 
									perror("send"); 
								}
							}
							else
							{
								int new_group_index = check_first_empty_group();
								groups[new_group_index].id = buffer[6];
								groups[new_group_index].port = group_ports[new_group_index];
								puts(groups_announcer);
								if( write(sd, groups_announcer, strlen(groups_announcer)) == -1) 
								{ 
									perror("send"); 
								}
							}					
						}
					}

					
				}
			} 
		} 
	} 
	//close(master_socket);	
	return 0; 
} 
