#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "socket.h"

#ifndef PORT
	#define PORT 60000 // the port for the client server connection
#endif

void read_name(int client_index, struct sockname *connections, int *names,int *num_players);
int test(char *figure1,char *figure2);
void set_number(char ch, int *num);

int main(int argc, char **argv){
  int port;
  if(argc > 2){
    fprintf(stderr, "Usage: Extra arguments\n");
    exit(1);
  }else if(argc == 2){
    port = PORT + strtol(argv[1],NULL,10);
  }else{
    port = PORT;
  }

  struct sockaddr_in *server = init_server_addr(port); // server address information
  int listensoc = set_up_server_socket(server,port); // listen on sock id

  struct sockname connections[MAX_CONNECTIONS]; // client sock ids
  for(int index = 0; index < MAX_CONNECTIONS; index++){
    connections[index].sock_fd = -1;
    memset(connections[index].username,0,BUF_SIZE);
    connections[index].name_read = 0;
  }

  /* Accept two clients and Read in names of clients */
  int max_fd = listensoc;
  fd_set all_fds;
  FD_ZERO(&all_fds);
  FD_SET(listensoc, &all_fds);

  int num_players = 0;
  int names = 0;
  while(num_players < MAX_CONNECTIONS || names < MAX_CONNECTIONS){
    fd_set listen_fds = all_fds;
    int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
    if(nready == -1){
      perror("server: select");
      exit(1);
    }else if(nready == 0)
      continue;

    // Create new connections with clients
    if(num_players < MAX_CONNECTIONS && FD_ISSET(listensoc, &listen_fds)){
      int client_fd = accept_connection(listensoc, connections);
      if(client_fd > max_fd){
        max_fd = client_fd;
      }
      FD_SET(client_fd, &all_fds);
      num_players += 1;
      //printf("Accepted %d connection\n",num_players);
    }
    // Read in names of clients
    if(names < MAX_CONNECTIONS){
      for(int index = 0; index < MAX_CONNECTIONS; index++){
        if(connections[index].sock_fd > -1
           && FD_ISSET(connections[index].sock_fd, &listen_fds)){
          read_name(index,connections,&names,&num_players);
        }
      }
    }
  }
  //printf("Finish reading the names and start game.\n");

  /* Initialize and tell two clients to start game */
  max_fd = -1;
  FD_ZERO(&all_fds);
  for(int index = 0; index < MAX_CONNECTIONS; index++){
    if(connections[index].sock_fd > -1){
      int client_fd = connections[index].sock_fd;
      if(client_fd > max_fd){
        max_fd = client_fd;
      }
      FD_SET(client_fd, &all_fds);
    }
  }
  int total = 0;
  int win[MAX_CONNECTIONS] = {0};
  char instru[4] = {'1','\r','\n','\0'};
  char gestures[MAX_CONNECTIONS][4] = {'\0'};
  char result[256] = {'\0'};
  int s_bytes,r_bytes;

  char startgame[BUF_SIZE] = {'\0'};
  strcpy(startgame,"All players ready, start game!\r\n");
  s_bytes = write(connections[0].sock_fd,startgame,strlen(startgame));
  check_error(s_bytes);
  s_bytes = write(connections[1].sock_fd,startgame,strlen(startgame));
  check_error(s_bytes);

  /* Read gestures from two clients, decide and send result */
  while(1){
    s_bytes = write(connections[0].sock_fd,instru,strlen(instru));
    check_error(s_bytes);
    s_bytes = write(connections[1].sock_fd,instru,strlen(instru));
    check_error(s_bytes);
    if(instru[0] == '0'){
      close(connections[0].sock_fd);
      close(connections[1].sock_fd);
      break;
    }
    int sum = 0;
    int first = 0;
    int second = 0;
    // Read in two gestures
    while(sum != MAX_CONNECTIONS){
      fd_set listen_fds = all_fds;
      int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
      if(nready == -1){
        perror("server: select");
        exit(1);
      }
      if(first==0 && FD_ISSET(connections[0].sock_fd, &listen_fds)){
        r_bytes = read(connections[0].sock_fd,gestures[0],3);
        check_error(r_bytes);
        first = 1;
        //printf("Finish reading figure1\n");
      }

      if(second==0 && FD_ISSET(connections[1].sock_fd, &listen_fds)){
        r_bytes = read(connections[1].sock_fd,gestures[1],3);
        check_error(r_bytes);
        second = 1;
        //printf("Finish reading figure2\n");
      }
      sum = first + second;
    }
    // Decide winner and send result
    if(gestures[0][0] == 'e' || gestures[1][0] == 'e'){
      instru[0] = '0';
      char TOTAL[BUF_SIZE];
      char WIN1[BUF_SIZE];
      char WIN2[BUF_SIZE];
      sprintf(TOTAL,"%d",total);
      sprintf(WIN1,"%d",win[0]);
      sprintf(WIN2,"%d",win[1]);

      strcat(WIN1," in ");
      strcat(WIN1,TOTAL);
      strcat(WIN1," rounds.\r\n");
      strcat(WIN2," in ");
      strcat(WIN2,TOTAL);
      strcat(WIN2," rounds.\r\n");
      s_bytes = write(connections[0].sock_fd,WIN1,strlen(WIN1));
      check_error(s_bytes);
      s_bytes = write(connections[1].sock_fd,WIN2,strlen(WIN2));
      check_error(s_bytes);
    }else{
      int test_result = test(gestures[0],gestures[1]);
      memset(gestures,0,sizeof(gestures));

      total += 1;
      if(test_result == 1){
        strcat(result,connections[0].username);
        strcat(result," beat ");
        strcat(result,connections[1].username);
        strcat(result,".\r\n");
        win[0] += 1;
      }else if(test_result == 2){
        strcat(result,connections[1].username);
        strcat(result," beat ");
        strcat(result,connections[0].username);
        strcat(result,".\r\n");
        win[1] += 1;
      }else{
        strcat(result,connections[0].username);
        strcat(result," tie with ");
        strcat(result,connections[1].username);
        strcat(result,".\r\n");
      }
      s_bytes = write(connections[0].sock_fd,result,strlen(result));
      check_error(s_bytes);
      s_bytes = write(connections[1].sock_fd,result,strlen(result));
      check_error(s_bytes);
      memset(gestures,0,sizeof(gestures));
      memset(result,0,sizeof(result));
    }
  }
  free(server);
  close(listensoc);
  return 0;
}

// Read in names of clients
void read_name(int client_index, struct sockname *connections, int *names, 
                   int *num_players){
  int fd = connections[client_index].sock_fd;
  char buf[BUF_SIZE];
  char *after = buf;
  int nbytes, inbuf;
  int room = BUF_SIZE;
  while((nbytes = read(fd, after, room))>0){
    int index = find_network_newline(buf);
    if(index < 0){
      inbuf += nbytes;
      after = buf + inbuf;
      room = BUF_SIZE - inbuf;
      continue;
    }else{
      strcat(connections[client_index].username,buf);
      if(index >= 2){
        connections[client_index].username[index - 2] = '\0';
      }
      connections[client_index].name_read = 1;
      *names += 1;
      break;
    }
  }
}

// Decide who is the winner
int test(char *figure1,char *figure2){
  int num1,num2;
  set_number(figure1[0],&num1);
  set_number(figure2[0],&num2);
  int diff = num1 - num2;
  if(diff > 2)
    num1 -= 5;
  else if(diff < -2)
    num1 += 5;
  if(num1 > num2)
    return 1;
  else if(num1 < num2)
    return 2;
  else
    return 0;
}
void set_number(char ch, int *num){
  switch(ch){
    case 'r':
      *num = 0;
      break;
    case 'S':
      *num = 1;
      break;
    case 'p':
      *num = 2;
      break;
    case 'l':
      *num = 3;
      break;
    case 's':
      *num = 4;
      break;
    default:
      break;
  }
}
