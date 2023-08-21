#include "server.h"

#define PORT 4950
#define PORTSTR "4950"
#define MAX_EVENTS 15

static void 
obtain_server_socket (TlcaServer *server)
{
  // Obtain string representation of port
  char *portstr = calloc (floor (log10 (server->listening_port) + 1) + 1, 1);
  sprintf (portstr, "%d", server->listening_port);
  // Create server socket

  // Find addresses to bind to
  int yes = 1;
  int result = 0;
  struct addrinfo hints, *ai, *p;

  memset (&hints, 0, sizeof (hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((result = getaddrinfo (NULL, portstr, &hints, &ai)) != 0) {
    server->err = 1;
    server->errcode = result;
    return;
  }

  // Loop through found addresses until one that can be successfully
  // bind'd to is found
  int server_sock = -1;
  for (p = ai; p != NULL; p = p->ai_next) {
    // Attempt to create the socket
    server_sock = socket (p->ai_family, p->ai_socktype, p->ai_protocol);

    if (server_sock == -1) {
      fprintf (stderr, "%d: %s\n", server_sock, "Socket FD = -1");
      continue;
    }

    // Attempt to set the socket's address to re-usable so the application
    // can re-bind quickly if relaunched
    if (setsockopt (server_sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)) == -1) {
      fprintf (stderr, "%d: %s\n", server_sock, "setsockopt() failed");
      continue;
    }

    // Attempt to bind the socket to the address we can listen on the
    // address later on
    if (bind (server_sock, p->ai_addr, p->ai_addrlen)== -1) {
      fprintf (stderr, "%d: %s: %s\n", server_sock, "bind() failed", strerror (errno));
      close (server_sock);
      continue;
    }

    break;
  }

  // If p == NULL, then we looped through past the last valid address, meaning
  // that there was no valid address to bind to
  if (p == NULL) {
    server->err = 2;
    return;
  }

  // Attempt to listen for incoming addresses
  if (listen (server_sock, 128) == -1) {
    server->err = 3;
    return;
  }

  // Output the address the socket is listening on
  if (p->ai_family == AF_INET) {
    struct in_addr servaddr = ((struct sockaddr_in *)p->ai_addr)->sin_addr;
    char *str_serveraddr = calloc (INET_ADDRSTRLEN, 1);
    inet_ntop (p->ai_family, &servaddr, str_serveraddr, INET_ADDRSTRLEN);
    server->listening_addr = str_serveraddr;
  } else {
    struct in6_addr servaddr = ((struct sockaddr_in6 *)p->ai_addr)->sin6_addr;
    char *str_serveraddr = calloc (INET6_ADDRSTRLEN, 1);
    inet_ntop (p->ai_family, &servaddr, str_serveraddr, INET6_ADDRSTRLEN);
    server->listening_addr = str_serveraddr;
  }

  // Free the linked list of addresses
  freeaddrinfo (ai);

  server->server_sock = server_sock;
}

static void
obtain_epoll_instance (TlcaServer *server)
{
  int epollfd;
  epollfd = epoll_create1 (0);

  if (epollfd == -1) {
    server->err = 4;
    server->errcode = errno;
    return;
  }

  struct epoll_event server_event;
  server_event.events = EPOLLIN;
  server_event.data.fd = server->server_sock;

  if (epoll_ctl (epollfd, EPOLL_CTL_ADD, server->server_sock, &server_event) == -1) {
    server->err = 5;
    server->errcode = errno;
    return;
  }

  server->epoll_fd = epollfd;
}

TlcaServer *
tlca_server_new(int max_connections)
{
  TlcaServer *server;
  server = (TlcaServer *)calloc (sizeof (TlcaServer), 1);

  server->max_connections = max_connections;
  server->max_events = MAX_EVENTS;
  server->listening_port = PORT;

  obtain_server_socket (server);
  if (server->err)
    return server;

  obtain_epoll_instance (server);
  if (server->err)
    return server;

  return server;
}

int
tlca_server_run (TlcaServer *server)
{
  // Loop continously, accepting connections
  struct sockaddr_storage client_addr;
  socklen_t client_addr_len = sizeof (client_addr);
  int client_sock = -1;

  int nfds;
  struct epoll_event events [MAX_EVENTS];

  for (;;) {
    // Block until a connection is made
    nfds = epoll_wait (server->epoll_fd, events, MAX_EVENTS, -1);

    for (int i = 0; i < nfds; ++i)
    {
      // A connection is ready to be accepted
      if (events[i].data.fd == server->server_sock) {
        // Attempt to accept the connection
        if ((client_sock = accept (server->server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
          continue;
        }
        printf ("Got connection");

        // Output the client's address
        if (client_addr.ss_family == AF_INET) {
          struct in_addr *addr = &(((struct sockaddr_in *)&client_addr)->sin_addr);
          char straddr[INET_ADDRSTRLEN];
          memset (straddr, 0, INET_ADDRSTRLEN);
          inet_ntop (AF_INET, addr, straddr, INET_ADDRSTRLEN);
          printf (" from %s\n", straddr);
        } else {
          struct in6_addr *addr = &(((struct sockaddr_in6 *)&client_addr)->sin6_addr);
          char straddr[INET6_ADDRSTRLEN];
          memset (straddr, 0, INET6_ADDRSTRLEN);
          inet_ntop (AF_INET6, addr, straddr, INET6_ADDRSTRLEN);
          printf (" from %s\n", straddr);
        }
        // Send a small message, and close the connnection
        send (client_sock, "Hello!", 6, 0);
        close (client_sock);
      }
    }
  }
}
