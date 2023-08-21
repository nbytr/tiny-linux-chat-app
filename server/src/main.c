#include "server.h"

int main (void)
{
  TlcaServer *server;

  server = tlca_server_new (128);

  return tlca_server_run (server);
}
