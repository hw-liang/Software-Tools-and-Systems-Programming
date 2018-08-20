#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>     /* inet_ntoa */
#include <netdb.h>         /* gethostname */
#include <sys/socket.h>

#include "socket.h"

/******************************************************************************
 * Server-specific functions
 *****************************************************************************/
/*
 * Initialize a server address associated with the given port.
 */
struct sockaddr_in *init_server_addr(int port){
  struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
  // Allow sockets across machines.
  addr->sin_family = AF_INET;
  // The port the process will listen on.
  addr->sin_port = htons(port);
  // Clear this field; sin_zero is used for padding for the struct.
  memset(&(addr->sin_zero), 0, 8);
  // Listen on all network interfaces.
  addr->sin_addr.s_addr = INADDR_ANY;// Personal platform
  return addr;
}

/*
 * Create and set up a socket for a server to listen on.
 */
int set_up_server_socket(struct sockaddr_in *server,int port){
  int soc = socket(AF_INET, SOCK_STREAM, 0);
  if(soc < 0){
    perror("socket");
    exit(1);
  }
  // Make sure we can reuse the port immediately after the
  // server terminates. Avoids the "address in use" error
  int on = 1;
  int status = setsockopt(soc, SOL_SOCKET, SO_REUSEADDR,
      (const char *)&on, sizeof(on));
  if(status < 0){
    perror("setsockopt");
    exit(1);
  }
  // Associate the process with the address and a port
  if(bind(soc, (struct sockaddr *)server, sizeof(*server)) > 0){
    // bind failed; could be because port is in use.
    server->sin_port = htons(port+1);
    if(bind(soc, (struct sockaddr *)server, sizeof(*server)) < 0){
      close(soc);
      perror("bind");
      exit(1);
    }
  }
  // Set up a queue in the kernel to hold pending connections.
  if (listen(soc, 5) < 0){
    // listen failed
    close(soc);
    perror("listen");
    exit(1);
  }
  return soc;
}

/*
 * Wait for and accept a new connection.
 * Return -1 if the accept call failed.
 */
int accept_connection(int fd, struct sockname *connections){
  int user_index = 0;
  while(user_index < MAX_CONNECTIONS
       && connections[user_index].sock_fd != -1){
    user_index++;
  }
  if(user_index == MAX_CONNECTIONS){
    fprintf(stderr, "server: max concurrent connections\n");
    return -1;
  }

  int client_fd = accept(fd, NULL, NULL);
  if(client_fd < 0){
    perror("server: accept");
    close(fd);
    exit(1);
  }
  connections[user_index].sock_fd = client_fd;
  memset(connections[user_index].username,0,BUF_SIZE);
  connections[user_index].name_read = 0;
  return client_fd;
}


/******************************************************************************
 * Client-specific functions
 *****************************************************************************/
/*
 * Create a socket and connect to the server indicated by the port and hostname
 */
int connect_to_server(int port, const char *hostname){
  int soc = socket(AF_INET, SOCK_STREAM, 0);
  if(soc < 0){
    perror("socket");
    exit(1);
  }
  struct sockaddr_in addr;
  // Allow sockets across machines.
  addr.sin_family = AF_INET;
  // The port the server will be listening on.
  addr.sin_port = htons(port);
  // Clear this field; sin_zero is used for padding for the struct.
  memset(&(addr.sin_zero), 0, 8);
  // Lookup host IP address.
  struct hostent *hp = gethostbyname(hostname);
  if(hp == NULL){
    fprintf(stderr, "unknown host %s\n", hostname);
    exit(1);
  }
  addr.sin_addr = *((struct in_addr *) hp->h_addr);
  // Request connection to server.
  if(connect(soc,(struct sockaddr *)&addr,sizeof(addr)) == -1){
    addr.sin_port = htons(port + 1);
    if(connect(soc, (struct sockaddr *)&addr, sizeof(addr)) == -1){
      perror("connect");
      exit(1);
    }
  }
  return soc;
}

/******************************************************************************
 * Server-Client-share functions
 *****************************************************************************/
/*
 * Find the seperator("\r\n") of sentence
 */
int find_network_newline(char *buf){
  char *find = "\r\n";
  int n = strlen(buf);
  char search[n+1];
  strcpy(search, buf);
  char *ptr = strstr(search, find);
  if(ptr == NULL){
    return -1;
  }else{
    int index = ptr - search + 2;
    return index;
  }
}

void check_error(int nbytes){
  if(nbytes == -1){
    perror("read or write error");
    exit(1);
  }
}
