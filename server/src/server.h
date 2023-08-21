#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <errno.h>

typedef struct
{
  int server_sock;
  int epoll_fd;

  int max_connections;
  int max_events;

  char *listening_addr;
  int listening_port;

  int err;
  int errcode;
} TlcaServer;

TlcaServer *
tlca_server_new (int max_connections);

int
tlca_server_run (TlcaServer *server);
