#pragma once

#include "errordefs.h"

#include "utility/intlist.h"

typedef struct
{
  int server_sock;
  int epoll_fd;

  int max_connections;
  int max_events;

  UtilIntList *connections;

  char *listening_addr;
  int listening_port;

  TlcaServerErrors err;
  int errcode;
} TlcaServer;

TlcaServer *
tlca_server_new (int max_connections);

int
tlca_server_run (TlcaServer *server);
