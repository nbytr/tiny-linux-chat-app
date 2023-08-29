#include <stdio.h>
#include <string.h>

#include "server.h"
#include "cli.h"

#include "errordefs.h"


int main (int argc, char *argv[])
{
  // Setup the default program arguments
  CliArguments args;
  args.port = 4950;
  args.max_connections = 128;

  // Replace the defaults with any user provided arguments
  if (process_arguments (argc, argv, &args) == -1) {
    fprintf (stderr, "Invalid arguments! Type --help to see the valid arguments.\n");
    return -1;
  }

  TlcaServer *server;

  server = tlca_server_new (args.port, args.max_connections);

  if (server->err) {
    printf ("Error creating server: %s\n", tlca_server_err_to_str (server->err));

    if (server->err == ERR_NO_VALID_ADDR)
      printf ("%s\n", strerror (server->errcode));
    return server->err;
  }
  printf ("Listening on %s:%d\n", server->listening_addr, server->listening_port);

  return tlca_server_run (server);
}
