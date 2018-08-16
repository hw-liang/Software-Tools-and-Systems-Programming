#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "socket.h"

#ifndef PORT
  #define PORT 60000 // the port for the client server connection
#endif

#define L_NAME 128
#define L_RESULT 256
#define L_INPUT 4

int check_figure(char *figure);
int check_end(char *array);

int main(int argc, char **argv){
	int port;
	char name[L_NAME] = {'\0'};
	char figure[L_INPUT] = {'\0'};
	char result[L_RESULT] = {'\0'};

	if(argc == 1){
		fprintf(stderr, "Usage: invalid hostname\n");
		exit(1);
	}else if(argc > 3){
		fprintf(stderr, "Usage: Extra arguments\n");
		exit(1);
	}else if(argc == 3){
		port = PORT + strtol(argv[2],NULL,10);
	}else{
		port = PORT;
	}

	// Connect to server
	int soc = connect_to_server(port, argv[1]);
	int s_bytes,r_bytes;

	// Send client's name
	char keep_name[L_NAME];
	printf("Please input the name of the player:\n");
	scanf("%s",name);
	strcpy(keep_name,name);
	strcat(name,"\r\n");
	s_bytes = write(soc, name, strlen(name));
	check_error(s_bytes);

	char buf[BUF_SIZE] = {'\0'};
	int not_first_time = 0;
	while(1){
		// Initialize and read in result and instruction
		int check = 0;
		int inbuf = 0;
		int room = sizeof(buf);
		char *after = buf;
		while(check < 2){
			r_bytes = read(soc,after,room);
			check_error(r_bytes);
			inbuf += r_bytes;
			room = BUF_SIZE - inbuf;
			after = buf + inbuf;
			check = check_end(buf);
		}
		//printf("Reading in %s\n",buf);
		int new_loc;
		new_loc = find_network_newline(buf);
		buf[new_loc-2] = '\0';
		strcpy(result,buf);

		// Write result to stdout and stderr
		if(buf[new_loc] == '0'){
			char FINAL[BUF_SIZE] = {'\0'};
			strcpy(FINAL,"Game over and you won ");
			strncat(FINAL,result,new_loc - 2);
			printf("%s\n",FINAL);
			break;
		}
		if(not_first_time)
			fprintf(stdout,"In last round: ");
		printf("%s\n",result);
		if(strstr(result,"tie") != NULL){
			fprintf(stderr,"tie\n");
		}else if(strstr(result,keep_name) == result){
			fprintf(stderr,"win\n");
		}else if(strstr(result,keep_name) != NULL && strstr(result,keep_name) != result){
			fprintf(stderr,"lose\n");
		}
		memset(result,0,sizeof(result));
		memset(buf,0,sizeof(buf));

		// Read in gesture from stdin
		int valid = 1;
		while(valid){
			if(valid == 1)
				printf("Please input a figure among{'r','p','s','l','S','e'}:\n");
			else
				printf("Oops, not valid. please input a figure among{'r','p','s','l','S','e'}:\n");
			scanf("%s",figure);
			valid += 1;
			if(check_figure(figure)){
				strncat(figure,"\r\n",2);
				s_bytes = write(soc,figure,strlen(figure));
				check_error(s_bytes);
				valid = 0;
			}
		}
		not_first_time = 1;
	}

	close(soc);
	return 0;
}

int check_figure(char *figure){
	int valid = 0;
	if(strcmp(figure,"r")==0 || strcmp(figure,"p")==0 || strcmp(figure,"s")==0 
        || strcmp(figure,"l")==0 || strcmp(figure,"S")==0 || strcmp(figure,"e")==0)
		valid = 1;
	return valid;
}

// Count the number of seperator("\r\n") in buffer
int check_end(char *buf){
	int n = strlen(buf);
	int num = 0;
	for(int i=0;i<n-1;i++){
		if(buf[i] == '\r' && buf[i+1] == '\n')
			num += 1;
	}
	return num;
}
