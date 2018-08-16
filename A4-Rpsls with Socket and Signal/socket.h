#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <netinet/in.h>    /* Internet domain header, for struct sockaddr_in */

#define BUF_SIZE 256
#define MAX_BACKLOG 5 // pending connections that server will hold
#define MAX_CONNECTIONS 2 // clients involved in this game
#define PORT 60000

struct sockname {
	int sock_fd;
	char username[BUF_SIZE];
	int name_read;
};

struct sockaddr_in *init_server_addr(int port);
int set_up_server_socket(struct sockaddr_in *self, int num_queue);
int accept_connection(int fd, struct sockname *connections);

int connect_to_server(int port, const char *hostname);

int find_network_newline(char *buf);
void check_error(int nbytes);
#endif /* _SOCKET_H_*/
