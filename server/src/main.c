#include <stdio.h>
#include <string.h>

#include "server.h"

#include "errordefs.h"

int main (void)
{
  TlcaServer *server;

  server = tlca_server_new (128);

  if (server->err) {
    printf ("Error creating server: %s\n", tlca_server_err_to_str (server->err));

    if (server->err == ERR_NO_VALID_ADDR)
      printf ("%s\n", strerror (server->errcode));
    return server->err;
  }
  printf ("Listening on %s:%d\n", server->listening_addr, server->listening_port);

  return tlca_server_run (server);
}
